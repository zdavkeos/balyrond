
include_directories( ../ )

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

QT5_WRAP_UI ( UI_SRC
    toolbox_roundness.ui
    )

add_library(ToolboxRoundness SHARED
    toolbox_roundness.cpp
    toolbox_roundness.h
    ${UI_SRC}  )

target_link_libraries(ToolboxRoundness
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    plotjuggler_base
    plotjuggler_qwt)

install(TARGETS ToolboxRoundness DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY}  )
