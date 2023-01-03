cmake_minimum_required(VERSION 3.10.2)

project(balyrond_pj_plugins)

#---- Enable C++17 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#-------  Include Qt dependencies -------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

if(APPLE AND EXISTS /opt/homebrew/opt/qt@5)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")
    set(CMAKE_MACOSX_RPATH 1)
endif()

find_package(Qt5 REQUIRED COMPONENTS
    Widgets
    Xml
    Svg
    SerialPort
    OpenGL)

find_package(plotjuggler REQUIRED)

include_directories(
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5Svg_INCLUDE_DIRS}
    ${plotjuggler_INCLUDE_DIR}
    )

if(plotjuggler_FOUND)
    message(STATUS "PlotJuggler FOUND")
    message(STATUS "plotjuggler_INCLUDE_DIR: ${plotjuggler_INCLUDE_DIR}")
    message(STATUS "plotjuggler_LIBRARIES: ${plotjuggler_LIBRARIES}")
else()
    message(FATAL_ERROR "PlotJuggler NOT FOUND")
endif()

###########################################
add_definitions( ${QT_DEFINITIONS} -DQT_PLUGIN )

set( PJ_LIBRARIES
    Qt5::Widgets
    Qt5::Xml
    Qt5::Svg
    Qt5::SerialPort
    ${plotjuggler_LIBRARIES} )

#--------------------------------------------------------

include_directories(${plotjuggler_INCLUDE_DIR})

#------- Create the libraries -------

QT5_WRAP_UI ( BALYROND_UI_SRC  DataStreamBalyrond/datastream_balyrond_dialog.ui  )
#QT5_ADD_RESOURCES (RES_SRC  DataStreamBalyrond/icons.qrc )

add_library(DataStreamBalyrond SHARED
    DataStreamBalyrond/datastream_balyrond.h
    DataStreamBalyrond/datastream_balyrond.cpp
    DataStreamBalyrond/datastream_balyrond_dialog.h
    DataStreamBalyrond/datastream_balyrond_dialog.cpp
    ${BALYROND_UI_SRC}
    ${RES_SRC}
    )

target_link_libraries(DataStreamBalyrond PRIVATE
    ${PJ_LIBRARIES})

include_directories(
    # Hacky...
    ../PlotJuggler/plotjuggler_base/src
    ../PlotJuggler/3rdparty/qwt/src
    )

QT5_WRAP_UI ( BALYROND_UI_SRC  ToolboxRoundness/toolbox_roundness.ui  )
#QT5_ADD_RESOURCES (RES_SRC  DataStreamBalyrond/icons.qrc )
    
add_library(ToolboxRoundness SHARED
        ToolboxRoundness/toolbox_roundness.h
        ToolboxRoundness/toolbox_roundness.cpp
        ${BALYROND_UI_SRC}
        ${RES_SRC}
    )
    
target_link_libraries(ToolboxRoundness PRIVATE
    ${PJ_LIBRARIES})

#------- Install the libraries -------
install(
    TARGETS
        DataStreamBalyrond
        ToolboxRoundness
    DESTINATION bin )


