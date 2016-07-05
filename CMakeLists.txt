cmake_minimum_required(VERSION 2.8)

project(setdynamixelmotorid)

set(SOURCE_FILES setDynamixelMotorId.cpp)

include_directories(/home/dahuang8/projects/DynamixelSDK/c++/include)

link_directories(/home/dahuang8/projects/DynamixelSDK/c++/build/linux64)

add_executable(setdynamixelmotorid ${SOURCE_FILES})

target_link_libraries(setdynamixelmotorid dxl_x64_cpp rt)

install(TARGETS setdynamixelmotorid RUNTIME DESTINATION bin)
