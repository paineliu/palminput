package com.palminput.keyboard;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.graphics.Rect;
import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;

import com.google.gson.Gson;
import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;
import com.palminput.keyboard.view.EnglishView;
import com.palminput.keyboard.view.HandView;
import com.palminput.keyboard.view.IKeyboard;
import com.palminput.keyboard.view.IOperater;
import com.palminput.keyboard.view.PinyinView;
import com.palminput.keyboard.view.SymbolView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Locale;


public class InputMgrView extends ViewGroup implements IOperater {


    public static final int INPUT_METHOD_CHINESE = 3;
    public static final int INPUT_METHOD_PINYIN = 1;
    public static final int INPUT_METHOD_HANDWRITE = 2;
    public static final int INPUT_METHOD_ENGLISH = 4;
    public static final int INPUT_METHOD_NUMBER = 5;
    public static final int INPUT_METHOD_SYMBOL_CN = 6;
    public static final int INPUT_METHOD_SYMBOL_CN_MORE =7;
    public static final int INPUT_METHOD_SYMBOL_EN = 8;
    public static final int INPUT_METHOD_SYMBOL_EN_MORE = 9;
    public static final int INPUT_METHOD_PASSWORD = 10;
    public static final int INPUT_METHOD_PHONENUM = 11;
    public static final int INPUT_METHOD_SYMBOL_NUMBER = 12;


    private PalmInputService  mInputService = null;

    private  IKeyboard mHandView;
    private  IKeyboard mPinyinView;
    private  IKeyboard mEnglishView;
    private  IKeyboard mNumberView;
    private  IKeyboard mSymbolViewNumber;
    private  IKeyboard mSymbolViewCn;
    private  IKeyboard mSymbolViewCnMore;
    private  IKeyboard mSymbolViewEn;
    private  IKeyboard mSymbolViewEnMore;

    private IKeyboard mCurrentKeyboard;
    private IKeyboard mCurrentKeyboardMain;
    private IKeyboard mCurrentKeyboardCn;

    private SkinResPool mResPool;
    private Gson gson = new Gson();
    private String mSkinPath;

