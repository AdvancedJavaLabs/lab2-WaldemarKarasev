cmake_minimum_required(VERSION 3.10)
project(library CXX)


include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    simpleamqp
    GIT_REPOSITORY https://github.com/alanxz/SimpleAmqpClient.git
    GIT_TAG v2.5.1
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(simpleamqp)

add_library(common-func 
    app.cpp
    rabbitmq_client.cpp
    message.cpp
    text_utils.cpp
    thread_pool/thread_pool.cpp
)

target_link_libraries(common-func PUBLIC SimpleAmqpClient nlohmann_json)

message("AAAAAAAAAAAAAAA: " ${simpleamqp_SOURCE_DIR}/src)
target_include_directories(common-func PUBLIC ${simpleamqp_SOURCE_DIR}/src)