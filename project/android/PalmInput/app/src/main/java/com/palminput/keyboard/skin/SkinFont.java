package com.palminput.keyboard.skin;

public class SkinFont {
    private int size;
    private SkinColor color;
    private double ratio;

    public SkinFont(SkinColor color, int size) {
        this.color = color;
        this.size = size;
        this.ratio = 1.0;
    }

    public int getSize() {
        return (int)(size * ratio);
    }

    public void setSize(int size) {
        this.size = size;
    }

    public SkinColor getColor() {
        return color;
    }

    public void setColor(SkinColor color) {
        this.color = color;
    }

    public double getRatio() {
        return ratio;
    }

    public void setRatio(double ratio) {
        this.ratio = ratio;
    }
}
