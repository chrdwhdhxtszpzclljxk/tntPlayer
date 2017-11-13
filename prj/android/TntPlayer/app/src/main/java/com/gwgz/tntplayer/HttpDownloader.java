package com.gwgz.tntplayer;

import android.os.Environment;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

/**
 * Created by Administrator on 2017-11-13.
 */

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
            /*
            if(filesys.isFileExist(path + fileName)){
                return 1;
            }else{
                inputStream = getInputStreamFromUrlStr(urlStr);
                File resultFile = filesys.write2SDFromInput(path, fileName, inputStream);
                if(resultFile == null)
                    return -1;
            }
            */
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

