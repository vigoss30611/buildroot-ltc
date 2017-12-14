package com.infotm.dv.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.MeasureSpec;
import android.view.ViewParent;
import android.widget.AbsSeekBar;

public class VerticalSeekBar extends AbsSeekBar
{
  private String Tag="VerticalSeekBar";
  private int height;
  private boolean isFromUser;
  private OnSeekBarChangeListener mOnSeekBarChangeListener;
  private Drawable mThumb;
  private int width;

  public VerticalSeekBar(Context paramContext)
  {
    this(paramContext, null);
  }

  public VerticalSeekBar(Context paramContext, AttributeSet paramAttributeSet)
  {
    this(paramContext, paramAttributeSet, 16842875);
  }

  public VerticalSeekBar(Context paramContext, AttributeSet paramAttributeSet, int paramInt)
  {
    super(paramContext, paramAttributeSet, paramInt);
  }

  private void attemptClaimDrag()
  {
    if (getParent() == null)
      return;
    getParent().requestDisallowInterceptTouchEvent(true);
  }

  private void setThumbPos(int paramInt1, Drawable paramDrawable, float paramFloat, int paramInt2)
  {
    int i = paramInt1 - getPaddingLeft() - getPaddingRight();
    int j = paramDrawable.getIntrinsicWidth();
    int k = paramDrawable.getIntrinsicHeight();
    int l = (int)(paramFloat * (i - j + 2 * getThumbOffset()));
    Rect localRect = null;
    int i1 = 0;
    if (paramInt2 == -2147483648)
    {
      localRect = paramDrawable.getBounds();
      i1 = localRect.top;
    }
    for (int i2 = localRect.bottom; ; i2 = paramInt2 + k)
    {
      paramDrawable.setBounds(l, i1, l + j, i2);
     // return;
      i1 = -paramInt2;
    }
  }

  private void trackTouchEvent(MotionEvent paramMotionEvent)
  {
    int i = getHeight();
    int j = i - getPaddingBottom() - getPaddingTop();
    int k = (int)paramMotionEvent.getY();
    float f = 0;
    if (k > i - getPaddingBottom())
      f = 0.0F;
    while (true)
    {
      setProgress((int)(f * getMax()));
    //  return;
      if (k < getPaddingTop())
        f = 1.0F;
      f = (i - getPaddingBottom() - k) / j;
    }
  }

  public boolean dispatchKeyEvent(KeyEvent paramKeyEvent)
  {
    int i = paramKeyEvent.getAction();
    boolean bool = false;
    if (i == 0)
      switch (paramKeyEvent.getKeyCode())
      {
      default:
      case 19:
      case 20:
      case 21:
      case 22:
      }
    for (KeyEvent localKeyEvent = new KeyEvent(0, paramKeyEvent.getKeyCode()); ; localKeyEvent = new KeyEvent(0, 19))
      while (true)
      {
        bool = localKeyEvent.dispatch(this);
    //    return bool;
        localKeyEvent = new KeyEvent(0, 22);
     //   continue;
        localKeyEvent = new KeyEvent(0, 21);
       // continue;
        localKeyEvent = new KeyEvent(0, 20);
      }
  }

  protected void onDraw(Canvas paramCanvas)
  {
    paramCanvas.rotate(-90.0F);
    paramCanvas.translate(-this.height, 0.0F);
    super.onDraw(paramCanvas);
  }

  protected void onMeasure(int paramInt1, int paramInt2)
  {
    this.height = View.MeasureSpec.getSize(paramInt2);
    this.width = View.MeasureSpec.getSize(paramInt1);
    setMeasuredDimension(this.width, this.height);
  }

  void onProgressRefresh(float paramFloat, boolean paramBoolean)
  {
    Drawable localDrawable = this.mThumb;
    if (localDrawable != null)
    {
      setThumbPos(getHeight(), localDrawable, paramFloat, -2147483648);
      invalidate();
    }
    if (this.mOnSeekBarChangeListener == null)
      return;
    this.mOnSeekBarChangeListener.onProgressChanged(this, getProgress(), this.isFromUser);
  }

  protected void onSizeChanged(int paramInt1, int paramInt2, int paramInt3, int paramInt4)
  {
    super.onSizeChanged(paramInt2, paramInt1, paramInt3, paramInt4);
  }

  void onStartTrackingTouch()
  {
    if (this.mOnSeekBarChangeListener == null)
      return;
    this.mOnSeekBarChangeListener.onStartTrackingTouch(this);
  }

  void onStopTrackingTouch()
  {
    if (this.mOnSeekBarChangeListener == null)
      return;
    this.mOnSeekBarChangeListener.onStopTrackingTouch(this);
  }

  public boolean onTouchEvent(MotionEvent paramMotionEvent)
  {
    if (!isEnabled())
      return false;
    switch (paramMotionEvent.getAction())
    {
    default:
    case 0:
    case 2:
    case 1:
    case 3:
    }
    while (true)
    {Log.i(Tag,"onTouchEvent");
 //     return true;
      this.isFromUser = true;
      setPressed(true);
      onStartTrackingTouch();
      trackTouchEvent(paramMotionEvent);
   //   continue;
      trackTouchEvent(paramMotionEvent);
      attemptClaimDrag();
   //   continue;
      this.isFromUser = false;
      trackTouchEvent(paramMotionEvent);
      onStopTrackingTouch();
      setPressed(false);
    //  continue;
      this.isFromUser = false;
      onStopTrackingTouch();
      setPressed(false);
    }
  }

  public void setOnSeekBarChangeListener(OnSeekBarChangeListener paramOnSeekBarChangeListener)
  {
    this.mOnSeekBarChangeListener = paramOnSeekBarChangeListener;
  }

  public void setThumb(Drawable paramDrawable)
  {
    this.mThumb = paramDrawable;
    super.setThumb(null);
  }

  public static abstract interface OnSeekBarChangeListener
  {
    public abstract void onProgressChanged(VerticalSeekBar paramVerticalSeekBar, int paramInt, boolean paramBoolean);

    public abstract void onStartTrackingTouch(VerticalSeekBar paramVerticalSeekBar);

    public abstract void onStopTrackingTouch(VerticalSeekBar paramVerticalSeekBar);
  }
}

/* Location:           Z:\owen\ge\software\Android反编译工具包（升级）\java_decom\ApkDec-Release\Danale-audio_2537275081330.jar
 * Qualified Name:     com.danale.video.view.VerticalSeekBar
 * JD-Core Version:    0.5.4
 */