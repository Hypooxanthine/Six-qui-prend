cmake_minimum_required(VERSION 3.0)

project(StatMaker)

include_directories(include)

set(SOURCES
    src/main.c
)
    
set(HEADERS
)

add_executable(StatMaker ${SOURCES} ${HEADERS})