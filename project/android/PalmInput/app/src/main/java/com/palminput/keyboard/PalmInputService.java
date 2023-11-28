package com.palminput.keyboard;

import android.graphics.Point;
import android.graphics.Rect;
import android.inputmethodservice.InputMethodService;
import android.os.SystemClock;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowManager;
import android.view.WindowMetrics;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.widget.RelativeLayout;

public class PalmInputService extends InputMethodService {
    PalmInputManager mInputManager;
    int mActionInfo;
    RelativeLayout mRL;

    private static final String[] pairSymbol = {"()", "[]", "{}", "<>", "“”", "‘’", "（）", "〔〕", "［］", "｛｝", "《》", "〈〉", "『』", "「」", "【】", "〖〗"};

    @Override
    public void onCreate() {
        super.onCreate();
        mInputManager = new PalmInputManager(this);
        mInputManager.onCreate();
    }
    public View onCreateInputView(){
        View view = mInputManager.onCreateInputView();
        mRL = new RelativeLayout(this);
        RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,RelativeLayout.LayoutParams.MATCH_PARENT);
        rlp.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

        mRL.addView(view,rlp);

        return mRL;
    }

    @Override
    public View onCreateCandidatesView() {
        return mInputManager.onCreateCandidatesView();
    }

    @Override
    public View onCreateExtractTextView() {
        View extraTextView = mInputManager.onCreateExtractTextView();
        return null == extraTextView ? super.onCreateExtractTextView() : extraTextView;
    }

    @Override
    public void onComputeInsets(Insets outInsets) {
        super.onComputeInsets(outInsets);
        int top;
        if (mRL != null) {
            top = mRL.getHeight() - getKeyboardHeight();
        } else {
            top = getWindowHeight() - getKeyboardHeight();
        }

        outInsets.contentTopInsets = outInsets.visibleTopInsets = top;
    }

    public int getKeyboardWidth() {
        return getMaxWidth();
    }

    private int getWindowHeight()
    {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.R)
        {
            WindowManager windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
            final WindowMetrics metrics = windowManager.getCurrentWindowMetrics();
            final WindowInsets windowInsets = metrics.getWindowInsets();
            android.graphics.Insets insets = windowInsets.getInsetsIgnoringVisibility(WindowInsets.Type.navigationBars() | WindowInsets.Type.displayCutout());
            final Rect bounds = metrics.getBounds();
            return bounds.bottom - insets.top - insets.bottom;
        }
        else
        {
            Display defaultDisplay = getDisplay();
            Point s = new Point();
            defaultDisplay.getRealSize(s);
            Point point = new Point();
            defaultDisplay.getSize(point);
            int x = point.x;
            int y = point.y;
            return y;
        }
    }

    public int getKeyboardHeight() {
        int keyboardMaxHeight = getWindowHeight() / 2;
        int keyboardheight = getMaxWidth() * 7 / 10;
        if (keyboardheight > keyboardMaxHeight) {
            keyboardheight = keyboardMaxHeight;
        }
        keyboardheight -= keyboardheight % 4;
        return keyboardheight;
    }

    @Override
    public void onStartInputView(EditorInfo info, boolean restarting) {
        super.onStartInputView(info, restarting);
        mActionInfo = getImeOptionsActionIdFromEditorInfo(info);
        mInputManager.onStartInputView(info, restarting);
    }

    @Override
    public void onFinishInput() {
        mInputManager.onFinishInput();
        super.onFinishInput();
    }

    @Override
    public void onFinishInputView(boolean finishingInput) {
        mInputManager.onFinishInputView(finishingInput);
        super.onFinishInputView(finishingInput);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        return super.onKeyUp(keyCode, event);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mInputManager.onDestroy();
    }

    public void hideKeyboard() {
        this.requestHideSelf(0);
    }

    public boolean deleteTextBeforeCursor(int n) {
        InputConnection mInputConnection = this.getCurrentInputConnection();
        if (mInputConnection != null) {
            return mInputConnection.deleteSurroundingText(n, 0);
        }
        return false;
    }

    public String getFinishButtonLabel() {
        String strText;
        int actionInfo = mActionInfo;

        switch (actionInfo){
            case EditorInfo.IME_ACTION_NONE:
                strText = this.getString(R.string.text_ime_action_none);
                break;
            case EditorInfo.IME_ACTION_GO:
                strText = this.getString(R.string.text_ime_action_go);
                break;
            case EditorInfo.IME_ACTION_SEARCH:
                strText = this.getString(R.string.text_ime_action_search);
                break;
            case EditorInfo.IME_ACTION_SEND:
                strText = this.getString(R.string.text_ime_action_send);
                break;
            case EditorInfo.IME_ACTION_NEXT:
                strText = this.getString(R.string.text_ime_action_next);
                break;
            case EditorInfo.IME_ACTION_DONE:
                strText = this.getString(R.string.text_ime_action_done);
                break;
            case EditorInfo.IME_ACTION_PREVIOUS:
                strText = this.getString(R.string.text_ime_action_previous);
                break;
            default:
                strText = this.getString(R.string.text_ime_action_default);
                break;
        }

        return strText;
    }

    public int getImeOptionsActionIdFromEditorInfo(final EditorInfo editorInfo) {
        if (editorInfo.actionId != 0) {
            return editorInfo.actionId;
        }
        else if ((editorInfo.imeOptions & EditorInfo.IME_FLAG_NO_ENTER_ACTION) != 0) {
            return EditorInfo.IME_ACTION_NONE;
        } else {
            return editorInfo.imeOptions & EditorInfo.IME_MASK_ACTION;
        }
    }

    public static boolean isPairSymbol(String sym) {
        if (sym != null && sym.length() == 2) {
            for (String s : pairSymbol) {
                if (sym.equals(s)) {
                    return true;
                }
            }
        }
        return false;
    }


    public boolean sendCommitText(CharSequence text, int newCursorPosition) {
        if (text == null || text.length() == 0) {
            return false;
        }

        InputConnection inputConnection = this.getCurrentInputConnection();

        if (text.equals("\n")) {
            if (inputConnection != null) {
                EditorInfo editorInfo = getCurrentInputEditorInfo();
                if (editorInfo.actionId != 0) {
                    return inputConnection.performEditorAction(editorInfo.actionId);
                }
                else if (editorInfo.imeOptions != 0) {
                    int action = getImeOptionsActionIdFromEditorInfo(editorInfo);
                    if (action != EditorInfo.IME_ACTION_NONE) {
                        inputConnection.performEditorAction(action);
                    } else {
                        inputConnection.commitText("\n", newCursorPosition);
                    }
                    return true;
                }
                else {
                    this.hideKeyboard();
                    return true;
                }
            }
        }
        else {
            if (inputConnection != null) {
                if (isPairSymbol(text.toString())) {
                    inputConnection.commitText(text, newCursorPosition);

                    int keyCode = KeyEvent.KEYCODE_DPAD_LEFT;
                    long uptime = SystemClock.uptimeMillis();
                    KeyEvent keyDownEvent = new KeyEvent(uptime, uptime,
                            KeyEvent.ACTION_DOWN, keyCode, 0, 0, 0, 0,
                            KeyEvent.FLAG_SOFT_KEYBOARD
                                    | KeyEvent.FLAG_KEEP_TOUCH_MODE
                                    | KeyEvent.FLAG_EDITOR_ACTION);
                    KeyEvent keyUpEvent = new KeyEvent(uptime, uptime, KeyEvent.ACTION_UP,
                            keyCode, 0, 0, 0, 0, KeyEvent.FLAG_SOFT_KEYBOARD
                            | KeyEvent.FLAG_KEEP_TOUCH_MODE
                            | KeyEvent.FLAG_EDITOR_ACTION);

                    inputConnection.sendKeyEvent(keyDownEvent);
                    inputConnection.sendKeyEvent(keyUpEvent);
                    return true;
                }

                return inputConnection.commitText(text, newCursorPosition);
            }
        }

        return false;
    }
}
