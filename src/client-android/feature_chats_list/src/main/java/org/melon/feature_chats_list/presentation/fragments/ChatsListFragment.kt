package org.melon.feature_chats_list.presentation.fragments

import android.os.Bundle
import android.view.View
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupAdapter
import com.xwray.groupie.kotlinandroidextensions.GroupieViewHolder
import kotlinx.android.synthetic.main.fragment_chats_list.view.*
import org.melon.feature_chats_list.R
import org.melon.feature_chats_list.di.DaggerChatsListComponent
import org.melon.feature_chats_list.presentation.items.ChatItem
import org.melon.feature_chats_list.presentation.viewmodels.ChatsListViewModel
import org.melon.melonmessenger.presentation.base.BaseFragment
import javax.inject.Inject


class ChatsListFragment : BaseFragment(R.layout.fragment_chats_list) {

    @Inject
    lateinit var viewModel: ChatsListViewModel

    override fun onCreate(savedInstanceState: Bundle?) {
        DaggerChatsListComponent.builder().context(requireContext()).build().inject(this)
        super.onCreate(savedInstanceState)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        val adapter = GroupAdapter<GroupieViewHolder>()

        with(view) {
            chatsListRv.adapter = adapter
            chatsListRv.layoutManager = LinearLayoutManager(requireContext())
            viewModel.chatsLiveData.observe(viewLifecycleOwner, { chats ->
                val chatsList = chats.map {
                    ChatItem(
                        it,
                        viewModel::onChatClick
                    )
                }
                adapter.update(chatsList)
            })

            viewModel.openChatFragment.observe(viewLifecycleOwner, {
                if (it) {
                    findNavController().navigate(R.id.action_chatsListFragment_to_chatContentFragment2)
                    viewModel.onNavigateToChatContent()
                }
            })

            newChatFab.setOnClickListener {
                viewModel.onNewChatClick()
            }
        }
    }
}