#include <android/log.h>//打印日志的头文件

#ifdef __cplusplus
extern "C" {
#endif

#define LOGOPEN 1 //日志开关，1为开，其它为关
#define LOG_TAG    "[CLOUDWISE-NDK]"
#if(LOGOPEN == 1)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(...) NULL
#define LOGE(...) NULL
#define LOGD(...) NULL
#endif

#ifdef __cplusplus
}
#endif