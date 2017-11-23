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
 */

package com.gwgz.tntplayer;

// Wrapper for native library

import android.graphics.Bitmap;
import android.view.Surface;

import java.nio.ByteBuffer;

public class GL2JNILib {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init(int width, int height);
     public static native void step();
    public native static short initNdkApp();
    public native static short gtmvreaderPush(String type,String tnow,String pubid,long filelen);
    public native static void setWritablePath(String path);
    public native static void setUnPwd(String un,String pwd,String id);
    public native static void setAssetManager(Object o);
    public native static void audioEngineStop();
    public native static void getVideoFrame(Bitmap bitmap);
    public native static boolean getVideoFrame1(ByteBuffer buf, int len);
    public native static boolean setVideoSurface(Surface surface);
    public native static String getVideowh();
    public static native void init(Object glSurface);
    public static native void step(String filepath);
}
