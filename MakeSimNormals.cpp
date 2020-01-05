/** MakeSimNormals, a example usage of simNormals
 *  converts a images into a natural looking normal map
 *  Written by Mikeal Simburger 12/29/2019
 */

#include <opencv2/core/utility.hpp>

#include "sim_normals.h"
using namespace simNormals;

int main( int argc, char** argv ){
  cv::String keys = {"{ h help | | print help }"
                     "{ i image | | input image path }"
                     "{ o out_image | | output image path }"
                     "{ min min_detail | 0.0 | min detail size }"
                     "{ max max_detail  | 0.0 | max detail size }"
                     "{ s strength | 0.25 | strength multiplier }"};

  cv::CommandLineParser parser(argc, argv, keys);
  parser.about("MakeSimNormals v1.0.1\nArgument Format: -i=exmaple.jpg");
  if (parser.has("help")) {
    parser.printMessage();
    return 0;
  }
  double min_detail = parser.get<double>("min_detail");
  double max_detail = parser.get<double>("max_detail");
  double strength = parser.get<double>("strength");
  std::string img_path = parser.get<std::string>("i");
  std::string out_path = parser.get<std::string>("o");
  auto has_output = parser.has("o");
  if (!parser.check()) {
    parser.printErrors();
    return 0;
  }
  // check for required input
  if (!parser.has("i")) {
    parser.printMessage();
    return 0;
  }

  // default output location for the lazy
  if (!has_output) {
    out_path = img_path;
    std::size_t found = out_path.find_last_of(".");
    out_path.insert(found, "_normal");
  }

  cv::Mat image = cv::imread(img_path, CV_LOAD_IMAGE_COLOR);
  if (!image.data) {
    std::cout << "Could not open or find the image:" << img_path << std::endl;
    return -1;
  }

  cv::Mat normal;
  if (!MakeNormals(image, strength, min_detail, max_detail, normal)) {
    return -1;
  }

    normal.convertTo(normal, image.type());    
    cv::imwrite( out_path, normal);
    std::cout << "normals written to: " << out_path << "\n";
    return 0;
}