cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(MrazzzMag)

set(SOURCES
        interface.h
        implementation.h
        implementation.cpp
        main.cpp
        )
add_executable(${PROJECT_NAME} ${SOURCES})


project(tests)

set(SOURCES
        ./tests/catch.hpp
        ./tests/tests.cpp

        interface.h

        implementation.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE .tests)