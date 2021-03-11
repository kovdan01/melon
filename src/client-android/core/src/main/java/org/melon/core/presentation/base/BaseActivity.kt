package org.melon.core.presentation.base

import androidx.annotation.LayoutRes
import androidx.appcompat.app.AppCompatActivity

open class BaseActivity : AppCompatActivity {
    constructor() : super()
    constructor(@LayoutRes layoutRes: Int) : super(layoutRes)
}
