#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "uploadFile.h"
#include "curl/curl.h"
#include "AppDelegate.h"
#include "homework.h"

char hexstr[256];

uploadFile::uploadFile(){
	CURLcode cURLcode = curl_global_init(CURL_GLOBAL_SSL);
	url_fileup = "http://192.168.18.108/fileup.ashx";
	url_login = "http://192.168.18.108/login.ashx";
	
	key.GenerateRandomWithKeySize(rng,1024); // 初始化本进程生命周期与web服务器交互的rsa秘钥。
	// str2hex 要用的到运算表
	hexstr['0'] = 0; hexstr['1'] = 1; hexstr['2'] = 2; hexstr['3'] = 3; hexstr['4'] = 4; hexstr['5'] = 5;
	hexstr['6'] = 6; hexstr['7'] = 7; hexstr['8'] = 8; hexstr['9'] = 9; hexstr['a'] = 10; hexstr['b'] = 11;
	hexstr['c'] = 12; hexstr['d'] = 13;	hexstr['e'] = 14; hexstr['f'] = 15;	hexstr['A'] = 10; hexstr['B'] = 11;
	hexstr['C'] = 12; hexstr['D'] = 13; hexstr['E'] = 14; hexstr['F'] = 15;
};
uploadFile::~uploadFile(){ curl_global_cleanup();};

uploadFile* uploadFile::me(){
	static uploadFile* me = NULL;
	if (!me){ me = new uploadFile(); me->cookiefile = App::basePath + "/curl_cookies.dat"; }
	return me;
}

bool uploadFile::login(){
	bool ret = false;
	if (cmdinit()){
		if (cmdlogin()){
			ret = true;
		}
	}
	return ret;
}

std::string uploadFile::hex2str(const std::string str){
	return hex2str(str.c_str(),str.length());
}

std::string uploadFile::hex2str(const char* str, const int len){
	int i = 0; std::string ret;
	std::ostringstream oss;
	oss << std::hex; oss << std::setfill('0'); oss << std::nouppercase;   //大写
	for (int i = 0; i < len; i++){
		unsigned char c = str[i]; oss << std::setw(2) << (unsigned int)c;
	}
	return oss.str();
}

int uploadFile::str2hex(std::string str,char* out){
	int len = str.length(),i = 0,j = 0;
	for (i = 0; i < len;){
		out[j] = hexstr[str[i++]] << 4 | hexstr[str[i++]];
		j++;
	}
	return j;
}

