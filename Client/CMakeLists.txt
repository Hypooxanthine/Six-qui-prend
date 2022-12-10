cmake_minimum_required(VERSION 3.0)

project(Client)

set(COMMON_PATH ../Common)

include_directories(include)
include_directories(${COMMON_PATH}/include)

set(SOURCES
    src/main.c
)
    
set(HEADERS
)

add_executable(Client ${SOURCES} ${HEADERS})

target_link_libraries(Client Common)