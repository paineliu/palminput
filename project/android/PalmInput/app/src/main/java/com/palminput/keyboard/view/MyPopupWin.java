package com.palminput.keyboard.view;

import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.widget.PopupWindow;

public class MyPopupWin {

	private final Context mContext;
	private final PopupWindow mWin;
    private final MyPopupView mView;
    private final int mWidth;
	private final int mHeight;
	private int mShowX;
	private int mShowY;
	private int mPointerId = -1;
	private boolean mNeedShow;
	View mParent;
	public MyPopupWin(Context context){
		mContext = context;
		mWidth = 140;
		mHeight = 200;

		mView = new MyPopupView(mContext);
		mWin = new PopupWindow(mView, mWidth, mHeight, true);

		mWin.setTouchable(true);
		mWin.setFocusable(false);
		mWin.setBackgroundDrawable(null);
		mWin.setOutsideTouchable(false);
		mWin.setClippingEnabled(false);
    }
	public boolean isShow() {
		return (mWin != null && mWin.isShowing());
	}

	public int getPointerId() {
		return mPointerId;
	}

	public boolean isNeedShow() {
		return mNeedShow;
	}

	public void setNeedShow(boolean needShow) {
		mNeedShow = needShow;
	}

    public void update(int dx, int dy, String str)
	{
		try {
			if (mWin.isShowing()){
				mView.update(str);
				mWin.update(mShowX - mWidth / 2 + dx, mShowY - mHeight + dy, mWidth, mHeight);
			} else {
				mView.update(str);
				mWin.showAtLocation(mParent, Gravity.NO_GRAVITY,mShowX - mWidth / 2 + dx, mShowY - mHeight + dy);

			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void prepareShow(View parent, int x, int y, int pointerId, String str) {
		hide();
		try {
			mNeedShow = true;
			mShowX = x;
			mShowY = y;
			mPointerId = pointerId;
			mParent = parent;
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
