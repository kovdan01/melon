package ru.romananchugov.melonmessenger.presentation

import android.app.Application
import dagger.hilt.android.HiltAndroidApp
import timber.log.Timber

@HiltAndroidApp
class App : Application() {


    override fun onCreate() {
        super.onCreate()

        //TODO: use startup library for it
        Timber.plant(Timber.DebugTree())
    }
}