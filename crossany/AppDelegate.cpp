#include "AudioEngine.h"
#include "AppDelegate.h"
//#include "HelloWorldScene.h"
#include "SceneLogin.h"
#include "SocketClient.h"
#include "SceneMain.h"
#include "DataLib.h"
#include "cryptopp562/dll.h"
#include <cocos2d.h>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <cocostudio\CocoStudio.h>

USING_NS_CC;

App::App() {

}

App::~App(){
	xiny120::dct::destroy();
	if (jsonDict != NULL) delete jsonDict;
}

tm App::localtime(const int64_t& _now){
	int64_t now = _now, nowlimit = LLONG_MAX;

	if (now < 0 || now >= nowlimit) now = 0;
	time_t now1 = now;
	return *::localtime(&now1);
}


//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void App::initGLContextAttrs(){
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool App::helper;
xiny120::json::sql::recordset App::mlogin; xiny120::homework::level App::mhomeworklevel;
rapidjson::Document* App::jsonDict = NULL; char App::pwdmd5[16];
int App::userid, App::classid, App::groupid;
unsigned char* App::a192k = NULL; int64_t App::a192klen = 0, App::a192klenloaded = 0;
std::string App::shelperback, App::shelperpause, App::shelperseek, App::swaiting_for_connect;
std::string App::shomeworkuploadsucess, App::shomeworkuploadfail, App::shomeworkupload, App::snetfail, App::snetrequest;
std::string App::shomeworkpass, App::shomeworkneed, App::shomeworkdanwei, App::shomeworktitle;
std::string App::shomeworkuploadtry, App::sokbutton, App::lessionId, App::sareusuredel;
std::string App::sCameraPath = "d:\\1A0001.jpg", App::sCameraBlock, App::shomeworkhit;
std::string App::basePath = ".\\", App::spause, App::stitle;
std::string App::syear, App::smonth, App::sday, App::shour, App::sminute, App::ssecond, App::sclickandplay, App::downandplay; 
std::string App::areusure, App::okdown, App::canceldown, App::avrspeed, App::perc, App::downloadandsee, App::notadv;
std::string App::fileioerror, App::nopayment, App::redownload, App::updateidx, App::hostioerror, App::downloading;
std::string App::gpk, App::gsk, App::s117, App::vcd40, App::mima1, App::gjzsgz;
std::string App::waiting_for_login, App::un_or_pwd_empty, App::un_or_pwd_typein, App::un_typein, App::pwd_typein;
std::string App::ssmallwin, App::shelptip, App::sshowpwd, App::scancelfornow, App::sexittip;

