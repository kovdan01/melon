package org.melon.melonmessenger.di

import android.content.Context
import androidx.room.Room
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import org.melon.melonmessenger.data.AppDatabase
import javax.inject.Singleton


@Module
@InstallIn(SingletonComponent::class)
class AppDatabaseModule {

    @Provides
    @Singleton
    fun providesChatContentDao(appDatabase: AppDatabase) =
        appDatabase.chatContentDao()

    @Provides
    @Singleton
    fun providesChatsListDao(appDatabase: AppDatabase) =
        appDatabase.chatsListDao()


    @Provides
    @Singleton
    fun providesChatContentDatabase(
        @ApplicationContext context: Context
    ): AppDatabase = Room.databaseBuilder(
        context,
        AppDatabase::class.java,
        AppDatabase.NAME
    ).build()
}