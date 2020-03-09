cmake_minimum_required(VERSION 3.10)

project(UNIX)

include_directories(include)

find_package(Boost COMPONENTS system filesystem REQUIRED)

set(EXTERNAL_LIBS "stdc++fs")

LINK_DIRECTORIES(/usr/include/boost)

set(CMAKE_CXX_STANDARD 17)

IF(APPLE)
    set(CMAKE_CXX_STANDARD_LIBRARIES "-mmacosx-version-min=10.15")
ENDIF(APPLE)

file(GLOB SOURCES "source/*.cpp")
set(SOURCES ${SOURCES})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Og -g -Wshadow -Wall -Wno-old-style-cast -pedantic -Wextra -Wold-style-cast")

add_executable(bash ${SOURCES})

target_link_libraries(bash
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${EXTERNAL_LIBS}
        )
