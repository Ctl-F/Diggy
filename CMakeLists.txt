cmake_minimum_required(VERSION 3.30)
project(Diggy)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

include_directories(Diggy PUBLIC ${SDL2_INCLUDE_DIRS} include ~/dev/glad/glad/include/)

add_executable(Diggy main.cpp renderer.cpp ~/dev/glad/glad/src/glad.c
    include/renderer.h
        include/common.h
        include/util.h
        util.cpp
        include/input.h
        input.cpp
        include/terrain.h
        terrain.cpp
        include/stb_image.h
        stb_image.cpp
        include/json.h)


target_link_directories(Diggy PUBLIC ${SDL2_LIBRARIES})
target_link_libraries(Diggy PUBLIC ${SDL2_LIBRARIES})
