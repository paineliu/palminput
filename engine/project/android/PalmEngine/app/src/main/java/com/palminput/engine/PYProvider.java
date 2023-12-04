package com.palminput.engine;
import android.content.res.AssetManager;

public class PYProvider {

    public native int create(AssetManager assetManager, String dictPathName, String userPathName);
    public native void reset();
    public native int save();

    public native boolean setOptions(PYOptions options);
    public native PYOptions getOptions();

    public native int addKey(int key);

    public native String getCompString();
    public native String getCommitString();

    public native int getCandItemTotal();
    public native String getCandItemString(int id);
    public native int selCandItem(int id);

    public native void destroy();

    static {
        System.loadLibrary("palmengine-lib");
    }
}
