package org.melon.feature_chats_list.di

import dagger.Binds
import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import org.melon.feature_chats_list.data.ChatsListRepository
import org.melon.feature_chats_list.data.ChatsListRepositoryImpl
import org.melon.feature_chats_list.domain.ChatsListUseCase
import org.melon.feature_chats_list.domain.ChatsListUseCaseImpl

@Module
@InstallIn(SingletonComponent::class)
abstract class ChatsListModule {
    @Binds
    abstract fun bindChatsListRepository(chatsListRepositoryImpl: ChatsListRepositoryImpl): ChatsListRepository

    @Binds
    abstract fun bindChatsListUseCase(chatsListUseCaseImpl: ChatsListUseCaseImpl): ChatsListUseCase
}
