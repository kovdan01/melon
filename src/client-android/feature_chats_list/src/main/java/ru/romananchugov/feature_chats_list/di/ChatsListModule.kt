package ru.romananchugov.feature_chats_list.di

import dagger.Binds
import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.android.components.ApplicationComponent
import dagger.hilt.android.components.FragmentComponent
import ru.romananchugov.feature_chats_list.data.ChatsListRepository
import ru.romananchugov.feature_chats_list.data.ChatsListRepositoryImpl
import ru.romananchugov.feature_chats_list.domain.ChatsListUseCase
import ru.romananchugov.feature_chats_list.domain.ChatsListUseCaseImpl

@Module
@InstallIn(FragmentComponent::class)
abstract class ChatsListModule {
    @Binds
    abstract fun bindChatsListRepository(chatsListRepositoryImpl: ChatsListRepositoryImpl): ChatsListRepository

    @Binds
    abstract fun bindChatsListUseCase(chatsListUseCaseImpl: ChatsListUseCaseImpl): ChatsListUseCase
}