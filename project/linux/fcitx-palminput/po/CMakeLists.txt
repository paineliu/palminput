file(GLOB PO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.po)
file(RELATIVE_PATH REL_SOURCE_ROOT ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})
if ("${REL_SOURCE_ROOT}" STREQUAL "")
    set(REL_SOURCE_ROOT ".")
endif("${REL_SOURCE_ROOT}" STREQUAL "")

set(POT_FILE fcitx-palminput.pot)

configure_file(POTFILES.in.in ${CMAKE_CURRENT_BINARY_DIR}/POTFILES.in)

extract_fcitx_addon_conf_postring()

add_custom_target(
    pot
    COMMAND INTLTOOL_EXTRACT=${INTLTOOL_EXTRACT} srcdir=${CMAKE_CURRENT_BINARY_DIR} ${INTLTOOL_UPDATE} --gettext-package fcitx-palminput --pot
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/conf.po
    )

# Update .po files and compile them to binary .gmo files
gettext_create_translations(${POT_FILE} ALL ${PO_FILES})

