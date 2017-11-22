package com.gwgz.tntplayer;

import android.content.Context;
import android.view.SurfaceView;

/**
 * Created by Administrator on 2017-11-22.
 */

public class ViERenderer
{
    public static SurfaceView CreateRenderer(Context context) {
        return CreateRenderer(context, false);
    }

    public static SurfaceView CreateRenderer(Context context,
                                             boolean useOpenGLES2) {
        if(useOpenGLES2 == true && ViEAndroidGLES20.IsSupported(context))
            return new ViEAndroidGLES20(context);
        else
            return null;
    }
}