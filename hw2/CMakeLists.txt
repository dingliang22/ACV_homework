add_library(common STATIC 
  src/common/path.cpp 
  src/common/image.cpp
  src/processing/threshold.cpp
  src/processing/connected_components.cpp
  src/processing/drawing.cpp
  src/hw2_1.cpp
)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(hw2 main.cpp)
target_link_libraries(hw2 common)