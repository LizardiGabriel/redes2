cmake_minimum_required(VERSION 3.27)
project(servidor C)

set(CMAKE_C_STANDARD 11)

add_executable(servidor servidor.c
        List.c
        List.h)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

target_link_libraries(servidor jansson)
