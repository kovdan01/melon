package ru.romananchugov.feature_chats_list.presentation.fragments

import android.os.Bundle
import android.view.View
import androidx.activity.OnBackPressedCallback
import androidx.navigation.fragment.findNavController
import ru.romananchugov.feature_chats_list.R
import ru.romananchugov.melonmessenger.presentation.base.BaseFragment

class ChatContentFragment : BaseFragment(R.layout.fragment_chat_content) {
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        requireActivity().onBackPressedDispatcher.addCallback(
            viewLifecycleOwner,
            object : OnBackPressedCallback(true) {
                override fun handleOnBackPressed() {
                    //TODO: why should i use it???
                    findNavController().popBackStack()
                }
            })
    }
}