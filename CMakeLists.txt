cmake_minimum_required(VERSION 3.19)
project(The_Artisan_of_Glimmith LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(The_Artisan_of_Glimmith
    WIN32 MACOSX_BUNDLE
    main.cpp
    artisanwindow.cpp
    artisanwindow.h
    artisanwindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
)

target_link_libraries(The_Artisan_of_Glimmith
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
)

install(TARGETS The_Artisan_of_Glimmith
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET The_Artisan_of_Glimmith
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
