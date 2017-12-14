package com.infotm.dv;

import com.danale.video.sdk.device.constant.RecordAction;
import com.danale.video.sdk.device.constant.RecordRate;
import com.danale.video.sdk.device.entity.Connection;
import com.danale.video.sdk.device.entity.LanDevice;
import com.danale.video.sdk.device.entity.Connection.LiveAudioReceiver;
import com.danale.video.sdk.device.entity.Connection.RawLiveVideoReceiver;
import com.danale.video.sdk.device.extend.DeviceExtendByteResultHandler;
import com.danale.video.sdk.device.extend.DeviceExtendDispatcher;
import com.danale.video.sdk.device.handler.DeviceResultHandler;
import com.danale.video.sdk.device.result.DeviceResult;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.danale.video.sdk.player.DanalePlayer;
import com.danale.video.sdk.player.DanalePlayer.OnPlayerStateChangeListener;
import com.danale.video.view.opengl.DanaleGlSurfaceView;
import com.infotm.dv.camera.InfotmCamera;

import android.app.Activity;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

public class PlaybackActivity extends Activity implements OnPlayerStateChangeListener, OnClickListener{
    
    private static final String TAG = "PlaybackActivity";
    
    private static final int SET_RATE = 125;
    private int mChannel;
    
    private int mDuration;
    private int mLapseTime;
    
    private AudioRecord mAudioRecord;
    private AudioTrack mAudioTrack;
    private Connection mConnection;
    private DanalePlayer mDanalePlayer;
    private LanDevice mDevice;
    
    private double mPreRate = 1.0D;
    private int mPreRateChoice = 1;
    private double mRate = 1.0D;
    private int mRateChoice = 1;
    private ArrayList<String> mRateList = new ArrayList();
    
    private Object mSyncObject = new Object();
    
    private SurfaceView mSurface;
    private FrameLayout mSurfaceFrame;
    private DanaleGlSurfaceView mGlSurfaceView;
    
    private View mControlView;
    private TextView mNameView;
    private ImageView mMediaBackView;
    private ImageView mPrevButton;
    private ImageView mNextButton;
    private ImageView mPlayPauseButton;
    private SeekBar mPlaybackSeekBar;
    private TextView mLapseView;
    private TextView mDurationView;
    private View mLoadingView;
    
    private String mCurrentMediaFile;
    private int mMediaType = InfotmCamera.MEDIA_INVALID;
    private ArrayList<String> mMediaList = null;
    
    private static final int STATE_ERROR              = -1; 
    private static final int STATE_IDLE               = 0;
    private static final int STATE_PREPARING          = 1;
    private static final int STATE_PREPARED           = 2;
    private static final int STATE_PLAYING            = 3;
    private static final int STATE_PAUSED             = 4;
    private static final int STATE_PLAYBACK_COMPLETED = 5;
    
    private static final int SERVER_STATE_WORKING = 0;
    private static final int SERVER_STATE_OK = 1;
    
    private static final int MSG_HIDE_CONTROL = 1026;
    private static final int MSG_PLAYBACK_COMPLETED = 1027;
    private static final int MSG_UPDATE_LAPSE_TIME = 1028;
    private static final int MSG_SEEK_CONTROL = 1029;
	
    private int mCurrentIndex = 0;
    private int mCurrentState = STATE_IDLE;
    private int mServerState = SERVER_STATE_OK;

