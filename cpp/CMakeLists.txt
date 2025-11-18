cmake_minimum_required(VERSION 3.10)
project(text_processor CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_COMPILER clang++)
# set(CMAKE_C_COMPILER clang)

add_compile_options("-std=c++20")

add_subdirectory(common)


add_executable(producer
    producer/job_factory.cpp
    producer/main.cpp
)
target_include_directories(producer PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(producer common-func)

add_executable(worker
    worker/main.cpp
    worker/worker.cpp
)
target_include_directories(worker PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(worker common-func)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")