cmake_minimum_required(VERSION 3.0)

project(IA)

set(COMMON_PATH ../Common)

include_directories(include)
include_directories(${COMMON_PATH}/include)

set(SOURCES
    src/main.c
    src/Helper.c
    src/Behaviour.c
)
    
set(HEADERS
    include/Helper.h
    include/Behaviour.h
)

add_executable(IA ${SOURCES} ${HEADERS})

target_link_libraries(IA Common)