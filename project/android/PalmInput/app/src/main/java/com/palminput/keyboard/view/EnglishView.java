package com.palminput.keyboard.view;

import android.content.Context;

import com.palminput.keyboard.InputMgrView;


public class EnglishView extends KBBaseView {

    public EnglishView(Context context) {
        super(context);
    }

    public boolean onKeyEvent(String id) {
        InputMgrView v1 = (InputMgrView) this.getParent();
        switch (id) {
            case "VK_BACKSPACE":
                mInputService.deleteTextBeforeCursor(1);
                break;
            case "VK_SPACE":
                mInputService.sendCommitText(" ", 1);
                break;
            case "VK_SHIFT":
                mShiftMode += 1;
                if (mShiftMode >=3) {
                    mShiftMode = 0;
                }
                invalidate();
                break;
        }

        if (id.length() == 1) {
            mInputService.sendCommitText(getDisplayString(id), 1);
            if (mShiftMode == 1)
            {
                mShiftMode = 0;
                invalidate();
            }
        }
        return true;
    }


}
