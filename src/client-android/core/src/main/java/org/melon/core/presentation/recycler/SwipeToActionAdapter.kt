package org.melon.core.presentation.recycler

import com.xwray.groupie.GroupieAdapter
import timber.log.Timber

class SwipeToActionAdapter : GroupieAdapter(), ItemTouchHelperAdapter {

    override fun onItemMove(fromPosition: Int, toPosition: Int) {
        Timber.tag("LOL").i("onItemMove: $fromPosition to $toPosition")
    }

    override fun onItemDismiss(position: Int) {
        Timber.tag("LOL").i("Dismissed: $position")
    }
}