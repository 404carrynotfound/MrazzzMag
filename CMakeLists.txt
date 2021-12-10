cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(MrazzzMag)

set(CLI_SOURCES
        ./queue/queue.h

        interface.h
        implementation.h
        implementation.cpp
        main.cpp
        )
add_executable(${PROJECT_NAME} ${CLI_SOURCES})

project(tests)

set(TEST_SOURCES
        ./queue/queue.h

        ./tests/catch.hpp
        ./tests/tests.cpp

        interface.h

        implementation.cpp
        )

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE .tests)

project(my_tests)

set(MY_TEST_SOURCES
        ./queue/queue.h

        ./tests/catch.hpp
        ./tests/my_tests.cpp

        interface.h

        implementation.cpp
        )

add_executable(${PROJECT_NAME} ${MY_TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE .tests)