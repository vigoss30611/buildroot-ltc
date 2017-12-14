
package com.infotm.dv.view;

import android.app.Dialog;
import android.content.Context;
import android.util.Log;
import android.widget.ViewAnimator;

import com.infotm.dv.R;

public class SmartyProgressBar extends Dialog {
    private static final String tag = "SmartyProgressBar";
    private static final int INDEX_CHECK = 1;
    private static final int INDEX_CROSS = 2;
    private static final int INDEX_SPINNER = 0;
    ViewAnimator mWrapper;
    int maxTime = 0;

    public SmartyProgressBar(Context paramContext) {
        super(paramContext, R.style.SmartyProgressBar);
        setContentView(R.layout.include_progress_bar);
        mWrapper = ((ViewAnimator) findViewById(R.id.wrapper_progress_bar));
        mWrapper.setDisplayedChild(0);
    }

    public void onFail() {
        mWrapper.setDisplayedChild(2);
    }

    public void onSuccess() {
        mWrapper.setDisplayedChild(1);
    }

    public void show() {
        maxTime = 0;
        mWrapper.setDisplayedChild(0);
        super.show();
    }
    
    public void show(int maxTime) {
        this.maxTime = maxTime;
        mWrapper.setDisplayedChild(0);
        super.show();
    }
    
    public void timeOutDismiss(){
        Log.v(tag, "----timeOutDismiss----");
        if (maxTime != 0){
            super.dismiss();
        }
    }
    
    @Override
    public void dismiss() {
        if (maxTime == 0){
            super.dismiss();
        }
    }
}
