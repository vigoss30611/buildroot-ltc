package com.infotm.dv;

import java.util.ArrayList;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.TextView;

import com.infotm.dv.camera.InfotmCamera;
import com.nostra13.universalimageloader.core.ImageLoader;

public class ShowDevicePictureActivity extends Activity{
    private View mControlView;
    private TextView mNameView;
    private ImageView mMediaBackView;
    private ImageView mPrevButton;
    private ImageView mNextButton;
    private ImageView mShowPicture;
    private String mCurrentMediaFile;
    private ArrayList<String> mMediaList;
    private static final int MSG_HIDE_CONTROL = 1026;
    private Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_show_device_picture);
        mControlView = findViewById(R.id.playback_info_control);
        mNameView = (TextView) findViewById(R.id.media_name);
        mShowPicture= (ImageView) findViewById(R.id.show_picture);
        mCurrentMediaFile = getIntent().getStringExtra(InfotmCamera.MEDIA_FILE_NAME);
         mMediaList = InfotmCamera.mPhotoList;
        mPrevButton = (ImageView) findViewById(R.id.btn_play_prev);
        mPrevButton.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				startShowPrevPicture();
			}
		});
        mNextButton = (ImageView) findViewById(R.id.btn_play_next);
        mNextButton.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				startShowNextPicture();
			}
		});


         mMediaBackView = (ImageView) findViewById(R.id.media_back);
         mMediaBackView.setOnClickListener(new OnClickListener() {
 			
 			@Override
 			public void onClick(View arg0) {
 				// TODO Auto-generated method stub
             	finish();
 			}
 		});
         mHandler = new Handler() {
             @Override
             public void handleMessage(Message msg) {
                 // TODO Auto-generated method stub
                 switch(msg.what) {
                     case MSG_HIDE_CONTROL:
                         mControlView.setVisibility(View.INVISIBLE);
                         break;
                 }
             }
         };
         mHandler.sendEmptyMessageDelayed(MSG_HIDE_CONTROL, 5000);
         showPicture();
    }

    public void startShowPrevPicture()
    {
        int targetIndex = 0;
        int index = mMediaList.indexOf(mCurrentMediaFile);
        if(index == 0) {
            targetIndex = mMediaList.size() - 1;
        } else if(index > 0) {
            targetIndex = index -1;
        }
        mCurrentMediaFile=mMediaList.get(targetIndex);
        showControlBar();
        showPicture();
    }

    public void startShowNextPicture()
    {
        int targetIndex = 0;
        int index = mMediaList.indexOf(mCurrentMediaFile);
        if(index >= (mMediaList.size() - 1)) {
            targetIndex = 0;
        } else if(index >= 0) {
            targetIndex = index +1;
        }
        mCurrentMediaFile=mMediaList.get(targetIndex);
        showControlBar();
        showPicture();
    }


    public void showPicture()
    {      
    	mNameView.setText(mCurrentMediaFile);
    	ImageLoader.getInstance().displayImage("http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Photo/"+mCurrentMediaFile, mShowPicture,FolderBrowserActivity.options);
    }

    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // TODO Auto-generated method stub
        if(event.getAction() == MotionEvent.ACTION_DOWN) {
            if(mControlView.getVisibility() == View.VISIBLE) {
                mHandler.removeMessages(MSG_HIDE_CONTROL);
                mControlView.setVisibility(View.INVISIBLE);
            } else {
                showControlBar();
            }
        }
        return super.onTouchEvent(event);
    }

    
    private void showControlBar() {
        mHandler.removeMessages(MSG_HIDE_CONTROL);
        mControlView.setVisibility(View.VISIBLE);
        mHandler.sendEmptyMessageDelayed(MSG_HIDE_CONTROL, 5000);
    }


}
