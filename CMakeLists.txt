cmake_minimum_required(VERSION 3.18)

set( CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} "D:/compilers/QT6/5.15.2/msvc2019/lib/cmake")

project(GitConfigurator)

include(cmakeconf/compiler_conf.cmake)
include(cmakeconf/building_output.cmake)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

add_executable(GitConfigurator WIN32)

target_sources(GitConfigurator
PRIVATE
    main.cpp
    configure_window.cpp
    configure_window.ui
)

target_link_libraries(GitConfigurator
PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
)

set_target_properties(GitConfigurator PROPERTIES AUTOMOC ON)
set_target_properties(GitConfigurator PROPERTIES AUTOUIC ON)