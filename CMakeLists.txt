cmake_minimum_required(VERSION 2.8)

project(setdynamixelmotorid)

set(DYNAMIXEL_DIR /home/dahuang8/projects/DynamixelSDK)

set(SOURCE_FILES setDynamixelMotorId.cpp)

include_directories(${DYNAMIXEL_DIR}/c++/include)

link_directories(${DYNAMIXEL_DIR}/c++/build/linux64)

add_executable(setdynamixelmotorid ${SOURCE_FILES})

target_link_libraries(setdynamixelmotorid dxl_x64_cpp rt)

install(TARGETS setdynamixelmotorid RUNTIME DESTINATION bin)
