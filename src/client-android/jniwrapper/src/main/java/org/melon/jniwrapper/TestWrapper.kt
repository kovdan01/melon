package org.melon.jniwrapper

class TestWrapper {


    companion object {
        /*
         * this is used to load the 'hello-jni' library on application
         * startup. The library has already been unpacked into
         * /data/data/com.example.hellojni/lib/libhello-jni.so
         * at the installation time by the package manager.
         */
        init {
            System.loadLibrary("hello-jni")
        }
    }

    external fun another(): Int

    fun testResult() = another()
}