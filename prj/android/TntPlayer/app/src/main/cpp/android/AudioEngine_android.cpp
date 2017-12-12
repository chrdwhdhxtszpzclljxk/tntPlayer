#include "AudioEngine_android.h"
#include "../gtmvreader.h"
#include <string>
#include <cstring>

NS_XINY120_BEGIN

static AudioEngine* __audioengine = NULL;
AudioEngine::AudioEngine() : volume(0xFFFF / 2), mstart(0), r(true), mopen(false),  next(false), pcm(0){}
AudioEngine::~AudioEngine(){ if (pcm != NULL) delete[] pcm; delete [] mslbuf[0]; delete [] mslbuf[1]; delete [] mslbuf[2]; }
AudioEngine* AudioEngine::me(){ if (__audioengine == NULL) { (__audioengine = new AudioEngine())->init(); }; return __audioengine; }
void AudioEngine::destroy(){ delete __audioengine; }

bool AudioEngine::init(){
	pcmlen = 8000 * 30 * 2; blockps = 10;
	phead = ptail = pcm = new int8_t[pcmlen];
	mslbuf[0] = new char[8000 * (16 / 8) / getblockps()];
	mslbuf[1] = new char[8000 * (16 / 8) / getblockps()];
	mslbuf[2] = new char[8000 * (16 / 8) / getblockps()];
	return true;
}

bool AudioEngine::isfull(const int32_t& _len){
	std::lock_guard<std::recursive_mutex> locka(ma); int32_t datalen = ptail - phead; int32_t left = pcmlen - (ptail - pcm);
	int32_t len = _len;
	if (left < len){
		memmove(pcm, phead, datalen); phead = pcm; ptail = pcm + datalen; left = pcmlen - (ptail - pcm);
	}
	if (left < len)
		return true;
	return false;
}
void AudioEngine::push(int8_t* p, const int32_t& _len){
	std::lock_guard<std::recursive_mutex> locka(ma); int32_t datalen = ptail - phead; int32_t left = pcmlen - (ptail - pcm);
	int32_t len = _len;
	if (left < len){
		memmove(pcm, phead, datalen); phead = pcm; ptail = pcm + datalen; left = pcmlen - (ptail - pcm);
	}
	if (left < len){ left = 0; return;	}
	memcpy(ptail, p, len); ptail += _len;
	//std::string pcmfile = App::basePath + "/test002.pcm";
	//FILE* fp = fopen(pcmfile.c_str(),"ab+");
	//fwrite(p,1,len,fp);
	//fclose(fp);

}

int8_t* AudioEngine::pop(const int32_t& _len){
	int32_t len = _len ; std::lock_guard<std::recursive_mutex> locka(ma);
	int8_t* ret = phead; phead += len;
	if (phead > ptail){ phead -= len; return NULL; }

	//std::string pcmfile = App::basePath + "/test0021.pcm";
	//FILE* fp = fopen(pcmfile.c_str(),"ab+");
	//fwrite(ret,1,len,fp);
	//fclose(fp);


	return ret;
}

int8_t* AudioEngine::poplast(int32_t& _len){
	int32_t len = _len; std::lock_guard<std::recursive_mutex> locka(ma);
	_len = ptail - phead;
	if(_len <= 0) return 0;
	int8_t* ret = new int8_t[len]; memset(ret, 0, len);	memcpy(ret, phead, ptail - phead);
	phead = ptail;
	return ret;
}

bool AudioEngine::empty(){ std::lock_guard<std::recursive_mutex> locka(ma); return ptail == phead; };
int32_t AudioEngine::size(){ std::lock_guard<std::recursive_mutex> locka(ma); return ptail - phead; };
void AudioEngine::clear(){ std::lock_guard<std::recursive_mutex> locka(ma); phead = ptail = pcm; };
void AudioEngine::playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {

}

void AudioEngine::_newsnd_cb(SLAndroidSimpleBufferQueueItf bq, void *context){
	/* �������������н��棬������Ϊ sndlen����������snd�Ž����� */
	AudioEngine* eng = (AudioEngine*)context; int8_t* a; int32_t retry = 6000, buflen = 8000 * (16 / 8) / eng->getblockps();
	a = eng->pop(buflen);
	while (a == NULL && !gtmvreader::me()->getbreaknow() && retry >= 0 && !eng->breaknow){ std::this_thread::sleep_for (std::chrono::milliseconds(10));; retry--; a = eng->pop(buflen);  }
	if (a != NULL) { // len == 0 ��ʾ������
		memcpy(eng->mslbuf[0],a,buflen);
		(*bq)->Enqueue(bq, eng->mslbuf[0], buflen);
		eng->mstart += (UNITS / eng->getblockps() * eng->getspeed());
		//std::string pcmfile = App::basePath + "/test0021.pcm";
		//FILE* fp = fopen(pcmfile.c_str(),"ab+");
		//fseek(fp,0,SEEK_END);
		//fwrite(a,1,buflen,fp);
		//fclose(fp);

		//log("mstart: %d\r\n",(int)eng->mstart);
	}else{ // ����100�ζ�ʧ�ܵĻ���ֻ��˵�����ˡ�
		a = eng->poplast(buflen);
		if(a != 0){
			memcpy(eng->mslbuf[0],a,buflen);
			(*bq)->Enqueue(bq, eng->mslbuf[0], buflen);
			delete[] a;
		}
	}
}

