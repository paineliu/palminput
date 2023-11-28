package com.palminput.keyboard.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.MotionEvent;
import android.view.View;

import com.palminput.keyboard.PalmInputPreferences;
import com.palminput.keyboard.PalmInputService;
import com.palminput.keyboard.InputMgrView;
import com.palminput.keyboard.skin.SkinBorder;
import com.palminput.keyboard.skin.SkinButton;
import com.palminput.keyboard.skin.SkinButtonPaint;
import com.palminput.keyboard.skin.SkinButtonStyle;
import com.palminput.keyboard.skin.SkinButtonStyleState;
import com.palminput.keyboard.skin.SkinImage;
import com.palminput.keyboard.skin.SkinKeyboard;
import com.palminput.keyboard.skin.SkinResPool;

import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class KBBaseView extends View implements IKeyboard{

    public final static int ALIGN_LEFT   = 0x0;
    public final static int ALIGN_CENTER = 0x1;
    public final static int ALIGN_RIGHT  = 0x2;

    public final static int ALIGN_TOP     = 0x0;
    public final static int ALIGN_MIDDLE  = 0x4;
    public final static int ALIGN_BOTTOM  = 0x8;

    public final static int KEY_CLICK = 1;
    public final static int KEY_REPEAT  =2;
    public final static int KEY_SLID_LEFT    =3;
    public final static int KEY_SLID_UP  = 4;
    public final static int KEY_SLID_RIGHT  = 5;
    public final static int KEY_SLID_DOWN  = 6;


    private boolean mFirstShow = false;
    int mKeyRepeatTimes = 0;
    protected PalmInputService mInputService = null;
    private Paint mPaint;
    private final Rect mBounds;
    protected int mInputMethodId = 0;
    protected HashMap<Integer, Point> mDownPointMap = new HashMap<Integer, Point>();
    int mWidth;
    int mHeight;
    protected int mShiftMode = 0;
    static protected boolean mSymbolLocked = false;
    Bitmap mBitmap;
    Canvas mCanvas;
    Paint mBitmapPaint;
    private SkinKeyboard mKeyboardSkin = null;
    private SkinResPool mResPool;
    protected IOperater mOperater;
    protected boolean mShowPop;
    MyPopupWin mPopWin;

    Timer mTimer;

    public KBBaseView(Context context) {

        super(context);
        mShowPop = false;
        mInputService = (PalmInputService) context;
        mWidth = 0;
        mHeight = 0;
        mPopWin = new MyPopupWin(context);
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mBounds = new Rect();
        mTimer = null;
        PalmInputPreferences preferences = new PalmInputPreferences(mInputService);
        mSymbolLocked = preferences.getSymbolLocked();
    }

    public void onShow() {
        mFirstShow = true;
        mShiftMode = 0;
    }

    public void onHide() {
        mDownPointMap.clear();
    }

    protected void onMeasure(int width, int height) {
        super.onMeasure(width, height);
        setMeasuredDimension(mInputService.getKeyboardWidth(), mInputService.getKeyboardHeight());
    }


    public String getShowState() { return ""; }

    public boolean isShiftlocked() {
        return  false;
    }
    public boolean isSymbollocked() {
        return  false;
    }


    public boolean onKeyEvent(String keyId, int actionId) {
        SkinButton btn = getButton(keyId);
        if(actionId == KEY_REPEAT) {
            switch (keyId) {
                case "VK_BACKSPACE":
                    mKeyRepeatTimes++;
                    mOperater.deleteTextBeforeCursor();
                    break;
                default:
                    break;
            }
        }

        if (actionId == KEY_CLICK) {
            switch (keyId) {
                case "VK_BACKSPACE":
                    if (mKeyRepeatTimes == 0) {
                        mOperater.deleteTextBeforeCursor();
                    }
                    break;
                case "VK_SPACE":
                    mOperater.sendCommitText(" ");
                    break;
                case "VK_SHIFT":
                    mShiftMode += 1;
                    if (mShiftMode >= 3) {
                        mShiftMode = 0;
                    }
                    break;
                case "VK_NUMBER":
                    mOperater.setInputMethod(InputMgrView.INPUT_METHOD_SYMBOL_NUMBER);
                    break;
                case "VK_SYMBOL_CN":
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
                    mOperater.setInputMethod(InputMgrView.INPUT_METHOD_ENGLISH);
                    break;
                case "VK_LANGUAGE":
                    // mOperater.nextInputMethod(mInputMethodId);
                    break;
                case "VK_MENU":
                    mOperater.showMenu();
                    break;
                case "VK_CHINESE":
                    mOperater.setInputMethod(InputMgrView.INPUT_METHOD_CHINESE);
                    break;
                case "VK_HIDE":
                    mOperater.hideKeyboard();
                    break;
                case "VK_LOCK":
                    mSymbolLocked = !mSymbolLocked;
                    PalmInputPreferences preferences = new PalmInputPreferences(mInputService);
                    preferences.setSymbolLocked(mSymbolLocked);
                    break;
                case "VK_PREV_KEYBOARD":
                    mOperater.returnMainInputMethod();
                    break;
                case "VK_DONE":
                    mOperater.sendCommitText("\n");
                    break;

                default:
                    if (mShiftMode == 0) {
                        mOperater.sendCommitText(btn.getLabel());
                    } else {
                        mOperater.sendCommitText(btn.getLabel().toUpperCase());
                        if (mShiftMode == 1) {
                            mShiftMode = 0;
                        }
                    }
                    break;
            }
        }

        if ((btn != null) && (!keyId.startsWith("VK_")) && (actionId == KEY_SLID_UP)) {
            if (btn.getTag() != null) {
                mInputService.sendCommitText(btn.getTag(), 1);
            }
        }

        if ((btn != null) && (!keyId.startsWith("VK_")) && (actionId == KEY_SLID_DOWN)) {
            mInputService.sendCommitText(btn.getLabel().toUpperCase(), 1);
        }

        return true;
    }

    public SkinButton getButton(String btnId) {
        List<SkinButton> buttons = mKeyboardSkin.getButtons();

        for (int i = buttons.size() - 1; i >= 0; i--) {
            SkinButton btn = buttons.get(i);
            if (btn.getId() == btnId) {
                return btn;
            }
        }

        return null;
    }

    public SkinButton getButton(Point point) {
        List<SkinButton> buttons = mKeyboardSkin.getButtons();

        for (int i = buttons.size() - 1; i >= 0; i--) {
            SkinButton btn = buttons.get(i);
            String showState = btn.getShowState();
            if (showState != null) {
                if (!showState.equals(getShowState())) {
                    continue;
                }
            }

            if (btn.getRealRect().contains(point.x, point.y)) {
                return btn;
            }
        }

        return null;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        int actionType = event.getActionMasked();
        int point_index = event.getActionIndex();
        int point_id = event.getPointerId(point_index);

        switch (actionType) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                {
                    if (event.getPointerCount() == 1) {
                        mDownPointMap.clear();
                    }

                    for (int i = 0; i < event.getPointerCount(); i++) {
                        int id1 = event.getPointerId(i);
                        if (id1 == point_id) {
                            SkinButton btn = getButton(new Point((int) event.getX(i), (int) event.getY(i)));
                            if (btn != null) {
                                int[] location = new int[2];
                                getLocationInWindow(location);
                                int x1 = location[0];
                                int y1 = location[1];
                                int popX = btn.getDispRect().centerX();
                                int popY = btn.getDispRect().top + y1;
                                if (mShowPop && !btn.getId().startsWith("VK_")) {
                                    mPopWin.prepareShow(this, popX, popY, id1, btn.getLabel());
                                } else {
                                    mPopWin.setNeedShow(false);
                                }
                            }

                            mDownPointMap.put(point_id, new Point((int) event.getX(i), (int) event.getY(i)));
                            break;
                        }
                    }

                    if (event.getPointerCount() == 1) {
                        SkinButton btn = getButton(new Point((int) event.getX(), (int) event.getY()));
                        if (btn != null && btn.getId().equals("VK_BACKSPACE")) {
                            mTimer = new Timer();
                            mKeyRepeatTimes = 0;
                            TimerTask task = new TimerTask() {
                                @Override
                                public void run() {
                                    post(new Runnable() {
                                        @Override
                                        public void run() {
                                            onKeyEvent("VK_BACKSPACE", KEY_REPEAT);
                                        }
                                    });
                                }
                            };
                            mTimer.schedule(task, 200, 80);
                        }
                    } else {
                        if (mTimer != null) {
                            mTimer.cancel();
                            mTimer.purge();
                            mTimer = null;
                        }
                    }

                    invalidate();
                }
                break;

            case MotionEvent.ACTION_MOVE:
                {
                    for (int i = 0; i < event.getPointerCount(); i++) {
                        int id1 = event.getPointerId(i);
                        {
                            if (mPopWin.isNeedShow() && id1 == mPopWin.getPointerId()) {
                                int dy = (int) event.getY(i) - mDownPointMap.get(id1).y;
                                SkinButton btnDown = getButton(mDownPointMap.get(id1));
                                if (btnDown != null && dy > 0)
                                {
                                    if (dy > btnDown.getDispRect().height() / 3 * 2) {
                                        mPopWin.update(0, 0, btnDown.getLabel().toUpperCase());
                                    }
                                    else {
                                        if (mPopWin.isShow()) {
                                            mPopWin.update(0, 0, btnDown.getLabel());
                                        }
                                    }
                                }
                                else {
                                    if (btnDown.getTag() != null && (-dy > btnDown.getDispRect().height() / 3 * 1)) {
                                        mPopWin.update(0, dy, btnDown.getTag());
                                    }
                                    else {
                                        if (mPopWin.isShow()) {
                                            mPopWin.update(0, dy, btnDown.getLabel());
                                        }
                                    }
                                }
                            }
                            invalidate();
                        }
                    }
                }
                break;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                {
                    for (int i = 0; i < event.getPointerCount(); i++) {
                        int id1 = event.getPointerId(i);
                        if (id1 == point_id)
                        {
                            SkinButton downButton = getButton(mDownPointMap.get(id1));
                            SkinButton upButton = getButton(new Point((int) event.getX(i), (int) event.getY(i)));

                            if (downButton != null) {
                                String id = downButton.getId();
                                if (upButton != null && downButton.getId().equals(upButton.getId())) {
                                    onKeyEvent(id, KEY_CLICK);
                                } else {
                                    int x0 = mDownPointMap.get(id1).x;
                                    int y0 = mDownPointMap.get(id1).y;
                                    int x1 = (int) event.getX(i);
                                    int y1 = (int) event.getY(i);

                                    if (id.equals("VK_BACKSPACE")) {
                                      if (x1 < x0) {
                                          onKeyEvent(id, KEY_SLID_LEFT);
                                      }

                                    } else {
                                        if (y1 < y0) {
                                            onKeyEvent(id, KEY_SLID_UP);
                                        } else {
                                            onKeyEvent(id, KEY_SLID_DOWN);
                                        }
                                    }
                                }
                            }
                            mPopWin.hide();
                            mDownPointMap.remove(point_id);
                        }
                    }

                    if (mTimer != null) {
                        mTimer.cancel();
                        mTimer.purge();
                        mTimer = null;
                    }
                    invalidate();
                }
                break;

            default:
                if (mTimer != null) {
                    mTimer.cancel();
                    mTimer.purge();
                    mTimer = null;
                }
                mPopWin.hide();
                mDownPointMap.clear();
                invalidate();
                break;

        }

        return true;
    }

    private void drawButton(Canvas canvas, SkinButton button, SkinButtonStyle style, SkinButtonStyleState styleState)
    {
        String showState = button.getShowState();
        if (showState != null) {
            if (!showState.equals(getShowState())) {
                return;
            }
        }

        Bitmap bitmap = mResPool.getBitmap(style.getNormalImgFile());

        if (isShiftlocked() && button.getLabel()!= null && button.getId().equals("VK_SHIFT")) {
            bitmap = mResPool.getBitmap(style.getShiftImgFile());
        }

        if (bitmap != null) {
            canvas.drawBitmap(bitmap,
                    new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()), button.getDispRect(), mPaint);
        }

        // 绘制button

        SkinButtonPaint btnPaint = styleState.getSkinPaint();
        Rect btnPadding = styleState.getBtnPadding();
        SkinBorder btnBorder = styleState.getBtnBorder();

        if (btnPaint != null)
        {
            RectF oval3 = new RectF(button.getDispRect().left + btnPadding.left,
                    button.getDispRect().top + btnPadding.top,
                    button.getDispRect().right - btnPadding.right,
                    button.getDispRect().bottom - btnPadding.bottom);

            // 背景
            if (btnPaint.getPaintBg() != null) {
                canvas.drawRect(button.getDispRect(), btnPaint.getPaintBg());
            }

            //阴影
            if (btnPaint.getPaintShadow() != null) {
                canvas.drawRoundRect(oval3, btnBorder.getRx(), btnBorder.getRy(), btnPaint.getPaintShadow());
            }

            // 前景
            if (btnPaint.getPaintFg() != null)  {
                canvas.drawRoundRect(oval3, btnBorder.getRx(), btnBorder.getRy(), btnPaint.getPaintFg());
            }

            //边框
            if (btnPaint.getPaintBorder() != null) {
                canvas.drawRoundRect(oval3, btnBorder.getRx(), btnBorder.getRy(), btnPaint.getPaintBorder());
            }
        }

        // 绘制label

        if (button.getLabel() != null && button.getLabel().length() > 0) {
            String imgFileName = getLabelImageFile(button.getLabel());
            if (imgFileName != null) {
                bitmap = mResPool.getBitmap(imgFileName);
                if (bitmap != null) {
                    double ratio = styleState.getLabelFont().getRatio();
                    int left = button.getDispRect().left + (button.getDispRect().width() - (int) (bitmap.getWidth() * ratio)) / 2;
                    int top = button.getDispRect().top + (button.getDispRect().height() - (int) (bitmap.getHeight() * ratio)) / 2;
                    canvas.drawBitmap(bitmap, new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()),
                            new Rect(left, top, (int) (left + bitmap.getWidth() * ratio), (int) (top + bitmap.getHeight() * ratio))
                            , mPaint);
                }
            }
            else {
                String strTagText = getDisplayTagString(button.getId());

                String strDispText = getDisplayString(button.getId());
                Rect rect = new Rect(button.getDispRect().left + btnPadding.left,
                        button.getDispRect().top + btnPadding.top,
                        button.getDispRect().right - btnPadding.right,
                        button.getDispRect().bottom - btnPadding.bottom);

                if (strTagText != null) {
                    rect.top += (rect.bottom - rect.top) / 6;
                }

                drawLabel(canvas, rect,ALIGN_CENTER | ALIGN_MIDDLE,  styleState.getLabelFont().getColor().getARGB(), styleState.getLabelFont().getSize(), strDispText);
            }
        }

        // 绘制tag
        String strTagText = getDisplayTagString(button.getId());

        if (strTagText != null) {

            Rect rect = new Rect(button.getDispRect().left + btnPadding.left,
                    button.getDispRect().top + btnPadding.top,
                    button.getDispRect().right - btnPadding.right,
                    button.getDispRect().bottom - btnPadding.bottom);

            rect.bottom = rect.top + (rect.bottom - rect.top) * 2 / 5;
            drawLabel(canvas, rect,ALIGN_CENTER | ALIGN_MIDDLE, styleState.getTagFont().getColor().getARGB(), styleState.getTagFont().getSize(), strTagText);
        }

    }
    private void drawButton(Canvas canvas, SkinButton button) {
        SkinButtonStyle style =  mKeyboardSkin.getButtonStyles().get(button.getStyleId());
        SkinButtonStyleState styleState =  style.getNormal();
        drawButton(canvas, button, style, styleState);
    }

    private String getLabelImageFile(String strLabel) {

        if (strLabel.equals("&language;")) {
            return "emoji.png";
        }

        if (strLabel.equals("&chinese;")) {
            return "en_cn.png";
        }

        if (strLabel.equals("&english;")) {
            return "cn_en.png";
        }

        if (strLabel.equals("&lock;")) {
            if (mSymbolLocked) {
                return "lock.png";
            } else {
                return "unlock.png";
            }
        }

        if (strLabel.equals("&shift;")) {
            if (mShiftMode == 0) {
                return "shift_normal.png";
            }
            else if (mShiftMode == 1)
            {
                return "shift_upper.png";
            }
            else if (mShiftMode == 2) {
                return "shift_locked.png";
            }

            return "shift_normal.png";
        }

        if (strLabel.equals("&backspace;")) {
            return "backspace.png";
        }

        if (strLabel.equals("&space;")) {
            return "space.png";
        }

        if (strLabel.equals("&menu;")) {
            return "menu.png";
        }

        if (strLabel.equals("&hide;")) {
            return "hide.png";
        }

        return null;
    }

    public String getDisplayString(String btnId) {

        for (int i=0; i<mKeyboardSkin.getButtons().size(); i++)
        {
            if (mKeyboardSkin.getButtons().get(i).getId().equals(btnId))
            {
                if (mKeyboardSkin.getButtons().get(i).getLabel().equals("&done;")) {
                    return mInputService.getFinishButtonLabel();
                }

                if (mShiftMode == 0) {
                    return mKeyboardSkin.getButtons().get(i).getLabel();
                }
                else
                {
                    return mKeyboardSkin.getButtons().get(i).getLabel().toUpperCase();
                }
            }
        }

        return "";
    }

    public String getDisplayTagString(String btnId) {

        for (int i=0; i<mKeyboardSkin.getButtons().size(); i++)
        {
            if (mKeyboardSkin.getButtons().get(i).getId().equals(btnId))
            {
                return mKeyboardSkin.getButtons().get(i).getTag();
            }
        }

        return "";
    }

    private void drawPressButton(Canvas canvas, SkinButton button) {
        SkinButtonStyle style =  mKeyboardSkin.getButtonStyles().get(button.getStyleId());
        SkinButtonStyleState styleState =  style.getPress();
        drawButton(canvas, button, style, styleState);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        if (mWidth == 0 && mHeight == 0) {

            mWidth = w;
            mHeight = h;

            mBitmap = Bitmap.createBitmap(mWidth, mHeight, Bitmap.Config.ARGB_8888);
            mCanvas = new Canvas(mBitmap);

            mBitmapPaint = new Paint();
            mPaint = new Paint();
            mPaint.setAntiAlias(true);
            mPaint.setStyle(Paint.Style.STROKE);
            RectF dirtyRect = new RectF(0, 0, w, h);
            drawBackgroup(mCanvas, dirtyRect);
            drawButtons(mCanvas, dirtyRect);
        }
    }

    private void drawImages(Canvas canvas, RectF dirtyRect) {
        List<SkinImage> images = mKeyboardSkin.getImages();
        if (images != null) {
            for (SkinImage img : images) {
                Bitmap bitmap = mResPool.getBitmap(img.getImgFile());
                if (bitmap != null) {
                    canvas.drawBitmap(bitmap, new Rect(0,0, bitmap.getWidth(), bitmap.getHeight()), img.getDispRect(), mPaint);
                }
            }
        }
    }

    private void drawBackgroup(Canvas canvas, RectF dirtyRect) {

        String bkImageFile = mKeyboardSkin.getBgImg();
        Bitmap bitmap = mResPool.getBitmap(bkImageFile);
        if (bitmap != null) {
            canvas.drawBitmap(bitmap, new Rect(0,0, bitmap.getWidth(), bitmap.getHeight()), mKeyboardSkin.getDispRect(), mPaint);
        }
        else
        {
            Paint p = new Paint();
            int argb = mKeyboardSkin.getBgColor().getARGB();
            p.setColor(argb);

            canvas.drawRect(mKeyboardSkin.getDispRect(), p);
        }
    }

    private void drawButtons(Canvas canvas, RectF dirtyRect) {
        List<SkinButton> buttons = mKeyboardSkin.getButtons();
        if (buttons != null) {
            for (SkinButton b : buttons) {
                drawButton(canvas, b);
            }
        }
    }

    private void drawPressButtons(Canvas canvas, RectF dirtyRect) {
        List<SkinButton> buttons = mKeyboardSkin.getButtons();
        if (buttons != null && mDownPointMap.size() > 0) {
            String background = mKeyboardSkin.getBgImg();
            Bitmap bitmap = mResPool.getBitmap(background);

            for (SkinButton b : buttons) {
                for (Point v : mDownPointMap.values()) {
                    if (b.getRealRect().contains(v.x, v.y)) {
                        if (bitmap != null) {
                            canvas.drawBitmap(bitmap, b.getDispRect(), b.getDispRect(), mPaint);
                        }
                        drawPressButton(canvas, b);
                    }
                }
            }
        }
    }

    private void drawLabel(Canvas canvas, Rect rect, int align, int color, int size, String text)
    {
        mPaint.setTextSize(size);
        mPaint.setColor(color);
        mPaint.setStyle(Paint.Style.FILL);
        Paint.FontMetricsInt fontMetrics = mPaint.getFontMetricsInt();

        if ((align & ALIGN_CENTER) != 0) {
            mPaint.setTextAlign(Paint.Align.CENTER);
        } else if ((align & ALIGN_RIGHT) != 0) {
            mPaint.setTextAlign(Paint.Align.RIGHT);
        } else {
            mPaint.setTextAlign(Paint.Align.LEFT);
        }

        if (align == (ALIGN_LEFT | ALIGN_TOP)) {
            canvas.drawText(text, rect.left, rect.top - fontMetrics.ascent, mPaint);
        } else {
            Rect minRect = new Rect();
            mPaint.getTextBounds(text,0, text.length(),minRect);

            int topY = rect.top - fontMetrics.ascent;
            int middleY = rect.top + rect.height() / 2 + (fontMetrics.bottom - fontMetrics.top) / 2 - fontMetrics.bottom;
            int bottomY = rect.bottom  - minRect.height() - fontMetrics.ascent - fontMetrics.descent;

            int centerX = rect.left + (rect.width()) / 2;
            int leftX = rect.left;
            int rightX = rect.right;

            if (align == (ALIGN_CENTER | ALIGN_TOP)) {
                canvas.drawText(text, centerX, topY, mPaint);
            }
            else if (align == (ALIGN_RIGHT | ALIGN_TOP)) {
                canvas.drawText(text, rightX, topY, mPaint);
            }
            else if (align == (ALIGN_LEFT | ALIGN_MIDDLE)) {
                canvas.drawText(text, leftX, middleY, mPaint);
            }
            else if (align == (ALIGN_CENTER | ALIGN_MIDDLE)) {
                canvas.drawText(text, centerX, middleY, mPaint);
            }
            else if (align == (ALIGN_RIGHT | ALIGN_MIDDLE)) {
                canvas.drawText(text, rightX, middleY, mPaint);
            }
            else if (align == (ALIGN_LEFT | ALIGN_BOTTOM)) {
                canvas.drawText(text, leftX, bottomY, mPaint);
            }
            else if (align == (ALIGN_CENTER | ALIGN_BOTTOM)) {
                canvas.drawText(text, centerX, bottomY, mPaint);
            }
            else if (align == (ALIGN_RIGHT | ALIGN_BOTTOM)) {
                canvas.drawText(text, rightX, bottomY, mPaint);
            }
            else {
                canvas.drawText(text, leftX, topY, mPaint);
            }
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        drawBackgroup(canvas, null);
        drawImages(canvas, null);
        drawButtons(canvas, null);
        drawPressButtons(canvas, null);

    }

    @Override
    public boolean isInit() {
        return mKeyboardSkin != null;
    }

    @Override
    public void init(IOperater operater, int methodId, SkinResPool resPool, SkinKeyboard keyboardSkin) {
        mInputMethodId = methodId;
        mResPool = resPool;
        mKeyboardSkin = keyboardSkin;
        mOperater = operater;

    }

    @Override
    public void show() {

    }

    @Override
    public void hide() {

    }

    @Override
    public void destory() {

    }
}
