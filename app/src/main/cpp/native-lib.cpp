#include <jni.h>
#include <string>
#include "Util.h"

using namespace std;

static JavaVM *j_vm = NULL;
static jclass j_class = NULL;

static jint debug = 1;


//这种方法是最早加载的，比JIN_Onload更早,JIN_Onload是由系统调用的。静态注册的函数是在java层调用时才执行
__attribute__((constructor)) void firstGoHere() {
    LOGE("Go here before JIN_Onload : %d", 99);
};


//静态注册Native方法：无参数,有返回值
extern "C"
JNIEXPORT jstring JNICALL
Java_com_my_ndkdemo_NDKUtil_stringFromJNI(JNIEnv *env, jclass clazz) {
    // TODO: implement stringFromJNI()
    string hello = "have no param, with return";
    return env->NewStringUTF(hello.c_str());
}

//静态注册的Native方法, 有参数无返回值
extern "C"
JNIEXPORT void JNICALL
Java_com_my_ndkdemo_NDKUtil_setJNILogEnable(JNIEnv *env, jclass clazz, jint type) {
    // TODO: implement setJNILogEnable()
    if (type == 1) {
        debug = 1;
    } else {
        debug = 0;
    }
    if (debug == 1) {
        LOGE("你传入的 type = %d, 有日志", type);
    }
}

/************************************************ 以下是动态注册方法 *************************************************/
#define JNI_REG_CLASS "com/my/ndkdemo/NDKUtil" // path of Java file

//动态注册的Native方法（无参数，有返回值）
JNIEXPORT jint JNICALL getVersionCode(JNIEnv *env, jclass clazz) {
    if (debug == 1) {
        LOGE("动态注册的Native方法（无参数，有返回值） ===============  getVersionCode ----------- 10");
    }
    return 10;
}

// 动态注册Native方法（有参数，有返回值）
JNIEXPORT jstring JNICALL getVersion(JNIEnv *env, jclass clazz, jint code) {
    if (debug == 1) {
        LOGE("动态注册Native方法（有参数，有返回值）  ===============    版本首位是5 末位是 3");
    }
    string tmp = to_string(code);
    string version = "5." + tmp + ".3";
    return env->NewStringUTF(version.c_str());
}


// C调用Java方法（带参数不带返回值）
void callVoidFromJava() {
    JNIEnv *env;
    j_vm->AttachCurrentThread(&env, NULL);
    jmethodID methodid = env->GetStaticMethodID(j_class, "getVoidToC", "(ILjava/lang/String;)V");
    env->CallStaticVoidMethod(j_class, methodid, 10, env->NewStringUTF("C-Name not return"));
    if (debug == 1) {
        LOGE("callVoidFromJava  ================ Java To C");
    }
}

// C调用Java方法（带参数带返回值）
char *callStringFromJava() {
    JNIEnv *env;
    j_vm->AttachCurrentThread(&env, NULL);
    jmethodID methodid = env->GetStaticMethodID(j_class, "getStringToC",
                                                "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jstr = (jstring) env->CallStaticObjectMethod(j_class, methodid,
                                                         env->NewStringUTF("C-Name with return"));

    char *java = (char *) env->GetStringUTFChars(jstr, NULL);
    char *addChar = " || cross callStringFromJava";
    strcat(java, addChar); //拼接两个字符串
    if (debug == 1) {
        LOGE("callStringFromJava  ============== Java To C : %s", java);
    }
    return java;
}
//动态注册的Native方法，然后调用Java方法
JNIEXPORT jstring JNICALL callJavaString(JNIEnv *env, jclass clazz) {
    char *result = callStringFromJava();
    return env->NewStringUTF(result);
}

//动态注册的Native方法，然后调用Java方法
JNIEXPORT void JNICALL callJavaVoid(JNIEnv *env, jclass clazz) {
    callVoidFromJava();
}


//C++里方法绑定
static JNINativeMethod g_methods[] = {
        {"getVersionCode", "()I",                   (void *) getVersionCode},
        {"getVersion",     "(I)Ljava/lang/String;", (void *) getVersion},
        {"callJavaVoid",   "()V",                   (void *) callJavaVoid},
        {"callJavaString", "()Ljava/lang/String;",  (void *) callJavaString}
};

static int
registerNativeMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods,
                      int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);

    if (clazz == NULL) {
        return -1;
    }

    j_class = (jclass) env->NewGlobalRef((jobject) clazz);

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return -1;
    }

    return 0;
}

static int registerNative(JNIEnv *env) {
    return registerNativeMethods(env, JNI_REG_CLASS, g_methods,
                                 sizeof(g_methods) / sizeof(g_methods[0]));
}

// must define this function
JNIEXPORT int JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass javaClass = env->FindClass(JNI_REG_CLASS);
    if (javaClass == NULL) {
        return JNI_ERR;
    }

    int method_count = sizeof(g_methods) / sizeof(g_methods[0]);
    if (env->RegisterNatives(javaClass, g_methods, method_count) < 0) {
        return JNI_ERR;
    }

    if (registerNative(env) != JNI_OK) {
        return -1;
    }

    j_vm = vm;

    return JNI_VERSION_1_6;
}
