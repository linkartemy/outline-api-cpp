cmake_minimum_required(VERSION 3.10)
project(outline_vpn_api_cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard and compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "g++-13")
set(CMAKE_CXX_FLAGS "-Wall -O2")

# Добавление подкаталогов
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

# Поиск и подключение зависимостей
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system asio json url)
find_package(fmt REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/exceptions)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(outline_vpn_api ${Boost_LIBRARIES})
endif()

# Подключение заголовочных файлов
include_directories(${CURL_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})

# Source files
set(SOURCES
src/OutlineClient.cpp
)

# Создание основной библиотеки
add_library(outline_vpn_api STATIC ${SOURCES})

# Линковка зависимостей
target_link_libraries(outline_vpn_api PRIVATE
    ${CURL_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto

    Boost::system
    Boost::asio
    Boost::json
    Boost::url
)

add_executable(example examples/basic/main.cpp)
add_executable(test examples/test/main.cpp)
target_include_directories(example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Настройка тестов
enable_testing()
add_subdirectory(tests)
