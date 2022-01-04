cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_REQUIRED ON)
set(VERBOSE 1)


file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE HDR src/*.hpp)
file(GLOB_RECURSE SRC_C libs/*.c src/*.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ilibs/include -Ilibs/GLAD/include")

include_directories(libs/include libs/GLAD/include)
project(flightSimulator LANGUAGES CXX C)
add_executable(main.bin ${SRC_C} ${SRC} ${HDR})
target_link_libraries(main.bin glfw GL dl m stdc++)

