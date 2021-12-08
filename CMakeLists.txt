cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(MrazzzMag)

set(SOURCES
        ./priority_queue/priority_queue.h

        ./queue/queue.h

        interface.h
        implementation.h
        implementation.cpp
        main.cpp
        )
add_executable(${PROJECT_NAME} ${SOURCES})

project(priority_queue)

set(SOURCES
        ./priority_queue/priority_queue.h
        ./priority_queue/priority_queue_tests/doctest.h
        ./priority_queue/priority_queue_tests/priority_queue_tests.cpp
        )
add_executable(${PROJECT_NAME} ${SOURCES})

project(tests)

set(SOURCES
        ./priority_queue/priority_queue.h

        ./queue/queue.h

        ./tests/catch.hpp
        ./tests/tests.cpp

        interface.h

        implementation.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE .tests)