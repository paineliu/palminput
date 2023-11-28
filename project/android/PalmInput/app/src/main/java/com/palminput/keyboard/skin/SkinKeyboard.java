package com.palminput.keyboard.skin;

import android.graphics.Rect;

import java.util.ArrayList;
import java.util.List;

public class SkinKeyboard {

    public SkinCand getCand() {
        return cand;
    }

    public void setCand(SkinCand cand) {
        this.cand = cand;
    }

    public List<SkinButton> getButtons() {
        return buttons;
    }

    public void setButtons(List<SkinButton> buttons) {
        this.buttons = buttons;
    }

    public Rect getOrigRect() {
        return origRect;
    }

    public Rect getDispRect() {
        return dispRect;
    }

    public void setOrigRect(Rect origRect) {
        this.origRect = origRect;
    }

    private String bgImg;
    private SkinColor bgColor;
    private SkinColor candBgColor;
    private SkinColor candTextColor;
    private SkinColor compBgColor;
    private SkinColor compTextColor;

    private SkinCand cand;

    private List<SkinButtonStyle> buttonStyles = new ArrayList<SkinButtonStyle>();
    private List<SkinButton> buttons = new ArrayList<SkinButton>();
    private List<SkinImage> images = new ArrayList<SkinImage>();

    private Rect origRect;
    private Rect dispRect;
    private float radio;
    private float radioX;
    private float radioY;

    public void resize(Rect rect) {

        radioX = (float) rect.width() / origRect.width();
        radioY = (float) rect.height() / origRect.height();
        radio = Math.min(radioX, radioY);

        if (dispRect != rect && origRect != null) {
            dispRect = rect;
            if (cand != null) {
                cand.setDispRect(new Rect(cand.getOrigRect().left * rect.width() / origRect.width(),
                        cand.getOrigRect().top * rect.height() / origRect.height(),
                        cand.getOrigRect().right * rect.width() / origRect.width(),
                        cand.getOrigRect().bottom * rect.height() / origRect.height()));
                cand.getFont().setRatio(radio);
            }

            List<SkinButton> buttons = getButtons();

            for (SkinButton b : buttons) {
                b.setDispRect(new Rect(b.getOrigRect().left * rect.width() / origRect.width(),
                        b.getOrigRect().top * rect.height() / origRect.height(),
                        b.getOrigRect().right * rect.width() / origRect.width(),
                        b.getOrigRect().bottom * rect.height() / origRect.height()));

                if (b.getTouchRect() != null) {
                    b.setRealRect(new Rect(b.getTouchRect().left * rect.width() / origRect.width(),
                            b.getTouchRect().top * rect.height() / origRect.height(),
                            b.getTouchRect().right * rect.width() / origRect.width(),
                            b.getTouchRect().bottom * rect.height() / origRect.height()));
                } else {
                    b.setRealRect(new Rect(b.getOrigRect().left * rect.width() / origRect.width(),
                            b.getOrigRect().top * rect.height() / origRect.height(),
                            b.getOrigRect().right * rect.width() / origRect.width(),
                            b.getOrigRect().bottom * rect.height() / origRect.height()));
                }
            }

            List<SkinButtonStyle> styles = getButtonStyles();
            for (SkinButtonStyle s : styles) {
                if(s.getNormal().getLabelFont() != null) {
                    s.getNormal().getLabelFont().setRatio(radio);
                    s.getNormal().getTagFont().setRatio(radio);
                    s.getPress().getLabelFont().setRatio(radio);
                    s.getPress().getTagFont().setRatio(radio);
                    s.makeResource();
                }
            }

            List<SkinImage> images = getImages();
            for (SkinImage img : images) {
                img.setDispRect(new Rect(img.getOrigRect().left * rect.width() / origRect.width(),
                        img.getOrigRect().top * rect.height() / origRect.height(),
                        img.getOrigRect().right * rect.width() / origRect.width(),
                        img.getOrigRect().bottom * rect.height() / origRect.height()));
            }
        }
    }

    public List<SkinImage> getImages() {
        return images;
    }

    public void setImages(List<SkinImage> images) {
        this.images = images;
    }

    public List<SkinButtonStyle> getButtonStyles() {
        return buttonStyles;
    }

    public void setButtonStyles(List<SkinButtonStyle> buttonStyles) {
        this.buttonStyles = buttonStyles;
    }

    public String getBgImg() {
        return bgImg;
    }

    public void setBgImg(String bgImg) {
        this.bgImg = bgImg;
    }

    public SkinColor getBgColor() {
        return bgColor;
    }

    public void setBgColor(SkinColor bgColor) {
        this.bgColor = bgColor;
    }

    public SkinColor getCandBgColor() {
        return candBgColor;
    }

    public void setCandBgColor(SkinColor candBgColor) {
        this.candBgColor = candBgColor;
    }

    public SkinColor getCandTextColor() {
        return candTextColor;
    }

    public void setCandTextColor(SkinColor candTextColor) {
        this.candTextColor = candTextColor;
    }

    public SkinColor getCompBgColor() {
        return compBgColor;
    }

    public void setCompBgColor(SkinColor compBgColor) {
        this.compBgColor = compBgColor;
    }

    public SkinColor getCompTextColor() {
        return compTextColor;
    }

    public void setCompTextColor(SkinColor compTextColor) {
        this.compTextColor = compTextColor;
    }
}
