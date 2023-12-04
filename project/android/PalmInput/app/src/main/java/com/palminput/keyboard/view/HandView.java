package com.palminput.keyboard.view;

import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.palminput.keyboard.PalmInputService;
import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;
import com.palminput.engine.HWProvider;

import static android.view.MotionEvent.ACTION_DOWN;
import static android.view.MotionEvent.ACTION_UP;

public class HandView extends ViewGroup implements IKeyboard{
    private PalmInputService mInputService = null;

    private HandPanel mHandPanel;
    private RecyclerView mRecyclerView;
    private final CompWin mCompWin;
    private boolean mDispatchEvent = false;
    IOperater mOperater;
    HWProvider mHWProvider = new HWProvider();
    HandAdapter mAdapter;

    public HandView(Context context) {
        super(context);
        mInputService = (PalmInputService) context;
        mCompWin = new CompWin(context);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int measureWidth = measureWidth(widthMeasureSpec);
        int measureHeight = measureHeight(heightMeasureSpec);
        measureChildren(widthMeasureSpec, heightMeasureSpec);
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

    public void updateCand()
    {
        if (mHWProvider.getCandItemTotal() == 0)
        {
            mRecyclerView.setVisibility(INVISIBLE);
        }
        else {
            mRecyclerView.setVisibility(VISIBLE);
            mRecyclerView.scrollToPosition(0);
            mAdapter.notifyDataSetChanged();
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
        int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            View child = this.getChildAt(i);
            if (child instanceof RecyclerView) {
                int hv = b-t;
                child.layout(5, 5, r-l-10, 100 + 100 * (hv-504)/504);
            }
            else {
                child.layout(0, 0, r-l, b-t);
            }
        }
    }

    public void reset()
    {
        mHandPanel.reset();
        mHWProvider.reset();
        mRecyclerView.setVisibility(INVISIBLE);
    }

    public void selItem(int position) {
        mOperater.sendCommitText(mHWProvider.getCandItemString(position));
        mHWProvider.reset();
        mHandPanel.reset();
        mRecyclerView.setVisibility(INVISIBLE);
    }


    @Override
    public boolean isInit() {
        return mHandPanel != null;
    }

    @Override
    public void init(IOperater operater, int methodId, SkinResPool resPool, SkinKeyboard keyboardSkin) {
        if (mHandPanel == null) {
            mOperater = operater;
            mHandPanel = new HandPanel(mInputService);
            mHandPanel.init(operater, methodId, resPool, keyboardSkin);
            mRecyclerView = new RecyclerView(mInputService);
            mRecyclerView.setBackgroundColor(keyboardSkin.getCandBgColor().getARGB());
            LinearLayoutManager manager = new LinearLayoutManager(mInputService);
            manager.setOrientation(LinearLayoutManager.HORIZONTAL);

            mRecyclerView.setLayoutManager(manager);//线性

            mHWProvider.create(mInputService.getAssets(), "pinyin/", mInputService.getFilesDir().toString());
            mAdapter = new HandAdapter(mInputService, mHWProvider, keyboardSkin.getCandBgColor().getARGB(), keyboardSkin.getCandTextColor().getARGB());

            mAdapter.setOnItemClickListener(new HandAdapter.OnItemClickListener(){
                @Override
                public void onItemClick(View view , int position){
                    selItem(position);
                }
            });
            mRecyclerView.setAdapter(mAdapter);
            addView(mHandPanel);
            addView(mRecyclerView);
            mRecyclerView.setVisibility(INVISIBLE);
            //mRecyclerView.setAlpha(0.5f);

        }
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (mRecyclerView.getVisibility() == VISIBLE)
        {
            int actionType = ev.getActionMasked();
            if (actionType == ACTION_DOWN)
            {
                if (ev.getY() < mRecyclerView.getHeight())
                {
                    mDispatchEvent = true;
                }
                else
                {
                    mDispatchEvent = false;
                }
            }

            if (mDispatchEvent) {
                if (actionType != ACTION_UP)
                {
                    mHandPanel.onTouchEvent(ev);
                }
                else
                {
                    if (ev.getY() < mRecyclerView.getHeight())
                    {
                        mHandPanel.resetLast();
                    }
                    else
                    {
                        mHandPanel.onTouchEvent(ev);
                    }
                }
            }
        }
        return super.dispatchTouchEvent(ev);
    }

    @Override
    public void show() {
        reset();
    }

    @Override
    public void hide() {
        reset();
    }

    @Override
    public void destory() {

    }
}
