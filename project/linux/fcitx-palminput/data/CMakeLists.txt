fcitx_add_addon_conf_file(fcitx-palminput.conf)
fcitx_add_inputmethod_conf_file(palminput.conf)

install(FILES palminput.png RENAME fcitx-palminput.png DESTINATION share/icons/hicolor/48x48/apps)
install(FILES palminput-16.png RENAME fcitx-palminput.png DESTINATION share/icons/hicolor/16x16/apps)
install(FILES palminput.png DESTINATION share/fcitx/imicon RENAME palminput.png)
install(FILES palminput-16.png DESTINATION share/fcitx/skin/default RENAME palminput.png)
install(FILES palminput-16.png DESTINATION share/fcitx/skin/classic RENAME palminput.png)

#install(FILES ../engine/dict/py_sys.lex DESTINATION share/fcitx/palminput RENAME py_sys.lex)


