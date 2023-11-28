package com.palminput.keyboard.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.View;

public class CompView extends View {
    Rect mTargetRect = new Rect(0, 0, 0, 0);
    String mCompString;
    int mBgColor;
    int mTextColor;

    public CompView(Context context) {
        super(context);
    }

    public void updateComp(String str) {
        mCompString = str;
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        mTargetRect.right = this.getWidth();
        mTargetRect.bottom = this.getHeight();
        drawLabel(canvas, mTargetRect, 0, Color.WHITE, 50, mCompString);
    }

    public void setCompColor(int bgColor, int textColor)
    {
        mBgColor = bgColor;
        mTextColor = textColor;
    }

    private void drawLabel(Canvas canvas, Rect rect, int align, int color, int size, String text)
    {
        Paint bkPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        Rect minRect = new Rect();

        bkPaint.setTextSize(size);
        Paint.FontMetricsInt fontMetrics = bkPaint.getFontMetricsInt();
        bkPaint.getTextBounds(text,0, text.length(), minRect);

        bkPaint.setColor(mBgColor);
        //bkPaint.setAlpha(128);
        Rect bkRect = new Rect();
        bkRect.left = 0;
        bkRect.top = 0;
        bkRect.bottom = rect.bottom;
        bkRect.right = minRect.right + 30;
        canvas.drawRect(bkRect, bkPaint);

        bkPaint.setColor(mTextColor);

        int middleY = rect.top + rect.height() / 2 + (fontMetrics.bottom - fontMetrics.top) / 2 - fontMetrics.bottom;
        int leftX = rect.left + 15;
        canvas.drawText(text, leftX, middleY, bkPaint);
    }
}
