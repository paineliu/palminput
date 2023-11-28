package com.palminput.keyboard.engine;

public class PYOptions {
    public int mFuzzyFlags;
    public boolean mPredictEnable;
    static public int PY_FUZZY_SM_Z_ZH = 0x00000001;
    static public int PY_FUZZY_SM_C_CH = 0x00000002;
    static public int PY_FUZZY_SM_S_SH = 0x00000004;
    static public int PY_FUZZY_SM_F_H = 0x00000008;
    static public int PY_FUZZY_SM_N_L = 0x00000010;
    static public int PY_FUZZY_SM_L_R = 0x00000020;
    static public int PY_FUZZY_YM_AN_ANG = 0x00000040;
    static public int PY_FUZZY_YM_EN_ENG = 0x00000080;
    static public int PY_FUZZY_YM_IN_ING = 0x00000100;
    static public int PY_FUZZY_YM_IAN_IANG = 0x00000200;
    static public int PY_FUZZY_YM_UAN_UANG = 0x00000400;
    static public int PY_FUZZY_QP_WANG_HUANG = 0x00000800;
    //static public int PY_FUZZY_QP_AOU_AOUXX = 0x00001000;
    static public int PY_FUZZY_JP_Z_ZH = 0x00002000;
    static public int PY_FUZZY_JP_C_CH = 0x00004000;
    static public int PY_FUZZY_JP_S_SH = 0x00008000;
    static public int PY_FUZZY_JP_ZH_Z = 0x00010000;
    static public int PY_FUZZY_JP_CH_C = 0x00020000;
    static public int PY_FUZZY_JP_SH_S = 0x00040000;
    static public int PY_FUZZY_JP_F_H = 0x00080000;
    static public int PY_FUZZY_JP_N_L = 0x00100000;
    static public int PY_FUZZY_JP_L_R = 0x00200000;

    static public int PY_FUZZY_F_H = PY_FUZZY_JP_F_H | PY_FUZZY_SM_F_H;
    static public int PY_FUZZY_N_L = PY_FUZZY_JP_N_L | PY_FUZZY_SM_N_L;
    static public int PY_FUZZY_L_R = PY_FUZZY_JP_L_R | PY_FUZZY_SM_L_R;
    static public int PY_FUZZY_Z_ZH =
            PY_FUZZY_SM_Z_ZH | PY_FUZZY_JP_Z_ZH | PY_FUZZY_JP_ZH_Z;
    static public int PY_FUZZY_C_CH =
            PY_FUZZY_SM_C_CH | PY_FUZZY_JP_C_CH | PY_FUZZY_JP_CH_C;
    static public int PY_FUZZY_S_SH =
            PY_FUZZY_SM_S_SH | PY_FUZZY_JP_S_SH | PY_FUZZY_JP_SH_S;

    public PYOptions(int fuzzyFlags, boolean predictEnable){
        this.mFuzzyFlags = fuzzyFlags;
        this.mPredictEnable = predictEnable;
    }

}
