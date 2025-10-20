cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(unpac CXX)

add_library(giga SHARED
    src/giga/app.cpp
    src/giga/archive.cpp
    src/giga/bytestream.cpp
    src/giga/endianness.cpp
    src/giga/lzss.cpp
    src/giga/platform.cpp
)

target_include_directories(giga
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/giga
)

add_executable(unpac)

target_sources(unpac
    PRIVATE
        src/main.cpp
        src/arcv.cpp
        src/g03_item_set.cpp
        src/nmr_save.cpp
)

target_include_directories(unpac
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(unpac PRIVATE giga)
