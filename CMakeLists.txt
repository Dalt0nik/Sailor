cmake_minimum_required(VERSION 3.0.0)
project(Sailor VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Sailor src/main.cpp)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(Sailor PRIVATE cpr::cpr)