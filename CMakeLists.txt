cmake_minimum_required(VERSION 3.2)
project(kmeans VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

add_executable(kmeans src/main.c src/data.c src/kmeans.c)
target_include_directories(kmeans PRIVATE src)

if (UNIX)
    target_link_libraries(kmeans PRIVATE m)
endif()