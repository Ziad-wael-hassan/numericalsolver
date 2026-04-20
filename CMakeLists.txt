cmake_minimum_required(VERSION 3.21)

project(NumericalSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)
# find_package(Qt6 QUIET COMPONENTS Charts)  # Disabled due to CMake config error; Charts optional

file(GLOB_RECURSE NUMERICAL_SOLVER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

add_executable(NumericalSolver ${NUMERICAL_SOLVER_SOURCES})

target_include_directories(NumericalSolver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(NumericalSolver
    PRIVATE
        Qt6::Widgets
        Qt6::Concurrent
)

if (TARGET Qt6::Charts)
    target_link_libraries(NumericalSolver PRIVATE Qt6::Charts)
    target_compile_definitions(NumericalSolver PRIVATE NUMERICAL_SOLVER_HAS_QTCHARTS=1)
endif()
