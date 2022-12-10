cmake_minimum_required(VERSION 3.0)

project(Common)

include_directories(include)

set(SOURCES
    src/Messages.c
    src/Connect.c
    src/Cards.c
    src/Players.c
    src/Clients.c
)
    
set(HEADERS
    include/Types.h
    include/Messages.h
    include/Connect.h
    include/Cards.h
    include/Players.h
    include/Clients.h
)

add_library(Common ${SOURCES} ${HEADERS})