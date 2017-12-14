package com.infotm.dv;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences.Editor;
import android.content.res.Configuration;
import android.graphics.drawable.AnimationDrawable;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.net.TrafficStats;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.widget.VideoView;

import com.danale.video.jni.DanaDevSession.AudioInfo;
import com.danale.video.sdk.device.entity.Connection;
import com.danale.video.sdk.device.entity.Connection.OnConnectionErrorListener;
import com.danale.video.sdk.device.entity.LanDevice;
import com.danale.video.sdk.device.handler.DeviceResultHandler;
import com.danale.video.sdk.device.result.DeviceResult;
import com.danale.video.sdk.device.result.StartLiveVideoResult;
import com.danale.video.sdk.platform.constant.CollectionType;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.danale.video.sdk.player.DanalePlayer;
import com.danale.video.view.opengl.DanaleGlSurfaceView;
import com.infotm.dv.adapter.InfotmSetting;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.CommandSender;
import com.infotm.dv.connect.CommandService;
import com.infotm.dv.connect.NetService;
import com.infotm.dv.connect.streamRecv;
import com.infotm.dv.view.CameraModePopup;
import com.infotm.dv.view.SmartyProgressBar;
import com.infotm.dv.view.SwitchButton;
import com.infotm.dv.wifiadapter.WifiApMan;
//import android.app.ActionBar;
//import com.actionbarsherlock.app.SherlockActivity;
//import com.actionbarsherlock.view.Menu;
//import com.actionbarsherlock.view.MenuItem;

public class MainActivity extends BaseActivity implements Connection.LiveAudioReceiver, DanalePlayer.OnPlayerStateChangeListener, Callback{
    private static final String tag = "MainActivity";
    String s4 = "rtsp://192.168.0.113/sample_h264_300kbit.mp4";
    VideoView mVideoView = null;
    ImageView mWifiView;
    TextView mFrameFpsView;
    ImageView mAdvancedView;
    ImageView mLowLightView;
    ImageView mBatteryView;
    TextView mCapacityView;
    String videos = "";
    int videoTimeRemainSeconds = 0;
    int recordedTime = 0;//now single video time
    private ToggleButton mBtnShutter;
    //private LinearLayout img_shutter_contain;
    
    //private CheckableImageButton mBtnControls;
    private CameraModePopup mPopupControls;
    MediaPlayer mMediaPlayer;
    ImageButton mBtnSettings;
    ImageButton mBtnMore;
	ImageButton mBtnBurst;
	ImageButton mBtnBack;
	private ImageButton btn_sd_record;
	private ImageButton btn_live_video_audio;
	private ImageButton btn_short_record;
	TextView mBtnQuality;
    SmartyProgressBar mSmartyProgressBar;
    WifiManager mWifiManager;

    private App app;
	private Context context;
	private ProgressBar progressBar;
	//private TextView reSync;
	
	private Connection connection;
	private LanDevice device;
	private DanalePlayer danalePlayer;
	private DanaleGlSurfaceView glSurfaceView;
	private SurfaceView surfaceView;
	private boolean isVideoOpened;
	private boolean isProgressBar=false;
	private boolean needConnState =false;
	private boolean isAudioOpened;
	private boolean isTalkOpened;
	private int mStateAudio=0;
	int isForward=1;
	int rePreviewFailCount=0;
	
	private AudioTrack audioTrack;
	private AudioRecord audioRecord;
	private boolean isPlatDevice;
	private CollectionType collectionType;
	private int channel = 1;
	private int videoQuality = 40;
    private int qualityType=0;
    private SoundPool soundPool;
    private HashMap<Integer,Integer> soundPoolMap;
    private final static int MSG_SEND_OK = 0;
    private final static int MSG_START = 1;
    private final static int MSG_STOP = 2;
    private final static int MSG_COUNTING = 3;
	private final static int MSG_SHORT_RECORD = 4;
	private final static int MSG_RESTART_PREVIEW = 5;
	private final static int MSG_STOP_PREVIEW = 6;
	private final static int MSG_CONN_CHECK = 7;
	private final static int MSG_NETWORK_DISCONNECT = 8;
    private Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            Log.v(tag, "------handleMessage:"+ msg.what);
            switch (msg.what) {
                case MSG_START:
    		            progressBar.setVisibility(View.GONE);
    					isProgressBar=false;
                    break;
                case MSG_STOP:

                    break;
                case MSG_COUNTING:
                    recordedTime ++;
                    mCapacityView.setText(videos + " / " + refreshTimeCounterString());
                    mHandler.sendEmptyMessageDelayed(MSG_COUNTING, 1000);
                    break;
                case MSG_SHORT_RECORD:
					 btn_short_record.setImageResource(R.drawable.live_video_control_record);
   		 			 btn_short_record.setEnabled(true);
					break;
					
                case MSG_RESTART_PREVIEW:
                	Log.i(tag, "reloadMediaPreview---------------->MSG_RESTART_PREVIEW");
                	//int isVisibility=progressBar.getVisibility();
                	//if(isVisibility==View.VISIBLE){
    		        if (!mCoReceOn){
    		            registerReceiver(mCommandBroadcastReceiver, new IntentFilter(CommandService.ACTION_DATACHANGE));
    		            mCoReceOn = true;
    		        }
    				Log.e(tag,"main onResume");
    				startmedia();
                	//}

                	break;
                	
                case MSG_STOP_PREVIEW:
        			Log.i(tag, "reloadMediaPreview---------------->MSG_STOP_PREVIEW");
    		        if (null != mCommandBroadcastReceiver && mCoReceOn){
    		            unregisterReceiver(mCommandBroadcastReceiver);
    		            mCoReceOn = false;
    		        }
    				stopmedia();
    				if(mStateAudio!=0)
    				{
    				  onStopLiveAudio();
    				}
    		        if (audioTrack != null)
    				{
    					audioTrack.stop();
    		            audioTrack.release();
    		            audioTrack=null;
    				}
    				Message msg1=mHandler.obtainMessage(); 
    				msg1.what=MSG_RESTART_PREVIEW;
    				mHandler.sendMessageDelayed(msg1, 600);
                	break;
				  case MSG_CONN_CHECK:
				  	  Log.i(tag, "MSG_CONN_CHECK needConnState:"+needConnState);
					  if(needConnState==false)
				  	  {
				  	     stopmedia();
					  }
					  break;
					  
				  case MSG_NETWORK_DISCONNECT:
					       showDeviceDisconnectDialog();
					  break;
                default:
                    break;
            }
        };
    };
    BroadcastReceiver mCommandBroadcastReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context context, Intent intent) {
            String ss = intent.getStringExtra("data");
            Log.v(tag, "------datachanged:" + ss);
            refreshUI();
        }
    };
    private boolean mCoReceOn = false;
    
    BroadcastReceiver mWifiStrenthListen;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setBarByOrientation();
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);
        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        isForward=mPref.getInt("connect_mode", 1);
        startService(new Intent(getApplicationContext(),CommandService.class));
//        registerReceiver(mCommandBroadcastReceiver, new IntentFilter(CommandService.ACTION_DATACHANGE));
//        mCoReceOn = true;
        
        registerWifiStrenthListen();
        
        findViews();
		initData();
		
		
		RelativeLayout mrlay;
		mrlay = (RelativeLayout) findViewById(R.id.preview_back1);
        if(mrlay!=null)
        {
			RelativeLayout.LayoutParams pp =(RelativeLayout.LayoutParams) mrlay.getLayoutParams();
			Display display = getWindowManager().getDefaultDisplay(); 
			Log.i("MainActivity", "height:" + display.getWidth()); 
			pp.height = (display.getWidth() * 9 )/ 16;
			mrlay.setLayoutParams(pp);
        }
        
            View v = (View) findViewById(R.id.preview_back);
            if (null != v){
                v.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {/*
                        Log.v(tag, "--------onclick----");
                        View headerView = (View) findViewById(R.id.info_bar);
                        View footerView = (View) findViewById(R.id.footer);
                        if (headerView.getVisibility() == View.VISIBLE){
                            headerView.setVisibility(View.GONE);
                            footerView.setVisibility(View.GONE);
                            btn_short_record.setVisibility(View.GONE);
                        } else {
                            headerView.setVisibility(View.VISIBLE);
                            footerView.setVisibility(View.VISIBLE);
                            btn_short_record.setVisibility(View.VISIBLE);
                        }
                    */}
                });
            }
