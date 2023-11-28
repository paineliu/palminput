package com.palminput.keyboard.skin;

public class SkinGrad {
    private float postion;
    private int color;
    private int alpha;

    public SkinGrad(int color, int postion) {
        this.color = color;
        this.postion = postion;
    }


    public int getColor() {
        return (((100 - alpha) * 0xFF / 100) << 24) | color;
    }

    public void setColor(int color) {
        this.color = color;
    }

    public int getAlpha() {
        return alpha;
    }

    public void setAlpha(int alpha) {
        this.alpha = alpha;
    }

    public float getPostion() {
        return postion;
    }

    public void setPostion(float postion) {
        this.postion = postion;
    }
}
