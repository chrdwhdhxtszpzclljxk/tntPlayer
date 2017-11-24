package com.gwgz.tntplayer;

/**
 * Created by Administrator on 2017-11-24.
 */

/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: mnorst@foxmail.com
 */

        import java.io.IOException;
        import java.io.InputStream;

        import javax.microedition.khronos.egl.EGLConfig;
        import javax.microedition.khronos.opengles.GL10;


        import android.content.Context;
        import android.content.res.TypedArray;
        import android.graphics.Bitmap;
        import android.graphics.BitmapFactory;
        import android.graphics.Paint;
        import android.opengl.GLSurfaceView;
        import android.opengl.GLUtils;
        import android.text.TextPaint;
        import android.util.AttributeSet;
        import android.util.Log;
        import com.google.zxing.BarcodeFormat;
        import com.google.zxing.EncodeHintType;
        import com.google.zxing.MultiFormatWriter;
        import com.google.zxing.WriterException;
        import com.google.zxing.common.BitMatrix;
        import com.google.zxing.qrcode.QRCodeWriter;

/**
 * A simple GLSurfaceView sub-class that demonstrate how to perform
 * OpenGL ES 1.x rendering into a GL Surface.
 */
public class GLJNIView extends GLSurfaceView {

    private static final String LOG_TAG = GLJNIView.class.getSimpleName();

    private Renderer renderer;

    /**
     * 自定义View时,如果自定义View有属性,这个方法一定要覆写!!
     *
     * @param context
     * @param attrs
     */
    public GLJNIView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context,attrs);
    }

    public GLJNIView(Context context) {
        super(context);
        init(context,null);
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        //用于纹理映射的绑定，并把绑定后的ID传递给C++代码，供其调用
        private int[] mTexture = new int[2];
        //用于加载Bitmap的context
        private Context mContext;
        public Renderer(Context ctx) {
            mContext = ctx;
        }

        public void onDrawFrame(GL10 gl) {
            long l = GL2JNILib.step();

            long base = 1000000;
            long totalf = l / base;
            long curf = l  - totalf * base;
            Log.d("", "onDrawFrame: " + Long.toString(totalf) + "," + Long.toString(curf));
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GL2JNILib.resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            //用来绑定Bitmap纹理
            genTexture(gl, mContext);
            //调用本地setTexture方法，把纹理绑定的ID传递给C++代码，以供其调用
            GL2JNILib.setTexture(mTexture);
            GL2JNILib.init();
        }

        /**
         * 加载Bitmap的方法，
         * 用来从res中加载Bitmap资源
         * */
        private Bitmap loadBitmap(Context context, int resourceId) {
            InputStream is = context.getResources().openRawResource(resourceId);
            Bitmap bitmap = null;
            try {

                // 利用BitmapFactory生成Bitmap
                bitmap = BitmapFactory.decodeStream(is);
            } finally {
                try {

                    // 关闭流
                    is.close();
                    is = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
            return bitmap;

        }

        /**
         * 绑定Bitmap纹理
         * */

        private void genTexture(GL10 gl, Context context) {
            //生成纹理
            gl.glGenTextures(2, mTexture, 0);
            //加载Bitmap
            Bitmap bitmap = loadBitmap(context, R.mipmap.ic_launcher);
            if (bitmap != null) {
                //如果bitmap加载成功，则生成此bitmap的纹理映射
                gl.glBindTexture(GL10.GL_TEXTURE_2D, mTexture[0]);
                //设置纹理映射的属性
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER,
                        GL10.GL_NEAREST);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER,
                        GL10.GL_NEAREST);
                //生成纹理映射
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
                //释放bitmap资源
                bitmap.recycle();
            }

        }
    }


    private void init(Context context, AttributeSet attrs ) {
        // setEGLConfigChooser会对fps产生影响
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);

        renderer = new Renderer(context);
        setRenderer(renderer);
    }


}