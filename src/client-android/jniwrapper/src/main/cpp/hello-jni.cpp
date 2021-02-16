#include <cstring>
#include <jni.h>
#include <iostream>
#include <melon/jni_tests.hpp>


extern "C" JNIEXPORT jint JNICALL
Java_org_melon_jniwrapper_TestWrapper_another(JNIEnv *env, jobject thiz) {
    auto str = string_test();

    return str.length();
}