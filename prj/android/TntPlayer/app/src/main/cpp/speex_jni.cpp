/*
 * speex_jni.cpp
 *
 *  Created on: 2013-12-12
 *      Author: Administrator
 */

#include "speex_jni.h"
//#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Hot", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Hot", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Hot", __VA_ARGS__))

speex::speex(void){
	m_pBits = new SpeexBits;
	int frame_size = 0, bitRate = 0, Quality = 3;
	speex_bits_init(m_pBits);
	m_pCodeState = speex_decoder_init(&speex_nb_mode);
	//speex_decoder_ctl(m_pCodeState, SPEEX_GET_FRAME_SIZE, &frame_size);
	//speex_decoder_ctl(m_pCodeState, SPEEX_SET_QUALITY, &Quality);
	//speex_decoder_ctl(m_pCodeState, SPEEX_GET_BITRATE,&bitRate);
}

speex::~speex(void){ speex_bits_destroy(m_pBits); delete m_pBits; speex_decoder_destroy(m_pCodeState);}

// multi speex buffer to decode, echo time decode 20 byte;
bool speex::decode(const char* speexbuf, const unsigned int speexlen,short int * pcm16, unsigned int& pcm16len){
	char* pNode = (char* )speexbuf; short int * psiPcm16Node = pcm16; pcm16len = 0;
	while((pNode - speexbuf) < speexlen){
		speex_bits_read_from(m_pBits, pNode, 20);
		int iRet = speex_decode_int(m_pCodeState, m_pBits, psiPcm16Node);
		speex_bits_reset(m_pBits); pNode += 20; psiPcm16Node += 160; pcm16len += 160;
	}
	return true;
}
