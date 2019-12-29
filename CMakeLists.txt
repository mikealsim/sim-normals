cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_STANDARD 17)
project (MakeSimNormals)
find_package( OpenCV REQUIRED )
add_executable(MakeSimNormals MakeSimNormals.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})