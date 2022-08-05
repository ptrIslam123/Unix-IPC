cmake_minimum_required(VERSION 3.16)

project(engine)

add_subdirectory(buffers)
add_subdirectory(net_api)
add_subdirectory(utils)

add_library(${PROJECT_NAME} STATIC )

target_link_libraries(${PROJECT_NAME} PUBLIC
    buffer
    net_api
    utils
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
