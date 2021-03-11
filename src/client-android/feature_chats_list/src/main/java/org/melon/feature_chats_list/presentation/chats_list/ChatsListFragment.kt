package org.melon.feature_chats_list.presentation.chats_list

import android.os.Bundle
import android.view.View
import androidx.fragment.app.setFragmentResultListener
import androidx.fragment.app.viewModels
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupAdapter
import com.xwray.groupie.GroupieViewHolder
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.android.synthetic.main.fragment_chats_list.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chats_list.R
import org.melon.feature_chats_list.presentation.chat_creation.ChatCreationFragment

@AndroidEntryPoint
class ChatsListFragment : BaseFragment(R.layout.fragment_chats_list) {

    private val viewModel by viewModels<ChatsListViewModel>()

    override fun onCreate(savedInstanceState: Bundle?) {

        setFragmentResultListener(ChatCreationFragment.REQUEST_KEY_CHAT_CREATION) { requestKey, bundle ->
            // We use a String here, but any type that can be put in a Bundle is supported
            val result = bundle.getString(ChatCreationFragment.BUNDLE_KEY_CHAT_NAME).toString()
            viewModel.onNewChatCreated(result)
            // Do something with the result
        }

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
                    findNavController().navigate(R.id.chatContentAction)
                    viewModel.onNavigateToChatContent()
                }
            })

            viewModel.openCreateChatFragment.observe(viewLifecycleOwner, {
                if (it) {
                    findNavController().navigate(R.id.chatCreationAction)
                    viewModel.onNavigateToChatCreate()
                }
            })

            newChatFab.setOnClickListener {
                viewModel.onNewChatClick()
            }
        }
    }
}