//        playRtspStream(s4);
        mSmartyProgressBar = new SmartyProgressBar(this);
        initUiVars();
        mCamera.setSetHelper(this);
        
        Log.v(tag, "----onCreate----mNetStatus:"+mCamera.mNetStatus);
        if (mCamera.mNetStatus == InfotmCamera.STAT_YES_NET_YES_PIN){
//            startInitConfig(mHandler,mSmartyProgressBar);//在BaseActivty的onresume会触发onNetworkStateUpdate
        }
        
        soundPool = new SoundPool(3, AudioManager.STREAM_MUSIC, 20);
        soundPoolMap = new HashMap<Integer, Integer>();   
        soundPoolMap.put(1, soundPool.load(this, R.raw.camera4, 1));
        soundPoolMap.put(2, soundPool.load(this, R.raw.key, 2));
    }
    
    
	private void findViews(){
	   
		glSurfaceView = (DanaleGlSurfaceView) findViewById(R.id.gl_sv);
		surfaceView = (SurfaceView) findViewById(R.id.sv);
		progressBar = (ProgressBar) findViewById(R.id.pb);
/*		reSync = (TextView) findViewById(R.id.re_sync);
		reSync.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				Message msg=mHandler.obtainMessage(); 
				msg.what=MSG_STOP_PREVIEW;
				mHandler.sendMessage(msg);
			}
		});*/
		

	}

	 public String apiVersion="1.0.150815";
	 public int channelNum=0;
	 public String deviceId =App.DEVICE_UID;
	 public DeviceType deviceType=DeviceType.IPC;
	 public String ip="192.168.43.58";//"10.42.0.1";
	 public String netmask="255.255.255.255";
	 public int p2pPort=12349;
	 public String sn="danale_2015_di3";
	
	private void initData(){
		context = this;
		app = (App) getApplication();
		
		Intent intent = getIntent();
		isPlatDevice = intent.getBooleanExtra("isPlatformDevice", true);
		if (isPlatDevice){
			device = app.getPlatformDeviceById(intent.getStringExtra("deviceId"));
			connection = device.getConnection();
			collectionType = app.getPlatformDeviceById(intent.getStringExtra("deviceId")).getCollectionType();
           InfotmCamera.currentIP=device.getIp();
		}else{
			device = app.getLanDeviceByIp(intent.getStringExtra("deviceIp"));
			InfotmCamera.currentIP=intent.getStringExtra("deviceIp");
			Log.i(tag,"initData:"+intent.getStringExtra("deviceIp"));
			if(device!=null)
		    {
				connection = device.getConnection();
				connection.setLocalAccessAuth("admin", "admin");
				
		    }else
		    {
				device= new LanDevice();
				device.setAccessAuth("zhutou","wobushi");
				device.setApiVersion(apiVersion);
				device.setChannelNum(channelNum);
				device.setDeviceId(deviceId);
				device.setDeviceType(deviceType);
				device.setIp(intent.getStringExtra("deviceIp"));
				device.setNetmask(netmask);
				device.setP2pPort(p2pPort);
				device.setSn(sn);
				connection = device.getConnection();
			}
		}

		Log.i(tag,"1initData"+device.getIp()+" isPlatDevice:"+isPlatDevice);
		InfotmCamera.apIP = device.getIp();
		audioTrack = new AudioTrack(3, 48000, 12, 2, AudioTrack.getMinBufferSize(48000, 12, 2), 1);
		int bufferSizeInBytes = AudioRecord.getMinBufferSize(48000, 12, 2);
		audioRecord = new AudioRecord(7, 48000, 12, 2, bufferSizeInBytes);
		Log.i(tag,"before glsurfaceview.init");
		glSurfaceView.init();
		Log.i(tag,"after glsurfaceview.init");
		//danalePlayer = new DanalePlayer(this,surfaceView,glSurfaceView);
		//danalePlayer.setOnPlayerStateChangeListener(this);
		//danalePlayer.preStart(false, DeviceType.IPC);
		
		isVideoOpened = false;
		//璁剧疆杩炴帴涓柇鐨勫洖璋?
		connection.setOnConnectionErrorListener(new OnConnectionErrorListener() {
			
			@Override
			public void onConnectionError() {
				//寤鸿鍦ㄦ澶勮繘琛宻top瑙嗛鎿嶄綔鍚?缁х画start瑙嗛
			}
		});
		//recordScreen();
	}

	 public FileOutputStream fout;
		private Connection.RawLiveVideoReceiver rawVideoRecv=new Connection.RawLiveVideoReceiver() {
			
			@Override
			public void onReceive(int channel, int format, long timeStamp, boolean isKeyFrame,
					byte[] data) {
				//if(isKeyFrame)	
				Log.i("ac", "startmedia onReceive:"+data.length);	
			
				//danalePlayer.handleData(channel, format, timeStamp, isKeyFrame, data);
				onFrame(data,0,data.length,format,timeStamp);
				/*
				 try{ 
	                 if(fout==null)
					  fout =new FileOutputStream(Environment.getExternalStorageDirectory().getAbsolutePath()+"/video360.h264");
				        fout.write(data); 
				         //fout.close(); 
				        } 

				       catch(Exception e){ 

				        e.printStackTrace(); 

				       } */
			}
		};
		/*
		private void startmedia(){
			
			Log.e(tag,"startmedia:"+isVideoOpened);
				if(isVideoOpened == false){
					videoDecode(surfaceView.getHolder());
				progressBar.setVisibility(View.VISIBLE);
				isProgressBar=true;
				//reSync.setVisibility(View.VISIBLE);
				//å¼€å¯è§†é¢‘æ’­
				connection.startLiveVideo(0, channel, videoQuality, 
						rawVideoRecv
						//danalePlayer
					,new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult arg0) {
					Log.e(tag,"startmedia onSuccess");
						isVideoOpened = true;
					//	danalePlayer.preStart(false, DeviceType.IPC);
					//	danalePlayer.setVideoInfo(((StartLiveVideoResult)arg0).getVideoInfo());
						//danalePlayer.showDecodeTypeToast(true);
					//	danalePlayer.useBuffer(true);
					//	danalePlayer.setBufferSize(2) ;
					//	danalePlayer.setTotalCacheQueueSize(6);
						rePreviewFailCount=0;
					}
					
					@Override
					public void onFailure(DeviceResult arg0, int arg1) {
					Log.e(tag,"startmedia onFailure rePreviewFailCount:"+rePreviewFailCount);
						progressBar.setVisibility(View.GONE);
						isProgressBar=false;
						//reSync.setVisibility(View.VISIBLE);
						isVideoOpened = false;
						//danalePlayer.stop(false);
						//danalePlayer.drawBlack();
						rePreviewFailCount++;
						if(rePreviewFailCount>=3){
							rePreviewFailCount=0;
							mHandler.sendEmptyMessageDelayed(MSG_NETWORK_DISCONNECT, 200);
						}
					}
				});
				mRecordScreen=true;
			    //recordScreen();
				}
				
			}
		
		private void stopmedia(){
			
			Log.e(tag,"stopmedia:"+isVideoOpened);
			if(isVideoOpened == true){
			    connection.stopLiveVideo(0, channel,rawVideoRecv
			    		//danalePlayer
			    		, new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult arg0) {
						isVideoOpened = false;
						//danalePlayer.stop(false);
						//danalePlayer.drawBlack();
	                    
						Log.e(tag,"stopmedia onSuccess");
					}
					
					@Override
					public void onFailure(DeviceResult arg0, int arg1) {
						isVideoOpened = true;
						Log.e(tag,"stopmedia onFailure");
					}
				});
			 }
			mRecordScreen=false;
			mShortRecordInfoList.clear();
			
		}*/
	
	private streamRecv.StreamReceiver strReceiver = new streamRecv.StreamReceiver()
	{
		public void onStreamReceive(int channel, int format, long timeStamp, boolean isKeyFrame,byte[] data,int nRet) 
		{
			//danalePlayer.handleData(channel, format, timeStamp, isKeyFrame, data);
			if(nRet>0)
			{
			   onFrame(data,0,data.length,format,timeStamp);
			}
			else
			{
			   Log.e(tag,"StreamReceiver close:"+nRet);
			}
		}
	};
	private streamRecv  streamRecvC=null;
	private boolean isStartDecode =false;
	private void startmedia()
	{
		
		Log.e(tag,"startmedia:"+isVideoOpened);
			if(isVideoOpened == false)
			{
				videoDecode(surfaceView.getHolder());
			    progressBar.setVisibility(View.VISIBLE);
			    isProgressBar=true;
			    mRecordScreen=true;
			    isVideoOpened = true;
			    rePreviewFailCount=0;
			    if(streamRecvC==null)
			    {
			    	streamRecvC=new streamRecv();
			    	streamRecvC.startrecv(InfotmCamera.apIP, strReceiver);
			    }
			}
			
	}
	
	private void stopmedia()
	{
		
		Log.e(tag,"stopmedia:"+isVideoOpened);
		if(isVideoOpened == true)
		{
			isVideoOpened = false;
			if(streamRecvC!=null)
			{
				streamRecvC.stoprecv();
				streamRecvC=null;
			}
			Log.e(tag,"stopmedia onSuccess");
		}
		mRecordScreen=false;
		mShortRecordInfoList.clear();
	}
	
  private void onClickAudio()
  {
    if (mStateAudio!=0)
    {
      audioTrack.stop();
      onStopLiveAudio();
    }else
    {
      if(audioTrack==null)
	  {
	    audioTrack = new AudioTrack(3, 48000, 12, 2, AudioTrack.getMinBufferSize(48000, 12, 2), 1);
	  }
      onStartLiveAudio();
    }
  }

  private void onStartLiveAudio()
  {
    mStateAudio = 1;
    btn_live_video_audio.setImageResource(R.drawable.live_video_control_audio_select);
    btn_live_video_audio.setEnabled(true);
    final AudioInfo info = new AudioInfo();
    connection.startLiveAudio(0, this.channel, info,this, new DeviceResultHandler()
    {
      public void onFailure(DeviceResult paramDeviceResult, int paramInt)
      {
        btn_live_video_audio.setImageResource(R.drawable.live_video_control_audio);
		 btn_live_video_audio.setEnabled(true);
        mStateAudio = 0;
      }

      public void onSuccess(DeviceResult paramDeviceResult)
      {
          mHandler.postDelayed(new Runnable() {// audio may not write yet
            @Override
            public void run() {
                // TODO Auto-generated method stub
                try {
                    audioTrack.play();
                } catch (IllegalStateException e) {
                    // TODO: handle exception
                }
            }
        }, 1000);

        mStateAudio = 2;
      }
    });
  }

  private void onStopLiveAudio()
  {
    mStateAudio = 0;
    btn_live_video_audio.setImageResource(R.drawable.live_video_control_audio);
    btn_live_video_audio.setEnabled(true);
    connection.stopLiveAudio(0, this.channel, new DeviceResultHandler()
    {
      public void onFailure(DeviceResult paramDeviceResult, int paramInt)
      {
      }

      public void onSuccess(DeviceResult paramDeviceResult)
      {
      }
    });
  }
	  
     public void onReceiveAudio(byte[] paramArrayOfByte)
  {
    if ((mStateAudio != 2) || (audioTrack == null) || (paramArrayOfByte == null))
      return;
    audioTrack.write(paramArrayOfByte, 0, paramArrayOfByte.length);
  }
    //////////////////////////////////////\u89c6\u9891\u8d28\u91cf///////////////////////////////////////
	/**
	 * \u8c03\u8282\u89c6\u9891\u8d28\u91cf(1\u5230100)
	 */

     private void showChannelPickDialog()
  {
	
        final String[] arrayFruit = new String[] { getResources().getString(R.string.video_quality_smooth),
        											getResources().getString(R.string.video_quality_origin),
        											getResources().getString(R.string.video_quality_rear) }; 
 
        Dialog alertDialog = new AlertDialog.Builder(this,AlertDialog.THEME_HOLO_LIGHT) 
                .setTitle(R.string.select_channel_type)
                . setIcon(R.drawable.ic_launcher) 
                .setSingleChoiceItems(arrayFruit,qualityType, new DialogInterface.OnClickListener() { 
  
                    @Override 
                    public void onClick(DialogInterface dialog, int which) { 
                    	qualityType= which; 
					    dialog.dismiss();
					    if(qualityType==0)
					    {
					    	videoQuality=40;
					    	mBtnQuality.setText(R.string.video_quality_smooth);
					    }else if(qualityType==1)
					    {
					    	mBtnQuality.setText(R.string.video_quality_origin);
					    	videoQuality=55;
					    }else if(qualityType==2)
					    {
					    	mBtnQuality.setText(R.string.video_quality_rear);
					    	videoQuality=60;
					    }
				        connection.setVideoQuality(0, channel, videoQuality, new DeviceResultHandler() {
				            @Override
				            public void onSuccess(DeviceResult result) {
				            }
				            
				            @Override
				            public void onFailure(DeviceResult result, int errorCode) {
				                //Toast.makeText(context, "\u89c6\u9891\u8d28\u91cf\u8bbe\u7f6e\u5931\u8d25", Toast.LENGTH_SHORT).show();
				            }
				        });
                    } 
                }).  
                create(); 
        alertDialog.show(); 
  }
	
	private void onClickQuality(){
		showChannelPickDialog();
		/*
	    if(videoQuality > 50) {
	        videoQuality = 40;
	    } else {
	        videoQuality = 80;
	    }
	    mBtnQuality.setText(videoQuality > 50 ? R.string.video_quality_origin : R.string.video_quality_smooth);
        connection.setVideoQuality(0, channel, videoQuality, new DeviceResultHandler() {
            @Override
            public void onSuccess(DeviceResult result) {
                 if(videoQuality>50)
                 {
                     mBtnQuality.setText(R.string.video_quality_origin);
                 }else// if(videoQuality>25)
                 {
                       mBtnQuality.setText(R.string.video_quality_smooth);
                 }
            }
            
            @Override
            public void onFailure(DeviceResult result, int errorCode) {
                //Toast.makeText(context, "\u89c6\u9891\u8d28\u91cf\u8bbe\u7f6e\u5931\u8d25", Toast.LENGTH_SHORT).show();
            }
        });
	*/
	
	}

    private void onClickRecordScreen()
	{
	     mRecordScreenStart=true;
		 btn_short_record.setImageResource(R.drawable.live_video_control_record_select);
   		 btn_short_record.setEnabled(true);
	}
	
	@Override
	public void onVideoPlaying(int channel) {
		progressBar.setVisibility(View.GONE);
		//reSync.setVisibility(View.VISIBLE);
		isVideoOpened = true;

	}

    
    @Override
    protected void onResume() {
        super.onResume();
        if (!mCoReceOn){
            registerReceiver(mCommandBroadcastReceiver, new IntentFilter(CommandService.ACTION_DATACHANGE));
            mCoReceOn = true;
        }
		Log.e(tag,"main onResume");
		startmedia();
		needConnState =true;
		mObserveWifidataThread=new ObserveWifidataThread();
		mObserveWifidataThread.start();
    }

    
    @Override
    protected void onPause() {
        super.onPause();
        Log.e(tag,"main onPause");
        if (null != mCommandBroadcastReceiver && mCoReceOn){
            unregisterReceiver(mCommandBroadcastReceiver);
            mCoReceOn = false;
        }
        if(mObserveWifidataThread!=null)
        mObserveWifidataThread.stopObserve();
		if(mStateAudio!=0)
		{
		  onStopLiveAudio();
		}
        if (audioTrack != null)
		{
			audioTrack.stop();
            audioTrack.release();
            audioTrack=null;
		}
		needConnState =false;
		mHandler.sendEmptyMessageDelayed(MSG_CONN_CHECK, 100);
    }

    
    @Override
    protected void onStop() 
    {
		super.onStop();
	}

	
	public  void stopAp()
	{
		 WifiApMan apMan = new WifiApMan(this) ;
		apMan.setWifiManager(mWifiManager);
		if(apMan.isWifiApEnabled())
		{ 
		
		   apMan.setWifiApEnabled(apMan.getWifiApConfiguration(),false);
		}
		if (!mWifiManager.isWifiEnabled()) 
		{
            mWifiManager.setWifiEnabled(true);
        }
	}

    public void sendSta()
	{

		String SSID="INFOTM_cardv";
		String pwd="admin888";
		String KEYMGMT="WPA-PSK";

		mPref = PreferenceManager.getDefaultSharedPreferences(this);
    	    String storedSsid = mPref.getString("wifi", null);
    	    if(storedSsid == null)
            {
              storedSsid = SSID;
            }else
            {
               Log.i(tag,"sendSta ssid:"+storedSsid);
            }
    	    pwd=mPref.getString(storedSsid, "admin888");
        final String string = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                mCamera.getNextCmdIndex(), "action", 3, "action.wifi.set:"+"AP;;"+storedSsid+";;"+pwd+";;"+KEYMGMT+";;");
        mCamera.sendCommand(new InfotmCamera.OperationResponse() {
            
            public void onCompele(){
                mHandler.postDelayed(new Runnable() {
                    public void run() {
                       stopAp();
                    }
                }, 0L);
            }
            @Override
            public boolean execute() {
                return mCamera.mCommandSender.sendCommand(string);
            }
            
            @Override
            public void onSuccess() {
                onCompele();
				 Log.i(tag,"sendSta onSuccess");
            }
            
            @Override
            public void onFail() {
                onCompele();
				 Log.i(tag,"sendSta onFail");
            }
        },mHandler);
	}
	
    public void stopAllAp()
	{
	  Log.i(tag,"stopAllAp");
	  WifiApMan apMan = new WifiApMan(this) ;
	  apMan.setWifiManager(mWifiManager);
	  if(apMan.isWifiApEnabled())
	  { 
		  sendSta();
	  }
	}

	
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (null != mCommandBroadcastReceiver && mCoReceOn){
            unregisterReceiver(mCommandBroadcastReceiver);
            mCoReceOn = false;
        }
        if (null != mWifiStrenthListen){
            unregisterReceiver(mWifiStrenthListen);
        }
		stopmedia();
		
		if(mStateAudio!=0)
		{
		  onStopLiveAudio();
		}
        if (audioTrack != null)
		{
		    audioTrack.stop();
            audioTrack.release();
            audioTrack=null;
        }
        if(isForward==0){
		stopAllAp();
        }
		// 
		 mHandler.removeMessages(MSG_COUNTING);
	    Log.i(tag,"onDestroy");
	    //set wifi to null
	    Editor e=mPref.edit();
	    e.putString("wifi", null);
	    e.putString("deviceip", null);
	    e.commit();
    }

	@Override
	public void onBackPressed() {
			  super.onBackPressed();
			  Log.i(tag,"onBackPressed");
			  stopService(new Intent(getApplicationContext(), NetService.class));
			  stopService(new Intent(getApplicationContext(),CommandService.class));
		  }



	
    private void registerWifiStrenthListen(){
        mWifiView = (ImageView) findViewById(R.id.img_status_wifi);
        mWifiView.setImageLevel(getWifiLevel());//init wifi icon
        mWifiStrenthListen = new BroadcastReceiver(){
            @Override
            public void onReceive(Context context, Intent intent) {
                mWifiView.setImageLevel(getWifiLevel());
            }
        };
        registerReceiver(mWifiStrenthListen, new IntentFilter(WifiManager.RSSI_CHANGED_ACTION));
    }
    
    private int getWifiLevel(){
        int imgLv = 5;
        if (null != mWifiManager){
            WifiInfo wInfo = mWifiManager.getConnectionInfo();
            int i = wInfo.getRssi();
            if (i > -60){
                imgLv = 4;
            } else if (i > -70){
                imgLv = 3;
            } else if (i > -80){
                imgLv = 2;
            } else if (i > -90){
                imgLv = 1;
            } else {
                imgLv = 0;
            }
        }
        return imgLv;
    }
    
    private void initUiVars(){
        mFrameFpsView = (TextView) findViewById(R.id.txt_res_frame_fov);
        mAdvancedView = (ImageView) findViewById(R.id.img_advance);
        mLowLightView = (ImageView) findViewById(R.id.img_lowlight);
        mBatteryView = (ImageView) findViewById(R.id.bv_status_battery);
        mCapacityView = (TextView) findViewById(R.id.txt_status_number);
        mBtnShutter = (ToggleButton) findViewById(R.id.img_shutter);
        //img_shutter_contain= (LinearLayout) findViewById(R.id.img_shutter_contain);
        mBtnShutter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            Log.v(tag, "initUiVars");
            soundPool.play(soundPoolMap.get(2), 1, 1, 0, 0, 1);
                mCamera.sendCommand(new InfotmCamera.CameraOperation() {
                    @Override
                    public boolean execute() {
                        String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                mCamera.getNextCmdIndex(), "action", 1, 
                                "action.shutter.press"+":shutter" +";");
                        return mCamera.mCommandSender.sendCommand(cmd);
                    }
                }, null);
            }
        });
		Log.v(tag, "initUiVars2");
	    mBtnBurst = ((ImageButton)findViewById(R.id.img_burst));
        mBtnBurst.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	soundPool.play(soundPoolMap.get(1), 1, 1, 0, 0, 1);
                mCamera.sendCommand(new InfotmCamera.CameraOperation() {
                    @Override
                    public boolean execute() {
                       //INFT_CAMERA_FRONT=0,
						 // INFT_CAMERA_REAR=1,
						 // INFT_CAMERA_FRONT_REAR=2,
						 // INFT_CAMERA_REAR_FRONT,
                       String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                mCamera.getNextCmdIndex(), "action", 2, 
                                "action.shutter.press"+":photo" +";"+"camera:"+ ((videoQuality<56)?0:1)+";");
                        return mCamera.mCommandSender.sendCommand(cmd);
                    }
                }, null);
            }
        });
        btn_sd_record = ((ImageButton)findViewById(R.id.btn_sd_record));
        btn_sd_record.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//            	RecordActivity.startActivity(context, device.getDeviceId(), channel, DeviceType.IPC, System.currentTimeMillis(), 10);
                Intent browserIntent = new Intent(MainActivity.this, FolderBrowserActivity.class);
                browserIntent.putExtra(InfotmCamera.DEVICE_IP, device.getIp());
                startActivity(browserIntent);
            }
        });
		btn_live_video_audio= ((ImageButton)findViewById(R.id.btn_live_video_audio));
        btn_live_video_audio.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	onClickAudio();
            }
        });		

        btn_short_record= ((ImageButton)findViewById(R.id.btn_short_record));
        btn_short_record.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	onClickRecordScreen();
            }
        });	
		
		mBtnQuality = ((TextView)findViewById(R.id.btn_quality));
		if(qualityType==0)
	    {
	    	videoQuality=40;
	    	mBtnQuality.setText(R.string.video_quality_smooth);
	    }else if(qualityType==1)
	    {
	    	mBtnQuality.setText(R.string.video_quality_origin);
	    	videoQuality=55;
	    }else if(qualityType==2)
	    {
	    	mBtnQuality.setText(R.string.video_quality_rear);
	    	videoQuality=60;
	    }
        mBtnQuality.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               onClickQuality();
            }
        });
		
        mBtnSettings = ((ImageButton)findViewById(R.id.img_settings));
        mBtnSettings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(tag, "------start setting");
                Intent intent = new Intent(MainActivity.this, CameraSettingsActivity.class);
                intent.putExtra("camera_guid", ".getGuid()");
                startActivity(intent);
            }
        });
        
        mBtnMore = ((ImageButton)findViewById(R.id.img_more));
        mBtnMore.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(tag, "------start more");
                showMoreDialog();
            }
        });
        mBtnBack = (ImageButton)findViewById(R.id.img_back);
        mBtnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            	onBackPressed();
            }
        });
        /*mBtnControls = (CheckableImageButton) findViewById(R.id.img_control);
        mBtnControls.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (null != mPopupControls && !mPopupControls.isShowing()){
//                    View parentView = findViewById(R.id.preview_back);
//                    Log.v(tag,"----------------hei:" + parentView.getHeight());
                    View footer = findViewById(R.id.info_bar);
                    mPopupControls.showAtLocation(footer, Gravity.BOTTOM, 0, footer.getHeight());
                }
            }
        });*/
		Log.v(tag, "initUiVars3");
        initModePopup();
    }
    
    private void refreshUI(){
//        mFrameFpsView
//        mAdvancedView
        Log.i(tag, "------mCamera.mCurrentWorking---" +mCamera.mCurrentWorking);
        if ("on".equalsIgnoreCase(mCamera.mCurrentWorking)){
            mBtnShutter.setBackgroundDrawable(getResources().getDrawable(R.anim.shutter_circle_blink));
        	final AnimationDrawable anim =(AnimationDrawable)mBtnShutter.getBackground();
        	anim.start();
            if (!mBtnShutter.isChecked()){
                recordedTime = 0;
                mHandler.sendEmptyMessageDelayed(MSG_COUNTING, 1000);
                mBtnShutter.setChecked(true);	
                Log.i(tag,"mBtnShutter.setChecked(true)");
            }
        } else {
        	//final AnimationDrawable anim =(AnimationDrawable)mBtnShutter.getBackground();
        	//anim.stop();
        	mBtnShutter.setBackgroundDrawable(getResources().getDrawable(R.drawable.shutter_circle));
            if (mBtnShutter.isChecked()){
                recordedTime = 0;
                mHandler.removeMessages(MSG_COUNTING);
                mBtnShutter.setChecked(false);
                Log.i(tag,"mBtnShutter.setChecked(false)");
            }
        }
        try {
			if(null!=mCamera.mCurrentBatteryLv)
            mBatteryView.setImageLevel(Integer.parseInt(mCamera.mCurrentBatteryLv));
        } catch (Exception e) {
            Log.e(tag, "----error curbatterylv:"+mCamera.mCurrentBatteryLv);
            e.printStackTrace();
        }
//        mBtnControls
        if (null != mPopupControls && mPopupControls.isShowing()){
            mPopupControls.updateModeBtns(mCamera.mCurrentMode);
        }
        String smallModeValue = "";
        String resolution = "";
        String fpsString = "";
        int fps = 0;
        boolean advancedOn = false;
        boolean lowlightOn = false;
        if (Utils.MODE_G_VIDEO.equalsIgnoreCase(mCamera.mCurrentMode)){
            advancedOn = "on".equalsIgnoreCase(mCamera.getValueFromSetting(Utils.KEY_VIDEO_ADVANCE));
            lowlightOn = "on".equalsIgnoreCase(mCamera.getValueFromSetting(Utils.KEY_VIDEO_LOWLIGHT));
            
            smallModeValue = Utils.MODE_G_VIDEO;//mCamera.getValueFromSetting(Utils.KEY_VIDEO_MODE);
            if (Utils.MODE_VIDEO_1.equalsIgnoreCase(smallModeValue) ||
                    Utils.MODE_VIDEO_3.equalsIgnoreCase(smallModeValue)){
                resolution = mCamera.getValueFromSetting(Utils.KEY_VIDEO_RESOLUTION);
                fpsString = mCamera.getValueFromSetting(Utils.KEY_VIDEO_FPS);
                fps = Integer.parseInt("30");//(fpsString);
            }else if (Utils.MODE_VIDEO_2.equalsIgnoreCase(smallModeValue)){
                resolution = mCamera.getValueFromSetting(Utils.KEY_VIDEO_RESOLUTION);
                fpsString = mCamera.getValueFromSetting(Utils.KEY_VIDEO_INTERVAL);
                fpsString = fpsString.subSequence(0, 2).toString().trim() + "S";
                fps = Integer.parseInt(mCamera.getValueFromSetting(Utils.KEY_VIDEO_FPS));
            }
        } else if (Utils.MODE_G_CAR.equalsIgnoreCase(mCamera.mCurrentMode)){
            advancedOn = "on".equalsIgnoreCase(mCamera.getValueFromSetting(Utils.KEY_CAR_ADVANCE));
            lowlightOn = "on".equalsIgnoreCase(mCamera.getValueFromSetting(Utils.KEY_CAR_LOWLIGHT));
            smallModeValue = Utils.MODE_CAR_1;
            resolution = mCamera.getValueFromSetting(Utils.KEY_CAR_RESOLUTION);
            fpsString = mCamera.getValueFromSetting(Utils.KEY_CAR_FPS);
            fps = Integer.parseInt(fpsString);
        } else if (Utils.MODE_G_PHOTO.equalsIgnoreCase(mCamera.mCurrentMode)){
            advancedOn = "on".equalsIgnoreCase(mCamera.getValueFromSetting(Utils.KEY_PHOTO_ADVANCE));
            lowlightOn = "on".equalsIgnoreCase(mCamera.getValueFromSetting(Utils.KEY_PHOTO_LOWLIGHT));
            smallModeValue = mCamera.getValueFromSetting(Utils.KEY_PHOTO_MODE);
            resolution = mCamera.getValueFromSetting(Utils.KEY_PHOTO_MEGA);
            if (Utils.MODE_PHOTO_1.equalsIgnoreCase(smallModeValue)){
//                fps = mCamera.getValueFromSetting(Utils.KEY_PHOTO_TIMER);//off 2 10
                InfotmSetting iSetting = mCamera.getObjFromSetting(Utils.KEY_PHOTO_TIMER);
                if (null != iSetting){
                    switch (iSetting.getSelectedIndex()) {
                        case 0:
                            fpsString = "";
                            break;
                        case 1:
                            fpsString = "2S";
                            break;
                        case 2:
                            fpsString = "10S";
                            break;
                            default:
                                fpsString = "not fount for test:" + iSetting.getSelectedIndex();
                                break;
                    }
                }
            } else if (Utils.MODE_PHOTO_2.equalsIgnoreCase(smallModeValue)){
                fpsString = mCamera.getValueFromSetting(Utils.KEY_PHOTO_INTERVAL);
                fpsString = fpsString.subSequence(0, 2).toString().trim() + "FPS";
            } else if (Utils.MODE_PHOTO_3.equalsIgnoreCase(smallModeValue)){
                InfotmSetting iSetting = mCamera.getObjFromSetting(Utils.KEY_PHOTO_SHUTTER);
                fpsString = iSetting.getChosedItmShowName();
            }
        }
        mAdvancedView.setVisibility(advancedOn ? View.VISIBLE : View.GONE);
        mLowLightView.setVisibility(lowlightOn ? View.VISIBLE : View.GONE);
        Log.i(tag, "---refreshUI---modeValue:"+smallModeValue + " mCurrentMode:" + mCamera.mCurrentMode);
        if (null != smallModeValue && smallModeValue.length() != 0 && 
                null != CameraModePopup.RESIDMAP.get(smallModeValue.toLowerCase().trim())&&("off".equalsIgnoreCase(mCamera.mCurrentWorking))){
            //mBtnControls.setImageResource(CameraModePopup.RESIDMAP.get(smallModeValue.toLowerCase().trim()));
              enableUI();
            //mBtnControls.setVisibility(View.VISIBLE);
        } else {
            disableUI();
            //mBtnControls.setVisibility(View.GONE);
            Log.e(tag, "---refreshUI-mBtnControls--"+ smallModeValue );
        }
        if (null != resolution && resolution.length() != 0 && null != fpsString){
            if (fpsString.length() != 0){
                mFrameFpsView.setText(resolution + "/" + fpsString);
            } else {
                mFrameFpsView.setText(resolution);
            }
        } else {
            mFrameFpsView.setText("");
        }
                if (!mBtnShutter.isChecked()){
			if(null!=mCamera.mCurrentCapaity)
            mCapacityView.setText(getCapacityShowText(mCamera.mCurrentCapaity, mCamera.mCurrentMode, resolution, fps));
        }
        
        Log.v(tag, "resolution:"+ resolution + " fps:"+fpsString);
    }
    
    private void disableUI(){
        //mBtnControls.setChecked(false);
        mBtnSettings.setEnabled(false);
    }
    
    private void enableUI(){
        //mBtnControls.setChecked(true);
        mBtnSettings.setEnabled(true);
    }
    
    private String getCapacityShowText(String capacityInfo,String bigMode,String resolution, int fps){
        String[] lines = capacityInfo.split("/");
        try {
            if (lines.length == 3){
                long remainSize = Long.parseLong(lines[0]);
                if (Utils.MODE_G_VIDEO.equalsIgnoreCase(bigMode) || 
                        Utils.MODE_G_CAR.equalsIgnoreCase(bigMode)){
//                    int fps = Integer.parseInt(fpsString);
                	Log.i(tag, "----------->resolution:"+resolution);
                	int pixels =1080;
                	if("1080FHD".equals("resolution")){
                		pixels =1080;
                	}else if("720P 30FPS".equals("resolution")){
                		pixels =720;
                	}
                    videos = lines[1];
                    videoTimeRemainSeconds = Utils.GetVideoTimeRemain(remainSize, pixels, fps);
                    return videos + " / " + refreshTimeCounterString();
                } else if (Utils.MODE_G_PHOTO.equalsIgnoreCase(bigMode)){
                    if (resolution.contains("8")){
                        return lines[2] + " / " + Utils.GetPictureRemain(remainSize, 8);
                    } else if (resolution.contains("5")){
                        return lines[2] + " / " + Utils.GetPictureRemain(remainSize, 5);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }
    
    private String refreshTimeCounterString() {
        // TODO Auto-generated method stub
        String countingText = formatTwo(recordedTime / 3600) + ":" + formatTwo((recordedTime % 3600) / 60) + ":" 
                + formatTwo(recordedTime % 60);
        int remain = videoTimeRemainSeconds - recordedTime;
        String remainText = formatTwo(remain / 3600) + ":" + formatTwo((remain % 3600) / 60) + ":" + formatTwo(remain % 60);
        return countingText + " " + remainText;
    }
    
    private String formatTwo(int i){
        if (i < 10){
                return "0" + i;
        } else {
            return "" + i;
        }
    }
    
    private void initModePopup(){
        mPopupControls = new CameraModePopup(this);
//        mPopupControls.setAnimationStyle(animationStyle)
        mPopupControls.init(mCamera, getWindowManager().getDefaultDisplay(), null);
    }

    private void playRtspStream(String uriString) {
        mVideoView.setVideoURI(Uri.parse(uriString));
        mVideoView.requestFocus();
        mVideoView.start();
    }
    
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        // TODO Auto-generated method stub
        super.onConfigurationChanged(newConfig);
        int ori = newConfig.orientation;
        Log.v(tag, "----onConfigurationChanged----");
        //setBarByOrientation();
        RelativeLayout mrlay;
        mrlay = (RelativeLayout) findViewById(R.id.preview_back1);
        if(mrlay!=null)
        {
            RelativeLayout.LayoutParams pp =(RelativeLayout.LayoutParams) mrlay.getLayoutParams();
            Display display = getWindowManager().getDefaultDisplay(); 
            Log.i("MainActivity", "height:" + display.getWidth()); 
            pp.height = (display.getWidth() * 9 )/ 16;
            mrlay.setLayoutParams(pp);
        }
    }
    
    private void setBarByOrientation(){
        int ori = getResources().getConfiguration().orientation;
        if (ori == Configuration.ORIENTATION_LANDSCAPE){
            Log.v(tag, "------ORIENTATION_LANDSCAPE----");
//            requestWindowFeature(Window.FEATURE_NO_TITLE);//继承actionbarActivity导致不能在
            getActionBar().hide();
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.ICE_CREAM_SANDWICH){
                getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
            }
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);//
        } else if (ori == Configuration.ORIENTATION_PORTRAIT){//quit fullscreen and show actionbar
            getActionBar().show();
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);
            final WindowManager.LayoutParams attrs = getWindow().getAttributes();
            attrs.flags &= (~WindowManager.LayoutParams.FLAG_FULLSCREEN);
            getWindow().setAttributes(attrs);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);            
//            mVideoView.setOnClickListener(null);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
//        getSupportMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    
    @Override
    protected void onNetworkStateUpdate(Intent i) {
        Log.i(tag, "---onNetworkStateUpdate---stat:" + mCamera.mNetStatus);
//        super.onNetworkStateUpdate(i);
        if (mCamera.mNetStatus < InfotmCamera.STAT_YES_NET_YES_PIN){
            mVideoView.postDelayed(new Runnable() {
                @Override
                public void run() {
                    // TODO Auto-generated method stub
                   // startService(new Intent(getApplicationContext(), NetService.class));
                }
            }, 5000);
            
            showDialog(DIALOG_LOST);
        } else if (mCamera.mNetStatus == InfotmCamera.STAT_YES_NET_YES_PIN){
            startInitConfig(mHandler,0);
            removeDialog(DIALOG_LOST);
//            dismissDialog(DIALOG_LOST);
        } else if (mCamera.mNetStatus == InfotmCamera.STAT_Y_Y_Y_INIT) {
//            dismissDialog(DIALOG_LOST);
            removeDialog(DIALOG_LOST);
            refreshUI();
        }
    }


	


	@Override
	public void onVideoSizeChange(int arg0, int arg1, int arg2) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void onVideoTimout() {
		// TODO Auto-generated method stub
		
	}

//    @Override
//    public void onDataChanged(String ss) {
//        if (ss.equals("battery")) {
//
//        } else if (ss.equals("stop")) {
//            mHandler.sendEmptyMessage(MSG_STOP);
//        }
//    }



    private static final boolean VERBOSE = true;

    private static final String MIME_TYPE = "video/avc";
    private static  int WIDTH = 1280;
    private static  int HEIGHT = 720;
    private static final int BIT_RATE = 4000000;
    private static int FRAMES_PER_SECOND = 10;
    private static  int IFRAME_INTERVAL = 5;

    private static  int NUM_FRAMES = 80;

    // "live" state during recording
    //private MediaCodec.BufferInfo mBufferInfo;
    //private MediaCodec mEncoder;
    //private MediaMuxer mMuxer;
    private Surface mInputSurface;
    private int mTrackIndex;
    private boolean mMuxerStarted;
    private long mFakePts;
    private boolean mRecordScreen=true;
	private boolean mRecordScreenStart=false;
    private ArrayList<shortRecordInfo> mShortRecordInfoList = new ArrayList();
    private long mRecordTS =0;
 /*   private void recordScreen()
	{
	    
    	new Thread(new Runnable()
	    {
	      public void run()
	      {
	    	Log.i(tag,"recordScreen enter");
			long sleepTime=30;
			long lastts=0;
			long ts=0;
	        while(mRecordScreen==true)
	        {
	        	
	        	 Bitmap bmp= danalePlayer.getScreenshot();
	        	 ts=System.currentTimeMillis();
				 sleepTime=ts-lastts;
				 lastts=ts;
				 if((sleepTime>0)&&(sleepTime<30))
				 {
				      try {
					Thread.sleep(30-sleepTime);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						
					} 
				 }
	        	 mShortRecordInfoList.add(new shortRecordInfo(bmp,ts));
				 if(mRecordScreenStart==true)
				 {
				   if(ts>(mRecordTS+5000))
				   {
				     mRecordScreenStart = false;
					 FRAMES_PER_SECOND=mShortRecordInfoList.size()/10;
					 IFRAME_INTERVAL = FRAMES_PER_SECOND;
					 NUM_FRAMES =mShortRecordInfoList.size();
					 WIDTH = danalePlayer.getVideoWidth();
					 HEIGHT = (WIDTH==1920)?1080:720;
					 Log.i(tag,"NUM_FRAMES:"+NUM_FRAMES+" total:"+mShortRecordInfoList.size()+" WIDTH:"+WIDTH+" HEIGHT:"+HEIGHT);
					 generateMovie(new File(Environment.getExternalStorageDirectory().getAbsolutePath(), "infotm.mp4"));
					 mShortRecordInfoList.clear();
					 mHandler.sendEmptyMessage(MSG_SHORT_RECORD);
					 continue;
				   }
				 }
				 else
				 {
				   mRecordTS =ts;
				 }
				 if(mShortRecordInfoList.get(0).getTS()<(mRecordTS-5000))
				 {
				    mShortRecordInfoList.remove(0);
				 }
	        } 
	        Log.i(tag,"recordScreen leave");
	      }
	    }).start();
		
	}*/


/*    private void generateMovie(File outputFile) {
        try {
            prepareEncoder(outputFile);

            for (int i = 0; i < NUM_FRAMES; i++) {
                // Drain any data from the encoder into the muxer
                drainEncoder(false);

                // Generate a frame and submit it.
                  generateFrame(i);
//                submitFrame(computePresentationTimeNsec(i));
            }

            // Send end-of-stream and drain remaining output.
            drainEncoder(true);
        } catch (IOException ioe) {
            throw new RuntimeException(ioe);
        } finally {
            releaseEncoder();
        }
    }*/

    /**
     * Prepares the video encoder, muxer, and an input surface.
     */
/*    private void prepareEncoder(File outputFile) throws IOException {
        mBufferInfo = new MediaCodec.BufferInfo();

        MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE, WIDTH, HEIGHT);

        // Set some properties.  Failing to specify some of these can cause the MediaCodec
        // configure() call to throw an unhelpful exception.
        format.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        format.setInteger(MediaFormat.KEY_BIT_RATE, BIT_RATE);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, FRAMES_PER_SECOND);
        format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);
        if (VERBOSE) Log.d(tag, "format: " + format);

        // Create a MediaCodec encoder, and configure it with our format.  Get a Surface
        // we can use for input and wrap it with a class that handles the EGL work.
        mEncoder = MediaCodec.createEncoderByType(MIME_TYPE);
        Surface surface = glSurfaceView.getHolder().getSurface();
        if(surface==null)
        {
            Log.i(tag,"surface is null");        	
        }else
        {
        	Log.i(tag,"surface not null");
        }
        mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mInputSurface = mEncoder.createInputSurface();
        mEncoder.start();
        // Create a MediaMuxer.  We can't add the video track and start() the muxer here,
        // because our MediaFormat doesn't have the Magic Goodies.  These can only be
        // obtained from the encoder after it has started processing data.
        //
        // We're not actually interested in multiplexing audio.  We just want to convert
        // the raw H.264 elementary stream we get from MediaCodec into a .mp4 file.
        if (VERBOSE) Log.d(tag, "output will go to " + outputFile);
        mMuxer = new MediaMuxer(outputFile.toString(),
                MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);

        mTrackIndex = -1;
        mMuxerStarted = false;
    }*/

    /**
     * Releases encoder resources.  May be called after partial / failed initialization.
     */
/*    private void releaseEncoder() {
        if (VERBOSE) Log.d(tag, "releasing encoder objects");
        if (mEncoder != null) {
            mEncoder.stop();
            mEncoder.release();
            mEncoder = null;
        }
        if (mInputSurface != null) {
            mInputSurface.release();
            mInputSurface = null;
        }
        if (mMuxer != null) {
        	
            mMuxer.stop();
            mMuxer.release();
            mMuxer = null;
        }
    }*/

    /**
     * Extracts all pending data from the encoder.
     * <p>
     * If endOfStream is not set, this returns when there is no more data to drain.  If it
     * is set, we send EOS to the encoder, and then iterate until we see EOS on the output.
     * Calling this with endOfStream set should be done once, right before stopping the muxer.
     */
/*    private void drainEncoder(boolean endOfStream) {
        final int TIMEOUT_USEC = 10000;
        if (VERBOSE) Log.d(tag, "drainEncoder(" + endOfStream + ")");

        if (endOfStream) {
            if (VERBOSE) Log.d(tag, "sending EOS to encoder");
            mEncoder.signalEndOfInputStream();
        }

        ByteBuffer[] encoderOutputBuffers = mEncoder.getOutputBuffers();
        while (true) {
            int encoderStatus = mEncoder.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
            if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                // no output available yet
                if (!endOfStream) {
                    break;      // out of while
                } else {
                    if (VERBOSE) Log.d(tag, "no output available, spinning to await EOS");
                }
            } else if (encoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                // not expected for an encoder
                if (VERBOSE) Log.d(tag, "MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED");
                encoderOutputBuffers = mEncoder.getOutputBuffers();
            } else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                // should happen before receiving buffers, and should only happen once
                if (mMuxerStarted) {
                    throw new RuntimeException("format changed twice");
                }
                MediaFormat newFormat = mEncoder.getOutputFormat();
                Log.d(tag, "encoder output format changed: " + newFormat);

                // now that we have the Magic Goodies, start the muxer
                mTrackIndex = mMuxer.addTrack(newFormat);
                mMuxer.start();
                mMuxerStarted = true;
            } else if (encoderStatus < 0) {
                Log.w(tag, "unexpected result from encoder.dequeueOutputBuffer: " +
                        encoderStatus);
                // let's ignore it
            } else {
                ByteBuffer encodedData = encoderOutputBuffers[encoderStatus];
                if (encodedData == null) {
                    throw new RuntimeException("encoderOutputBuffer " + encoderStatus +
                            " was null");
                }

                if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    // The codec config data was pulled out and fed to the muxer when we got
                    // the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
                    if (VERBOSE) Log.d(tag, "ignoring BUFFER_FLAG_CODEC_CONFIG");
                    mBufferInfo.size = 0;
                }

                if (mBufferInfo.size != 0) {
                    if (!mMuxerStarted) {
                        throw new RuntimeException("muxer hasn't started");
                    }

                    // adjust the ByteBuffer values to match BufferInfo
                    encodedData.position(mBufferInfo.offset);
                    encodedData.limit(mBufferInfo.offset + mBufferInfo.size);
                    mBufferInfo.presentationTimeUs = mFakePts;
                    mFakePts += 1000000L / FRAMES_PER_SECOND;

                    mMuxer.writeSampleDat1703a(mTrackIndex, encodedData, mBufferInfo);
                  //  if (VERBOSE) Log.i(tag, "sent " + mBufferInfo.size + " bytes to muxer");
                }else
                {
                    Log.i(tag, "mBufferInfo.size = 0 ");
                }

                mEncoder.releaseOutputBuffer(encoderStatus, false);

                if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (!endOfStream) {
                        Log.w(tag, "reached end of stream unexpectedly");
                    } else {
                        if (VERBOSE) Log.d(tag, "end of stream reached");
                    }
                    break;      // out of while
                }
            }
        }
    }*/

    /**
     * Generates a frame, writing to the Surface via the "software" API (lock/unlock).
     * <p>
     * There's no way to set the time stamp.
     */
/*    private void generateFrame(int frameNum) {
    	Bitmap bmp= danalePlayer.getScreenshot();
        Canvas canvas = mInputSurface.lockCanvas(null);
        try {
            Paint paint = new Paint();
            canvas.drawBitmap(bmp,0, 0, paint);
        } finally {
            mInputSurface.unlockCanvasAndPost(canvas);
        }
    }*/
    
    //start 
    Dialog mMoreDialog;
    TextView mQualityRearView,mQualityOriginView,mQualitySmoothView;
    SwitchButton mAudioSet;
    public void showMoreDialog(){
    	mMoreDialog=new AlertDialog.Builder(this).create();
    	mMoreDialog.show();
    	Window window = mMoreDialog.getWindow();  
    	window.setContentView(R.layout.more_dialog_layout);
    	mMoreDialog.findViewById(R.id.more_cancel).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				destroyMoreDialog();
			}
		});
    	mMoreDialog.findViewById(R.id.more_setting).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
                Intent intent = new Intent(MainActivity.this, CameraSettingsActivity.class);
                intent.putExtra("camera_guid", ".getGuid()");
                startActivity(intent);
                destroyMoreDialog();
			}
		});
    	mMoreDialog.findViewById(R.id.more_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
                Intent browserIntent = new Intent(MainActivity.this, FolderBrowserActivity.class);
                browserIntent.putExtra(InfotmCamera.DEVICE_IP, device.getIp());
                startActivity(browserIntent);
                destroyMoreDialog();
			}
		});
    	mMoreDialog.findViewById(R.id.phone_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				String storedSsid = mPref.getString("wifi", null);
    			Intent intent= new Intent(MainActivity.this, PhoneLocalMediaDeviceActivity.class) ;
    			intent.putExtra("device", storedSsid);
    			startActivity(intent) ;
                destroyMoreDialog();
			}
		});
    	mMoreDialog.findViewById(R.id.more_upgrade).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
    			Intent intent= new Intent(MainActivity.this, UpgradeActivity.class) ;
    			startActivity(intent) ;
                destroyMoreDialog();
			}
		});
    	mQualityRearView=(TextView) mMoreDialog.findViewById(R.id.video_quality_rear);
    	mQualityOriginView=(TextView) mMoreDialog.findViewById(R.id.video_quality_origin);
    	mQualitySmoothView=(TextView) mMoreDialog.findViewById(R.id.video_quality_smooth);
    	if(videoQuality==40){
			mQualityRearView.setBackgroundResource(R.drawable.more_function_selector);
			mQualityOriginView.setBackgroundResource(R.drawable.more_function_selector);
			mQualitySmoothView.setBackgroundResource(R.drawable.quality_press);
    	}else if(videoQuality==55){
			mQualityRearView.setBackgroundResource(R.drawable.more_function_selector);
			mQualityOriginView.setBackgroundResource(R.drawable.quality_press);
			mQualitySmoothView.setBackgroundResource(R.drawable.more_function_selector);
    	}else{
			mQualityRearView.setBackgroundResource(R.drawable.quality_press);
			mQualityOriginView.setBackgroundResource(R.drawable.more_function_selector);
			mQualitySmoothView.setBackgroundResource(R.drawable.more_function_selector);
    	}
    	mQualityRearView.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
	    		if("on".equalsIgnoreCase(mCamera.mCurrentCameraRearStatus)){
				mQualityRearView.setBackgroundResource(R.drawable.quality_press);
				mQualityOriginView.setBackgroundResource(R.drawable.more_function_selector);
				mQualitySmoothView.setBackgroundResource(R.drawable.more_function_selector);
				videoQuality=60;
		        connection.setVideoQuality(0, channel, videoQuality, new DeviceResultHandler() {
		            @Override
		            public void onSuccess(DeviceResult result) {
		            	Toast.makeText(MainActivity.this, "Rear", 2000).show();
		            }
		            
		            @Override
		            public void onFailure(DeviceResult result, int errorCode) {
		                //Toast.makeText(context, "\u89c6\u9891\u8d28\u91cf\u8bbe\u7f6e\u5931\u8d25", Toast.LENGTH_SHORT).show();
		            }
		        });
	    		}else{
	    			Toast.makeText(MainActivity.this, R.string.rear_camera_not_insert_str, 2000).show();
	    		}
				
			}
		});
    	mQualityOriginView.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mQualityRearView.setBackgroundResource(R.drawable.more_function_selector);
				mQualityOriginView.setBackgroundResource(R.drawable.quality_press);
				mQualitySmoothView.setBackgroundResource(R.drawable.more_function_selector);
				videoQuality=55;
		        connection.setVideoQuality(0, channel, videoQuality, new DeviceResultHandler() {
		            @Override
		            public void onSuccess(DeviceResult result) {
		            	Toast.makeText(MainActivity.this, "Origin", 2000).show();
		            }
		            
		            @Override
		            public void onFailure(DeviceResult result, int errorCode) {
		                //Toast.makeText(context, "\u89c6\u9891\u8d28\u91cf\u8bbe\u7f6e\u5931\u8d25", Toast.LENGTH_SHORT).show();
		            }
		        });
			}
		});
    	mQualitySmoothView.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mQualityRearView.setBackgroundResource(R.drawable.more_function_selector);
				mQualityOriginView.setBackgroundResource(R.drawable.more_function_selector);
				mQualitySmoothView.setBackgroundResource(R.drawable.quality_press);
				videoQuality=40;
		        connection.setVideoQuality(0, channel, videoQuality, new DeviceResultHandler() {
		            @Override
		            public void onSuccess(DeviceResult result) {
		            	Toast.makeText(MainActivity.this, "Smooth", 2000).show();
		            }
		            
		            @Override
		            public void onFailure(DeviceResult result, int errorCode) {
		                //Toast.makeText(context, "\u89c6\u9891\u8d28\u91cf\u8bbe\u7f6e\u5931\u8d25", Toast.LENGTH_SHORT).show();
		            }
		        });
			}
		});
    	
    	
    	mAudioSet=(SwitchButton) mMoreDialog.findViewById(R.id.audio_switch);
    	mAudioSet.setOnChangeListener(new SwitchButton.OnChangeListener() {
			
			@Override
			public void onChange(SwitchButton sb, boolean state) {
				// TODO Auto-generated method stub
				onClickAudio();
			}
		});

    }
    
    public void destroyMoreDialog(){
    	if(mMoreDialog!=null){
    		mMoreDialog.dismiss();
    	}
    }
    

    //getConnectedIP
	private ArrayList<String> getConnectedIP() {
		Log.i(tag, "getConnectedIP");
		ArrayList<String> connectedIP = new ArrayList<String>();
		try {
		BufferedReader br = new BufferedReader(new FileReader(
		"/proc/net/arp"));
		String line;
		while ((line = br.readLine()) != null) {
		String[] splitted = line.split(" ");
		if (splitted != null && splitted.length >= 4) {
		String ip = splitted[0];
		if(ip.length()>6)
			connectedIP.add(ip);
		}
		}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return connectedIP;
}
    
    
    //Device Disconnect dialog
    Dialog mDeviceDisconnectDialog;
    public void showDeviceDisconnectDialog(){
    	if(mDeviceDisconnectDialog==null){
    		mDeviceDisconnectDialog=new AlertDialog.Builder(this).create();
    	}
    	try{
    	mDeviceDisconnectDialog.show();
    	Window window = mDeviceDisconnectDialog.getWindow();  
    	window.setContentView(R.layout.device_disconnect_dialog_layout);  
    	mDeviceDisconnectDialog.findViewById(R.id.ok).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				destroyDeviceDisconnectDialog();
			    //set wifi to null
			    Editor e=mPref.edit();
			    e.putString("wifi", null);
			    e.putString("deviceip", null);
			    e.commit();
				onBackPressed();
				Intent intent;
				if(isForward==2)
				{
				  intent = new Intent(MainActivity.this,DeviceListOnlineActivity.class) ;
				}else if(isForward==3)
			    {
				  intent = new Intent(MainActivity.this, UsbTetherSettings.class) ;
				 }else
			    {
				  intent = new Intent(MainActivity.this, ConnectActivity.class) ;
				 }
				startActivity(intent) ;
			}
		});
    	}catch(Exception e){
    		
    	}
    		
    }
    
    public void destroyDeviceDisconnectDialog(){
    	if(mDeviceDisconnectDialog!=null){
    		mDeviceDisconnectDialog.dismiss();
    		mDeviceDisconnectDialog=null;
    	}
    }
    public void updateDeviceTime()
	{
	  
        mCamera.sendCommand(new InfotmCamera.CameraOperation() {
            @Override
            public boolean execute() {
            	  String time_uString = Long.toString(System.currentTimeMillis()/1000);
                  Log.v(tag, "----UTC----" + time_uString);
                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                        mCamera.getNextCmdIndex(), "action", 1, 
                        CameraSettingsActivity.KEY_TIME_SET+":"+ time_uString  +";");
                return mCamera.mCommandSender.sendCommand(cmd);
            }
        }, null);           
	}
    //return NULL/0---7
	public String getBattery()
	{
			 Log.e(tag,"mCurrentBatteryLv:"+mCamera.mCurrentBatteryLv);
			 return mCamera.mCurrentBatteryLv;
	}
	//get  value in onsuccess
	public void getRecordTime()
	{
		 mCamera.mCurrentRecordTime="0";
		 mCamera.sendCommand(new InfotmCamera.OperationResponse() {
	            @Override
	            public boolean execute() {
	            	//Log.i(tag, "getRecordTime");
	                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
	                        mCamera.getNextCmdIndex(), "action", 1, 
	                        CameraSettingsActivity.KEY_RECORD_TIME+":"+ 0  +";");
	                 String ret=mCamera.mCommandSender.sendCommandGetString(cmd);
	                 //Log.i(tag, "getRecordTime:" + ret);
	                 int beginIndex = ret.lastIndexOf(":");
	                 int lastIndex=ret.lastIndexOf(";");
	                 if ((beginIndex > 0)&&(lastIndex>0))
	                 {
	                	 mCamera.mCurrentRecordTime = ret.substring(beginIndex, lastIndex);
	                 }
	                return true;
	            }
	            public void onSuccess() {
	            	  Log.i(tag, "getRecordTime onSuccess:"+mCamera.mCurrentRecordTime);
	            }
	            
	            public void onFail()
	            {
	            	  Log.i(tag, "getRecordTime onfail" );
	            }
	        }, mHandler);      
		     
		     return;
	}
	//return on/off for record
	public String getCurrentWorking()
	{
		     Log.e(tag,"getCurrentWorking:"+mCamera.mCurrentWorking);
		     return mCamera.mCurrentWorking;
	}
	//return free space/videoCount/photoCount/remain_sec/remain_pictures such as 7600692/2/0/0/0
	public String getSDCapacity()
	{
			 Log.e(tag,"mCurrentCapaity:"+mCamera.mCurrentCapaity);
			 return mCamera.mCurrentCapaity;
	}
	
    public ObserveWifidataThread mObserveWifidataThread;
    public class ObserveWifidataThread extends Thread{
    	int noDataCount=0;
        boolean  isOberse=true;
    	long currentTraffic=0;
    	long lastSystemTraffic=0;
    	long durationTraffic=0;
    	public void stopObserve(){
    		isOberse=false;
    	}
    	@Override
    	public void run() {
    			while(isOberse){
    				try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
    				lastSystemTraffic=currentTraffic;
    				currentTraffic=TrafficStats.getTotalRxBytes()+TrafficStats.getTotalTxBytes();
    				durationTraffic=currentTraffic-lastSystemTraffic;
    				if(durationTraffic<100000){
    					noDataCount++;
    					Log.i(tag, "noDataCount---->"+noDataCount);
    					if(noDataCount>=10){
    						noDataCount=0;
    						mHandler.sendEmptyMessage(MSG_STOP_PREVIEW);
    					}
    				}else{
    					noDataCount=0;
    				}
    			}
    	}
    }
    private MediaCodec mediaCodec = null;
    public void videoDecode(SurfaceHolder surfaceHolder)
    {
       Log.i(tag,"videoDecode");
    	surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);     
		
    }

    public void videoDecodeStart(SurfaceHolder surfaceHolder)
    {
        Log.i(tag,"videoDecodeStart");
    	if (mediaCodec == null)
    	{
	    	mediaCodec = MediaCodec.createDecoderByType("video/avc");
	    	MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", 1088, 1088);  
	        mediaCodec.configure(mediaFormat, surfaceHolder.getSurface(), null, 0);  
			Log.i(tag," mediaCodec.start");
	        mediaCodec.start();  
	        isStartDecode=true;
        }
    }
    public void videoDecodeStop()
    {
       Log.i(tag,"videoDecodeStop");
    	if (mediaCodec != null) {
			Log.i(tag," mediaCodec.stop");
			isStartDecode=false;
    		mediaCodec.stop();
    		mediaCodec.release();
    		mediaCodec = null;
        }
    }
    public void onFrame(byte[] buf, int offset, int length, int flag,long ts) { 
    	   if(isStartDecode)
    	   {
	    	   if(mediaCodec != null)
	    	   {
	    		    if(isProgressBar==true)
		            {
	    		    	 mHandler.sendEmptyMessage(MSG_START);
		            }
	    		   
		            ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();  
		                int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);  
		            if (inputBufferIndex >= 0) {  
		                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];  
		                inputBuffer.clear();  
		                inputBuffer.put(buf, offset, length);  
		                mediaCodec.queueInputBuffer(inputBufferIndex, 0, length, ts, 0);   
		            }  
		           MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();  
		           int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo,0);  
		           while (outputBufferIndex >= 0) {  
		               mediaCodec.releaseOutputBuffer(outputBufferIndex, true);  
		               outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);  
		           }  
	           }
    	   }
    }


	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		  videoDecodeStart(arg0);
	}


	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		 arg0.removeCallback(this); 
		videoDecodeStop();
	}  
	
	
}




