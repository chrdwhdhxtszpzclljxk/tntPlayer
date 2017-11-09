#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include <cocos2d.h>
//#include "CCSGUIReader.h"
#include "NetClient.h"
#include "NetGate.h"
#include "jsonrecordset.h"
#include "homework.h"

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  App : private cocos2d::Application{
public:
    App();
    virtual ~App();
	static bool helper;
	static xiny120::json::sql::recordset mlogin;
	static xiny120::homework::level mhomeworklevel;
	static rapidjson::Document* jsonDict;
	static char pwdmd5[16]; static int userid, classid,groupid;
	static unsigned char* a192k; static int64_t a192klen,a192klenloaded;
	static std::string shelperback, shelperpause, shelperseek, swaiting_for_connect;
	static std::string shomeworkuploadtry, sokbutton, lessionId, snetfail, snetrequest, sareusuredel;
	static std::string shomeworkuploadsucess, shomeworkuploadfail, shomeworkupload, shomeworkpass, shomeworkneed, shomeworkdanwei, shomeworktitle;
	static std::string sCameraPath, sCameraBlock, shomeworkhit;
	static std::string syear, smonth, sday, shour, sminute, ssecond, sclickandplay, notadv,spause, stitle;
	static std::string areusure, okdown, canceldown, avrspeed, perc, downloadandsee, downloading, downandplay;
	static std::string fileioerror, hostioerror, nopayment, redownload, updateidx, gpk, gsk, s117, vcd40, mima1, gjzsgz;
	static std::string waiting_for_login, un_or_pwd_empty, un_or_pwd_typein, un_typein, pwd_typein;
	static std::string ssmallwin, shelptip, sshowpwd, scancelfornow, sexittip;
	static tm localtime(const int64_t& now);
	static rapidjson::Document* getErrorMsg();
	static std::string basePath;
	static NetClient* getSocketMgr();
	//static NetGate* getSocketGate();
	static const char* getInfo(const rapidjson::Value& root,const char*);
	static bool load192k(const int64_t&, unsigned char* , int64_t& len);
	static void Sleep(int32_t);
	static std::string getgtmv(const int64_t& pub, const int64_t& year, const int64_t&);
	static std::string getgtmv(const int64_t& pub, const int64_t& year, const tm&);
	static std::string getgtmvidx(const int64_t& pub, const int64_t& year);
	static std::string getgtmvdir(const int64_t& pub, const int64_t& year, const int64_t&);
	static std::string getgtmvdir(const int64_t& pub, const int64_t& year, const tm&);
	static std::string getusername();
	static void setusername(const std::string&);
	static std::string getuserpwd();
	static void setuserpwd(const std::string&);

    virtual void initGLContextAttrs();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