    private Boolean bExit=false;
	private  long startTime=0;
	private  long seekbarChanged =0;
	private int mProgress =0;
    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub
            switch(msg.what) {
                case MSG_HIDE_CONTROL:
                    mControlView.setVisibility(View.INVISIBLE);
                    break;
                case MSG_PLAYBACK_COMPLETED:
                    startPlayNextMedia();
                    break;
                case MSG_UPDATE_LAPSE_TIME:
                    mLapseView.setText(String.format("%02d:%02d", mLapseTime / 60, mLapseTime % 60));
                    mHandler.sendEmptyMessageDelayed(MSG_UPDATE_LAPSE_TIME, 200);
                    break;
				  case MSG_SEEK_CONTROL:
			  		 Log.i(TAG,"startTime:"+startTime+" progress:"+mProgress);
					 long time=0;
					 time =mProgress/100;
					 sendSeekTime(time);
				  	 break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		bExit =false;
        setContentView(R.layout.activity_playback);
        
        mSurface = (SurfaceView) findViewById(R.id.player_surface);
        mSurfaceFrame = (FrameLayout) findViewById(R.id.player_surface_frame);
        mGlSurfaceView = ((DanaleGlSurfaceView)findViewById(R.id.player_gl_surface));
        
        mNameView = (TextView) findViewById(R.id.media_name);
        mControlView = findViewById(R.id.playback_info_control);
        mLoadingView =  findViewById(R.id.layout_live_video_loading);
        
        mNameView = (TextView) findViewById(R.id.media_name);
        mPrevButton = (ImageView) findViewById(R.id.btn_play_prev);
        mPrevButton.setOnClickListener(this);
        mNextButton = (ImageView) findViewById(R.id.btn_play_next);
        mNextButton.setOnClickListener(this);
        mPlayPauseButton = (ImageView) findViewById(R.id.btn_play_pause);
        mPlayPauseButton.setOnClickListener(this);
        mPlaybackSeekBar = (SeekBar) findViewById(R.id.seekbar_playback);
		 mPlaybackSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListenerImp());
		
