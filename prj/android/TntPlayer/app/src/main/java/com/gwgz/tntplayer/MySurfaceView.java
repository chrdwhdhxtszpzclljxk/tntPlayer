package com.gwgz.tntplayer;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.support.annotation.NonNull;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.nio.Buffer;

/**
 * TODO: document your custom view class.
 */
public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback, Runnable  {

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

        a.recycle();
        this.surfaceHolder = this.getHolder();
        this.surfaceHolder.addCallback(this);
        this.obj = new GameObject();
    }

    private Thread thread; // SurfaceView通常需要自己单独的线程来播放动画
    private Canvas canvas;
    private SurfaceHolder surfaceHolder;
    public boolean bStop = false;
    public int w,h;

    private GameObject obj;

    private void as(){
        try {
            Thread.sleep(50); // 这个就相当于帧频了，数值越小画面就越流畅
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        Paint p = new Paint();

        while (!bStop) {
            //byte[] frame = MainActy.getVideoFrame();
            //Log.i("MySurface",String.valueOf(frame.length));

            try {
                Thread.sleep(200); // 这个就相当于帧频了，数值越小画面就越流畅
            } catch (Exception e) {
                e.printStackTrace();
            }



            //BitmapFactory.Options op = new BitmapFactory.Options();
            //Bitmap bm = BitmapFactory.decodeByteArray(frame,0,frame.length, op);
            /*
            obj.getNextPos();
            canvas = this.surfaceHolder.lockCanvas(); // 通过lockCanvas加锁并得到該SurfaceView的画布
            if(canvas != null) {
                canvas.drawColor(Color.BLACK);
                obj.drawSelf(canvas); // 把SurfaceView的画布传给物件，物件会用这个画布将自己绘制到上面的某个位置
                this.surfaceHolder.unlockCanvasAndPost(canvas); // 释放锁并提交画布进行重绘
                try {
                    //Thread.sleep(10); // 这个就相当于帧频了，数值越小画面就越流畅
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            */
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
            //this.img = BitmapFactory.decodeByteArray()
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
