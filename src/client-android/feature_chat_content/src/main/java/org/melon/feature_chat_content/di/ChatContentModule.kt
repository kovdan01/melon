package org.melon.feature_chat_content.di

import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.android.components.FragmentComponent

@Module
@InstallIn(FragmentComponent::class)
abstract class ChatContentModule {
//    @Binds
//    abstract fun bindChatsListRepository(chatsListRepositoryImpl: ChatsListRepositoryImpl): ChatsListRepository
//
//    @Binds
//    abstract fun bindChatsListUseCase(chatsListUseCaseImpl: ChatsListUseCaseImpl): ChatsListUseCase
}