    public static String getJson(String fileName,Context context) {

        StringBuilder stringBuilder = new StringBuilder();
        try {
            AssetManager assetManager = context.getAssets();
            BufferedReader bf = new BufferedReader(new InputStreamReader(
                    assetManager.open(fileName)));
            String line;
            while ((line = bf.readLine()) != null) {
                stringBuilder.append(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return stringBuilder.toString();
    }

    public static boolean isChineseLanguage() {
        String language = Locale.getDefault().getLanguage();
        return TextUtils.equals(language, Locale.CHINA.getLanguage());
    }

    public SkinKeyboard createSkin(Rect kbRect, String fileName) {

        String jsonString = getJson(fileName, this.getContext());
        SkinKeyboard skb = gson.fromJson(jsonString, SkinKeyboard.class);
        skb.resize(kbRect);

        return skb;
    }

    public SkinKeyboard createSkin(int id) {
        Rect kbRect = new Rect(0, 0, mInputService.getKeyboardWidth(), mInputService.getKeyboardHeight());
        switch (id){
            case INPUT_METHOD_NUMBER:
            case INPUT_METHOD_PHONENUM:
                return createSkin(kbRect, mSkinPath + "/number.json");
            case INPUT_METHOD_PINYIN:
                return createSkin(kbRect, mSkinPath + "/pinyin-qwerty.json");
            case INPUT_METHOD_HANDWRITE:
                return createSkin(kbRect, mSkinPath + "/handwrite.json");
            case INPUT_METHOD_SYMBOL_NUMBER:
                return createSkin(kbRect, mSkinPath + "/symbol-number.json");
            case INPUT_METHOD_SYMBOL_CN:
                return createSkin(kbRect, mSkinPath + "/symbol-qwerty-cn.json");
            case INPUT_METHOD_SYMBOL_CN_MORE:
                return createSkin(kbRect, mSkinPath + "/symbol-qwerty-cn-more.json");
            case INPUT_METHOD_SYMBOL_EN:
                return createSkin(kbRect, mSkinPath + "/symbol-qwerty-en.json");
            case INPUT_METHOD_SYMBOL_EN_MORE:
                return createSkin(kbRect, mSkinPath + "/symbol-qwerty-en-more.json");
            case INPUT_METHOD_ENGLISH:
                return createSkin(kbRect, mSkinPath + "/english-qwerty.json");
        }

        return null;
    }


    public InputMgrView(Context context) {
        super(context);

        mInputService = (PalmInputService) context;

        String SHARED_PREFERENCES_NAME = "FlutterSharedPreferences";
        SharedPreferences sp = context.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
        mSkinPath = sp.getString("flutter.skin_name","skin-white");

        mResPool = new SkinResPool(context, mSkinPath, mInputService.getKeyboardWidth());

        mSymbolViewNumber = new SymbolView(context);
        mSymbolViewCn = new SymbolView(context);
        mSymbolViewCnMore = new SymbolView(context);
        mSymbolViewEn = new SymbolView(context);
        mSymbolViewEnMore = new SymbolView(context);
        mEnglishView = new EnglishView(context);
        mHandView = new HandView(context);
        mPinyinView = new PinyinView(context);
        mNumberView = new SymbolView(context);
    }

    public void reloadSkin(Context context)
    {
        String SHARED_PREFERENCES_NAME = "FlutterSharedPreferences";
        SharedPreferences sp = context.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
        
        if (!mSkinPath.equals(sp.getString("flutter.skin_name","skin-white")))
        {
            mSkinPath = sp.getString("flutter.skin_name","skin-white");
            mResPool = new SkinResPool(context, mSkinPath, mInputService.getKeyboardWidth());

            mSymbolViewNumber = new SymbolView(context);
            mSymbolViewCn = new SymbolView(context);
            mSymbolViewCnMore = new SymbolView(context);
            mSymbolViewEn = new SymbolView(context);
            mSymbolViewEnMore = new SymbolView(context);
            mEnglishView = new EnglishView(context);
            mHandView = new HandView(context);
            mPinyinView = new PinyinView(context);
            mNumberView = new SymbolView(context);
        }
    }

    @Override
    public void deleteTextBeforeCursor() {
        mInputService.deleteTextBeforeCursor(1);
    }

    @Override
    public void sendCommitText(String str) {
        mInputService.sendCommitText(str, 1);
    }

    @Override
    public void returnMainInputMethod(){
        if (mCurrentKeyboard != null) {
            mCurrentKeyboard.hide();
        }

        mCurrentKeyboard = mCurrentKeyboardMain;

        // !!!!
        if (mCurrentKeyboard == mPinyinView || mCurrentKeyboard == mHandView)
        {
            mCurrentKeyboard = mEnglishView;
        }

        addView((View) mCurrentKeyboard);
        mCurrentKeyboard.show();
    }

    @Override
    public void showMenu() {

    }

    @Override
    public void setInputMethod(int id) {
        removeAllViews();
        PalmInputPreferences preferences = new PalmInputPreferences(mInputService);

        if (mCurrentKeyboard != null) {
            mCurrentKeyboard.hide();
        }

        switch(id) {
            case INPUT_METHOD_NUMBER:
            case INPUT_METHOD_PHONENUM:
                mCurrentKeyboard = mNumberView;
                mCurrentKeyboardMain = mCurrentKeyboard;
                break;
            case INPUT_METHOD_SYMBOL_NUMBER:
                mCurrentKeyboard = mSymbolViewNumber;
                break;
            case INPUT_METHOD_SYMBOL_CN:
                mCurrentKeyboard = mSymbolViewCn;
                break;
            case INPUT_METHOD_SYMBOL_CN_MORE:
                mCurrentKeyboard = mSymbolViewCnMore;
                break;
            case INPUT_METHOD_SYMBOL_EN:
                mCurrentKeyboard = mSymbolViewEn;
                break;
            case INPUT_METHOD_SYMBOL_EN_MORE:
                mCurrentKeyboard = mSymbolViewEnMore;
                break;
            case INPUT_METHOD_ENGLISH:
                mCurrentKeyboard = mEnglishView;
                mCurrentKeyboardMain = mCurrentKeyboard;
                break;
            case INPUT_METHOD_CHINESE:
                mCurrentKeyboard = mCurrentKeyboardCn;
                mCurrentKeyboardMain = mCurrentKeyboard;
                break;

            case INPUT_METHOD_HANDWRITE:
                mCurrentKeyboardCn = mHandView;
                mCurrentKeyboard = mHandView;
                mCurrentKeyboardMain = mCurrentKeyboard;
                preferences.setChineseInputMethod(INPUT_METHOD_HANDWRITE);
                break;
            case INPUT_METHOD_PINYIN:
            default:
                mCurrentKeyboardCn = mPinyinView;
                mCurrentKeyboard = mPinyinView;
                mCurrentKeyboardMain = mCurrentKeyboard;
                preferences.setChineseInputMethod(INPUT_METHOD_PINYIN);
                break;
        }

        // !!!!
        if (mCurrentKeyboard == mPinyinView || mCurrentKeyboard == mHandView)
        {
            mCurrentKeyboard = mEnglishView;
        }

        if (!mCurrentKeyboard.isInit()) {
            mCurrentKeyboard.init(this, id, mResPool, createSkin(id));
        }

        addView((View) mCurrentKeyboard);
        mCurrentKeyboard.show();
    }

    @Override
    public void nextInputMethod(int curId) {
        if (curId == INPUT_METHOD_PINYIN) {
            setInputMethod(INPUT_METHOD_HANDWRITE);
        }
        else {
            setInputMethod(INPUT_METHOD_PINYIN);
        }
    }

    public void onShow(int classInfo, int actionInfo) {

        reloadSkin(mInputService);
        if (classInfo == EditorInfo.TYPE_CLASS_PHONE || classInfo == EditorInfo.TYPE_CLASS_NUMBER) {
            setInputMethod(INPUT_METHOD_NUMBER);
        }
        else if ((classInfo & EditorInfo.TYPE_MASK_VARIATION) ==  EditorInfo.TYPE_TEXT_VARIATION_PASSWORD ||
                (classInfo & EditorInfo.TYPE_MASK_VARIATION) == EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD) {
            setInputMethod(INPUT_METHOD_ENGLISH);
        }
        else {
            if (isChineseLanguage()) {
                PalmInputPreferences preferences = new PalmInputPreferences(mInputService);
                setInputMethod(preferences.getChineseInputMethod());
            }
            else {
                setInputMethod(INPUT_METHOD_ENGLISH);
            }
        }
    }
    public void onHide(boolean finishingInput) {
        if (mCurrentKeyboard.isInit()) {
            mCurrentKeyboard.hide();
        }
    }

    @Override
    public void hideKeyboard() {
        mInputService.hideKeyboard();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int measureWidth = measureWidth(widthMeasureSpec);
        int measureHeight = measureHeight(heightMeasureSpec);
        // 计算自定义的ViewGroup中所有子控件的大小
        measureChildren(widthMeasureSpec, heightMeasureSpec);
        // 设置自定义的控件MyViewGroup的大小
        setMeasuredDimension(measureWidth, measureHeight);
    }

    private int measureWidth(int pWidthMeasureSpec) {
        int result = 0;
        int widthMode = MeasureSpec.getMode(pWidthMeasureSpec);// 得到模式
        int widthSize = MeasureSpec.getSize(pWidthMeasureSpec);// 得到尺寸

        switch (widthMode) {
            case MeasureSpec.AT_MOST:
            case MeasureSpec.EXACTLY:
                result = widthSize;
                break;
        }
        return result;
    }

    private int measureHeight(int pHeightMeasureSpec) {
        int result = 0;

        int heightMode = MeasureSpec.getMode(pHeightMeasureSpec);
        int heightSize = MeasureSpec.getSize(pHeightMeasureSpec);

        switch (heightMode) {
            case MeasureSpec.AT_MOST:
            case MeasureSpec.EXACTLY:
                result = heightSize;
                break;
        }
        return result;
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        int childCount = this.getChildCount();
        for (int i = 0; i < childCount; i++) {
            View child = this.getChildAt(i);
            child.layout(l, b - mInputService.getKeyboardHeight(), r, b);
        }
    }
}