rapidjson::Document* App::getErrorMsg(){
	if (jsonDict == NULL){ jsonDict = new rapidjson::Document(); }
	std::string jsonpath = "error_msg.Json";
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(jsonpath);
	jsonDict->Parse<0>(contentStr.c_str());
	if (jsonDict->HasParseError()){
		auto scene = Scene::create(); auto layer = Layer::create();
		auto label = Label::createWithSystemFont("config file error. reinstall the app", "", 20);
        cocos2d::Size s = scene->getContentSize();
		label->setAnchorPoint(Vec2(0, 0.5f));
		label->setPosition(s.width / 2, s.height / 2);
		layer->addChild(label); scene->addChild(layer);
		Director::getInstance()->runWithScene(scene);
		delete jsonDict;
		return NULL;
	}
	swaiting_for_connect = getInfo(*jsonDict, "waiting_for_connect");
	sexittip = getInfo(*jsonDict, "exittip");
	scancelfornow = getInfo(*jsonDict, "cancelfornow");
	sshowpwd = getInfo(*jsonDict, "showpwd");
	ssmallwin = getInfo(*jsonDict, "smallwin");
	shelptip = getInfo(*jsonDict, "helptip");
	shelperback = getInfo(*jsonDict, "helperback");
	shelperpause = getInfo(*jsonDict, "helperpause");
	shelperseek = getInfo(*jsonDict, "helperseek");
	sareusuredel = getInfo(*jsonDict, "areusuredel");
	snetfail = getInfo(*jsonDict, "netfail"); snetrequest = getInfo(*jsonDict, "netrequest");
	shomeworktitle = getInfo(*jsonDict, "homeworktitle");
	shomeworkpass = getInfo(*jsonDict, "homeworkpass");
	shomeworkneed = getInfo(*jsonDict, "homeworkneed");
	shomeworkdanwei = getInfo(*jsonDict, "homeworkdanwei");
	shomeworkuploadsucess = getInfo(*jsonDict, "homeworkuploadsucess");
	shomeworkuploadfail = getInfo(*jsonDict, "homeworkuploadfail");
	shomeworkupload = getInfo(*jsonDict, "homeworkupload");
	shomeworkuploadtry = getInfo(*jsonDict, "homeworkuploadtry");
	sokbutton = getInfo(*jsonDict, "okbutton");
	syear = getInfo(*jsonDict, "timestryear"); smonth = getInfo(*jsonDict, "timestrmonth"); sday = getInfo(*jsonDict, "timestrday");
	shour = getInfo(*jsonDict, "timestrhour"); sminute = getInfo(*jsonDict, "timestrminute"); ssecond = getInfo(*jsonDict, "timestrsecond");
	sclickandplay = getInfo(*jsonDict, "clickandplay"); App::downandplay = getInfo(*jsonDict, "downandplay");
	notadv = getInfo(*jsonDict, "notadv"); spause = getInfo(*jsonDict, "spause"); stitle = getInfo(*jsonDict, "title");
	areusure = getInfo(*jsonDict, "areusure"); okdown = getInfo(*jsonDict, "okdown"); canceldown = getInfo(*jsonDict, "canceldown");
	avrspeed = getInfo(*jsonDict, "avrspeed"); perc = getInfo(*jsonDict, "perc");
	downloadandsee = getInfo(*jsonDict, "downloadandsee"); downloading = getInfo(*jsonDict, "downloading");
	fileioerror = getInfo(*jsonDict, "fileioerror"); hostioerror = getInfo(*jsonDict, "hostioerror"); nopayment = getInfo(*jsonDict, "nopayment");
	redownload = getInfo(*jsonDict, "redownload"); updateidx = getInfo(*jsonDict, "updateidx");
	gpk = getInfo(*jsonDict, "gpk"); gsk = getInfo(*jsonDict, "gsk"); s117 = getInfo(*jsonDict, "s117");
	vcd40 = getInfo(*jsonDict, "vcd40"); mima1 = getInfo(*jsonDict, "mima1"); gjzsgz = getInfo(*jsonDict, "gjzsgz");
	waiting_for_login = getInfo(*jsonDict, "waiting_for_login"); un_or_pwd_empty = getInfo(*jsonDict, "un_or_pwd_empty");
	un_or_pwd_typein = getInfo(*jsonDict, "un_or_pwd_typein");
	un_typein = getInfo(*jsonDict, "un_typein"); pwd_typein = getInfo(*jsonDict, "pwd_typein");
	sCameraBlock = getInfo(*jsonDict, "CameraBlock"); shomeworkhit = getInfo(*jsonDict,"homeworkhit");
	const rapidjson::Value& val = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(*jsonDict, "hosts");
	std::stringstream ss; std::string istr;
	for (int i = 0; i < 50; i++){
		ss.clear(); ss << i; ss >> istr;
		std::string host = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(val, istr.c_str(), "");//getInfo(val, istr.c_str());
		if (host.empty()) break;
		//App::getSocketGate()->pushhost(host);
		App::getSocketMgr()->pushhost(host);
	}
	App::getSocketMgr()->randomhost();
	return jsonDict;
}

NetClient* App::getSocketMgr(){
    static NetClient* socketMgr = NULL;
	if (socketMgr == NULL){
		socketMgr = new NetClient();
	}
	return socketMgr;
}

/*
NetGate* App::getSocketGate(){
    static NetGate* socketMgr = NULL;
	if (socketMgr == NULL){
		socketMgr = new NetGate();
	}
	return socketMgr;
}
*/

const char* App::getInfo(const rapidjson::Value& root, const char* id){
	return cocostudio::DictionaryHelper::getInstance()->getStringValue_json(root, id,"setup error!");
}

