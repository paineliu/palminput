package com.palminput.keyboard.skin;

import android.graphics.Rect;

public class SkinButton {
    public Rect getDispRect() {
        return dispRect;
    }

    public void setDispRect(Rect dispRect) {
        this.dispRect = dispRect;
    }

    public boolean isKeyRepeated() {
        return keyRepeated;
    }

    public void setKeyRepeated(boolean keyRepeated) {
        this.keyRepeated = keyRepeated;
    }

    private String id;

    private int styleId;
    private String showState;
    private Rect origRect;
    private Rect touchRect;
    private Rect realRect;
    private Rect dispRect;
    private String label;
    private String tag;
    private boolean keyRepeated;


    public Rect getOrigRect() {
        return origRect;
    }

    public void setOrigRect(Rect origRect) {
        this.origRect = origRect;
    }

    public String getLabel() {
        return label;
    }

    public void setLabel(String label) {
        this.label = label;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getShowState() {
        return showState;
    }

    public void setShowState(String showState) {
        this.showState = showState;
    }

    public int getStyleId() {
        return styleId;
    }

    public void setStyleId(int styleId) {
        this.styleId = styleId;
    }

    public Rect getTouchRect() {
        return touchRect;
    }

    public void setTouchRect(Rect touchRect) {
        this.touchRect = touchRect;
    }

    public Rect getRealRect() {
        return realRect;
    }

    public void setRealRect(Rect realRect) {
        this.realRect = realRect;
    }

    public String getTag() {
        return tag;
    }

    public void setTag(String tag) {
        this.tag = tag;
    }
}
