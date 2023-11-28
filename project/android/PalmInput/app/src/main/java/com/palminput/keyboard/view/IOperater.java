package com.palminput.keyboard.view;

import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;

public interface IOperater {
    void deleteTextBeforeCursor();

    void sendCommitText(String str);

    void setInputMethod(int id);

    void returnMainInputMethod();

    void nextInputMethod(int id);

    void hideKeyboard();

    void showMenu();

}
