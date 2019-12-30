#include "sim_normals.h"
using namespace simNormals;

int main( int argc, char** argv ){
    double min_detail = 0.0;
    double max_detail = 0.0;
    double strength = 0.25;

    if (argc < 2 || argc > 5) {
      std::cout << " Usage: makeNormals ImagePath strength[1>0; default:.25] "
                   "MinDataSize[>= 0.0] MaxDataSize[>= 0.0]"
                << std::endl;
      return -1;
    }

    if( argc >= 3){
      std::string::size_type sz;
      strength = std::stod(argv[2], &sz);
      strength = std::max(strength, 0.0);
    }

    if (argc >= 4) {
      std::string::size_type sz;
      min_detail = std::stod(argv[3], &sz);
      min_detail = std::max(min_detail, 0.0);
    }
    if (argc == 5) {
      std::string::size_type sz;
      max_detail = std::stod(argv[4], &sz);
      max_detail = std::max(max_detail, 0.0);
    }

    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    if(! image.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

     cv::Mat normal;
     if (!MakeNormals(image, strength, min_detail, max_detail, normal)) {
       return -1;
     }

    normal.convertTo(normal, image.type());    
    std::string out_path = argv[1];
    std::size_t found = out_path.find_last_of(".");
    out_path.insert(found,"_normal"); 

    cv::imwrite( out_path, normal);
    return 0;
}