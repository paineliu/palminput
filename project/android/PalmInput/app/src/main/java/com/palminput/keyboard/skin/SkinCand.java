package com.palminput.keyboard.skin;

import android.graphics.Rect;

public class SkinCand {
    public Rect getDispRect() {
        return dispRect;
    }

    public void setDispRect(Rect dispRect) {
        this.dispRect = dispRect;
    }

    public int getMaxRow() {
        return maxRow;
    }

    public void setMaxRow(int maxRow) {
        this.maxRow = maxRow;
    }

    public String getImgFile() {
        return imgFile;
    }

    public void setImgFile(String imgFile) {
        this.imgFile = imgFile;
    }

    private SkinFont font;
    private boolean autoHide;

    private Rect origRect;
    private Rect dispRect;
    private int maxRow;
    private int maxCol;
    private String imgItemNormal;
    private String imgItemPress;
    private String imgFile;

    public String getImgItemNormal() {
        return imgItemNormal;
    }

    public void setImgItemNormal(String imgItemNormal) {
        this.imgItemNormal = imgItemNormal;
    }

    public String getImgItemPress() {
        return imgItemPress;
    }

    public void setImgItemPress(String imgItemPress) {
        this.imgItemPress = imgItemPress;
    }


    public int getMaxCol() {
        return maxCol;
    }

    public void setMaxCol(int maxCol) {
        this.maxCol = maxCol;
    }

    public Rect getOrigRect() {
        return origRect;
    }

    public void setOrigRect(Rect origRect) {
        this.origRect = origRect;
    }

    public SkinFont getFont() {
        return font;
    }

    public void setFont(SkinFont font) {
        this.font = font;
    }

    public boolean isAutoHide() {
        return autoHide;
    }

    public void setAutoHide(boolean autoHide) {
        this.autoHide = autoHide;
    }
}
