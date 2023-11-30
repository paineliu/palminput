cmake_minimum_required(VERSION 3.12)
project(py_test C)

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

add_executable(pe_test ${PALMINPUT_SRCS})
target_link_libraries(pe_test -libus-1.0)
target_link_libraries(pe_test -lgio-2.0)
target_link_libraries(pe_test -lgobject-2.0)
target_link_libraries(pe_test -lglib-2.0)
target_link_libraries(pe_test -lenchant)



# target_link_libraries(fcitx-palminput ${CMAKE_CURRENT_SOURCE_DIR}/../../../Engine/Libs/PYEngine/Linux/Lib/libpyengine.a)