        mMediaBackView = (ImageView) findViewById(R.id.media_back);
        mMediaBackView.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
            	finish();
			}
		});
        
        mLapseView = (TextView) findViewById(R.id.text_lapse);
        mDurationView = (TextView) findViewById(R.id.text_duration);
        
        mHandler.sendEmptyMessageDelayed(MSG_HIDE_CONTROL, 5000);
        init();

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                // TODO Auto-generated method stub
                mConnection = mDevice.getConnection();
                startMedia();
            }
        });
    }


	public byte[] getBytes(int s, boolean bBigEnding) {
			 byte[] buf = new byte[4];
			  
			 if (bBigEnding) {
				  for (int i = buf.length - 1; i >= 0; i--) {
					   buf[i] = (byte) (s & 0x000000ff);
					   s >>= 8;
				  }
			 } else {
				  System.out.println("1");
				  for (int i = 0; i < buf.length; i++) {
					   buf[i] = (byte) (s & 0x000000ff);
					   s >>= 8;
				  }
			 }
			  
			 return buf;
		}
	public static final byte[] longToByteArray_Little(long value) {
		     return new byte[] { (byte)(int)value, (byte)(int)(value >>> 8), (byte)(int)(value >>> 16), (byte)(int)(value >>> 24), (byte)(int)(value >>> 32), (byte)(int)(value >>> 40), 
		       (byte)(int)(value >>> 48), (byte)(int)(value >>> 56) };
	   }
		public byte[] getBytes(long s, boolean bBigEnding) {
			 byte[] buf = new byte[8];
			  
			 if (bBigEnding) {
				  for (int i = buf.length - 1; i >= 0; i--) {
					   buf[i] = (byte) (s & 0x00000000000000ff);
					   s >>= 8;
				  }
			 }
			 else {
				  for (int i = 0; i < buf.length; i++) {
					   buf[i] = (byte) (s & 0x00000000000000ff);
					   s >>= 8;
				  }
			 }
			  
			 return buf;
		}


    public void sendSeekTime(long time) {
		byte[] data = new byte[16];
		byte[] ret;
		ret=getBytes((long)1,false);
		System.arraycopy (ret,0,data,0,8);
		ret=getBytes(time,false);
		System.arraycopy (ret,0,data,8,8);
		DeviceExtendByteResultHandler handler = new DeviceExtendByteResultHandler() {

			@Override
			public void onSuccess(byte[] arg0) {
				DeviceExtendDispatcher.getInstance()
						.removeIndistinctDeviceExtendResultHandler(this);

			}
		};
		mConnection.transferByteDatas(getChannelID(), data, handler);
	}


    
	private class OnSeekBarChangeListenerImp implements
			 SeekBar.OnSeekBarChangeListener {

		 public void onProgressChanged(SeekBar seekBar, int progress,
				 boolean fromUser) {
		 }
	
		 public void onStartTrackingTouch(SeekBar seekBar) {
	        System.out.println("onStart-->"+seekBar.getProgress());
		 }

		 public void onStopTrackingTouch(SeekBar seekBar) {
			 // TODO Auto-generated method stub
			 System.out.println("onStop-->"+seekBar.getProgress());
			 seekbarChanged =1;
			 mProgress=seekBar.getProgress();
			 mLapseTime = mProgress/100;
			 mHandler.sendEmptyMessage(MSG_SEEK_CONTROL);
		 }
	 }
	 public String apiVersion="1.0.150815";
	 public int channelNum=0;
	 public String deviceId =App.DEVICE_UID;
	 public DeviceType deviceType=DeviceType.IPC;
	 public String ip="192.168.43.58";//"10.42.0.1";
	 public String netmask="255.255.255.255";
	 public int p2pPort=12349;
	 public String sn="danale_2015_di3";

    /**
     * Initialize the danale player and audio track.
     */
    private void init() {
        mAudioTrack = new AudioTrack(3, 48000, 12, 2, AudioTrack.getMinBufferSize(48000, 12, 2), 1);
        int bufferSizeInBytes = AudioRecord.getMinBufferSize(48000, 12, 2);
        //mAudioRecord = new AudioRecord(7, 48000, 12, 2, bufferSizeInBytes);
        
        mGlSurfaceView.setKeepScreenOn(true);
        mGlSurfaceView.init();
        mDanalePlayer = new DanalePlayer(this,mSurface, mGlSurfaceView);
        mDanalePlayer.setOnPlayerStateChangeListener(this);
        
        String deviceIp = getIntent().getStringExtra(InfotmCamera.DEVICE_IP);
        mDevice = ((App) getApplication()).getLanDeviceByIp(deviceIp);
        if(mDevice == null) {
            Log.e(TAG, "get device failed:"+deviceIp);
            mDevice= new LanDevice();
			mDevice.setAccessAuth("zhutou","wobushi");
			mDevice.setApiVersion(apiVersion);
			mDevice.setChannelNum(channelNum);
			mDevice.setDeviceId(deviceId);
			mDevice.setDeviceType(deviceType);
			mDevice.setIp(deviceIp);
			mDevice.setNetmask(netmask);
			mDevice.setP2pPort(p2pPort);
			mDevice.setSn(sn);
        }
        
        String mediaFile = getIntent().getStringExtra(InfotmCamera.MEDIA_FILE_NAME);
        if(mediaFile == null || mediaFile.length() < 25) {
            Log.e(TAG, "mCurrentMediaFile: " + mediaFile);
            finish();
            return;
        }
        onMediaFileChanged(mediaFile);
        
        mMediaType = getIntent().getIntExtra(InfotmCamera.MEDIA_TYPE, InfotmCamera.MEDIA_INVALID);
        switch (mMediaType) {
            case InfotmCamera.MEDIA_VIDEO:
                mMediaList = InfotmCamera.mVideoList;
                break;
            case InfotmCamera.MEDIA_VIDEO_LOCKED:
                mMediaList = InfotmCamera.mLockedVideoList;
                break;
            case InfotmCamera.MEDIA_PHOTO:
                mMediaList = InfotmCamera.mPhotoList;
            default:
                break;
        }
        if(mMediaList == null || mMediaList.indexOf(mCurrentMediaFile) < 0) {
            Log.e(TAG, "media list not found");
            finish();
            return;
        }
        
        if(mMediaType == InfotmCamera.MEDIA_PHOTO) {
            mPlaybackSeekBar.setVisibility(View.INVISIBLE);
            mPlayPauseButton.setVisibility(View.INVISIBLE);
            mLapseView.setVisibility(View.INVISIBLE);
            mDurationView.setVisibility(View.INVISIBLE);
        }
        
        mNameView.setText(mCurrentMediaFile);
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

    @Override
    public void onClick(final View v) {
        // TODO Auto-generated method stub
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                // TODO Auto-generated method stub
                switch(v.getId()) {
                    case R.id.btn_play_prev:
                    	 if(mCurrentState >= STATE_PREPARED) 
                        startPlayPrevMedia();
                        break;
                    case R.id.btn_play_pause:
                        if(mCurrentState == STATE_PLAYING) {
                            pauseMedia();
                        } else if(mCurrentState == STATE_IDLE) {
                            startMedia();
                        } else {
                            resumeMedia();
                        }
                        break;
                    case R.id.btn_play_next:
                    	if(mCurrentState >= STATE_PREPARED) 
                        startPlayNextMedia();
                        break;
                }
            }
        }, 100);
    }
    
    private void showControlBar() {
        mHandler.removeMessages(MSG_HIDE_CONTROL);
        mControlView.setVisibility(View.VISIBLE);
        mHandler.sendEmptyMessageDelayed(MSG_HIDE_CONTROL, 5000);
    }
    
