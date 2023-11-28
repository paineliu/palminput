package com.palminput.keyboard.skin;

public class SkinShadow {
    private SkinColor color;
    private int radius;
    private int dx;
    private int dy;

    public SkinShadow(int SkinColor) {
        this.color = color;
        this.radius = 1;
        this.dx = 0;
        this.dy = 0;
    }

    public SkinColor getColor() {
        return color;
    }

    public void setColor(SkinColor color) {
        this.color = color;
    }

    public int getRadius() {
        return radius;
    }

    public void setRadius(int radius) {
        this.radius = radius;
    }

    public int getDx() {
        return dx;
    }

    public void setDx(int dx) {
        this.dx = dx;
    }

    public int getDy() {
        return dy;
    }

    public void setDy(int dy) {
        this.dy = dy;
    }
}
