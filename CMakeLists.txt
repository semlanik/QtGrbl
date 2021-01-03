cmake_minimum_required(VERSION 3.5)

project(qtgrbl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Core Quick LinguistTools SerialPort REQUIRED)

set(TS_FILES qtgrbl_en_US.ts)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/QmlPolicyStateMachine")

if(ANDROID)
    add_library(qtgrbl SHARED
      main.cpp
      qml.qrc
      ${TS_FILES}
    )
else()
    add_executable(qtgrbl
      src/main.cpp
      src/grblengine.cpp
      src/grblconsole.cpp
      src/grblserial.cpp
      src/grblconsolerecord.cpp
      src/universallistmodel.cpp
      src/universallistmodelbase.cpp
      src/grblremotemessagebuffer.cpp
      src/grblgcodestate.cpp
      src/qml/qml.qrc
      ${TS_FILES}
    )
endif()

target_include_directories(qtgrbl PRIVATE src)
target_compile_definitions(qtgrbl
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(qtgrbl
  PRIVATE Qt5::Core Qt5::Quick Qt5::SerialPort)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
