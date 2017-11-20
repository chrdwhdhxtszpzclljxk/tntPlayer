package com.gwgz.tntplayer;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

/**
 * TODO: document your custom view class.
 */
public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback, Runnable  {
    private String mExampleString; // TODO: use a default from R.string...
    private int mExampleColor = Color.RED; // TODO: use a default from R.color...
    private float mExampleDimension = 0; // TODO: use a default from R.dimen...
    private Drawable mExampleDrawable;

    private TextPaint mTextPaint;
    private float mTextWidth;
    private float mTextHeight;

    public MySurfaceView(Context context) {
        super(context);
        init(null, 0);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(attrs, 0);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init(attrs, defStyle);
    }

    private void init(AttributeSet attrs, int defStyle) {
        // Load attributes
        final TypedArray a = getContext().obtainStyledAttributes(
                attrs, R.styleable.MySurfaceView, defStyle, 0);

        mExampleString = a.getString(
                R.styleable.MySurfaceView_exampleString);
        mExampleColor = a.getColor(
                R.styleable.MySurfaceView_exampleColor,
                mExampleColor);
        // Use getDimensionPixelSize or getDimensionPixelOffset when dealing with
        // values that should fall on pixel boundaries.
        mExampleDimension = a.getDimension(
                R.styleable.MySurfaceView_exampleDimension,
                mExampleDimension);

        if (a.hasValue(R.styleable.MySurfaceView_exampleDrawable)) {
            mExampleDrawable = a.getDrawable(
                    R.styleable.MySurfaceView_exampleDrawable);
            mExampleDrawable.setCallback(this);
        }

        a.recycle();

        // Set up a default TextPaint object
        mTextPaint = new TextPaint();
        mTextPaint.setFlags(Paint.ANTI_ALIAS_FLAG);
        mTextPaint.setTextAlign(Paint.Align.LEFT);

        // Update TextPaint and text measurements from attributes
        invalidateTextPaintAndMeasurements();

        this.surfaceHolder = this.getHolder();
        this.surfaceHolder.addCallback(this);
        this.obj = new GameObject();
    }

    private void invalidateTextPaintAndMeasurements() {
        mTextPaint.setTextSize(mExampleDimension);
        mTextPaint.setColor(mExampleColor);
        mTextWidth = mTextPaint.measureText(mExampleString);

        Paint.FontMetrics fontMetrics = mTextPaint.getFontMetrics();
        mTextHeight = fontMetrics.bottom;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        // TODO: consider storing these as member variables to reduce
        // allocations per draw cycle.
        int paddingLeft = getPaddingLeft();
        int paddingTop = getPaddingTop();
        int paddingRight = getPaddingRight();
        int paddingBottom = getPaddingBottom();

        int contentWidth = getWidth() - paddingLeft - paddingRight;
        int contentHeight = getHeight() - paddingTop - paddingBottom;

        // Draw the text.
        canvas.drawText(mExampleString,
                paddingLeft + (contentWidth - mTextWidth) / 2,
                paddingTop + (contentHeight + mTextHeight) / 2,
                mTextPaint);

        // Draw the example drawable on top of the text.
        if (mExampleDrawable != null) {
            mExampleDrawable.setBounds(paddingLeft, paddingTop,
                    paddingLeft + contentWidth, paddingTop + contentHeight);
            mExampleDrawable.draw(canvas);
        }
    }

    /**
     * Gets the example string attribute value.
     *
     * @return The example string attribute value.
     */
    public String getExampleString() {
        return mExampleString;
    }

    /**
     * Sets the view's example string attribute value. In the example view, this string
     * is the text to draw.
     *
     * @param exampleString The example string attribute value to use.
     */
    public void setExampleString(String exampleString) {
        mExampleString = exampleString;
        invalidateTextPaintAndMeasurements();
    }

    /**
     * Gets the example color attribute value.
     *
     * @return The example color attribute value.
     */
    public int getExampleColor() {
        return mExampleColor;
    }

    /**
     * Sets the view's example color attribute value. In the example view, this color
     * is the font color.
     *
     * @param exampleColor The example color attribute value to use.
     */
    public void setExampleColor(int exampleColor) {
        mExampleColor = exampleColor;
        invalidateTextPaintAndMeasurements();
    }

    /**
     * Gets the example dimension attribute value.
     *
     * @return The example dimension attribute value.
     */
    public float getExampleDimension() {
        return mExampleDimension;
    }

    /**
     * Sets the view's example dimension attribute value. In the example view, this dimension
     * is the font size.
     *
     * @param exampleDimension The example dimension attribute value to use.
     */
    public void setExampleDimension(float exampleDimension) {
        mExampleDimension = exampleDimension;
        invalidateTextPaintAndMeasurements();
    }

    /**
     * Gets the example drawable attribute value.
     *
     * @return The example drawable attribute value.
     */
    public Drawable getExampleDrawable() {
        return mExampleDrawable;
    }

    /**
     * Sets the view's example drawable attribute value. In the example view, this drawable is
     * drawn above the text.
     *
     * @param exampleDrawable The example drawable attribute value to use.
     */
    public void setExampleDrawable(Drawable exampleDrawable) {
        mExampleDrawable = exampleDrawable;
    }

    private Thread thread; // SurfaceView通常需要自己单独的线程来播放动画
    private Canvas canvas;
    private SurfaceHolder surfaceHolder;
    public boolean bStop = false;

    private GameObject obj;

    //public GwgzVideoView(Context c) {
    //    super(c);

    //}

    @Override
    public void run() {
        while (!bStop) {
            obj.getNextPos();
            canvas = this.surfaceHolder.lockCanvas(); // 通过lockCanvas加锁并得到該SurfaceView的画布
            if(canvas != null) {
                canvas.drawColor(Color.BLACK);
                obj.drawSelf(canvas); // 把SurfaceView的画布传给物件，物件会用这个画布将自己绘制到上面的某个位置
                this.surfaceHolder.unlockCanvasAndPost(canvas); // 释放锁并提交画布进行重绘
                try {
                    Thread.sleep(10); // 这个就相当于帧频了，数值越小画面就越流畅
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder arg0) {
        bStop = true;
        //Toast.makeText(getApplicationContext(), "SurfaceView已经销毁", Toast.LENGTH_LONG).show();
    }

    @Override
    public void surfaceCreated(SurfaceHolder arg0) {
        //Toast.makeText(getApplicationContext(), "SurfaceView已经创建", Toast.LENGTH_LONG).show();
        bStop = false;
        this.thread = new Thread(this);
        this.thread.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
        // 这里是SurfaceView发生变化的时候触发的部分
    }


    /*
     * 这个类用来当测试的物件，会沿着方形路线持续移动
     */
    class GameObject {
        private float x;
        private float y;
        private Bitmap img;
        private Paint paint;

        public GameObject() {
            this.img = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher);
            this.x = 100;
            this.y = 100;
            this.paint = new Paint();
        }

        // 在SurfaceView加锁同步后传给自己的Canvas上绘制自己
        public void drawSelf(Canvas canvas) {
            canvas.drawBitmap(img, x, y, paint);
        }

        // 获取物件下一次要绘制的位置(这里是沿着一个边长为400的正方形不断运动的)
        public void getNextPos() {
            if (y == 100 && x != 500)
                x += 5;
            else if (x == 500 && y != 500)
                y += 5;
            else if (y == 500 && x != 100)
                x -= 5;
            else if (x == 100 && y != 100)
                y -= 5;
        }
    }

}
