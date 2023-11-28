package com.palminput.keyboard.skin;

import android.graphics.Paint;
import android.graphics.Rect;

import java.util.ArrayList;
import java.util.List;

public class SkinButtonStyleState {

    private SkinButtonPaint skinPaint;

    private SkinColor bgColor;

    private SkinFont labelFont;
    private SkinFont tagFont;

    private Rect btnPadding;

    private SkinColor btnColor;
    private SkinBorder btnBorder;
    private SkinShadow btnShadow;
    private final List<SkinGrad> btnGrad = new ArrayList<SkinGrad>();


    public SkinColor getBgColor() {
        return bgColor;
    }

    public void setBgColor(SkinColor bgColor) {
        this.bgColor = bgColor;
    }

    public Rect getBtnPadding() {
        return btnPadding;
    }

    public void setBtnPadding(Rect btnPadding) {
        this.btnPadding = btnPadding;
    }

    public SkinColor getBtnColor() {
        return btnColor;
    }

    public void setBtnColor(SkinColor btnColor) {
        this.btnColor = btnColor;
    }

    public SkinBorder getBtnBorder() {
        return btnBorder;
    }

    public void setBtnBorder(SkinBorder btnBorder) {
        this.btnBorder = btnBorder;
    }

    public SkinShadow getBtnShadow() {
        return btnShadow;
    }

    public void setBtnShadow(SkinShadow btnShadow) {
        this.btnShadow = btnShadow;
    }

    public SkinFont getLabelFont() {
        return labelFont;
    }

    public void setLabelFont(SkinFont labelFont) {
        this.labelFont = labelFont;
    }

    public SkinButtonPaint getSkinPaint() {
        return skinPaint;
    }


    public SkinFont getTagFont() {
        return tagFont;
    }

    public void setTagFont(SkinFont tagFont) {
        this.tagFont = tagFont;
    }

    public void makeResource() {

        skinPaint = new SkinButtonPaint();

        if (getBgColor() != null) {
            Paint paintBg = new Paint();
            paintBg.setAntiAlias(true);
            paintBg.setColor(getBgColor().getARGB());
            if (skinPaint != null) {
                skinPaint.setPaintBg(paintBg);
            }
        }

        if (getBtnShadow() != null)
        {
            Paint paintShadow = new Paint();
            paintShadow.setAntiAlias(true);
            paintShadow.setColor(getBtnColor().getARGB());
            paintShadow.setShadowLayer(getBtnShadow().getRadius(), getBtnShadow().getDx(), getBtnShadow().getDy(), getBtnShadow().getColor().getARGB());
            if (skinPaint != null) {
                skinPaint.setPaintShadow(paintShadow);
            }
        }

        if (getBtnBorder() != null)
        {
            Paint paintBorder = new Paint();
            paintBorder.setAntiAlias(true);
            paintBorder.setColor(getBtnBorder().getColor().getARGB());
            paintBorder.setStyle(Paint.Style.STROKE);
            paintBorder.setStrokeWidth(getBtnBorder().getWidth());
            if (skinPaint != null) {
                skinPaint.setPaintBorder(paintBorder);
            }
        }

        if (getBtnColor() != null)
        {
            Paint paintFg = new Paint();
            paintFg.setAntiAlias(true);
            paintFg.setColor(getBtnColor().getARGB());
            if (skinPaint != null) {
                skinPaint.setPaintFg(paintFg);
            }
        }

//        if (getBmGrad() != null)
//        {
//            int [] colors = new int[getBmGrad().size()];
//            float [] postions = new float[getBmGrad().size()];
//            for(int i=0; i<getBmGrad().size(); i++)
//            {
//                colors[i] = getBmGrad().get(i).getColor();
//                postions[i] = getBmGrad().get(i).getPostion();
//            }
//        LinearGradient linearGradient = new LinearGradient(button.getDispRect().left+20, button.getDispRect().top+20, button.getDispRect().right-40, button.getDispRect().bottom-40,
//                colors,position, Shader.TileMode.CLAMP);
//
//        paint2.setShader(linearGradient);
//        paint2.setStyle(Paint.Style.FILL);
//        }
    }

}
