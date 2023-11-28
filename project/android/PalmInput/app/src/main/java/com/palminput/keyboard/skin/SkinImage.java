package com.palminput.keyboard.skin;

import android.graphics.Rect;

public class SkinImage {
    private String imgFile;
    private Rect origRect;
    private Rect dispRect;

    public String getImgFile() {
        return imgFile;
    }

    public void setImgFile(String imgFile) {
        this.imgFile = imgFile;
    }

    public Rect getOrigRect() {
        return origRect;
    }

    public void setOrigRect(Rect origRect) {
        this.origRect = origRect;
    }

    public Rect getDispRect() {
        return dispRect;
    }

    public void setDispRect(Rect dispRect) {
        this.dispRect = dispRect;
    }
}
