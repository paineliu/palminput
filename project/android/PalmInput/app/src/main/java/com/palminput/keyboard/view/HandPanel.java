package com.palminput.keyboard.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.RectF;
import android.util.Log;
import android.view.MotionEvent;

import com.palminput.keyboard.PalmInputPreferences;
import com.palminput.keyboard.PalmInputService;
import com.palminput.keyboard.InputMgrView;
import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;
import com.palminput.engine.HWProvider;

import java.util.ArrayList;
import java.util.Timer;

import static android.view.MotionEvent.ACTION_DOWN;
import static android.view.MotionEvent.ACTION_MOVE;
import static android.view.MotionEvent.ACTION_UP;

public class HandPanel extends KBBaseView {
    private static final float STROKE_WIDTH = 6f;
    private static final float HALF_STROKE_WIDTH = STROKE_WIDTH / 2;

    private final Paint mStrokePaint;
    private final Path mPath;

    HWProvider hwAdapter;
    Timer mTimer;
    Point mDownPoint = new Point(-1, -1);
    Point mLastPoint = new Point(-1, -1);
    ArrayList<ArrayList<PointF>> mList = new ArrayList<ArrayList<PointF>>();
    ArrayList<PointF> mArray = new ArrayList<PointF>();
    private final RectF mDirtyRect;
    float lastTouchX;
    float lastTouchY;
    int mDownPointId = 0;


    public HandPanel(Context context) {
        super(context);
        mInputMethodId  = InputMgrView.INPUT_METHOD_HANDWRITE;
        mInputService = (PalmInputService) context;
        mWidth = 0;
        mHeight = 0;

        mDirtyRect = new RectF();

        hwAdapter = new HWProvider();
        hwAdapter.create(context.getAssets(), "", "");

        mStrokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mStrokePaint.setColor(Color.WHITE);
        mStrokePaint.setStrokeJoin(Paint.Join.ROUND);
        mStrokePaint.setStrokeWidth(STROKE_WIDTH);
        mStrokePaint.setStyle(Paint.Style.STROKE);

        mPath = new Path();

    }
    @Override
    public void init(IOperater operater, int methodId, SkinResPool resPool, SkinKeyboard keyboardSkin) {
        super.init(operater, methodId, resPool, keyboardSkin);
        mStrokePaint.setColor(keyboardSkin.getCandTextColor().getARGB());
    }
    /**
     * Called when replaying history to ensure the dirty region includes all
     * points.
     */
    private void expandDirtyRect(float historicalX, float historicalY) {
        if (historicalX < mDirtyRect.left) {
            mDirtyRect.left = historicalX;
        } else if (historicalX > mDirtyRect.right) {
            mDirtyRect.right = historicalX;
        }
        if (historicalY < mDirtyRect.top) {
            mDirtyRect.top = historicalY;
        } else if (historicalY > mDirtyRect.bottom) {
            mDirtyRect.bottom = historicalY;
        }
    }

    /**
     * Resets the dirty region when the motion event occurs.
     */
    private void resetDirtyRect(float eventX, float eventY) {

        // The lastTouchX and lastTouchY were set when the ACTION_DOWN
        // motion event occurred.
        mDirtyRect.left = Math.min(lastTouchX, eventX);
        mDirtyRect.right = Math.max(lastTouchX, eventX);
        mDirtyRect.top = Math.min(lastTouchY, eventY);
        mDirtyRect.bottom = Math.max(lastTouchY, eventY);
    }


    private void drawStrokeToPath() {
        mPath.reset();
        for (ArrayList<PointF> stroke: mList){
            mPath.moveTo(stroke.get(0).x, stroke.get(0).y);
            for (int i=0; i<stroke.size(); i++){
                mPath.lineTo(stroke.get(i).x, stroke.get(i).y);
            }
        }
    }

    protected void updateCand() {
        int[] myInt = new int[mArray.size() * 2];
        int nIndex = 0;
        for (int i=0; i<mArray.size(); i++) {
            myInt[nIndex++] = (int) mArray.get(i).x;
            myInt[nIndex++] = (int) mArray.get(i).y;
        }

        hwAdapter.addStroke(myInt);

        HandView v = (HandView) getParent();
        v.updateCand();
        if (mArray.size() > 0) {
            mList.add(new ArrayList<>(mArray));
        }
        mArray.clear();
    }

