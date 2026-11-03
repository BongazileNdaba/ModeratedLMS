cmake_minimum_required(VERSION 3.20)
project(ModeratedLMS VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_path(SODIUM_INCLUDE_DIR NAMES sodium.h
    PATHS "C:/msys64/ucrt64/include" /usr/include REQUIRED)
find_library(SODIUM_LIBRARY NAMES sodium
    PATHS "C:/msys64/ucrt64/lib" /usr/lib /usr/lib/x86_64-linux-gnu REQUIRED)

set(LMS_SOURCES
    src/repositories/Database.cpp
    src/repositories/UserRepository.cpp
    src/services/AuthenticationService.cpp
    src/services/PasswordHasher.cpp
    src/services/WorkflowService.cpp
)

add_library(lms_core ${LMS_SOURCES})
target_include_directories(lms_core PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${SODIUM_INCLUDE_DIR}"
)
target_link_libraries(lms_core PUBLIC SQLite::SQLite3 "${SODIUM_LIBRARY}")

add_executable(ModeratedLMS main.cpp)
target_link_libraries(ModeratedLMS PRIVATE lms_core)

if(MSVC)
    target_compile_options(lms_core PRIVATE /W4)
else()
    target_compile_options(lms_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    # Register each independent automated test suite with CTest.
    add_executable(WorkflowServiceTests tests/WorkflowServiceTests.cpp)
    target_link_libraries(WorkflowServiceTests PRIVATE lms_core GTest::gtest_main)
    gtest_discover_tests(WorkflowServiceTests)

    add_executable(AuthenticationServiceTests tests/AuthenticationServiceTests.cpp)
    target_link_libraries(AuthenticationServiceTests PRIVATE lms_core GTest::gtest_main)
    gtest_discover_tests(AuthenticationServiceTests)

    add_executable(PasswordHasherTests tests/PasswordHasherTests.cpp)
    target_link_libraries(PasswordHasherTests PRIVATE lms_core GTest::gtest_main)
    gtest_discover_tests(PasswordHasherTests)

    # Instrument business-logic/repository code and link the GCC coverage runtime
    # into every executable that links the instrumented library.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(lms_core PRIVATE --coverage -O0 -g)
        foreach(test_target IN ITEMS
                WorkflowServiceTests
                AuthenticationServiceTests
                PasswordHasherTests
                ModeratedLMS)
            target_link_options(${test_target} PRIVATE --coverage)
        endforeach()
    endif()
endif()
