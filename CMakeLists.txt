cmake_minimum_required(VERSION 3.20)
project(LOBSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "-g -O0 -fsanitize=address,undefined")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")


add_library(LOBSim
    src/LOBSim.cpp
)

target_include_directories(LOBSim PUBLIC include)
target_compile_options(LOBSim PRIVATE -Wall -Wextra)


add_executable(replay src/main.cpp)
target_link_libraries(replay PRIVATE LOBSim)
target_compile_options(replay PRIVATE -Wall -Wextra)

add_executable(test tests/test_main.cpp tests/test_book_diff.cpp)
target_link_libraries(test PRIVATE LOBSim)
target_include_directories(test PRIVATE third_party)


