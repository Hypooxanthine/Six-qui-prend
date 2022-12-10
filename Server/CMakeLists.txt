cmake_minimum_required(VERSION 3.0)

project(Server)

set(COMMON_PATH ../Common)

include_directories(include)
include_directories(${COMMON_PATH}/include)

set(SOURCES
    src/main.c
    src/Game.c
)
    
set(HEADERS
    include/Game.h
)

add_executable(Server ${SOURCES} ${HEADERS})

target_link_libraries(Server Common)