std::string App::getgtmv(const int64_t& pub, const int64_t& year, const int64_t& t){
	tm tm1 = App::localtime(t);
	return getgtmv(pub,year,tm1);
}
std::string App::getgtmv(const int64_t& pub, const int64_t& year, const tm& tm1){
	char path[1024] = {0};
	sprintf(path,"%s/data/%d/%d/%04d%02d%02d%02d%02d%02d.gtmv",
			App::basePath.c_str(),int32_t(pub),tm1.tm_year + 1900,tm1.tm_year+1900,tm1.tm_mon,tm1.tm_mday,
			tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
	return path;
}
std::string App::getgtmvidx(const int64_t& pub, const int64_t& year){
	char path[1024] = {0};
	sprintf(path,"%s/idx/%d.dat", App::basePath.c_str(),int32_t(pub));
	return path;
}
std::string App::getgtmvdir(const int64_t& pub, const int64_t& year, const int64_t& t){
	tm tm1 = App::localtime(t);
	return getgtmvdir(pub,year,tm1);
}
std::string App::getgtmvdir(const int64_t& pub, const int64_t& year, const tm& tm1){
	char path[1024] = {0};
	sprintf(path,"%s/data/%d/%d/", App::basePath.c_str(),int32_t(pub),tm1.tm_year + 1900);
	return path;
}
std::string App::getusername(){
	char path[1024] = { 0 };
	sprintf(path, "%s/user.dat", App::basePath.c_str());
	FILE* fp = fopen(path, "rb+");
	memset(path, 0, sizeof(path));
	if (fp != NULL){
		fread(path, 1, sizeof(path), fp);
		fclose(fp);
	}
	return path;
}
void App::setusername(const std::string& name){
	char path[1024] = { 0 };
	sprintf(path, "%s/user.dat", App::basePath.c_str());
	FILE* fp = fopen(path, "wb");
	memset(path, 0, sizeof(path));
	if (fp != NULL){
		fseek(fp, 0, SEEK_SET);
		fwrite(name.c_str(), 1, name.length(), fp);
		fclose(fp);
	}
}

std::string App::getuserpwd(){
	char path[1024] = { 0 }; unsigned char *out = NULL; int i,len;
	sprintf(path, "%s/userinfo.dat", App::basePath.c_str());
	FILE* fp = fopen(path, "rb+");
	memset(path, 0, sizeof(path));
	if (fp != NULL){
		fread(path, 1, sizeof(path), fp);
		fclose(fp);
		len = strlen(path);
		len = base64Decode((unsigned char*)path, len, &out);
		int* val = (int*)out;
		for (i = 0; i < len / sizeof(int); i++){
			path[i] = (val[i] - i) / 2;
		}
		path[i] = 0;
		free(out);
	}
	return path;
}

void App::setuserpwd(const std::string& pwd){
	char path[1024] = { 0 }; std::string pwds; int val[200],len; char* out = NULL;
	sprintf(path, "%s/userinfo.dat", App::basePath.c_str());
	for (int i = 0; i < pwd.length(); i++){ val[i] = pwd[i] * 2 + i;}
	FILE* fp = fopen(path, "wb");
	memset(path, 0, sizeof(path));
	len = base64Encode((unsigned char*)val, pwd.length() * sizeof(int), &out);
	if (fp != NULL){
		fseek(fp, 0, SEEK_SET);
		fwrite(out, 1, len, fp);
		fclose(fp);
	}
	free(out);
}

bool App::load192k(const int64_t& idx, unsigned char* buf, int64_t& len){
	int lentmp; std::string file; std::stringstream ss; ss << "playctrl/" << idx - '0' << "_192k.png"; ss >> file; //"0_192k.png";
	len = 0;
	if (buf != 0){
		Data d = FileUtils::getInstance()->getDataFromFile(file);
		lentmp = d.getSize() / 2;
		short* src, *dst;
		if (lentmp > 0){
			src = (short*)d.getBytes();
			dst = (short*)buf;
			int j = 0;
			for (int i = 0; i < lentmp; i++){
				dst[j] = src[i] / 15;
				j++;
				dst[j] = src[i] / 15;
				j++;
				//dst[j] = src[i] / 15;
				//j++;
			}
			//memcpy(buf, d.getBytes(), len);
			len = j * 2;
			return true;
		}
	}
	return false;
}
void App::Sleep(int32_t span){
	std::this_thread::sleep_for(std::chrono::milliseconds(span));
}

bool App::applicationDidFinishLaunching() {// initialize director
	basePath = FileUtils::getInstance()->getWritablePath();

	//sCameraPath = basePath + "/Camera";
	//FileUtils::getInstance()->createDirectory(sCameraPath);
	//sCameraPath += "/homework_o.jpg";
	FileUtils::getInstance()->createDirectory(basePath + "/idx");
	FileUtils::getInstance()->createDirectory(basePath + "/data");
	FileUtils::getInstance()->createDirectory(basePath + "/info");

	FileUtils::getInstance()->addSearchPath(basePath);
    FileUtils::getInstance()->addSearchPath("res");
    //FileUtils::getInstance()->isFileExist("res/UiMain0.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("UiMain0.plist");
 	if (getErrorMsg() == NULL) return true;
   auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create(App::stitle);
        director->setOpenGLView(glview);
    }
	glview->setDesignResolutionSize(960, 640, ResolutionPolicy::EXACT_FIT);

	App::a192klen = 1024 * 1024 * 20;
	App::a192k = new unsigned char[App::a192klen];
	
    director->setDisplayStats(false);    // turn on display FPS
    director->setAnimationInterval(1.0 / 30);    // set FPS. the default value is 1.0/60 if you don't call this

	getSocketMgr();
	xiny120::gtmvreader::me();

	director->runWithScene(new SceneLogin());
	int wid = 960, high = 640;// = GetSystemMetrics(SM_CXSCREEN), high = GetSystemMetrics(SM_CYSCREEN);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	wid = GetSystemMetrics(SM_CXSCREEN) - 6;
	high = GetSystemMetrics(SM_CYSCREEN) - 80;
	::SetWindowPos(glview->getWin32Window(), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#endif
	glview->setFrameSize(wid, high);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void App::applicationDidEnterBackground() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)// || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)

#else
    Director::getInstance()->stopAnimation();
	if(!xiny120::AudioEngine::me()->ispause()) xiny120::AudioEngine::me()->pause();
#endif
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void App::applicationWillEnterForeground() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)// || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)

#else
    Director::getInstance()->startAnimation();
	xiny120::AudioEngine::me()->resume();
#endif
    //if(xiny120::AudioEngine::me()->ispause()) xiny120::AudioEngine::me()->pause();
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
