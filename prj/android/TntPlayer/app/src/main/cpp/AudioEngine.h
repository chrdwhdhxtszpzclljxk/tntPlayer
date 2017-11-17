#ifndef _AUDIOENGINE_H
#define _AUDIOENGINE_H
#include "protocol.h"
#include "xiny120_ns.h"
#include <stdio.h>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <list>


//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
//#include "win32/AudioEngine_win32.h"
//#endif

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) /*CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX*/
#include "android/AudioEngine_android.h"
//#endif

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//#include "iOS/AudioEngine_iOS.h"
//#endif

#endif