/*    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        // TODO Auto-generated method stub
        if(ev.getAction() == MotionEvent.ACTION_DOWN) {
            Log.d(TAG, "dispatchTouchEvent");
            showControlBar();
        }
        return super.dispatchTouchEvent(ev);
    }*/

    private int duration() {
        int duration = 0;
        if(mCurrentMediaFile == null || (!mCurrentMediaFile.contains(".mp4")&&!mCurrentMediaFile.contains(".mkv"))) {
            duration = 0;
            return duration;
        }
        
        String tmpS = mCurrentMediaFile.replace("LOCK_", "");
        if(tmpS.length() < 25) {
            return 0;
        }
        //2016_0227_120834_0060.mp4
        duration = Integer.parseInt(tmpS.substring(17, 21));
        Log.d(TAG, "filename: " + mCurrentMediaFile + ", duration: " + duration);
        
        return duration;
    }
    
    private int getChannelID() {
        int channelId = InfotmCamera.CHANNEL_FRONT;
        if(mCurrentMediaFile.contains(".jpg")) {
            channelId = InfotmCamera.CHANNEL_PHOTO;
        } else if(mCurrentMediaFile.contains("LOCK")) {
          	if(mCurrentMediaFile.contains("_R")) 
		  	{
            	channelId = InfotmCamera.CHANNEL_LOCK_REAR;
       		}else
       		{
            	channelId = InfotmCamera.CHANNEL_LOCK;
			}
        } else if(mCurrentMediaFile.contains("_R")) {
            channelId = InfotmCamera.CHANNEL_REAR;
        }
        Log.i(TAG, "channelId--->:"+channelId);
        return channelId;
    }
    
    /**
     * Parse the current media file
     * @return
     * The current media file's start time
     */
    private long getStartTime() {
        long time = 0;
        String timeS = mCurrentMediaFile.replace("LOCK_", "");
        SimpleDateFormat localSimpleDateFormat = new SimpleDateFormat("yyyy_MMdd_HHmmss");
        try {
            Date d = localSimpleDateFormat.parse(timeS.substring(0, 16));
            time = d.getTime();// / 1000;
            Log.d(TAG, "time: " + time + ", string: " + timeS.substring(0, 16));
        } catch (ParseException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return time;
    }
    
/*    private void playVideoByName(String fileName) {
        
    }*/
    private void onMediaFileChanged(String mediaFile) {
        mCurrentMediaFile = new String(mediaFile);
        mChannel = getChannelID();
        mDuration = duration();
        mLapseTime = 0;
        
        mPlaybackSeekBar.setProgress(0);
        mPlaybackSeekBar.setMax(mDuration * 100);
        mNameView.setText(mCurrentMediaFile);
        mLapseView.setText("00:00");
        mDurationView.setText(String.format("%02d:%02d", mDuration / 60, mDuration % 60));
    }
    
    private DeviceResultHandler mSetRateHandler = new DeviceResultHandler() {
        @Override
        public void onSuccess(DeviceResult arg0) {
            // TODO Auto-generated method stub
        }
        
        @Override
        public void onFailure(DeviceResult arg0, int arg1) {
            // TODO Auto-generated method stub
        }
    };
    
    
    private void setRate(int paramInt)
    {
        Log.i(TAG, "setRate");
        this.mPreRateChoice = this.mRateChoice;
        this.mPreRate = this.mRate;
//        mChannel = getChannelID();
        switch (paramInt)
        {
            case 0:
                this.mRateChoice = 0;
                this.mRate = 2.0D;
                this.mConnection.recordSetRate(SET_RATE, this.mChannel, RecordRate._0_5X,  mSetRateHandler);
                // this.mTimeLineView.setPlayRate(this.mRate);
                return;
            case 1:
                this.mRateChoice = 1;
                this.mRate = 1.0D;
                this.mConnection.recordSetRate(SET_RATE, this.mChannel, RecordRate._1_0X, mSetRateHandler);
                // this.mTimeLineView.setPlayRate(this.mRate);
                return;
            case 2:
                this.mRateChoice = 2;
                this.mRate = 0.5D;
                this.mConnection.recordSetRate(SET_RATE, this.mChannel, RecordRate._2_0X, mSetRateHandler);
                // this.mTimeLineView.setPlayRate(this.mRate);
                return;
            case 3:
                this.mRateChoice = 3;
                this.mRate = 0.25D;
                this.mConnection.recordSetRate(SET_RATE, this.mChannel, RecordRate._4_0X, mSetRateHandler);
                // this.mTimeLineView.setPlayRate(this.mRate);
                break;
            default:
                return;
        }
    }

    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        resumeMedia();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
        pauseMedia();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
		mHandler.removeCallbacksAndMessages(null);
		bExit =true;
        mSurface.setKeepScreenOn(false);
        mGlSurfaceView.setKeepScreenOn(false);
                // TODO Auto-generated method stub
        stopMedia();
		
//        mStateAudio = 0;
        if (mAudioTrack != null) {
            mAudioTrack.flush();
            mAudioTrack.release();
            mAudioTrack=null;
        }
    }

    /**
     * Implement OnPlayerStateChangeListener method
     */
    @Override
    public void onVideoPlaying(int channel) {
        // TODO Auto-generated method stub
        Log.d(TAG, "onVideoPlaying, channel: " + channel);
        mCurrentState = STATE_PLAYING;
    }

    /**
     * Implement OnPlayerStateChangeListener method
     */
    @Override
    public void onVideoSizeChange(int channel, int videoWidth, int videoHeight) {
        // TODO Auto-generated method stub
        Log.d(TAG, "onVideoSizeChange, channel: " + channel + ", (" + videoWidth + ", " + videoHeight + ")");
    }

    /**
     * Implement OnPlayerStateChangeListener method
     */
    @Override
    public void onVideoTimout() {
        // TODO Auto-generated method stub
        Log.d(TAG, "onVideoTimout");
        this.mDanalePlayer.stop(false);
        if(mCurrentState >= STATE_PREPARING) {
            stopMedia();
            mCurrentState = STATE_IDLE;
         }
    }
    
    /**
     * Wait the server state to be ok
     */
    private void waitUtilServerOK() {
        if(mServerState == SERVER_STATE_OK)
            return;
        
        int i = 0;
        for(i = 0; i < 40; i ++) {
            try {
                if(mServerState == SERVER_STATE_OK)
                    break;
                Thread.sleep(50);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        if(i >= 100)//wait no more than 5s
            mServerState = SERVER_STATE_OK;
    }
    
    public void pauseMedia()
    {
        Log.i(TAG, "pauseVideo");
        if(mCurrentState != STATE_PLAYING)
            return;
        mHandler.removeMessages(MSG_UPDATE_LAPSE_TIME);
        mDanalePlayer.pause();
        mAudioTrack.pause();
        mPlayPauseButton.setImageLevel(0);
        mCurrentState = STATE_PAUSED;
        mServerState = SERVER_STATE_WORKING;
        this.mConnection.recordAction(1, getChannelID(), RecordAction.PAUSE,
                new DeviceResultHandler() {
                    public void onFailure(DeviceResult paramDeviceResult, int paramInt)
                    {
                        mServerState = SERVER_STATE_OK;
                    }

                    public void onSuccess(DeviceResult paramDeviceResult)
                    {
                        Log.d(TAG, "pauseVideo onSuccess");
                        mServerState = SERVER_STATE_OK;
                    }
                });
        return;
    }

    public void resumeMedia()
    {
        Log.i(TAG, "resumeMedia");
        if(mCurrentState != STATE_PAUSED)
            return;
        mHandler.removeMessages(MSG_UPDATE_LAPSE_TIME);
        mHandler.sendEmptyMessage(MSG_UPDATE_LAPSE_TIME);
        mDanalePlayer.resume();
        mAudioTrack.play();
        mPlayPauseButton.setImageLevel(1);
        mServerState = SERVER_STATE_WORKING;
        mCurrentState = STATE_PLAYING;
        this.mConnection.recordAction(1, getChannelID(), RecordAction.PLAY,
                new DeviceResultHandler() {
                    public void onFailure(DeviceResult paramDeviceResult, int paramInt)
                    {
                        Log.d(TAG, "resumeMedia onFailure");
                        mServerState = SERVER_STATE_OK;
                    }

                    public void onSuccess(DeviceResult paramDeviceResult)
                    {
                        Log.d(TAG, "resumeMedia onSuccess");
                        mServerState = SERVER_STATE_OK;
                        //mDanalePlayer.resume();
                    }
                });
    }

    public void startPlayPrevMedia()
    {
        if(bExit==true)
		{
		   return;
		}
        if (mCurrentState >= STATE_PREPARING) {
            stopMedia();
        }
        int targetIndex = 0;
        int index = mMediaList.indexOf(mCurrentMediaFile);
        if(index == 0) {
            targetIndex = mMediaList.size() - 1;
        } else if(index > 0) {
            targetIndex = index -1;
        }
        waitUtilServerOK();
        showControlBar();
        onMediaFileChanged(mMediaList.get(targetIndex));
        startMedia();
    }

    public void startPlayNextMedia()
    {
		 if(bExit==true)
		{
		   return;
		}
        if (mCurrentState >= STATE_PREPARING) {
            stopMedia();
        }
        
        int targetIndex = 0;
        int index = mMediaList.indexOf(mCurrentMediaFile);
        if(index >= (mMediaList.size() - 1)) {
            targetIndex = 0;
        } else if(index >= 0) {
            targetIndex = index +1;
        }
        waitUtilServerOK();
        showControlBar();
        onMediaFileChanged(mMediaList.get(targetIndex));
        startMedia();
    }

    RawLiveVideoReceiver mLiveVideoReceiver = new RawLiveVideoReceiver() {
        @Override
        public void onReceive(int channel, int format, long timeStamp, boolean isKeyFrame,
                byte[] data) {
            // TODO Auto-generated method stub
            Log.d(TAG, "channel: " + channel+"--format: " + format+"--timestamp: " + timeStamp+"--isKeyFrame: " + isKeyFrame);
            mDanalePlayer.onReceive(channel, format, timeStamp, isKeyFrame, data);
            
            if(mMediaType == InfotmCamera.MEDIA_PHOTO)
                return;
            if(mDuration > 0) {
                int progress = (int)(timeStamp / 10);//10ms
                if(seekbarChanged==1)
				  {
				     if((progress>(mPlaybackSeekBar.getProgress()-200))&&(progress<(mPlaybackSeekBar.getProgress()+200)))
					  {
					     seekbarChanged=0;
					  }
				  }
				  if(seekbarChanged==0)
                  mPlaybackSeekBar.setProgress(progress);
            }
			 if(seekbarChanged==0)
            mLapseTime = (int) (timeStamp / 1000);
          //check the playback end, we use a delayed msg to implement it
            if(mLapseTime  > mDuration - 2) {
                //Log.d(TAG, "playback end. timeStamp: " + timeStamp + ", duration: " + mDuration);
                //startPlayNextMedia();
                mHandler.removeMessages(MSG_PLAYBACK_COMPLETED);
                mHandler.sendEmptyMessageDelayed(MSG_PLAYBACK_COMPLETED, 800);
            }
            //mLapseView.setText(String.format("%02d:%02d", timeStamp / 1000 / 60, timeStamp / 1000 % 60));
        }
    };

    public void startMedia()
    {
        if(mCurrentState >= STATE_PREPARING)
            stopMedia();
        mPlayPauseButton.setImageLevel(1);
        mHandler.removeMessages(MSG_UPDATE_LAPSE_TIME);
        mHandler.sendEmptyMessage(MSG_UPDATE_LAPSE_TIME);
        LiveAudioReceiver  audioReceiver = new Connection.LiveAudioReceiver() {
            @Override
            public void onReceiveAudio(byte[] data)
            {
                Log.d(TAG, "onReceiveAudio");
                if ((mAudioTrack == null) || (data == null) || mMediaType == InfotmCamera.MEDIA_PHOTO)
                    return;
                mAudioTrack.write(data, 0, data.length);
            }
        };
        mLoadingView.setVisibility(View.VISIBLE);
        mCurrentState = STATE_PREPARING;
        mServerState = SERVER_STATE_WORKING;
		if(bExit==true)
		{
		   return;
		}
        mAudioTrack.play();
        startTime =getStartTime();
        boolean bret = mConnection.recordPlayByVideoRaw(0, getChannelID(), startTime, mLiveVideoReceiver, audioReceiver,
                new DeviceResultHandler() {
                    @Override
                    public void onSuccess(DeviceResult arg0) {
                        Log.e(TAG, "startMedia onSuccess");
                        mDanalePlayer.preStart(false, DeviceType.IPC);
                       // mDanalePlayer.showDecodeTypeToast(true);
                        mDanalePlayer.useBuffer(false);
                        //mDanalePlayer.setBufferSize(10) ;
                        mDanalePlayer.setTotalCacheQueueSize(50);
                        mCurrentState = STATE_PLAYING;
                        mServerState = SERVER_STATE_OK;
                        mLoadingView.setVisibility(View.GONE);
                    }

                    @Override
                    public void onFailure(DeviceResult arg0, int arg1) {
                        Log.e(TAG, "startMedia onFailure");
                        mDanalePlayer.stop(false);
                       // mDanalePlayer.drawBlack();
                        mCurrentState = STATE_IDLE;
                        mServerState = SERVER_STATE_OK;
                    }
                });
        Log.i(TAG, "recordPlayByVideoRaw:" + bret + " index:" + mCurrentIndex);
    }

    public void stopMedia()
    {
        mPlayPauseButton.setImageLevel(0);
        Log.i(TAG, "stopPlayMedia");
        if(mCurrentState < STATE_PREPARING)
            return;
        mHandler.removeMessages(MSG_UPDATE_LAPSE_TIME);
        mCurrentState = STATE_IDLE;
        mDanalePlayer.stop(false);
        if(mMediaType != InfotmCamera.MEDIA_PHOTO)
            //mDanalePlayer.drawBlack();
        mAudioTrack.stop();
        mServerState = SERVER_STATE_WORKING;
        mConnection.recordStop(0, getChannelID(), new DeviceResultHandler() {
            @Override
            public void onSuccess(DeviceResult arg0) {
                Log.e(TAG, "stopMedia onSuccess");
                mServerState = SERVER_STATE_OK;
            }

            @Override
            public void onFailure(DeviceResult arg0, int arg1) {
                Log.e(TAG, "stopMedia onFailure");
                mServerState = SERVER_STATE_OK;
            }
        });
    }

}
