package org.melon.feature_chat_content.di

import dagger.Binds
import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import org.melon.feature_chat_content.data.chat_content.ChatContentRepositoryImpl
import org.melon.feature_chat_content.domain.chat_content.ChatContentRepository

@Module
@InstallIn(SingletonComponent::class)
abstract class ChatContentModule {

    @Binds
    abstract fun bindChatContentRepository(chatsListRepositoryImpl: ChatContentRepositoryImpl): ChatContentRepository
}
