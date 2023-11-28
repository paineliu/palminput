package com.palminput.keyboard.view;

import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;

public interface IKeyboard {
    boolean isInit();
    void init(IOperater operater, int methodId, SkinResPool resPool, SkinKeyboard keyboardSkin);
    void show();
    void hide();
    void destory();
}

