cmake_minimum_required(VERSION 3.16)

project(lr6)

set(CMAKE_CXX_STANDARD 17)

# Qt автообработка
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Пути к старым лабам
include_directories(
    ../../lr4
    ../../lr5
)

# Если FFTW установлен
find_library(FFTW_LIB fftw3)

# Исполняемый файл
add_executable(lr6
    main.cpp
    lab6.cpp
    lab6.h
    lab6.ui

    ../../lr5/param.cpp
    ../../lr4/lab4.cpp

    qcustomplot.cpp
)

target_link_libraries(lr6
    Qt6::Widgets
    ${FFTW_LIB}
)