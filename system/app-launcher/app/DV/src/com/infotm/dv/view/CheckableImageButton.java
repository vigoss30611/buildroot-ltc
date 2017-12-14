package com.infotm.dv.view;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.Checkable;
import android.widget.ImageButton;

public class CheckableImageButton extends ImageButton implements Checkable{
  private static final int[] CHECKED_STATE_SET;
  private boolean mChecked;

  static{
    int[] arrayOfInt = new int[1];
    arrayOfInt[0] = 16842912;//0x10100A0
    CHECKED_STATE_SET = arrayOfInt;
  }

  public CheckableImageButton(Context paramContext, AttributeSet paramAttributeSet){
    super(paramContext, paramAttributeSet);
  }

  public boolean isChecked(){
    return this.mChecked;
  }

  public int[] onCreateDrawableState(int paramInt) {
    int[] arrayOfInt = super.onCreateDrawableState(paramInt + 1);
    if (isChecked())
      mergeDrawableStates(arrayOfInt, CHECKED_STATE_SET);
    return arrayOfInt;
  }

  public void setChecked(boolean paramBoolean) {
    if (this.mChecked != paramBoolean) {
      this.mChecked = paramBoolean;
      refreshDrawableState();
    }
  }

  public void toggle() {
    if (mChecked){
        setChecked(false);
        mChecked = false;
    } else {
        setChecked(true);
        mChecked = true;
    }
  }
}