package com.palminput.keyboard.view;

import android.content.Context;

import com.palminput.keyboard.InputMgrView;


public class SymbolView extends KBBaseView {

    public SymbolView(Context context) {
        super(context);
    }
    public boolean onKeyEvent(String keyId, int actionId)
    {
        super.onKeyEvent(keyId, actionId);
        if (mInputMethodId != InputMgrView.INPUT_METHOD_PHONENUM && mInputMethodId != InputMgrView.INPUT_METHOD_NUMBER && !keyId.startsWith("VK_")) {
            if (!mSymbolLocked && !Character.isDigit(keyId.charAt(0))) {
                mOperater.returnMainInputMethod();
            }
        }

        return true;
    }
}

