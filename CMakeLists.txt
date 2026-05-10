cmake_minimum_required(VERSION 3.19)
project(The_Artisan_of_Glimmith LANGUAGES CXX)

# 确保开启了自动资源编译
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

# 显式定义资源文件
set(TS_RESOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc"
)

qt_add_executable(The_Artisan_of_Glimmith
    WIN32 MACOSX_BUNDLE
    main.cpp
    artisanwindow.cpp
    artisanwindow.h
    artisanwindow.ui
    ${TS_RESOURCES}
)

target_link_libraries(The_Artisan_of_Glimmith
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
)

# ... 剩下的 install 逻辑保持不变 ...

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
