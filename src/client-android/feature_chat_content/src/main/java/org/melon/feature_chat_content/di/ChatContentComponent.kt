package org.melon.feature_chat_content.di

import android.content.Context
import dagger.BindsInstance
import dagger.Component
import org.melon.feature_chat_content.presentation.chat_content.ChatContentFragment

@Component(modules = [ChatContentModule::class])
interface ChatContentComponent {

    fun inject(fragment: ChatContentFragment)

    @Component.Builder
    interface Builder {
        fun context(@BindsInstance context: Context): Builder
        fun build(): ChatContentComponent
    }
}
