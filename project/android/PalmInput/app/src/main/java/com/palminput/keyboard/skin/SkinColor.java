package com.palminput.keyboard.skin;

public class SkinColor {
    private int rgb;
    private int alpha;

    public SkinColor(int rgb, int alpha) {
        this.rgb = rgb;
        this.alpha = alpha;
    }

    public int getARGB() {
        return (((100 - alpha) * 0xFF / 100) << 24) | rgb;
    }


    public int getRgb() {
        return rgb;
    }

    public void setRgb(int rgb) {
        this.rgb = rgb;
    }


    public int getAlpha() {
        return alpha;
    }

    public void setAlpha(int alpha) {
        this.alpha = alpha;
    }
}
