#include <string>
#include <android/log.h>
#include <pthread.h>
#include "native-lib.h"

#define LOG_TAG "MAIN_CPP"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR ,LOG_TAG ,__VA_ARGS__)

static bool isLoop = false;
static pthread_t loopID;

void onStart(ANativeActivity *activity) {
    LOGE("Application is start");
}

void onResume(ANativeActivity *activity) {
    LOGE("onResume");
}

void *onSaveInstanceState(ANativeActivity *activity, size_t *outSize) {
    LOGE("onSaveInstanceState");
}

void onPause(ANativeActivity *activity) {
    LOGE("onPause");
}

void onStop(ANativeActivity *activity) {
    LOGE("onStop");
}

void onDestroy(ANativeActivity *activity) {
    LOGE("onDestroy");
}

void onWindowFocusChanged(ANativeActivity *activity, int hasFocus) {
    LOGE("onWindowFocusChanged");
}

void onNativeWindowCreated(ANativeActivity *activity, ANativeWindow *window) {
    LOGE("Native Window Created !");
    /*声明窗口缓冲*/
    ANativeWindow_Buffer window_buffer = {0};
    /*画布上锁*/
    ANativeWindow_lock(window, &window_buffer, NULL);
    /*计算面积*/
    int mArea = window_buffer.width * window_buffer.height;
    /*获取像素地址*/
    unsigned long *screen = static_cast<unsigned long *>(window_buffer.bits);
    /*循环赋值*/
    for (int i = 0; i < mArea; ++i) {
        screen[i] = 0xffffffff;
    }
    /*解锁并提交绘制*/
    ANativeWindow_unlockAndPost(window);
}

void onNativeWindowDestroyed(ANativeActivity *activity, ANativeWindow *window) {
    LOGE("onNativeWindowDestroyed");
}

void onInputQueueCreated(ANativeActivity *activity, AInputQueue *queue) {
    LOGE("onInputQueueCreated");
    isLoop = true;
    activity->instance = (void *) queue;
    pthread_create(&loopID, NULL, looper, activity);
}

void onInputQueueDestroyed(ANativeActivity *activity, AInputQueue *queue) {
    LOGE("onInputQueueDestroyed");
    isLoop = false;
}

void onConfigurationChanged(ANativeActivity *activity) {
    LOGE("onConfigurationChanged");
}

void onLowMemory(ANativeActivity *activity) {
    LOGE("onLowMemory");
}

void bindLifeCycle(ANativeActivity *activity) {
    LOGE("bindLifeCycle");
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
}

void *looper(void *args) {
    LOGE("looper start !");
    ANativeActivity *activity = (ANativeActivity *) args;
    AInputQueue *queue = (AInputQueue *) activity->instance;
    AInputEvent *event = NULL;
    while (isLoop) {
        if (!AInputQueue_hasEvents(queue)) {
            continue;
        }
        AInputQueue_getEvent(queue, &event);
//        float mx = AMotionEvent_getX(event, 0);
//        float my = AMotionEvent_getY(event, 0);
        switch (AInputEvent_getType(event)) {
            case AINPUT_EVENT_TYPE_MOTION: {
                switch (AMotionEvent_getAction(event)) {
                    case AMOTION_EVENT_ACTION_DOWN: {
                        LOGE("Touch Screen Down");
                        break;
                    }
                    case AMOTION_EVENT_ACTION_UP: {
                        LOGE("Touch Screen UP");
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case AINPUT_EVENT_TYPE_KEY: {
                switch (AKeyEvent_getAction(event)) {
                    case AKEY_EVENT_ACTION_DOWN: {
                        LOGE("key down");
                        switch (AKeyEvent_getKeyCode(event)) {
                            case AKEYCODE_BACK: {
                                LOGE("BACK down");
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    case AKEY_EVENT_ACTION_UP: {
                        LOGE("key up");
                        switch (AKeyEvent_getKeyCode(event)) {
                            case AKEYCODE_BACK: {
                                LOGE("BACK up");
                                ANativeActivity_finish(activity);
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            default:
                break;
        }
        AInputQueue_finishEvent(queue, event, 1);
    }
    return args;
}

void ANativeActivity_onCreate(ANativeActivity *activity, void *savedState, size_t savedStateSize) {
    LOGE("init LifeCycle");
    bindLifeCycle(activity);
}

