cmake_minimum_required(VERSION 3.16)

project(foreach LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(foreach src/main.cpp)

# Set the actual output binary name
# zsh on macOS has built-in "foreach" which classes so change to "for-each" heres
set_target_properties(foreach PROPERTIES
    OUTPUT_NAME "for-each"
)

# Nice warnings (optional)
if (MSVC)
  target_compile_options(foreach PRIVATE /W4 /permissive-)
else()
  target_compile_options(foreach PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS foreach RUNTIME DESTINATION bin)
