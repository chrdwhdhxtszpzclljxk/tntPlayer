package com.gwgz.tntplayer;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Looper;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.os.Handler;
import android.text.format.Time;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.webkit.JsPromptResult;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONStringer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Set;
import java.util.TimeZone;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 */
public class PlayActy extends AppCompatActivity {
    MyHandler myHandler;
    /**
     * Whether or not the system UI should be auto-hidden after
     * {@link #AUTO_HIDE_DELAY_MILLIS} milliseconds.
     */
    private static final boolean AUTO_HIDE = true;

    /**
     * If {@link #AUTO_HIDE} is set, the number of milliseconds to wait after
     * user interaction before hiding the system UI.
     */
    private static final int AUTO_HIDE_DELAY_MILLIS = 3000;

    /**
     * Some older devices needs a small delay between UI widget updates
     * and a change of the status and navigation bar.
     */
    private static final int UI_ANIMATION_DELAY = 300;
    private final Handler mHideHandler = new Handler();
    private View mContentView;
    private final Runnable mHidePart2Runnable = new Runnable() {
        @SuppressLint("InlinedApi")
        @Override
        public void run() {
            // Delayed removal of status and navigation bar

            // Note that some of these constants are new as of API 16 (Jelly Bean)
            // and API 19 (KitKat). It is safe to use them, as they are inlined
            // at compile-time and do nothing on earlier devices.
            mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        }
    };
    private View mControlsView;
    private final Runnable mShowPart2Runnable = new Runnable() {
        @Override
        public void run() {
            // Delayed display of UI elements
            ActionBar actionBar = getSupportActionBar();
            if (actionBar != null) {
                actionBar.show();
            }
            mControlsView.setVisibility(View.VISIBLE);
        }
    };
    private boolean mVisible;
    private final Runnable mHideRunnable = new Runnable() {
        @Override
        public void run() {
            hide();
        }
    };
    /**
     * Touch listener to use for in-layout UI controls to delay hiding the
     * system UI. This is to prevent the jarring behavior of controls going away
     * while interacting with activity UI.
     */
    private final View.OnTouchListener mDelayHideTouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View view, MotionEvent motionEvent) {
            if (AUTO_HIDE) {
                //delayedHide(AUTO_HIDE_DELAY_MILLIS);
            }
            return false;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(new MySurfaceView(getApplicationContext())); // 别忘了开始的时候载入我们加工好的的SurfaceView
        setContentView(R.layout.acty_play);
        myHandler = new MyHandler();
        Bundle bundle = this.getIntent().getExtras();
        String type = bundle.getString("type");
        String tnow = bundle.getString("tnow");
        String pubid = bundle.getString("pubid");
        mVisible = true;
        //mContentView = new MySurfaceView(getApplicationContext());//findViewById(R.id.fullscreen_content);
        //FrameLayout fl = findViewById(R.id.framelayout_main);

        //FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
         //       new FrameLayout.LayoutParams(FrameLayout.LayoutParams.FILL_PARENT,
          //              FrameLayout.LayoutParams.FILL_PARENT));

        //fl.addView(mContentView);
        mControlsView = findViewById(R.id.fullscreen_content_controls);
        mContentView = findViewById(R.id.fullscreen_content);
        mContentView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggle();
            }
        });

        findViewById(R.id.dummy_button).setOnTouchListener(mDelayHideTouchListener);
        InitWebView();


        try {
            getHttpfileLength(tnow,pubid);
            return;
        }catch(Exception e){
            System.out.println(e.toString());
        }

    }




    private void  getHttpfileLength(String tnow0,String pubid0) {
        final String key = pubid0 + tnow0;
        synchronized (MainActy.listdl) {
            if (MainActy.listdl.containsKey(key)) {
                Toast.makeText(PlayActy.this, "该课件已在缓存列表！请稍后...", Toast.LENGTH_LONG).show();
                return; // 正在下载中。。。
            }
        }
        final DownloadItem di = new DownloadItem();
        di.tnow = tnow0;
        di.pubid = pubid0;
        new Thread(new Runnable(){
            @Override
            public void run() {
                try {
                    HttpDownloader downloader = new HttpDownloader();
                    String url0 = di.geturlp();
                    String json = downloader.download(di.geturlp());
                    JSONObject jso = new JSONObject(json);
                    if(jso.getInt("cs") != 1){
                        Message msg = new Message();
                        Bundle b = new Bundle();// 存放数据
                        b.putString("cmd","toast");
                        b.putString("msg",jso.getString("err"));
                        msg.setData(b);
                        PlayActy.this.myHandler.sendMessage(msg);
                        return;
                    }
                    URL u = new URL(di.geturl());
                    HttpURLConnection urlcon = (HttpURLConnection) u.openConnection();
                    urlcon.setRequestProperty("Accept-Encoding", "identity");
                    int fileLength = urlcon.getContentLength();
                    di.filesize = fileLength;
                    File f = new File(di.getlocal());
                    long len = f.length();
                    if(len == fileLength){ // ready to play!
                        //MainActy.gtmvreaderPush("0",di.tnow,di.pubid,fileLength);
                    }else {
                        synchronized (MainActy.listdl) {
                            MainActy.listdl.put(key,di);
                        }
                        try {
                            f.delete();
                        }catch (Exception e){

                        }
                        final String key0 = key;
                        new Thread(new Runnable(){
                            @Override
                            public void run() {
                                DownloadItem di = null;
                                synchronized (MainActy.listdl){
                                    di = MainActy.listdl.get(key0);
                                }
                                if(di != null) {
                                    //show();
                                    HttpDownloader downloader = new HttpDownloader();
                                    int result = downloader.downloadFile(di.geturl(), di.getpath(),di.getfile());//"/data/" + pubid0 + "/", tnow0 + ".gtmv");
                                    synchronized (MainActy.listdl){
                                        MainActy.listdl.remove(key0);
                                    }
                                }
                            }
                        }).start();
                    }

                    MainActy.gtmvreaderPush("0",di.tnow,di.pubid,fileLength);
                }catch(Exception e){
                    Log.d("PlayActy",e.getMessage());
                }
            }
        }).start();
    }

    /**
     * 接受消息,处理消息 ,此Handler会与当前主线程一块运行
     * */

    class MyHandler extends Handler {
        public MyHandler() {
        }

        public MyHandler(Looper L) {
            super(L);
        }

        // 子类必须重写此方法,接受数据
        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub

            super.handleMessage(msg);
            // 此处可以更新UI
            Bundle b = msg.getData();
            String cmd = b.getString("cmd");
            if(cmd.equals("toast")){
                Toast.makeText(PlayActy.this,b.getString("msg"),Toast.LENGTH_LONG).show();
            }else if(cmd.equals("dlprogress")){
                //Toast.makeText(PlayActy.this,"正在下载...",Toast.LENGTH_LONG).show();
            }
            //PlayActy.this.button.append(color);


        }
    }

    @Override
    protected void onPause() {
        MainActy.audioEngineStop();
        super.onPause();
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        // Trigger the initial hide() shortly after the activity has been
        // created, to briefly hint to the user that UI controls
        // are available.
        delayedHide(3000);
    }

    private void toggle() {
        if (mVisible) {
            hide();
        } else {
            show();
        }
    }

    private void hide() {
        // Hide UI first
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.hide();
        }
        mControlsView.setVisibility(View.GONE);
        mVisible = false;

        // Schedule a runnable to remove the status and navigation bar after a delay
        mHideHandler.removeCallbacks(mShowPart2Runnable);
        mHideHandler.postDelayed(mHidePart2Runnable, UI_ANIMATION_DELAY);
    }

    @SuppressLint("InlinedApi")
    private void show() {
        // Show the system bar
        mContentView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
        mVisible = true;

        // Schedule a runnable to display UI elements after a delay
        mHideHandler.removeCallbacks(mHidePart2Runnable);
        mHideHandler.postDelayed(mShowPart2Runnable, UI_ANIMATION_DELAY);
    }

    /**
     * Schedules a call to hide() in delay milliseconds, canceling any
     * previously scheduled calls.
     */
    private void delayedHide(int delayMillis) {
        mHideHandler.removeCallbacks(mHideRunnable);
        mHideHandler.postDelayed(mHideRunnable, delayMillis);
    }

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE};

    public static void verifyStoragePermissions(Activity activity) {
        // Check if we have write permission
        int permission = ActivityCompat.checkSelfPermission(activity,
                Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permission != PackageManager.PERMISSION_GRANTED) {
            // We don't have permission so prompt the user
            ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE,
                    REQUEST_EXTERNAL_STORAGE);
        }
    }


    public class Filesys {
        private String SDPATH = null;
        public String getSDPath(){
            return SDPATH;
        }

        //构造函数
        public Filesys(String path){
            SDPATH = path;

        }

        //在SDCARD中创建文件
        public File createSDFile(String fileName) throws IOException {
            File file = new File(SDPATH + fileName);
            file.createNewFile();
            return file;
        }


        //在SDCARD中创建目录
        public File createSDDir(String dirName){
            File dir = new File(SDPATH + dirName);
            dir.mkdirs();
            Log.i("filesys","create :" + SDPATH + dirName);
            return dir;
        }

        //判断SD卡上的文件是否存在
        public boolean isFileExist(String fileName){
            File file = new File(SDPATH + fileName);
            Log.i("filesys","isFileExist :" + SDPATH + fileName);
            return file.exists();
        }

        //判断SD卡上的文件是否存在
        public void remove(String fileName){
            File file = new File(SDPATH + fileName);
            if (file.isFile() && file.exists()) {
                file.delete();
            }
            Log.i("filesys","remove :" + SDPATH + fileName);
        }

        //将inputStream中的数据写入到SD卡中
        public File write2SDFromInput(String path, String fileName,InputStream input){
            File file = null;
            OutputStream output = null;
            int len = -1;
            try{
                createSDDir(path);
                file = createSDFile(path + fileName);
                output = new FileOutputStream(file);

                byte buffer[] = new byte[4 * 1024];
                while((len = input.read(buffer)) != -1){
                    //output.write(buffer);  //使用此函数会导致下载的文件偏大
                    output.write(buffer, 0, len);
                    Message msg = new Message();
                    Bundle b = new Bundle();// 存放数据
                    b.putString("cmd","dlprogress");
                    b.putString("dlprogress", String.valueOf(len));
                    msg.setData(b);
                    Log.d("MyHandler", "sendMessage......" + String.valueOf(len) + fileName);
                    PlayActy.this.myHandler.sendMessage(msg);
                }
                input.close();
                output.flush();
            }catch(Exception e){
                e.printStackTrace();
            }finally{
                try{
                    output.close();
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
            return file;
        }
    }


    public class HttpDownloader {
        private URL url = null;
        /*根据url下载文本文件*/
        public String download(String urlString){
            StringBuffer sb = new StringBuffer();
            BufferedReader buffer = null;
            String line = null;
            try{
                url = new URL(urlString);
                HttpURLConnection urlConnection = (HttpURLConnection)url.openConnection();
            /*使用IO流读取数据getInputStream()  return 字节流InputStreamReader()   return 字符流*/
                buffer = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                while((line = buffer.readLine()) != null){
                    sb.append(line);
                }
            }catch(Exception e){
                e.printStackTrace();
            }finally{
                try{
                    buffer.close();
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
            return sb.toString();
        }


        //下载任意格式的文件并且保存到SD card
    /* return:
    * 1  表示文件已经存在
    * -1 保存出错
    * 0 表示成功
    *
    * param:
    * urlStr 下载的原链接
    * fileName  保存在SDcard中的名字
    * path  保存在SDcard下的目录
    */
        public int downloadFile(String urlStr, String path, String fileName){
            InputStream inputStream = null;
            try{
                Filesys filesys = new Filesys(MainActy.basePath);
                if(filesys.isFileExist(path + fileName)){
                    filesys.remove(path+fileName);
                }
                inputStream = getInputStreamFromUrlStr(urlStr);
                File resultFile = filesys.write2SDFromInput(path, fileName, inputStream);
                if(resultFile == null)
                    return -1;
            }catch(Exception e){
                e.printStackTrace();
                return -1;
            }finally{
                try{
                    inputStream.close();
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
            return 0;
        }

        public InputStream getInputStreamFromUrlStr(String urlStr) throws IOException {
            InputStream input = null;
            URL url = new URL(urlStr);
            HttpURLConnection urlConnection = (HttpURLConnection)url.openConnection();
            input = urlConnection.getInputStream();
            return input;
        }

    }

    protected void InitWebView(){
        WebView wv = findViewById(R.id.dummy_button);
        wv.setBackgroundColor(0);
        WebSettings wst = wv.getSettings();
        wst.setJavaScriptEnabled(true);
        wst.setAllowFileAccessFromFileURLs(true);
        wst.setAllowUniversalAccessFromFileURLs(true);
        wst.setJavaScriptCanOpenWindowsAutomatically(true);

        wv.setWebChromeClient(
                new WebChromeClient(){
                    @Override
                    public boolean onJsPrompt(WebView view, String url, String message, String defaultValue, JsPromptResult result) {
                        Uri uri = Uri.parse(message);
                        if ( uri.getScheme().equals("js")) {
                            if (uri.getAuthority().equals("cpp")) {
                                System.out.println("js调用了Android的方法" + message);
                                HashMap<String, String> params = new HashMap<>();
                                Set<String> collection = uri.getQueryParameterNames();
                                String acionData = uri.getQueryParameter("cmd");
                                String par1 = uri.getQueryParameter("arg1");

                                if(acionData.equals("toast")){
                                    Toast.makeText(PlayActy.this, par1, Toast.LENGTH_LONG).show();
                                }else if(acionData.equals("play")){
                                    String type = uri.getQueryParameter("type");
                                    String tnow = uri.getQueryParameter("tnow");
                                    String pubid = uri.getQueryParameter("pubid");
                                    Intent intent = new Intent();
                                    intent.setClass(PlayActy.this, PlayActy.class);
                                    Bundle bundle=new Bundle();
                                    bundle.putString("type",type );
                                    bundle.putString("tnow",tnow );
                                    bundle.putString("pubid",pubid );
                                    intent.putExtras(bundle);
                                    startActivity(intent);
                                }else if(acionData.equals("login")){

                                }
                                result.confirm("js调用了Android的方法成功啦");
                            }
                            return true;
                        }
                        return super.onJsPrompt(view, url, message, defaultValue, result);
                    }

                    @Override
                    public boolean onJsAlert(WebView view, String url, String message, JsResult result) {
                        return super.onJsAlert(view, url, message, result);
                    }

                    @Override
                    public boolean onJsConfirm(WebView view, String url, String message, JsResult result) {
                        return super.onJsConfirm(view, url, message, result);
                    }
                }
        );

        wv.setWebViewClient(new WebViewClient(){
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                Log.i("webview", "url = "+url);
                view.loadUrl(url);
                return true;
            }
            @Override
            public void onReceivedError(WebView view, int errorCode,
                                        String description, String failingUrl) {
                super.onReceivedError(view, errorCode, description, failingUrl);
                //这里进行无网络或错误处理，具体可以根据errorCode的值进行判断，做跟详细的处理。
                view.loadUrl("file:///android_asset/www/error.html");
            }
            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                Log.i("onPageStarted", url);
                super.onPageStarted(view, url, favicon);
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                Log.i("onPageFinished", url);
                super.onPageFinished(view, url);
            }
        });



        wv.loadUrl("file:///android_asset/www/playcontroler.html");
    }


}
