package com.gwgz.tntplayer;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

/**
 * Created by Administrator on 2017-11-14.
 */

public class DownloadItem {
    public String tnow;
    public String pubid;
    public String geturlp(){
        String urlpath = "https://gpk01.gwgz.com:666/ashx/login.ashx?pubid=" + pubid +
                "&un=" + MainActy.un + "&pwd=" + MainActy.pwd + "&tnow=" + tnow + "&cmd=play"+
                "&type=0";
        return urlpath;
    }
    public String geturl(){
        Date d = new Date(Long.parseLong(tnow) * 1000);
        SimpleDateFormat dateFormat =new SimpleDateFormat("yyyy/MM/yyyy-MM-dd_HH_mm_ss");
        TimeZone timeZone = TimeZone.getTimeZone("GMT+8");
        dateFormat.setTimeZone(timeZone);
        String filepath = dateFormat.format(d);
        String urlpath = "https://gpk01.gwgz.com:666/data/" + pubid + "/" + filepath + ".gtmv";

        return urlpath;
    }
    public String getlocal(){
        String localfile = MainActy.basePath + "/data/"+pubid+"/" + tnow + ".gtmv";
        return localfile;
    }
    public String getpath(){
        String localfile = "/data/"+pubid+"/";
        return localfile;
    }
    public String getfile(){
        String localfile =  tnow + ".gtmv";
        return localfile;
    }

}
