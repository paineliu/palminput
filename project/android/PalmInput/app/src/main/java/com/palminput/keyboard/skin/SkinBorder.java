package com.palminput.keyboard.skin;

public class SkinBorder {
    private SkinColor color;
    private int width;
    private int rx;
    private int ry;

    public SkinBorder(SkinColor color) {
        this.color = color;
        this.width = 1;
        this.rx = 0;
        this.ry = 0;
    }


    public SkinColor getColor() {
        return color;
    }

    public void setColor(SkinColor color) {
        this.color = color;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getRx() {
        return rx;
    }

    public void setRx(int rx) {
        this.rx = rx;
    }

    public int getRy() {
        return ry;
    }

    public void setRy(int ry) {
        this.ry = ry;
    }
}
