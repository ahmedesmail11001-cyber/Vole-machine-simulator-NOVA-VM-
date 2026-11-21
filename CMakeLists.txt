cmake_minimum_required(VERSION 3.16)

project(NovaVM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# works with both Qt 5 and Qt 6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(NovaVM
    # --- GUI files ---
    main.cpp
    MainWindow.cpp   MainWindow.h   MainWindow.ui
    CodeEditor.cpp   CodeEditor.h
    WaveformWidget.cpp WaveformWidget.h
    CodeParser.cpp   CodeParser.h
    resources.qrc

    # --- simulator files (UI.cpp / UI.h = old console menu, not used by the GUI) ---
    Alu.cpp          Alu.h
    CPU.cpp          CPU.h
    Instruction.cpp  Instruction.h
    Machine.cpp      Machine.h
    Memory.cpp       Memory.h
    ProgramLoader.cpp ProgramLoader.h
    RegisterFile.cpp RegisterFile.h
    Screen.cpp       Screen.h
)

# lets the generated ui_MainWindow.h find CodeEditor.h and WaveformWidget.h
target_include_directories(NovaVM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NovaVM PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(NovaVM PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)