bool uploadFile::cmdinit(){
	CURLcode cURLcode; CURL *cURL; bool ret = false; CryptoPP::AES::Encryption aes; CryptoPP::RSA rsa;
	cURL = curl_easy_init();
	if (cURL){
		std::string n, e; byte nchar[1024] = { 0 }, out[1024] = {0};
		CryptoPP::RSA::PublicKey pubkey(key); // 取出rsa公共秘钥（n->GetModulus,e->GetPublicExponent）交给服务器，让他把关键数据用rsa加密。
		CryptoPP::Integer ie = key.GetPublicExponent(); // 大整数。
		CryptoPP::Integer in = key.GetModulus(); // 大整数。
		in.Encode(nchar, in.MinEncodedSize()); // 把大整数序列化到byte数组。
		n = hex2str((char*)nchar, in.MinEncodedSize()); // 把byte数组转换成对应的hex字符串
		memset(nchar, 0, sizeof(nchar));
		ie.Encode(nchar, ie.MinEncodedSize());// 同上面处理n的过程
		e = hex2str((char*)nchar, ie.MinEncodedSize());
	
		curl_httppost *post = NULL; curl_httppost *last = NULL; std::string res; // 向web服务器发送rsa公钥(n和e)
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "cmd", CURLFORM_COPYCONTENTS, "init", CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "n", CURLFORM_COPYCONTENTS, n.c_str(), CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "e", CURLFORM_COPYCONTENTS, e.c_str(), CURLFORM_END);
		curl_easy_setopt(cURL, CURLOPT_URL, url_login.c_str());
		curl_easy_setopt(cURL, CURLOPT_TIMEOUT, 30);//超时时间10秒    
		curl_easy_setopt(cURL, CURLOPT_HTTPPOST, post);//请求方式POST    
		curl_easy_setopt(cURL, CURLOPT_COOKIEFILE, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIEJAR, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIESESSION, false); // 这个开关貌似是让每个单独的 curl_easy_init 之间共享服务器的session的。
		curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, read_from_server);
		curl_easy_setopt(cURL, CURLOPT_WRITEDATA, &res);
		if ((cURLcode = curl_easy_perform(cURL)) == CURLE_OK){ // 成功与ashx交互。
			rapidjson::Document json; // cocos2dx内置的json分析器，服务器返回json数据包
			json.Parse<0>(res.c_str());
			if (!json.HasParseError()){
				std::string cmdstatus = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "cmdstatus", "0");
				if (cmdstatus == "1"){ // 1表示向服务器发送的init命令成功，服务器返回了rsa加密的aes秘钥。剩下的交互用aes加密。
					std::string aiv = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "aesiv", "");
					std::string akey = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "aeskey", "");
					CryptoPP::RSA::PrivateKey privateKey(key);
					{
						CryptoPP::RSAES_PKCS1v15_Decryptor dec(privateKey);
						memset(nchar, 0, sizeof(nchar));
						int len = str2hex(aiv, (char*)nchar);
						memset(out, 0, sizeof(out));
						CryptoPP::DecodingResult result = dec.Decrypt(rng, nchar, len, out);
						if (result.isValidCoding){
							CryptoPP::Base64Decoder base64;
							base64.Put(out, strlen((char*)out));
							base64.MessageEnd();
							memset(aesiv, 0, sizeof(aesiv));
							aesivlen = base64.Get((byte*)aesiv, sizeof(aesiv));
							ret = true;
						}
					}
					if (ret){
						ret = false;
						CryptoPP::RSAES_PKCS1v15_Decryptor dec(privateKey);
						memset(nchar, 0, sizeof(nchar));
						int len = str2hex(akey, (char*)nchar);
						memset(out, 0, sizeof(out));
						CryptoPP::DecodingResult result = dec.Decrypt(rng, nchar, len, out);
						if (result.isValidCoding){
							CryptoPP::Base64Decoder base64;
							base64.Put(out, strlen((char*)out));
							base64.MessageEnd();
							memset(aeskey, 0, sizeof(aeskey));
							aeskeylen = base64.Get((byte*)aeskey, sizeof(aeskey));
							ret = true;
						}
					}

				}
			}
		}
		curl_easy_cleanup(cURL);
	}
	return ret;
}

