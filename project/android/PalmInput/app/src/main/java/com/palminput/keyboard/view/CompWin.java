package com.palminput.keyboard.view;

import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.widget.PopupWindow;

public class CompWin {

	private final Context mContext;
	private final PopupWindow mWin;
    private final CompView mView;

	public CompWin(Context context){
		mContext = context;
        mView = new CompView(mContext);
		mWin = new PopupWindow(mView, 1080, 100, true);

		mWin.setTouchable(true);
		mWin.setFocusable(false);
		mWin.setBackgroundDrawable(null);
		mWin.setOutsideTouchable(false);
		mWin.setClippingEnabled(false);
    }

    public void setCompColor(int bgColor, int textColor)
	{
		mView.setCompColor(bgColor, textColor);
	}

    public void updateComp(String str)
	{
		mView.updateComp(str);

	}
    public void update(int x, int y)
	{
		try {
			mWin.update(x, y - 100, 200, 200);

		}
		catch (Exception e) {
			e.printStackTrace();
		}

	}
	public void show(View parent, int x, int y) {


		try {
            mWin.showAtLocation(parent, Gravity.NO_GRAVITY,x, y - 100);

		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	

	public void hide() {
		try {
			if(mWin != null && mWin.isShowing()) {
				mWin.dismiss();
			}
		} catch (Exception e) {
            e.printStackTrace();
		}
	}

}