    public void resetLast()
    {
        mArray.clear();
        mDownPoint.x = -1;
        mDownPoint.y = -1;
        drawStrokeToPath();
        invalidate();
    }

    public void reset()
    {
        mPath.reset();
        mList.clear();
        mArray.clear();
        mDownPoint.x = -1;
        mDownPoint.y = -1;
        invalidate();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int actionType = event.getActionMasked();
        int point_index = event.getActionIndex();
        int point_id = event.getPointerId(point_index);

        if (event.getAction() == ACTION_DOWN)
        {
            if (event.getPointerCount() == 1) {
                mDownPointMap.clear();
            }

            mDownPointId = point_id;
            mPath.moveTo(event.getX(), event.getY());
            lastTouchX = event.getX();
            lastTouchY =  event.getY();
            mLastPoint.set((int) event.getX(), (int) event.getY());

            mArray.clear();
            mDownPoint.set((int)event.getX(), (int)event.getY());

            mArray.add(new PointF(event.getX(), event.getY()));

            invalidate(
                    (int) (mDirtyRect.left - HALF_STROKE_WIDTH),
                    (int) (mDirtyRect.top - HALF_STROKE_WIDTH),
                    (int) (mDirtyRect.right + HALF_STROKE_WIDTH),
                    (int) (mDirtyRect.bottom + HALF_STROKE_WIDTH));

            lastTouchX = event.getX();
            lastTouchY = event.getY();
            mLastPoint.set((int) event.getX(), (int) event.getY());
            mDownPointMap.put(point_id, new Point((int) event.getX(), (int) event.getY()));

        }
        else if (event.getAction() == ACTION_MOVE) {

            for (int i = 0; i < event.getPointerCount(); i++) {
                int id1 = event.getPointerId(i);
                if (id1 == mDownPointId)
                {
                    if (mDownPoint.x == -1 && mDownPoint.y == -1)
                    {
                        //Log.d("touch", "down point= " + mDownPoint);

                        mPath.moveTo(event.getX(), event.getY());

                        mArray.clear();
                        mDownPoint.set((int)event.getX(), (int)event.getY());
                        mArray.add(new PointF(event.getX(), event.getY()));
                    }
                    //Log.d("touch", "move point= " + event.getX() + " " + event.getY());

                    resetDirtyRect(event.getX(), event.getY());

                    int historySize = event.getHistorySize();
                    for (int j = 0; j < historySize; j++) {
                        float historicalX = event.getHistoricalX(j);
                        float historicalY = event.getHistoricalY(j);

                        PointF history_point = new PointF(historicalX, historicalY);
                        expandDirtyRect(historicalX, historicalY);
                        mArray.add(history_point);
                        mPath.lineTo(historicalX, historicalY);
                    }
                    mPath.lineTo(event.getX(), event.getY());
                    mArray.add(new PointF(event.getX(), event.getY()));

                    invalidate(
                            (int) (mDirtyRect.left - HALF_STROKE_WIDTH),
                            (int) (mDirtyRect.top - HALF_STROKE_WIDTH),
                            (int) (mDirtyRect.right + HALF_STROKE_WIDTH),
                            (int) (mDirtyRect.bottom + HALF_STROKE_WIDTH));

                    lastTouchX = event.getX();
                    lastTouchY = event.getY();
                    mLastPoint.set((int) event.getX(), (int) event.getY());

                }
            }
        }
        else if (event.getAction() == ACTION_UP ||  actionType == MotionEvent.ACTION_POINTER_UP)
        {
            if (point_id == mDownPointId) {
                resetDirtyRect(event.getX(), event.getY());
                if (mDownPoint.x != -1) {
                    //Log.d("touch", "up point= " + event.getX() + " " + event.getY());
                    int historySize = event.getHistorySize();
                    for (int i = 0; i < historySize; i++) {
                        float historicalX = event.getHistoricalX(i);
                        float historicalY = event.getHistoricalY(i);

                        PointF history_point = new PointF(historicalX, historicalY);
                        expandDirtyRect(historicalX, historicalY);
                        mArray.add(history_point);
                        mPath.lineTo(historicalX, historicalY);
                    }

                    mPath.lineTo(event.getX(), event.getY());
                    mArray.add(new PointF(event.getX(), event.getY()));

                    int p = 5;
                    if (Math.abs(mArray.get(0).x - mArray.get(mArray.size() - 1).x) < p && Math.abs(mArray.get(0).y - mArray.get(mArray.size() - 1).y) < p)
                    {
                        mPath.lineTo(event.getX() + p, event.getY() + p);
                        mArray.add(new PointF(event.getX() + p, event.getY() + p));
                        expandDirtyRect(event.getX() + p, event.getY() + p);
                    }
                }

                mDownPointMap.remove(point_id);

                //点击按键，当前笔画无效

                if (getButton(mDownPoint) != null && getButton(mDownPoint) == getButton(new Point((int) event.getX(), (int) event.getY()))) {
                    mArray.clear();
                    mPath.reset();
                    onKeyEvent(getButton(mDownPoint).getId(), KEY_CLICK);
                    mDownPoint.x = -1;
                    mDownPoint.y = -1;
                    invalidate();
                    return true;
                }

                //在按键上点击后滑动到其他位置，如果是第一笔则笔画无效

                if (getButton(mDownPoint) != null && mList.size() == 0) {
                    mDownPoint.x = -1;
                    mDownPoint.y = -1;
                    mArray.clear();
                    mPath.reset();
                    invalidate();
                    return true;
                }

                updateCand();

                invalidate(
                        (int) (mDirtyRect.left - HALF_STROKE_WIDTH),
                        (int) (mDirtyRect.top - HALF_STROKE_WIDTH),
                        (int) (mDirtyRect.right + HALF_STROKE_WIDTH),
                        (int) (mDirtyRect.bottom + HALF_STROKE_WIDTH));

                lastTouchX = event.getX();
                lastTouchY = event.getY();
                mLastPoint.set((int) event.getX(), (int) event.getY());
                mDownPoint.x = -1;
                mDownPoint.y = -1;

            }
        }
        else {
            if (actionType != MotionEvent.ACTION_POINTER_DOWN) {
                mList.clear();
                mArray.clear();
                mPath.reset();
                mDownPoint.x = -1;
                mDownPoint.y = -1;
                invalidate();

            }
        }

        return true;
    }

