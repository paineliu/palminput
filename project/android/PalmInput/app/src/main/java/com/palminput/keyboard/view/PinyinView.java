package com.palminput.keyboard.view;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.graphics.Color;
import android.view.View;
import android.view.ViewGroup;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.palminput.keyboard.PalmInputService;
import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;
import com.palminput.keyboard.engine.PYOptions;
import com.palminput.keyboard.engine.PYProvider;

import static com.palminput.keyboard.InputMgrView.INPUT_METHOD_PINYIN;

public class PinyinView extends ViewGroup implements IKeyboard{
    private PalmInputService mInputService = null;

    private PinyinPanel mPinyinView;
    private RecyclerView rv;
    private final CompWin mCompWin;
    IOperater mOperater;
    PYProvider pyAdapter = new PYProvider();
    PinyinAdapter adapter;
    int mCommitTimes;
    public PinyinView(Context context) {
        super(context);
        mInputService = (PalmInputService) context;
        mCompWin = new CompWin(context);
        mCommitTimes = 0;
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

    public int getCompLen()
    {
        return pyAdapter.getCompString().length();
    }

    public int commitCompString()
    {
        if(pyAdapter.getCompString().length() > 0) {
            mOperater.sendCommitText(pyAdapter.getCommitString());
            pyAdapter.reset();
            rv.setVisibility(INVISIBLE);
            mCompWin.hide();

        }
        return 0;
    }
    public void resetKey()
    {
        pyAdapter.reset();
        rv.setVisibility(INVISIBLE);
        mCompWin.hide();
//        mOperater.deleteTextBeforeCursor();
    }

    public void addKey(int key)
    {
        int[] location = new int[2];
        getLocationInWindow(location);
        int x1 = location[0];
        int y1 = location[1];
        pyAdapter.addKey(key);
        if (pyAdapter.getCompString().length() > 0)
        {
            mCompWin.show(this, 0, y1);
            mCompWin.updateComp(pyAdapter.getCompString());
            rv.setVisibility(VISIBLE);
            rv.scrollToPosition(0);
            adapter.notifyDataSetChanged();
        }
        else
        {
            rv.setVisibility(INVISIBLE);
            mCompWin.hide();
            //mOperater.deleteTextBeforeCursor();
        }
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
        // 记录总高度
        int mTotalHeight = 0;
        // 遍历所有子视图
        int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            View childView = getChildAt(i);
//
//            // 获取在onMeasure中计算的视图尺寸
//            int measureHeight = childView.getMeasuredHeight();
//            int measuredWidth = childView.getMeasuredWidth();
//
//            childView.layout(l, mTotalHeight, measuredWidth, mTotalHeight
//                    + measureHeight);
//
//            mTotalHeight += measureHeight;
            View child = this.getChildAt(i);
            if (child instanceof RecyclerView) {
//                int hk = mInputService.getKeyboardHeight();
//                int wk = mInputService.getKeyboardWidth();
                int hv = b-t;
//                int wv = r-l;
                child.layout(5, 5, r-l-10, 100 + 100 * (hv-504)/504);
            }
            else {
                childView.layout(0, 0, r-l, b-t);
            }
        }
    }

    public void selItem(int position) {
        if (pyAdapter.selCandItem(position) == 1)
        {
            mCommitTimes ++;
            mOperater.sendCommitText(pyAdapter.getCommitString());
            pyAdapter.reset();
            adapter.notifyDataSetChanged();
            if (pyAdapter.getCompString().length() > 0)
            {
                mCompWin.updateComp(pyAdapter.getCompString());
                rv.setVisibility(VISIBLE);
                adapter.notifyDataSetChanged();
            }
            else
            {
                mPinyinView.invalidate();
                rv.setVisibility(INVISIBLE);
                mCompWin.hide();
            }
        }
        else
        {
            if (pyAdapter.getCompString().length() > 0)
            {
                mCompWin.updateComp(pyAdapter.getCompString());
                rv.setVisibility(VISIBLE);
                rv.scrollToPosition(0);
                adapter.notifyDataSetChanged();
            }
            else
            {
                mPinyinView.invalidate();
                rv.setVisibility(INVISIBLE);
                mCompWin.hide();
            }
        }
    }

    @Override
    public boolean isInit() {
        return mPinyinView!= null;
    }

    @Override
    public void init(IOperater operater, int methodId, SkinResPool resPool, SkinKeyboard keyboardSkin) {
        if (mPinyinView == null) {
            mOperater = operater;
            mPinyinView = new PinyinPanel(mInputService);
            mPinyinView.init(operater, methodId, resPool, keyboardSkin);
            rv = new RecyclerView(mInputService);

            rv.setBackgroundColor(keyboardSkin.getCandBgColor().getARGB());
            LinearLayoutManager manager = new LinearLayoutManager(mInputService);
            manager.setOrientation(LinearLayoutManager.HORIZONTAL);

            rv.setLayoutManager(manager);//线性
            String userFileName = mInputService.getFilesDir().toString();
            AssetManager assetManager = mInputService.getAssets();
            pyAdapter.create(assetManager, "pinyin/", userFileName);
            String SHARED_PREFERENCES_NAME = "FlutterSharedPreferences";
            SharedPreferences sp = mInputService.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
            PYOptions options = new PYOptions(0, false);
            //options.mPredictEnable = sp.getBoolean("flutter.predict_enable",false);
            options.mFuzzyFlags = (int)sp.getLong("flutter.py_fuzzy_flags", 0);
            pyAdapter.setOptions(options);

            adapter = new PinyinAdapter(mInputService, pyAdapter, keyboardSkin.getCandBgColor().getARGB(), keyboardSkin.getCandTextColor().getARGB());
            mCompWin.setCompColor(keyboardSkin.getCompBgColor().getARGB(), keyboardSkin.getCompTextColor().getARGB());
            adapter.setOnItemClickListener(new PinyinAdapter.OnItemClickListener(){
                @Override
                public void onItemClick(View view , int position){
                    selItem(position);
                }
            });
            rv.setAdapter(adapter);
            addView(mPinyinView);
            addView(rv);
            rv.setVisibility(INVISIBLE);
        }
    }

    @Override
    public void show() {
        resetKey();
        String SHARED_PREFERENCES_NAME = "FlutterSharedPreferences";
        SharedPreferences sp = mInputService.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
        PYOptions options = new PYOptions(0, false);
        //options.mPredictEnable = sp.getBoolean("flutter.predict_enable",false);
        options.mFuzzyFlags = (int)sp.getLong("flutter.py_fuzzy_flags", 0);
        pyAdapter.setOptions(options);
    }

    @Override
    public void hide() {
        resetKey();
        if (mCommitTimes > 5) {
            mCommitTimes = 0;
            pyAdapter.save();
        }
    }

    @Override
    public void destory() {
        pyAdapter.destroy();
    }
}
