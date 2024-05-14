cmake_minimum_required(VERSION 3.0.0)
project(Sailor)

set(CMAKE_CXX_STANDARD 17)

set(DEFAULT_BUILD_TYPE "Release")

file(GLOB SOURCES "src/*.cpp")

add_executable(Sailor ${SOURCES})

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
include_directories(Include)

# FetchContent configuration for cpr
FetchContent_Declare(
    cpr 
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8 # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
)
FetchContent_MakeAvailable(cpr)

# FetchContent configuration for SQLite
FetchContent_Declare(
    sqlite 
    URL https://www.sqlite.org/2023/sqlite-amalgamation-3430000.zip # Use the latest amalgamation version URL
)
FetchContent_MakeAvailable(sqlite)

# FetchContent configuration for nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.10.5 # tag or desired version
)
FetchContent_MakeAvailable(json)

# Add SQLite to the project
add_library(sqlite3 STATIC ${sqlite_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite_SOURCE_DIR})

# Link the libraries to the executable
target_link_libraries(Sailor PRIVATE cpr::cpr sqlite3 nlohmann_json::nlohmann_json)