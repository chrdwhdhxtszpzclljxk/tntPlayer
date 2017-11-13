package com.gwgz.tntplayer;

import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Administrator on 2017-11-13.
 */

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
