package com.palminput.keyboard;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Point;
import android.inputmethodservice.InputMethodService;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;

public class PalmInputManager {
	
	PalmInputService mInputService = null;
	InputMgrView mInputMgrView = null;
    boolean isExtraEditTextMultilineMode;
    int screenWidth, screenHeight;

	public PalmInputManager(InputMethodService inputService) {
		mInputService = (PalmInputService) inputService;

		WindowManager manager = (WindowManager) inputService.getSystemService(Context.WINDOW_SERVICE);
        Point outSize = new Point();
        manager.getDefaultDisplay().getSize(outSize);
        screenWidth = outSize.x;
		screenHeight = outSize.y;
		isExtraEditTextMultilineMode = isExtraEditTextMultilineMode();
	}
	
	public void onCreate() {
		
	}
	
	public View onCreateInputView() {
		mInputMgrView = new InputMgrView(mInputService);
		return mInputMgrView;
	}

	public View onCreateExtractTextView() {
	    View extraTextView = null;
//	    if(isExtraEditTextMultilineMode){
//            extraTextView = LayoutInflater.from(mInputService).inflate(R.layout.custom_extract_multiline_text_view, null);
//        }else{
//	        extraTextView = LayoutInflater.from(mInputService).inflate(R.layout.custom_extract_text_view, null);
//        }
	    return extraTextView;
	}

    private boolean isExtraEditTextMultilineMode(){
        int keyboardHeight = mInputService.getKeyboardHeight();
        int extraEditTextHeight = mInputService.getResources().getDimensionPixelSize(R.dimen.extra_edit_text_size);

        return Math.abs(screenHeight - keyboardHeight) >= extraEditTextHeight * 2;
    }

	public View onCreateCandidatesView() {
		return null;
	}
	
	public void onStartInputView(EditorInfo info, boolean restarting) {

		int classInfo = info.inputType & (EditorInfo.TYPE_MASK_CLASS | EditorInfo.TYPE_MASK_VARIATION);
		int actionInfo = EditorInfo.IME_ACTION_NONE;

		if ((info.imeOptions & EditorInfo.IME_FLAG_NO_ENTER_ACTION) == 0) {
			actionInfo = info.imeOptions & EditorInfo.IME_MASK_ACTION;
		}
		mInputMgrView.onShow(classInfo, actionInfo);

		if (false) {
			WindowManager.LayoutParams lp = mInputService
					.getWindow().getWindow().getAttributes();

			Resources resources = mInputService.getResources();
			int resIdShow = resources.getIdentifier("config_showNavigationBar", "bool", "android");
			boolean hasNavigationBar = false;
			int navigationbarHeight = 0;

			if(resIdShow > 0){
				hasNavigationBar = resources.getBoolean(resIdShow);
			}

			if(hasNavigationBar){
				int resIdNavigationBar = resources.getIdentifier("navigation_bar_height", "dimen", "android");
				if(resIdNavigationBar > 0){
					navigationbarHeight = resources.getDimensionPixelSize(resIdNavigationBar);//navigationBar高度
				}
			}

			lp.alpha = 0.5f; //0.0-1.0
			lp.x = 10 - (mInputService.getMaxWidth() - mInputService.getKeyboardWidth()) / 2;
			lp.y = 10 + navigationbarHeight;
			lp.width = mInputService.getKeyboardWidth();
			lp.height = mInputService.getKeyboardHeight() + lp.gravity - 500;

			mInputService.getWindow().getWindow().setAttributes(lp);
		}
	}

	public void onFinishInput() {

	}

	public void onFinishInputView(boolean finishingInput) {
		mInputMgrView.onHide(finishingInput);
	}
	
	public void onDestroy() {

	}

}