    public boolean onKeyEvent(String id, int actionId) {

        HandView v = (HandView) getParent();
        switch (id) {
            case "VK_BACKSPACE":
                if (mList.size() > 0) {
                    mList.remove(mList.size() - 1);
                    hwAdapter.delLastStroke();
                    drawStrokeToPath();
                    v.updateCand();
                    invalidate();
                } else {
                    mOperater.deleteTextBeforeCursor();
                }
                break;
            case "VK_SPACE":
                if (mList.size() > 0) {
                    v.selItem(0);
                    mList.clear();
                    mPath.reset();
                    invalidate();
                }
                else {
                    invalidate();
                    mOperater.sendCommitText(" ");
                }
                break;
            case "VK_SYMBOL_CN":
                v.reset();
                mList.clear();
                mPath.reset();
                mOperater.setInputMethod(InputMgrView.INPUT_METHOD_SYMBOL_CN);
                break;
            case "VK_SYMBOL_CN_MORE":
                mOperater.setInputMethod(InputMgrView.INPUT_METHOD_SYMBOL_CN_MORE);
                break;
            case "VK_SYMBOL_EN":
                mOperater.setInputMethod(InputMgrView.INPUT_METHOD_SYMBOL_EN);
                break;
            case "VK_SYMBOL_EN_MORE":
                mOperater.setInputMethod(InputMgrView.INPUT_METHOD_SYMBOL_EN_MORE);
                break;

            case "VK_ENGLISH":
                v.reset();
                mList.clear();
                mPath.reset();
                mOperater.setInputMethod(InputMgrView.INPUT_METHOD_ENGLISH);
                break;

            case "VK_LANGUAGE":
                v.reset();
                mList.clear();
                mPath.reset();
                mOperater.nextInputMethod(InputMgrView.INPUT_METHOD_HANDWRITE);
                break;
            case "VK_RESET":

                mList.clear();
                mPath.reset();
                invalidate();

                break;
            case "VK_CHINESE":
                mOperater.setInputMethod(InputMgrView.INPUT_METHOD_CHINESE);
                break;

            case "VK_HIDE":
                v.reset();
                mPath.reset();
                mList.clear();
                mOperater.hideKeyboard();
                break;

            case "VK_DONE":
                mList.clear();
                mPath.reset();
                v.reset();
                mOperater.sendCommitText("\n");
                break;
        }
        return true;
    }

    @Override
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        canvas.drawPath(mPath, mStrokePaint);
    }
}
