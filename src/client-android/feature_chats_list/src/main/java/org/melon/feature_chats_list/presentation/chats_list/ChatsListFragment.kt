package org.melon.feature_chats_list.presentation.chats_list

import android.os.Bundle
import android.view.View
import androidx.fragment.app.setFragmentResultListener
import androidx.fragment.app.viewModels
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.xwray.groupie.GroupieAdapter
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.android.synthetic.main.fragment_chats_list.*
import org.melon.core.presentation.base.BaseFragment
import org.melon.feature_chat_content.presentation.chat_content.ChatContentFragment
import org.melon.feature_chats_list.R
import org.melon.feature_chats_list.presentation.chat_creation.ChatCreationFragment
import org.melon.feature_chats_list.presentation.chat_creation.ChatCreationFragmentDirections


@AndroidEntryPoint
class ChatsListFragment : BaseFragment(R.layout.fragment_chats_list) {

    private val viewModel by viewModels<ChatsListViewModel>()

    override fun onCreate(savedInstanceState: Bundle?) {

        setFragmentResultListener(ChatCreationFragment.REQUEST_KEY_CHAT_CREATION) { requestKey, bundle ->
            val result = bundle.getString(ChatCreationFragment.BUNDLE_KEY_CHAT_NAME).toString()
            viewModel.onNewChatCreated(result)
        }

        setFragmentResultListener(ChatCreationFragment.REQUEST_KEY_CHAT_RENAME) { requestKey, bundle ->
            bundle.getParcelable<ChatUi>(ChatCreationFragment.BUNDLE_KEY_CHAT_UI)
                ?.let(viewModel::onChatRenamed)
        }

        //TODO: remade it maybe with shared view model or sth
        setFragmentResultListener(ChatContentFragment.REQUEST_KEY_CHAT_UPDATE) { requestKey, bundle ->
            viewModel.onUpdateChatInfo(
                bundle.getParcelable(ChatContentFragment.BUNDLE_KEY_MESSAGE_UI)!!
            )
        }

        super.onCreate(savedInstanceState)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        val adapter = GroupieAdapter()

        with(view) {
            chatsListRv.adapter = adapter
            chatsListRv.layoutManager = LinearLayoutManager(requireContext())
            viewModel.chatsLiveData.observe(viewLifecycleOwner, { chats ->
                val chatsList = chats.map {
                    ChatItem(
                        it,
                        onLongClickListener = viewModel::onChatLongClick,
                        onClickListener = viewModel::onChatClick
                    )
                }
                adapter.update(chatsList)
            })

            viewModel.openChatFragment.observe(viewLifecycleOwner, {
                if (it != null) {
                    findNavController().navigate(ChatsListFragmentDirections.chatContentAction(it.chatId))
                    viewModel.onNavigateToChatContent()
                }
            })

            viewModel.openCreateChatFragment.observe(viewLifecycleOwner, {
                if (it) {
                    findNavController().navigate(
                        ChatCreationFragmentDirections.chatCreationAction(
                            null
                        )
                    )
                    viewModel.onNavigateToChatCreate()
                }
            })

            viewModel.openChatEditDialog.observe(viewLifecycleOwner, { chatUi ->
                if (chatUi != null) {
                    MaterialAlertDialogBuilder(context)
                        .setTitle(R.string.chat_edit_title)
                        .setItems(R.array.chat_edit_variants) { dialog, actionIndex ->
                            when (actionIndex) {
                                ChatEditAction.Rename.index -> {
                                    findNavController().navigate(
                                        ChatCreationFragmentDirections.chatCreationAction(
                                            chatUi
                                        )
                                    )
                                }
                                ChatEditAction.Delete.index -> {
                                    viewModel.onChatDelete(chatUi)
                                }
                                ChatEditAction.MarkAsUnread.index -> {
                                    viewModel.onChatMarkAsUnread(chatUi)
                                }
                            }
                        }
                        .show()

                    viewModel.onChatEditOpened()
                }
            })

            newChatFab.setOnClickListener {
                viewModel.onNewChatClick()
            }

            newChatFab.setOnLongClickListener {
                viewModel.createStubChat()
                true
            }
        }

        viewModel.onViewCreated()
    }

    sealed class ChatEditAction(val index: Int) {
        object Rename : ChatEditAction(0)
        object Delete : ChatEditAction(1)
        object MarkAsUnread : ChatEditAction(2)
    }
}
