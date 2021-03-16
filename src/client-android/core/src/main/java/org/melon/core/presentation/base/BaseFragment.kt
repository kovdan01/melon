package org.melon.core.presentation.base

import android.os.Bundle
import android.view.View
import androidx.activity.OnBackPressedCallback
import androidx.annotation.LayoutRes
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController

open class BaseFragment(@LayoutRes layoutRes: Int) : Fragment(layoutRes) {

}
