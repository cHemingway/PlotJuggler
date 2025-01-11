
include_directories( ../ )

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

QT5_WRAP_UI ( UI_SRC  datastream_serial.ui  )

SET( SRC datastream_serial.cpp)

add_library(DataStreamSerial SHARED ${SRC} ${UI_SRC}  )

target_link_libraries(DataStreamSerial
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    Qt5::SerialPort
    plotjuggler_base)

install(TARGETS DataStreamSerial DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY}  )
