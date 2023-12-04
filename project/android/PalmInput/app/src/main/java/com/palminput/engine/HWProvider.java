package com.palminput.engine;

import android.content.res.AssetManager;

public class HWProvider {

    public native int create(AssetManager assetManager, String dictPathName, String userPathName);
    public native void reset();

    public native int addStroke(int[] strokes);
    public native int delLastStroke();
    public native int getStrokeTotal();

    public native int getCandItemTotal();
    public native String getCandItemString(int id);

    public native void destroy();

    static {
        System.loadLibrary("palmengine-lib");
    }
}
