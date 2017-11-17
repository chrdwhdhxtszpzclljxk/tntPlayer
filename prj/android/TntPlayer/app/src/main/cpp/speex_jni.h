/*
 * speex_jni.h
 *
 *  Created on: 2013-12-12
 *      Author: Administrator
 */



#ifndef SPEEX_JNI_H_
#define SPEEX_JNI_H_
#include "include/speex/speex.h"

class speex{
public:
	speex(void);
	~speex(void);
private:
	SpeexBits*	m_pBits;
	void*		m_pCodeState;
public:
	bool decode(const char* pcSpeexBuf, const unsigned int uiSpeexBufLen,short int * pcsiPcm16, unsigned int& cuiPcm16Len);
};




#endif /* SPEEX_JNI_H_ */
