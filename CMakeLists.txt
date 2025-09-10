cmake_minimum_required(VERSION 3.16)
project(WordZ LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Sql
)
qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/wordzapplication.cpp
    src/wordz.cpp
    src/worddictionary.cpp

)

set(HEADERS
    src/wordzapplication.h
    src/wordz.h
    src/worddictionary.h
)

set(UIS
    src/wordz.ui
)


qt_add_resources(RESOURCE_SOURCES resources/Resource.qrc)

set(PROJECT_SOURCES
    ${SOURCES}
    ${UIS}
    ${HEADERS}
    ${RESOURCE_SOURCES}
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} resources/WordZ.rc)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
)

target_precompile_headers(${PROJECT_NAME} PUBLIC src/stdafx.h)

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tools/eng-zho.db")
    message(WARNING "警告: 缺少数据库文件 eng-zho.db，将无法复制！")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/tools/eng-zho.db
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/db/
)