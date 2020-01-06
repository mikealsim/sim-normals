/** Implimentatin File for create normal maps from images
 * Create by Mikeal Simburger 12/29/2019
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <limits.h>

namespace simNormals 
{ 
/** Normalizes a normal map split into channels, 
 *  param[ref] x     a normal channel, expected data range: [-1.0 - 1.0]
 *  param[ref] y     a normal channel, expected data range: [-1.0 - 1.0]
 *  param[ref] z     a normal channel, expected data range: [-1.0 - 1.0]
 *  channels are arbitrary
*/
void NormalizeNormals( cv::Mat& x, cv::Mat& y, cv::Mat& z){
    cv::Mat scale;
    cv::sqrt(z.mul(z) + y.mul(y) + x.mul(x) , scale );    
    z /= scale;
    y /= scale;
    x /= scale;
}

/** Normalizes a normal map, 
 *  param[ref] normal      cv::Mat 3 channels, expected data range: [0.0 - 1.0]
*/
void NormalizeNormals( cv::Mat& normal){
    auto ddepth = normal.depth();
    //needs to be signed float
    if( ddepth != CV_32F || ddepth != CV_64F ){
        normal.convertTo(normal, CV_32F );
    }
    normal = (normal *2.0) -1.0;
    std::vector<cv::Mat> mat_vec;    
    cv::split(normal, mat_vec);
    NormalizeNormals( mat_vec[0], mat_vec[1], mat_vec[2]);
    merge(mat_vec, normal);
    normal = (normal + 1.0) /2.0;
    // revert data type
    if( ddepth != CV_32F || ddepth != CV_64F ){
        normal.convertTo(normal, ddepth );
    }
}

/**  Creates normals from single channel cv::Mat 
 *   param[in]     single channel cv::Mat, expected data range: [0.0 - 1.0]
 *   return normal_map
*/
cv::Mat CreateNormal(cv::Mat gray, double strength = 1.0) {
  auto ddepth = CV_32F;
  std::vector<cv::Mat> mat_vec(3);
  /// Gradient X
  cv::Scharr(gray, mat_vec[2], ddepth, 1, 0, strength, 0.0, cv::BORDER_DEFAULT);
  /// Gradient Y
  cv::Scharr(gray, mat_vec[1], ddepth, 0, 1, strength, 0.0, cv::BORDER_DEFAULT);
  mat_vec[2] *= -1.0;
  mat_vec[1] *= -1.0;

  // find z
  cv::Mat sum = mat_vec[2].mul(mat_vec[2]) + mat_vec[1].mul(mat_vec[1]);
  cv::sqrt(sum, mat_vec[0]);
  mat_vec[0] = 1.0 - mat_vec[0];

  NormalizeNormals(mat_vec[0], mat_vec[1], mat_vec[2]);
  cv::Mat normal;
  merge(mat_vec, normal);
  normal = (normal + 1.0) / 2.0;
  return normal;
}

/** fast gausian blur via resizing
 * param[in] image              a cv::Mat
 * param[in] filter_size        desired effective smoothing
 * param[in] min_filter_size    smallest allowable smoothing after resizing
 * return cv::Mat
*/
cv::Mat FastGausianBlur(cv::Mat image, double filter_size, double min_filter_size=20.0 ){
    auto image_size = image.size();  
    cv::Mat t;
    while(filter_size/2.0 > min_filter_size &&  image.cols > 2 && image.rows > 2 ){
        filter_size /= 2.0;
        resize(image, image, cv::Size( image.cols/2.0, image.rows/2.0 ), 0,0, cv::INTER_AREA );
    }

    if( filter_size >0){
        GaussianBlur(image, t, cv::Size(0,0), filter_size, filter_size, cv::BORDER_DEFAULT );
        image = t;
    }
    resize(image, image, image_size, 0,0, cv::INTER_CUBIC );
    return image;
}

/** Create a natural looking normal map from a Photo
 *  param[in] image        color image
 *  param[in] strength     strength multipler[>=0.0; default 1.0]
 *  param[in] min_detail   min detail size[>=0.0]
 *  param[in] max_detail   max detail size[>=0.0]
 *  param[out] normal      the normals
 *  return sucess
 */
bool MakeNormals(const cv::Mat &image, double strength, double min_detail,
                 double max_detail, cv::Mat &normal) {

  strength /= 4.0;
  if (strength <= 0.0)
    strength = 1.0;
  constexpr auto kMin_Filter_size = 20.0;
  double data_max_range = 0.0;
  switch (image.depth()) {
  case CV_8U:
    data_max_range = std::numeric_limits<uchar>::max();
    break;
  case CV_16U:
    data_max_range = std::numeric_limits<ushort>::max();
    break;
  case CV_32F:
    data_max_range = 1.0;
    break;
  case CV_64F:
    data_max_range = 1.0;
    break;
  default:
    std::cout << "Error: Unhandled Data Format" << std::endl;
    return false;
    break;
  }

  cv::Mat gray;
  cv::cvtColor(image, gray, CV_BGR2GRAY);
  if (image.depth() != CV_32F || image.depth() != CV_64F) {
    gray.convertTo(gray, CV_32F);
  }
  gray /= data_max_range;

  if (min_detail > 0) {
    gray = FastGausianBlur(gray, min_detail, kMin_Filter_size);
  }
  if (max_detail > 0) {
    cv::Mat hp = FastGausianBlur(gray, max_detail, kMin_Filter_size);
    gray = gray - hp; // High Pass
  }

  normal = CreateNormal(gray, strength);
  int count = 1;
  // pyramid processing
  while (gray.cols / 2 > 3 && gray.rows / 2 > 3) {
    resize(gray, gray, cv::Size(gray.cols / 2, gray.rows / 2), 0, 0,
           cv::INTER_AREA);
    cv::Mat normal_pry = CreateNormal(gray, strength);
    resize(normal_pry, normal_pry, cv::Size(image.cols, image.rows), 0, 0,
           cv::INTER_CUBIC);
    // future: optional weighting based on pryimid level
    // addWeighted(normal, .85, normal_pry, 0.15 , 0.0, normal, CV_32F);
    normal += normal_pry;
    count++;
    if (max_detail > 0.0 && std::pow(2.0, count) > max_detail)
      break;
  }
  normal /= count;
  NormalizeNormals(normal);
  normal *= data_max_range;
  return true;
}
}