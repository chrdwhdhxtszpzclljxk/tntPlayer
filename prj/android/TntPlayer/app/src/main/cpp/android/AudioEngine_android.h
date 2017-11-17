#ifndef _AUDIOENGINE_ANDROID_H
#define _AUDIOENGINE_ANDROID_H

#include <stdio.h>
#include <stdint.h>


#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <list>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "../protocol.h"
#include "../xiny120_ns.h"


NS_XINY120_BEGIN

class AudioEngine{
public:
	static void _newsnd_cb(SLAndroidSimpleBufferQueueItf bq, void *context);
	static void playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
	static AudioEngine* me();
	static void destroy();
	virtual bool init();

	bool start(const float& speed = 1, const int32_t& sps = 8000, const int32_t& bps = 16, const int32_t& ch = 1, const int32_t& f = 1);// PWAVEFORMATEX pWaveformat);
	bool pause();
	bool ispause(){ return mpause; }
	bool resume();
	bool play();
	void stop();
	void clear();

	inline int32_t getblockps(){ return blockps; };
	inline float getspeed(){ return mspeed; }
	bool isfull(const int32_t& len);
	void push(int8_t* p, const int32_t& len);
	int8_t* pop(const int32_t& len);
	int8_t* poplast(int32_t& len);
	bool empty();
	int32_t size();
	std::atomic<uint64_t> mstart;
	void setbreaknow(){ breaknow = true; };
	bool getbreaknow(){ return breaknow; };

protected:
	AudioEngine();
	~AudioEngine();
private:
	bool mpause, mopen; uint16_t volume; void IoThread(AudioEngine*); std::thread t;
	SLObjectItf eng_obj = 0, mix_obj = 0, player_obj = 0; SLEngineItf enger; SLPlayItf player; SLAndroidSimpleBufferQueueItf buf_queue;// 声音引擎的对象介面  声音引擎  混音器对象介面 播放器对象
	SLEffectSendItf bqPlayerEffectSend;
	char* mslbuf[3];
	std::condition_variable c; // condition
	std::mutex m; // mutext
	std::atomic<bool> r, breaknow, next, waveoutreset; // r(run)是否退出整个播放伺服流程。breaknow 用户是否打断当前播放流程。
	//std::queue<xiny120::GtmvData*> audio;
	int8_t* pcm, *phead, *ptail;
	int32_t pcmlen, blockps; // block per second
	float  mspeed;
	std::recursive_mutex ma;
};

NS_XINY120_END

#endif
