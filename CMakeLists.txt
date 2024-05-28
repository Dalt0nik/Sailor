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

# FetchContent configuration for ImGui with addons
FetchContent_Declare(
    imgui_addons
    GIT_REPOSITORY https://github.com/Flix01/imgui.git
    GIT_TAG c2dd0c9d58fdd6f6e6d3cad58d8e0e80ca9aebf0 # Replace with the specific branch or commit
)
FetchContent_MakeAvailable(imgui_addons)

# FetchContent configuration for GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8 # Replace with the latest stable version or the version you want to use
)
FetchContent_MakeAvailable(glfw)

# FetchContent configuration for GLEW
FetchContent_Declare(
    glew
    URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.zip
)
FetchContent_MakeAvailable(glew)

# Add GLEW to the project
add_library(glew STATIC ${glew_SOURCE_DIR}/src/glew.c)
target_include_directories(glew PUBLIC ${glew_SOURCE_DIR}/include)
target_compile_definitions(glew PUBLIC GLEW_STATIC)

# Add SQLite to the project
add_library(sqlite3 STATIC ${sqlite_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite_SOURCE_DIR})

# Add ImGui and its backends to the project
add_library(imgui STATIC 
    ${imgui_addons_SOURCE_DIR}/imgui.cpp
    ${imgui_addons_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_addons_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_addons_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_addons_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_addons_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_addons_SOURCE_DIR} ${imgui_addons_SOURCE_DIR}/backends)

# Ensure GLFW include directories are added
include_directories(${glfw_SOURCE_DIR}/include)

# Add ImGuiDateChooser addon to the project
add_library(imgui_datechooser STATIC ${imgui_addons_SOURCE_DIR}/addons/imguidatechooser/imguidatechooser.cpp)
target_include_directories(imgui_datechooser PUBLIC ${imgui_addons_SOURCE_DIR}/addons/imguidatechooser ${imgui_addons_SOURCE_DIR})

# Link the libraries to the executable
target_link_libraries(Sailor PRIVATE cpr::cpr sqlite3 nlohmann_json::nlohmann_json imgui glfw glew imgui_datechooser)

# Link OpenGL (you might need to adjust this depending on your platform)
if (WIN32)
    target_link_libraries(Sailor PRIVATE opengl32)
else ()
    find_package(OpenGL REQUIRED)
    target_link_libraries(Sailor PRIVATE OpenGL::GL)
endif()
