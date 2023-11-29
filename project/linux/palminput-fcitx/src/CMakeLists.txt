
set(FCITX_PALMINPUT_SRCS
    eim.cpp
)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${LIBINTL_INCLUDE_DIR}
    ${FCITX4_FCITX_INCLUDE_DIRS}
    ${FCITX4_FCITX_CONFIG_INCLUDE_DIRS}
    ${FCITX4_FCITX_UTILS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Engine/Libs/PYEngine/Linux/Include
)

fcitx_add_addon(fcitx-palminput ${FCITX_PALMINPUT_SRCS})
target_link_libraries(fcitx-palminput ${LIBINTL_LIBRARIES})
target_link_libraries(fcitx-palminput ${CMAKE_CURRENT_SOURCE_DIR}/../../../Engine/Libs/PYEngine/Linux/Lib/libpyengine.a)

