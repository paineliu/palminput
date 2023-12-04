#include <jni.h>
#include <android/asset_manager_jni.h>
#include "PYAdapter.h"

void * g_py_adapter;

extern "C" JNIEXPORT jint JNICALL
Java_com_palminput_engine_PYProvider_create(
        JNIEnv* env,
        jobject /* this */,
        jobject assetManager,
        jstring dictPathname,
        jstring userPathname) {

    const char *c_dict_pathname = env->GetStringUTFChars(dictPathname, 0);
    const char *c_user_pathname = env->GetStringUTFChars(userPathname, 0);

    AAssetManager *nativeAsset = AAssetManager_fromJava(env, assetManager);
    g_py_adapter = PYAdapter_Create(nativeAsset, c_dict_pathname, c_user_pathname);

    env->ReleaseStringUTFChars(dictPathname, c_dict_pathname);
    env->ReleaseStringUTFChars(userPathname, c_user_pathname);

    if (g_py_adapter != 0) {
        return 0;
    }

    return -1;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_palminput_engine_PYProvider_getCompString(
        JNIEnv* env,
        jobject /* this */) {
    const char *str = PYAdapter_GetCompStringUTF8(g_py_adapter);
    return env->NewStringUTF(str);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_palminput_engine_PYProvider_getCommitString(
        JNIEnv* env,
        jobject /* this */) {
    const char *str = PYAdapter_GetCommitStringUTF8(g_py_adapter);
    return env->NewStringUTF(str);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_palminput_engine_PYProvider_addKey(
        JNIEnv* env,
        jobject /* this */,
        jint key) {
    PYAdapter_AddKey(g_py_adapter, key);
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_palminput_engine_PYProvider_getCandItemTotal(
        JNIEnv* env,
        jobject /* this */) {
    return PYAdapter_GetCandItemTotal(g_py_adapter);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_palminput_engine_PYProvider_getCandItemString(
        JNIEnv* env,
        jobject /* this */,
        jint id) {
    const char *str = PYAdapter_GetCandStringUTF8(g_py_adapter, id);
    return env->NewStringUTF(str);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_palminput_engine_PYProvider_selCandItem(
        JNIEnv* env,
        jobject /* this */,
        jint id) {
    return PYAdapter_SelCandItem(g_py_adapter, id);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_palminput_engine_PYProvider_getPinyinItemTotal(
        JNIEnv* env,
        jobject /* this */) {

    return PYAdapter_GetPinyinItemTotal(g_py_adapter);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_palminput_engine_PYProvider_getPinyinItem(
        JNIEnv* env,
        jobject /* this */,
        jint id) {
    const char *str = PYAdapter_GetPinyinStringUTF8(g_py_adapter, id);
    return env->NewStringUTF(str);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_palminput_engine_PYProvider_selPinyinItem(
        JNIEnv* env,
        jobject /* this */,
        jint id) {
    return PYAdapter_SelPinyinItem(g_py_adapter, id);
}

extern "C" JNIEXPORT void JNICALL
Java_com_palminput_engine_PYProvider_reset(
        JNIEnv* env,
        jobject /* this */) {
    PYAdapter_Reset(g_py_adapter);
}

extern "C" JNIEXPORT int JNICALL
Java_com_palminput_engine_PYProvider_save(
        JNIEnv* env,
        jobject /* this */) {
    PYAdapter_Save(g_py_adapter);
    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_palminput_engine_PYProvider_destroy(
        JNIEnv* env,
        jobject /* this */) {
    PYAdapter_Destroy(g_py_adapter);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_palminput_engine_PYProvider_setOptions(JNIEnv *env, jobject thiz, jobject opt_obj) {
    jclass opt_cls = env->GetObjectClass(opt_obj); //或得Student类引用
    jboolean tRet = false;

    if (opt_cls != NULL)
    {
        jfieldID fuzzyFlagsFieldID = env->GetFieldID(opt_cls,"mFuzzyFlags","I");
        jfieldID predictEnableFieldID = env->GetFieldID(opt_cls,"mPredictEnable","Z");

        jint fuzzyFlags = env->GetIntField(opt_obj, fuzzyFlagsFieldID);  //获得属性值
        jboolean predictEnable = env->GetBooleanField(opt_obj, predictEnableFieldID);//获得属性值

        PY_OPTION stOption;
        stOption.nFuzzyFlags = fuzzyFlags;
        stOption.bEnablePredict = predictEnable;
        PYAdapter_SetOptions(g_py_adapter, &stOption);

        tRet = true;
    }

    return tRet;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_palminput_engine_PYProvider_getOptions(JNIEnv *env, jobject thiz) {
    jclass opt_cls = env->FindClass("com/freeinput/engine/PYOptions");

    jmethodID constructMID = env->GetMethodID(opt_cls, "<init>", "(IZ)V");

    const PY_OPTION *pyOption = PYAdapter_GetOptions(g_py_adapter);

    jobject stu_ojb = env->NewObject(opt_cls, constructMID, (int) pyOption->nFuzzyFlags, pyOption->bEnablePredict != 0);

    return stu_ojb;
}