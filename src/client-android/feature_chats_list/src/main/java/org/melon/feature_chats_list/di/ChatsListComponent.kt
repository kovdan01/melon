package org.melon.feature_chats_list.di

import android.content.Context
import dagger.BindsInstance
import dagger.Component
import org.melon.feature_chats_list.presentation.fragments.ChatCreationFragment
import org.melon.feature_chats_list.presentation.fragments.ChatsListFragment

@Component(modules = [ChatsListModule::class])
interface ChatsListComponent {
    fun inject(fragment: ChatsListFragment)
    fun inject(fragment: ChatCreationFragment)

    @Component.Builder
    interface Builder {
        fun context(@BindsInstance context: Context): Builder
        fun build(): ChatsListComponent
    }
}
