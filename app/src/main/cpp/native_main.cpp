//
// Created by var on 2018/7/14.
//
#include <iostream>
#include <android/log.h>
#include "native_main.hpp"

static int32_t onInputEvent(struct android_app *app, AInputEvent *event) {
    /////////////////////点击位置获取 START//////////////////////////
    /*捕捉鼠标事件*/
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        /*获取触控点数*/
        int nNum = AMotionEvent_getPointerCount(event);
        /*初始化数组*/
        char szTrace[1024] = {0};
        /*打印数组*/
        sprintf(szTrace, "engine_handle_input num=[%d]", nNum);
        /*遍历点位并获取点位的X,Y坐标*/
        for (int nIdx = 0; nIdx < nNum; nIdx++) {
            /*获取X坐标*/
            int nX = static_cast<int>(AMotionEvent_getX(event, 0));
            /*获取Y坐标*/
            int nY = static_cast<int>(AMotionEvent_getY(event, 0));
            /*打印X,Y坐标*/
            sprintf(strrchr(szTrace, 0), " (%d %d)", nX, nY);
        }
        /*打印数组*/
        __android_log_print(ANDROID_LOG_DEBUG, "colorspace", "%s", szTrace);
        /*鼠标未抬起*/
        if (AKeyEvent_getAction(event) != AKEY_EVENT_ACTION_UP)
            /*返回*/
            return 1;
        /////////////////////点击位置获取 END//////////////////////////
        /////////////////////创建窗口缓冲帧 START/////////////////////
        //
        // ANativeWindow来自onNativeWindowCreated()方法
        //
        /*初始化窗口缓冲*/
        ANativeWindow_Buffer nativeWindow = {0};
        /*画布上锁*/
        int nRet = ANativeWindow_lock(app->pendingWindow, &nativeWindow, NULL);
        /*计算面积*/
        int nArea = nativeWindow.width * nativeWindow.height;
        /*像素位指针*/
        unsigned long *pdwScreen = (unsigned long *) nativeWindow.bits;
        /*当前选择的颜色索引*/
        static int s_nClr = 0;
        /*颜色存放数组,颜色模式 A B G R (Alpha, Blue, Green, Red)*/
        unsigned long pdwClr[] = {
                0x00000000, 0x000000ff, 0x0000ffff, 0x0000ff00,
                0x00ffff00, 0x00ff0000, 0x00ff00ff, 0x00ffffff};
        /*索引递增*/
        s_nClr++;
        /*如果索引超出范围则重置*/
        if (s_nClr > sizeof(pdwClr) / sizeof(unsigned long))
            s_nClr = 0;
        /*遍历填充像素点*/
        for (int nIdx = 0; nIdx < nArea; nIdx++) {
            pdwScreen[nIdx] = pdwClr[s_nClr];
        }
        /*解锁画布,提交绘制*/
        ANativeWindow_unlockAndPost(app->pendingWindow);
        /*返回*/
        return 1;
        /////////////////////创建窗口缓冲帧 END/////////////////////
    }
    return 0;
}

static void onAppCmd(struct android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            __android_log_print(ANDROID_LOG_DEBUG, "fuke", "engine_handle_cmd APP_CMD_SAVE_STATE");
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            __android_log_print(ANDROID_LOG_DEBUG, "fuke", "engine_handle_cmd APP_CMD_INIT_WINDOW");
            break;
        case APP_CMD_TERM_WINDOW:
            __android_log_print(ANDROID_LOG_DEBUG, "fuke", "engine_handle_cmd APP_CMD_TERM_WINDOW");
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            __android_log_print(ANDROID_LOG_DEBUG, "fuke",
                                "engine_handle_cmd APP_CMD_GAINED_FOCUS");
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            __android_log_print(ANDROID_LOG_DEBUG, "fuke", "engine_handle_cmd APP_CMD_LOST_FOCUS");
            break;
    }
}

extern void android_main(struct android_app *app) {
    app_dummy();
    app->onAppCmd = onAppCmd;
    app->onInputEvent = onInputEvent;
    while (1) {
        int ident;
        int events;
        struct android_poll_source *source;
        while ((ident = ALooper_pollAll(-1, NULL, &events, (void **) &source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(app, source);
            }
            // Check if we are exiting.
            if (app->destroyRequested != 0) {
                return;
            }
        }
    }
}
