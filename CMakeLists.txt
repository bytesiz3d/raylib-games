cmake_minimum_required(VERSION 3.20)
project(raylib-games LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(raylib
	GIT_REPOSITORY git@github.com:raysan5/raylib.git
	GIT_SHALLOW TRUE
	GIT_TAG 4.5.0)
FetchContent_Declare(raygui
	GIT_REPOSITORY git@github.com:raysan5/raygui.git
	GIT_SHALLOW TRUE
	GIT_TAG 3.6
	SOURCE_SUBDIR projects/CMake)

set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(raylib raygui)
link_libraries(raylib raygui)

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
	add_compile_options(/ZI)
endif()

if (${PLATFORM} STREQUAL "Web")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

add_executable(pong pong.cpp)
add_executable(snake snake.cpp)