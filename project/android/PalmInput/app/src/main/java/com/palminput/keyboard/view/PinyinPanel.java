package com.palminput.keyboard.view;

import android.content.Context;
import com.palminput.keyboard.InputMgrView;


public class PinyinPanel extends KBBaseView {

    public PinyinPanel(Context context) {
        super(context);
        mShowPop = true;
        mInputMethodId  = InputMgrView.INPUT_METHOD_PINYIN;
    }
    public String getDisplayString(String btnId) {
        PinyinView v = (PinyinView) getParent();
        int nCompLen = v.getCompLen();
        if (btnId.equals("EK_SPLIT") && nCompLen > 0) {
            return "'";
        } else {
            return super.getDisplayString(btnId);
        }
    }

    public String getDisplayTagString(String btnId) {
        PinyinView v = (PinyinView) getParent();
        int nCompLen = v.getCompLen();
        if (btnId.equals("EK_SPLIT") && nCompLen > 0) {
            return null;
        } else {
            return super.getDisplayTagString(btnId);
        }
    }
    public boolean onKeyEvent(String keyId, int actionId) {

        PinyinView v = (PinyinView) getParent();
        int nCompLen = v.getCompLen();
        if (keyId.equals("VK_LANGUAGE") || keyId.equals("VK_SYMBOL_CN") || keyId.equals("VK_ENGLISH"))
        {
            v.resetKey();
        }

        switch (keyId) {
            case "VK_SPACE":
                if (nCompLen > 0) {
                    v.selItem(0);
                }
                else {
                    mOperater.sendCommitText(" ");
                }

                break;

            case "EK_SPLIT":
                if (nCompLen > 0 && actionId == KEY_CLICK)
                {
                    v.addKey('\'');
                }

                break;
            case "VK_HIDE":
                mOperater.hideKeyboard();
                break;
            case "VK_DONE":
                if (v.getCompLen() > 0) {
                    v.commitCompString();
                }
                else {
                    mOperater.sendCommitText("\n");
                }
                break;

        }

        if (keyId.equals("VK_BACKSPACE") && nCompLen > 0)
        {
            if (actionId == KEY_SLID_LEFT) {
                v.resetKey();
            }
            else {
                v.addKey('\b');
            }
        }

        if (keyId.length() == 1 && actionId == KEY_CLICK)
        {
            char n = keyId.charAt(0);
            if (n >= 'A' && n <= 'Z')
            {
                v.addKey(n + 'a' - 'A');
            }
        }

        if (nCompLen == 0) {
            if (keyId.startsWith("VK_") || keyId.startsWith("EK_") || (actionId != KEY_CLICK)) {
                super.onKeyEvent(keyId, actionId);
            }
        }
        return true;
    }


}