bool uploadFile::cmdlogin(){
	CURLcode cURLcode; CURL *cURL; bool ret = false; 
	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption aes;
	cURL = curl_easy_init();
	if (cURL){
		int len; char name[1024] = { 0 }, pwd[1024] = {0}; //byte key[1024] = { 0 }; byte iv[1024] = { 0 }; 
		//len = str2hex(aeskey, (char*)key); str2hex(aesiv, (char*)iv);
		//CryptoPP::SecByteBlock
		strcpy(name, App::getusername().c_str());
		len = strlen(name);
		len = ((len / 16) + 1) * 16;
		aes.SetKeyWithIV((byte*)aeskey, aeskeylen, (byte*)aesiv, aesivlen); //.SetKey((byte*)aeskey, aeskeylen);//
		aes.ProcessString((byte*)name,len);
		CryptoPP::Base64Encoder base64enc;
		base64enc.Put((byte*)name, len);
		base64enc.MessageEnd();
		memset(name, 0, sizeof(name));
		base64enc.Get((byte*)name, sizeof(name));
		len = 16;
		strcpy(pwd,hex2str((char*)App::pwdmd5, 16).c_str());
		{
			len = strlen(pwd);
			CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption aes;
			aes.SetKeyWithIV((byte*)aeskey, aeskeylen, (byte*)aesiv, aesivlen);
			aes.ProcessString((byte*)pwd, len);
			CryptoPP::Base64Encoder base64enc;
			base64enc.Put((byte*)pwd, len);
			base64enc.MessageEnd();
			memset(pwd, 0, sizeof(pwd));
			base64enc.Get((byte*)pwd, sizeof(pwd));
		}
		curl_httppost *post = NULL; curl_httppost *last = NULL; std::string res; // 向web服务器发送rsa公钥(n和e)
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "cmd", CURLFORM_COPYCONTENTS, "login", CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "name", CURLFORM_COPYCONTENTS, name, CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "pwd", CURLFORM_COPYCONTENTS, pwd, CURLFORM_END);
		curl_easy_setopt(cURL, CURLOPT_URL, url_login.c_str());
		curl_easy_setopt(cURL, CURLOPT_TIMEOUT, 30);//超时时间10秒    
		curl_easy_setopt(cURL, CURLOPT_HTTPPOST, post);//请求方式POST    
		curl_easy_setopt(cURL, CURLOPT_COOKIEFILE, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIEJAR, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIESESSION, false); // 这个开关貌似是让每个单独的 curl_easy_init 之间共享服务器的session的。
		curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, read_from_server);
		curl_easy_setopt(cURL, CURLOPT_WRITEDATA, &res);
		//curl_easy_setopt(cURL, CURLOPT_READFUNCTION, read_local_send_to_server); //设置回调    
		//curl_easy_setopt(cURL, CURLOPT_READDATA, fp);
		//curl_easy_setopt(cURL, CURLOPT_UPLOAD, 1L);
		//curl_easy_setopt(cURL, CURLOPT_INFILESIZE_LARGE, file_size);
		//curl_easy_setopt(cURL, CURLOPT_VERBOSE, 1); //非零值表示你想CURL报告每一件意外的事情    
		if ((cURLcode = curl_easy_perform(cURL)) == CURLE_OK){ // 成功与ashx交互。
			rapidjson::Document json; // cocos2dx内置的json分析器，服务器返回json数据包
			json.Parse<0>(res.c_str());
			if (!json.HasParseError()){
				std::string cmdstatus = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "cmdstatus", "0");
				if (cmdstatus == "1"){ // 1表示向服务器发送的login命令成功.
					const rapidjson::Value& data = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, "cols");
					for (rapidjson::Value::ConstValueIterator it2 = data.onBegin(); it2 != data.onEnd(); it2++){
						cmdstatus = it2->GetString();
						App::mlogin.addcol(cmdstatus);
					}
					const rapidjson::Value& rows = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, "rows");
 					for (rapidjson::Value::ConstValueIterator it2 = rows.onBegin(); it2 != rows.onEnd(); it2++){
						rapidjson::Value::ConstValueIterator it3 = it2->onBegin();
						//for (int i = 0; i < App::mlogin.cols(); i++){
						cmdstatus = it3->GetString();
						it3++;
						App::userid = it3->GetInt();
						it3++;
						App::groupid = it3->GetInt();
					}
					ret = true;
				}
			}
		}
		curl_easy_cleanup(cURL);
	}
	return ret;
}

bool uploadFile::sendFile(const std::string& photoPath){
	bool ret = false;
	if (!(ret = cmdsendFile(photoPath))){
		if (login()) ret = cmdsendFile(photoPath);
	}
	return ret;
}

