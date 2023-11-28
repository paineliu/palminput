package com.palminput.keyboard;

import android.content.Context;
import android.content.SharedPreferences;

public class PalmInputPreferences {

    public static final String PREFS_NAME = "PalmInputPrefs";

    private final Context mContext;

    public PalmInputPreferences(Context context) {
        mContext = context;
    }


    public void setChineseInputMethod(int id) {
        // set preference
        SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt("chineseInputMethodId", id);
        editor.commit();
    }

    public int getChineseInputMethod() {
        // get preference
        SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME, 0);
        int id = settings.getInt("chineseInputMethodId", InputMgrView.INPUT_METHOD_PINYIN);
        return id;
    }

    public void setSymbolLocked(boolean islocked) {
        // set preference
        SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean("symbolLocked", islocked);
        editor.commit();
    }

    public boolean getSymbolLocked() {
        // get preference
        SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME, 0);
        return settings.getBoolean("symbolLocked", false);

    }
}
