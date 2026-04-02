cmake_minimum_required(VERSION 3.16)

project(SmartCampusSecondhandTradingSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql WebSockets)

set(PROJECT_SOURCES
    src/main.cpp
    src/loginpage.cpp
    src/loginpage.h
    src/registerpage.cpp
    src/registerpage.h
    src/forgotpasswordpage.h
    src/forgotpasswordpage.cpp
    src/commonwidgets.h
    src/commonwidgets.cpp
    src/databasemanager.h
    src/databasemanager.cpp
    src/httpclient.h
    src/httpclient.cpp
    src/websocketclient.h
    src/websocketclient.cpp

    src/user/mainwindow.cpp
    src/user/mainwindow.h
    src/user/mainwindow.ui
    src/user/GoodsDetailDialog.cpp
    src/user/GoodsDetailDialog.h
    src/user/DisputeSubmitDialog.cpp
    src/user/DisputeSubmitDialog.h
    src/user/paymentdialog.h
    src/user/paymentdialog.cpp
    src/user/reviewdialog.h
    src/user/reviewdialog.cpp
    src/user/profileeditdialog.h
    src/user/profileeditdialog.cpp
    src/user/creditscoredialog.h
    src/user/creditscoredialog.cpp
    src/user/reportsubmitdialog.h
    src/user/reportsubmitdialog.cpp
    src/user/homepage.h
    src/user/homepage.cpp
    src/user/publishpage.h
    src/user/publishpage.cpp
    src/user/usercenterpage.h
    src/user/usercenterpage.cpp
    src/user/messagespage.h
    src/user/messagespage.cpp
    src/user/orderspage.h
    src/user/orderspage.cpp

    src/admin/adminmainwindow.h
    src/admin/adminmainwindow.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

    qt_add_executable(SmartCampusSecondhandTradingSystem
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resource.qrc
        src/apiservice.h src/apiservice.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SmartCampusSecondhandTradingSystem APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SmartCampusSecondhandTradingSystem SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SmartCampusSecondhandTradingSystem
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SmartCampusSecondhandTradingSystem PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Sql Qt6::WebSockets)
target_link_libraries(SmartCampusSecondhandTradingSystem PRIVATE Qt6::Core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SmartCampusSecondhandTradingSystem)
endif()
set_target_properties(SmartCampusSecondhandTradingSystem PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SmartCampusSecondhandTradingSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SmartCampusSecondhandTradingSystem)
endif()
