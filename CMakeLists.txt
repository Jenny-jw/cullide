cmake_minimum_required (VERSION 3.9.0)
project(GL)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra")

add_executable(GL 
	main.cpp 
	../glad/gl.c 
)

target_link_libraries(GL glfw assimp glm::glm)