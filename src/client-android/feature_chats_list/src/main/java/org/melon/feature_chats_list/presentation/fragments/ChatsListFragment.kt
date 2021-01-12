package org.melon.feature_chats_list.presentation.fragments

import android.os.Bundle
import android.view.View
import androidx.fragment.app.setFragmentResultListener
import androidx.navigation.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.xwray.groupie.GroupAdapter
import com.xwray.groupie.kotlinandroidextensions.GroupieViewHolder
import kotlinx.android.synthetic.main.fragment_chats_list.view.*
import org.melon.feature_chats_list.R
import org.melon.feature_chats_list.di.DaggerChatsListComponent
import org.melon.feature_chats_list.presentation.items.ChatItem
import org.melon.feature_chats_list.presentation.viewmodels.ChatsListViewModel
import org.melon.melonmessenger.presentation.base.BaseFragment
import timber.log.Timber
import javax.inject.Inject


class ChatsListFragment : BaseFragment(R.layout.fragment_chats_list) {

    @Inject
    lateinit var viewModel: ChatsListViewModel

    override fun onCreate(savedInstanceState: Bundle?) {
        DaggerChatsListComponent.builder().context(requireContext()).build().inject(this)
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
