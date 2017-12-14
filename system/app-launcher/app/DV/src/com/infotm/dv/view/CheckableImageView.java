package com.infotm.dv.view;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.Checkable;
import android.widget.ImageView;

public class CheckableImageView extends ImageView implements Checkable{
    private static final int[] CHECKED_STATE_SET;
    private boolean mChecked;

    static{
        int[] arrayOfInt = new int[1];
        arrayOfInt[0] = 16842912;
        CHECKED_STATE_SET = arrayOfInt;
    }

    public CheckableImageView(Context paramContext, AttributeSet paramAttributeSet) {
        super(paramContext, paramAttributeSet);
    }

    public boolean isChecked() {
        return mChecked;
    }

    public int[] onCreateDrawableState(int paramInt) {
        int[] arrayOfInt = super.onCreateDrawableState(paramInt + 1);
        if (isChecked()){
            mergeDrawableStates(arrayOfInt, CHECKED_STATE_SET);
        }
        return arrayOfInt;
    }

    public void setChecked(boolean b) {
        if (this.mChecked != b) {
            this.mChecked = b;
            refreshDrawableState();
        }
    }

    public void toggle() {
        boolean bool = false;
        if (!mChecked){
            bool = true;
        }
        setChecked(bool);
    }
}
