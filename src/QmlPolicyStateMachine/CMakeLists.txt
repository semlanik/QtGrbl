cmake_minimum_required(VERSION 3.14)

set(TARGET QmlPolicyStateMachine)
set(TARGET_EXPORT ${TARGET}Targets)

project(${TARGET} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS -Muri=QmlPolicyStateMachine)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

file(GLOB SOURCES
    qmlpolicystatemachineplugin.cpp
    statepolicy.cpp)

file(GLOB HEADERS
    statepolicy.h)

file(GLOB QML_FILES *.qml)

add_library(${TARGET} SHARED ${SOURCES})

target_link_libraries(${TARGET} PRIVATE Qt5::Core Qt5::Quick Qt5::Qml)

export(TARGETS ${TARGET} NAMESPACE ${QT_PROTOBUF_PROJECT}:: FILE "${TARGET_EXPORT}.cmake")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/qmldir" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY ${QML_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
