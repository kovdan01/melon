#include <melon/core/jni_tests.hpp>

#include <jni.h>

extern "C" JNIEXPORT jint JNICALL
Java_org_melon_jniwrapper_TestWrapper_another(JNIEnv *env, jobject thiz)
{
    return 2;
}