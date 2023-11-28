package com.palminput.keyboard.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.View;

public class MyPopupView extends View {
    Rect mTargetRect = new Rect(0, 0, 0, 0);
    Paint mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    String mDispString;

    public MyPopupView(Context context) {
        super(context);
    }

    public void update(String str) {
        mDispString = str;
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        mTargetRect.right = this.getWidth();
        mTargetRect.bottom = this.getHeight();

        mPaint.setStrokeWidth(3);
        mPaint.setColor(Color.DKGRAY);
        RectF rect = new RectF(0, 0, this.getWidth(), this.getHeight());
        //mPaint.setAlpha(192);
        canvas.drawRoundRect(rect, 7, 7, mPaint);

        mPaint.setColor(Color.WHITE);
        mPaint.setStrokeWidth(3);
        mPaint.setTextSize(80);
        Paint.FontMetricsInt fontMetrics = mPaint.getFontMetricsInt();
        int baseline = (mTargetRect.bottom + mTargetRect.top - fontMetrics.bottom - fontMetrics.top) / 2;
        mPaint.setTextAlign(Paint.Align.CENTER);
        canvas.drawText(mDispString, mTargetRect.centerX(), baseline, mPaint);
    }

//    @Override
//    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
//        int w = MeasureSpec.getSize(widthMeasureSpec);
//        int h = MeasureSpec.getSize(heightMeasureSpec);
//        int kbh = Math.min(w, h) / 3;
//        setMeasuredDimension(w,
//                kbh);
//    }
}