bool uploadFile::cmdsendFile(const std::string& photoPath){
	CURLcode cURLcode; CURL *cURL; bool ret = false; CryptoPP::AES::Encryption aes; CryptoPP::RSA rsa;
	cURL = curl_easy_init(); std::string lessionid = "1";
	if (cURL){
		curl_httppost *post = NULL; curl_httppost *last = NULL; std::string res;
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "files[]", CURLFORM_FILE, photoPath.c_str(),
			CURLFORM_CONTENTTYPE, "Image/jpg", CURLFORM_END);//上传的文件的全路径并指定类型,curl自动打开文件上传

		curl_formadd(&post, &last, CURLFORM_COPYNAME, "type", CURLFORM_COPYCONTENTS, "homework", CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "lessionid", CURLFORM_COPYCONTENTS, App::lessionId.c_str(), CURLFORM_END);
		curl_easy_setopt(cURL, CURLOPT_URL, url_fileup.c_str());
		curl_easy_setopt(cURL, CURLOPT_TIMEOUT, 30);//超时时间10秒    
		curl_easy_setopt(cURL, CURLOPT_HTTPPOST, post);//请求方式POST    
		curl_easy_setopt(cURL, CURLOPT_COOKIEFILE, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIEJAR, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIESESSION, false);// 这个开关貌似是让每个单独的 curl_easy_init 之间共享服务器的session的。
		curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, read_from_server);
		curl_easy_setopt(cURL, CURLOPT_WRITEDATA, &res);
		if ((cURLcode = curl_easy_perform(cURL)) == CURLE_OK){ // 成功与ashx交互。
			rapidjson::Document json; // cocos2dx内置的json分析器，服务器返回json数据包
			json.Parse<0>(res.c_str());
			if (!json.HasParseError()){
				std::string cmdstatus = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "cmdstatus", "0");
				if (cmdstatus == "1"){ // 1表示向服务器发送的上传成功.
					ret = true;
				}
			}
		}
		curl_easy_cleanup(cURL);
	}
	return ret;
}

bool uploadFile::getCurLessions(){
	CURLcode cURLcode; CURL *cURL; bool ret = false; int i = 0;
	cURL = curl_easy_init();
	if (cURL){
		curl_httppost *post = NULL; curl_httppost *last = NULL; std::string res; // 向web服务器发送rsa公钥(n和e)
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "cmd", CURLFORM_COPYCONTENTS, "getcurlessions", CURLFORM_END);
		curl_easy_setopt(cURL, CURLOPT_URL, url_login.c_str());
		curl_easy_setopt(cURL, CURLOPT_TIMEOUT, 30);//超时时间10秒    
		curl_easy_setopt(cURL, CURLOPT_HTTPPOST, post);//请求方式POST    
		curl_easy_setopt(cURL, CURLOPT_COOKIEFILE, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIEJAR, cookiefile.c_str());
		curl_easy_setopt(cURL, CURLOPT_COOKIESESSION, false); // 这个开关貌似是让每个单独的 curl_easy_init 之间共享服务器的session的。
		curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, read_from_server);
		curl_easy_setopt(cURL, CURLOPT_WRITEDATA, &res);
		if ((cURLcode = curl_easy_perform(cURL)) == CURLE_OK){ // 成功与ashx交互。
			rapidjson::Document json; // cocos2dx内置的json分析器，服务器返回json数据包
			json.Parse<0>(res.c_str());
			if (!json.HasParseError()){
				std::string cmdstatus = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, "cmdstatus", "0");
				if (cmdstatus == "1"){ // 1表示向服务器发送的login命令成功.
					App::mhomeworklevel.clear();
					const rapidjson::Value& data = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, "cols");
					for (rapidjson::Value::ConstValueIterator it2 = data.onBegin(); it2 != data.onEnd(); it2++){}
					const rapidjson::Value& rows = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, "rows");
					for (rapidjson::Value::ConstValueIterator it2 = rows.onBegin(); it2 != rows.onEnd(); it2++){
						rapidjson::Value::ConstValueIterator it3 = it2->onBegin();
						xiny120::homework::lession val;
						val.mlevel = it3->GetString();
						it3++;
						val.mlessionid = it3->GetInt();
						it3++;
						val.mlessionname = it3->GetString();
						App::mhomeworklevel.add(val);
					}
					ret = true;
				}
			}
		}
		curl_easy_cleanup(cURL);
	}
	return ret;
}

size_t uploadFile::read_local_send_to_server(uint8_t *dataBack, size_t size, size_t nmemb, void *user_p){
	return fread(dataBack,size,nmemb,(FILE*)user_p);
}

size_t uploadFile::read_from_server(uint8_t *dataBack, size_t size, size_t nmemb, void *userp){
	std::string* res = (std::string*)userp;
	(*res).append(dataBack, dataBack + nmemb);
	return nmemb * size;
}