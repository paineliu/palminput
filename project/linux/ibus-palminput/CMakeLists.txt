cmake_minimum_required(VERSION 3.12)
project(ibus-engine-enchant C)

set(CMAKE_C_STANDARD 11)
link_directories(
    /usr/lib/x86_64-linux-gnu/
)
include_directories(
    /usr/include/ibus-1.0
    /usr/include/glib-2.0
    /usr/include/enchant/
    /usr/lib/x86_64-linux-gnu/glib-2.0/include/
    ${PROJECT_BINARY_DIR}
    ${LIBINTL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../engine/include
)

set(PALMINPUT_SRCS
    engine.c
    main.c
)

add_executable(ibus-engine-enchant ${PALMINPUT_SRCS})
target_link_libraries(ibus-engine-enchant -libus-1.0)
target_link_libraries(ibus-engine-enchant -lgio-2.0)
target_link_libraries(ibus-engine-enchant -lgobject-2.0)
target_link_libraries(ibus-engine-enchant -lglib-2.0)
target_link_libraries(ibus-engine-enchant -lenchant)

include (InstallRequiredSystemLibraries)

set(_VERSION_MAJOR 1)
set(_VERSION_MINOR 1)
set(_VERSION_PATCH 0)

set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")

# 安装文件
install(TARGETS ibus-engine-enchant DESTINATION /usr/libexec/)
install(FILES ./icons/ibus-enchant.svg DESTINATION /usr/share/ibus-tmpl/icons/)
install(FILES ./enchant.xml DESTINATION /usr/share/ibus/component/)

# 打包 
set(CPACK_PACKAGE_NAME "PalmInput")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "LiuTingchao")

# set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_HOST_SYSTEM_PROCESSOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "ibus (>= 1.5.22)")
set(CPACK_PACKAGE_DESCRIPTION "PalmInput Chinese input method")
set(CPACK_PACKAGE_CONTACT "PalmInput")

include(CPack) 


# target_link_libraries(fcitx-palminput ${CMAKE_CURRENT_SOURCE_DIR}/../../../Engine/Libs/PYEngine/Linux/Lib/libpyengine.a)

