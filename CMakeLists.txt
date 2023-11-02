cmake_minimum_required(VERSION 3.20)
project(raylib-games LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
if (MSVC)
	add_compile_options(/ZI)
endif()

option(BUILD_SHARED_LIBS OFF)

add_subdirectory(thirdparty/raylib)
add_subdirectory(thirdparty/raygui/projects/CMake)

if ("${PLATFORM}" STREQUAL "Web")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file=${raylib_SOURCE_DIR}/shell.html")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

link_libraries(raylib raygui)

add_executable(pong pong.c)
add_executable(snake snake.c)