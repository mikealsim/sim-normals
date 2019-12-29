#include "sim_normals.cpp"
namespace simNormals 
{ 
    bool MakeNormals(const cv::Mat& image, double min_detail, double max_detail, cv::Mat& normal);
}