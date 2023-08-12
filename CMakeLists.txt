cmake_minimum_required(VERSION 3.2)
project(gmm-kmeans VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE Debug)

add_executable(gmm
    src/main-gmm.c
    src/gmm.c
    src/kmeans.c
    src/data.c
)
target_include_directories(gmm PRIVATE src)

add_executable(kmeans
    src/main-kmeans.c
    src/kmeans.c
    src/data.c
)
target_include_directories(kmeans PRIVATE src)

add_executable(test-data
    src/test/test-data.c
    src/data.c
)
target_include_directories(test-data PRIVATE src)

if (UNIX)
    target_link_libraries(gmm PRIVATE m)
    target_link_libraries(kmeans PRIVATE m)
endif()