#define IF_FAIL(a) if(a != SL_RESULT_SUCCESS){ break;}

bool AudioEngine::start(const float& speed, const int32_t& sps, const int32_t& bps, const int32_t& ch, const int32_t& f){
	mstart = 0; if (mopen) stop();
	mspeed = speed; breaknow = false;
	SLresult r;	SLInterfaceID effect[1] = {SL_IID_ENVIRONMENTALREVERB}; SLboolean effect_bool[1] = {SL_BOOLEAN_FALSE}; // ��Ч ��Чǿ��ʵ���߼�
	do{
		IF_FAIL(slCreateEngine(&eng_obj, 0, NULL, 0, NULL, NULL));
		IF_FAIL((*eng_obj)->Realize(eng_obj, SL_BOOLEAN_FALSE));
		IF_FAIL((*eng_obj)->GetInterface(eng_obj, SL_IID_ENGINE, &enger));
		IF_FAIL((*enger)->CreateOutputMix(enger, &mix_obj, 1, effect, effect_bool));
		IF_FAIL((*mix_obj)->Realize(mix_obj, SL_BOOLEAN_FALSE));
	    SLDataLocator_AndroidSimpleBufferQueue loc_bufq;//
	    loc_bufq.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	    loc_bufq.numBuffers = 3;
	    SLDataFormat_PCM format_pcm;
	    format_pcm.formatType = SL_DATAFORMAT_PCM;
	    format_pcm.numChannels = 1;
	    format_pcm.samplesPerSec = SL_SAMPLINGRATE_8;
	    format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	    format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
	    format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
	    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
	    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
	    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mix_obj};
	    SLDataSink audioSnk = {&loc_outmix, NULL};
	    const SLInterfaceID ids[3] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
	    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	    IF_FAIL((*enger)->CreateAudioPlayer(enger, &player_obj, &audioSrc, &audioSnk, 3, ids, req));
	    IF_FAIL((*player_obj)->Realize(player_obj, SL_BOOLEAN_FALSE));
	    IF_FAIL((*player_obj)->GetInterface(player_obj, SL_IID_PLAY, &player));
	    IF_FAIL((*player_obj)->GetInterface(player_obj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &buf_queue));
	    IF_FAIL((*player_obj)->GetInterface(player_obj, SL_IID_EFFECTSEND,&bqPlayerEffectSend));
	    IF_FAIL((*buf_queue)->RegisterCallback(buf_queue, _newsnd_cb, this));
	    IF_FAIL((*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING));
	    mopen = true; mpause = false;
	    _newsnd_cb(buf_queue,this);

	    return true;
	}while(0);
	return false;
}

bool AudioEngine::play(){
	SLresult r; mstart = 0; bool ret = true,first = true; int8_t* a; int32_t hdrcount = 1000 / blockps, i, retry;
	mpause = false;
	hdrcount = hdrcount /mstart;
	if (!mopen) return false;
	r = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING );
	if(SL_RESULT_SUCCESS == r){
		ret = true;
	}else ret = false;
	return ret;
}

void AudioEngine::stop(){
	if (!mopen)	return;
	gtmvreader::me()->setbreaknow();
	if(player != 0) ((*player)->SetPlayState(player, SL_PLAYSTATE_PAUSED));
	if(player_obj != 0){
		(*player_obj)->Destroy(player_obj);
		player_obj = 0;
		player = 0;
		buf_queue = 0;
	}
	if(mix_obj != 0){
		(*mix_obj)->Destroy(mix_obj);
		mix_obj = 0;
	}

	if(eng_obj != 0){
		(*eng_obj)->Destroy(eng_obj);
		eng_obj = 0;
		enger = 0;
	}

	waveoutreset = true; mopen = false; mpause = false; mstart = 0;
	//SLObjectItf aud; SLEngineItf aud_eng; SLObjectItf aud_mix; SLObjectItf aud_player; SLPlayItf player_ctrl; SLBufferQueueItf aud_buf;// ��������Ķ������  ��������  ������������� ����������
}

bool AudioEngine::pause(){
	if (!mopen) return false;
	if(!mpause){ ((*player)->SetPlayState(player, SL_PLAYSTATE_PAUSED));}
	else{ ((*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING));}
	mpause = !mpause;
	return mpause;
}

bool AudioEngine::resume(){
	if (!mopen) return false;
	((*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING));
	mpause = false;
	return mpause;
}

NS_XINY120_END
