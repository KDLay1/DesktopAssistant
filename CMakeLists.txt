cmake_minimum_required(VERSION 3.21)

project(DesktopAssistant LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
    src/*.ui
    base/*.h
    models/*.cpp
    models/*.h
    services/*.cpp
    services/*.h
)

qt_add_executable(DesktopAssistant
    ${PROJECT_SOURCES}
)

target_include_directories(DesktopAssistant PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(DesktopAssistant
    PRIVATE
    Qt6::Widgets
    Qt6::Sql
)
