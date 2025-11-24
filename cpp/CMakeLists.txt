cmake_minimum_required(VERSION 3.10)
project(text_processor CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("-std=c++20")

add_subdirectory(common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer -Wno-uninitialized")
add_executable(producer
    producer/job_factory.cpp
    producer/main.cpp
)
target_include_directories(producer PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(producer common-func)

add_executable(worker
    worker/main.cpp
    worker/work.cpp
    worker/worker.cpp
)
target_include_directories(worker PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(worker common-func)

add_executable(aggregator
    aggregator/main.cpp
    aggregator/aggregator.cpp
    aggregator/job_reporter.cpp
    aggregator/merge_tool.cpp
)
target_include_directories(aggregator PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(aggregator common-func)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")