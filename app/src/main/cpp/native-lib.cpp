#include <jni.h>
#include <string>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

static JavaVM *j_vm = NULL;
static jclass j_class = NULL;

static jint debug = 1;

//静态注册Native方法：无参数
extern "C"
JNIEXPORT jstring JNICALL
Java_com_my_ndkdemo_NDKUtil_stringFromJNI(JNIEnv *env, jclass clazz) {
    // TODO: implement stringFromJNI()
    string hello = "静态注册Native方法：无参数 ";
    return env->NewStringUTF(hello.c_str());
}

//静态注册的Native方法, 有参数无返回值
extern "C"
JNIEXPORT void JNICALL
Java_com_my_ndkdemo_NDKUtil_setJNILogEnable(JNIEnv *env, jclass clazz, jint type) {
    // TODO: implement setJNILogEnable()
    printf("你传的参数是:%d", type);
}

/************************************************ 以下是动态注册方法 *************************************************/
#define JNI_REG_CLASS "com/my/ndkdemo/NDKUtil" // path of Java file

//动态注册的Native方法
JNIEXPORT jint JNICALL getVersionCode(JNIEnv *env, jclass clazz) {
    return 42;
}

// 动态注册Native方法
JNIEXPORT jstring JNICALL getVersion(JNIEnv *env, jclass arg, jint code){
    string tmp = to_string(code);
    string version = "5." + tmp + ".3";
    return env->NewStringUTF(version.c_str());
}

//C调用Java方法（带参数带返回值）
void callStringFromJava(){
    JNIEnv *env;
    j_vm->AttachCurrentThread(&env, NULL);
    jmethodID methodid = env->GetStaticMethodID(j_class,"getStringToC","(Ljava/lang/String;)Ljava/lang/String;");
    jstring str = (jstring)env->CallStaticObjectMethod(j_class, methodid,env->NewStringUTF("C-Name"));
    char* java = (char*)env->GetStringUTFChars(str, NULL);
    printf("%s",java);
}
//动态注册的Native方法，然后调用Java方法
JNIEXPORT void JNICALL callJavaString(JNIEnv *env, jclass arg) {
    callStringFromJava();
}


//C++里方法绑定
static JNINativeMethod g_methods[] = {
        {"getVersionCode", "()I", (void *) getVersionCode},
        {"getVersion", "(I)Ljava/lang/String;", (void*)getVersion},
        {"callJavaString", "()V", (void*)callJavaString}
};

static int registerNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods){
    jclass clazz;
    clazz = env->FindClass(className);

    if(clazz == NULL){
        return -1;
    }

    j_class = (jclass)env-> NewGlobalRef((jobject)clazz);

    if(env->RegisterNatives(clazz, gMethods, numMethods) < 0){
        return -1;
    }

    return 0;
}

static int registerNative(JNIEnv* env){
    return registerNativeMethods(env, JNI_REG_CLASS, g_methods, sizeof(g_methods)/ sizeof(g_methods[0]));
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

    if(registerNative(env) != JNI_OK){
        return -1;
    }

    j_vm = vm;

    return JNI_VERSION_1_6;
}
