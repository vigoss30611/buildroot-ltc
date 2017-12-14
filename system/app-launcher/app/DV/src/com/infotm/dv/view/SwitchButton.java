package com.infotm.dv.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import com.infotm.dv.R;

public class SwitchButton extends View{;
    OnChangeListener mListener;
    public Boolean mSwitchOn=false;
    public SwitchButton(Context context) {
        super(context);
    }

    public SwitchButton(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
    	Bitmap mSwitchOnBitmap = BitmapFactory.decodeResource(getResources(),R.drawable.danale_cloud_open);
    	Bitmap mSwitchOffBitmap = BitmapFactory.decodeResource(getResources(),R.drawable.danale_cloud_off);
        Paint paint = new Paint();
        paint.setColor(Color.RED);
        if(mSwitchOn){
        	canvas.drawBitmap(mSwitchOnBitmap, 0,0, paint);
        }else{
        	canvas.drawBitmap(mSwitchOffBitmap, 0,0, paint);
        }
        
    }
    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
    	// TODO Auto-generated method stub
    	
    	  switch (event.getAction()) {

    	  case MotionEvent.ACTION_DOWN:
    	        mSwitchOn = !mSwitchOn;  
    	        if(mListener != null) {  
    	            mListener.onChange(this, mSwitchOn);  
    	        }  
    	        invalidate(); 
    	   break;

    	  case MotionEvent.ACTION_MOVE:

    	   break;

    	  case MotionEvent.ACTION_UP:

    	   break;
    	  }
    	

    	return super.dispatchTouchEvent(event);

    }
    public void setChecked(boolean checked){
    	mSwitchOn=checked;
    	invalidate(); 
    }
	  public void setOnChangeListener(OnChangeListener listener) {  
	        mListener = listener;  
	    }  
	      
	    public interface OnChangeListener {  
	        public void onChange(SwitchButton sb, boolean state);  
	    } 
	  
}

	
