package com.palminput.keyboard.skin;

import android.graphics.Paint;
import android.graphics.Rect;

import java.util.ArrayList;
import java.util.List;

public class SkinButtonStyle {

    private String id;
    private String imgPrefix;

    private SkinButtonStyleState normal;
    private SkinButtonStyleState press;


    public String getNormalImgFile() {
        return imgPrefix + "normal.png";
    }

    public String getPressImgFile() {
        return imgPrefix + "press.png";
    }
    public String getShiftImgFile() {
        return imgPrefix + "shift.png";
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public void setImgPrefix(String imgPrefix) {
        this.imgPrefix = imgPrefix;
    }


    public void makeResource() {

        normal.makeResource();
        press.makeResource();
    }

    public SkinButtonStyleState getNormal() {
        return normal;
    }

    public void setNormal(SkinButtonStyleState normal) {
        this.normal = normal;
    }

    public SkinButtonStyleState getPress() {
        return press;
    }

    public void setPress(SkinButtonStyleState press) {
        this.press = press;
    }
}
