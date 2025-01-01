cmake_minimum_required(VERSION 3.15)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Actually fetch and populate GoogleTest
FetchContent_MakeAvailable(googletest)

add_executable(test_AccessKeys
    test_AccessKeys.cpp
)

# Link the test executable with GoogleTest libraries and our MyApiClient library
target_link_libraries(test_AccessKeys
    PRIVATE
        gtest
        gtest_main
        OutlineClient
)

# Add test run with ctest
add_test(NAME test_AccessKeys COMMAND test_AccessKeys)
