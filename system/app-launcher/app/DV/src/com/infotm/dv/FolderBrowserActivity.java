package com.infotm.dv;

import java.io.File;
import java.net.URL;
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationSet;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.infotm.dv.R.drawable;
import com.infotm.dv.adapter.FileListAdapter;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.camera.InfotmCamera.CameraOperation;
import com.infotm.dv.connect.CommandSender;
import com.nostra13.universalimageloader.cache.disc.naming.Md5FileNameGenerator;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.assist.ImageScaleType;

public class FolderBrowserActivity extends Activity{
    
    private static final String TAG = "FolderBrowserActivity";
    
    private static final int WHAT_REFRESHUI = 9527;
    private static final int WHAT_REQUEST_LIST = 9528;
    public  static final int REFLASH_SELECT_ALL_UI = 9529;
    
    private int mUILevel = 0;
    private int mSelectedMediaType = InfotmCamera.MEDIA_INVALID;
    
    private View mLevel0View;
    private View mLevel1View;
    
    private FileListAdapter FileListAdapter;
    private ListView mListView;
    public static InfotmCamera mCamera = null;
    public Menu mMenu;
    public MenuItem mSelectAllIitem;
    
    
	public static DisplayImageOptions options;

	public static DisplayImageOptions initOption() {
		options = new DisplayImageOptions.Builder()
				.showImageOnLoading(drawable.ic_photo) // 设置图片在下载期间显示的图片
				.showImageForEmptyUri(drawable.ic_photo)// 设置图片Uri为空或是错误的时候显示的图片
				.showImageOnFail(drawable.ic_photo) // 设置图片加载/解码过程中错误时候显示的图片
				.cacheInMemory(true)// 设置下载的图片是否缓存在内存中
				.cacheOnDisc(true)// 设置下载的图片是否缓存在SD卡中
				.considerExifParams(true) // 是否考虑JPEG图像EXIF参数（旋转，翻转）
				.imageScaleType(ImageScaleType.EXACTLY_STRETCHED)// 设置图片以如何的编码方式显示
				.bitmapConfig(Bitmap.Config.RGB_565)// 设置图片的解码类型//
				// .displayer(new FadeInBitmapDisplayer(100))//是否图片加载好后渐入的动画时间
				.resetViewBeforeLoading(true)// 设置图片在下载前是否重置，复位
				.build();

		return options;
	}
	
    
    
//    private static boolean mListUpdated = false;
//    private static long mUpdateTime = 0;
    
    private int mRetries = 0;
    
    Handler mFileHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub
            int retry = 0;
            switch (msg.what) {
                case CameraOperation.WHAT_SUCCESS:
                    mRetries = 0;
                    break;
                case CameraOperation.WHAT_FAIL:
                    mRetries ++;
                    if(mRetries > 10) {
                        mRetries = 0;
                        return;
                    }
                    mFileHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            // TODO Auto-generated method stub
                            RequestLatestPlaybackList();
                        }
                    }, 500);
                    break;
                case WHAT_REFRESHUI:
                    RefreshUI();
                    break;
                case WHAT_REQUEST_LIST:
                    mFileHandler.removeMessages(WHAT_REQUEST_LIST);
                    RequestLatestPlaybackList();
                    //request file list every 1 minutes
                    mFileHandler.sendEmptyMessageDelayed(WHAT_REQUEST_LIST, 60 * 1000);
                    break;
                case REFLASH_SELECT_ALL_UI:
                	if(FileListAdapter.isSelectAll){
                		mSelectAllIitem.setIcon(R.drawable.menu_checked);
                  	}else{
                  		mSelectAllIitem.setIcon(R.drawable.menu_nochecked);
                  	}
                    break;
                default:
                    break;
            }
        }
    };
    
    BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            // TODO Auto-generated method stub
        	if(!FileListAdapter.isSelectMode){
            String action = intent.getAction();
            if(InfotmCamera.ACTION_PLAYBACK_LIST_UPDATED.equals(action)) {
//                mListUpdated = true;
//                mUpdateTime = SystemClock.uptimeMillis() / 1000; 
                mFileHandler.sendEmptyMessage(WHAT_REFRESHUI);
                setProgressBarVisibility(false);
                setProgressBarIndeterminateVisibility(false);
                if(FileListAdapter!= null) {
                    FileListAdapter.notifyDataSetChanged();
                }
            }
        	}
        }
    };
    
    private void RefreshUI() {
        int videoCount = InfotmCamera.mVideoList.size();
        //findViewById(R.id.video_folder).setVisibility(videoCount > 0 ? View.VISIBLE : View.GONE);
        ((TextView)findViewById(R.id.video_count)).setText("" + videoCount);
        
        int videoLockedCount = InfotmCamera.mLockedVideoList.size();
        //findViewById(R.id.video_lock_folder).setVisibility(videoLockedCount > 0 ? View.VISIBLE : View.GONE);
        ((TextView)findViewById(R.id.video_lock_count)).setText("" + videoLockedCount);
        
        int photoCount = InfotmCamera.mPhotoList.size();
        //findViewById(R.id.photo_folder).setVisibility(photoCount > 0 ? View.VISIBLE : View.GONE);
        ((TextView)findViewById(R.id.photo_count)).setText("" + photoCount);
        
        int mediaCount = videoCount + videoLockedCount + photoCount;
        //findViewById(R.id.no_media_found_tips).setVisibility(mediaCount > 0 ? View.GONE : View.VISIBLE);
    }
    
    /**
     * Request the latest playback list in device.
     * @return
     */
    private boolean RequestLatestPlaybackList() {
        setProgressBarVisibility(true);
        setProgressBarIndeterminateVisibility(true);
        mCamera.sendCommand(new InfotmCamera.CameraOperation() {
            @Override
            public boolean execute() {
                String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                        mCamera.getNextCmdIndex(), "getfilelist", 1, ";");
                return mCamera.mCommandSender.sendCommand(cmd);
            }
        }, mFileHandler);
        
        return false;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        requestWindowFeature(Window.FEATURE_PROGRESS);
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.activity_file_browser);
        IntentFilter filter = new IntentFilter(InfotmCamera.ACTION_PLAYBACK_LIST_UPDATED);
        registerReceiver(mReceiver, filter);
        
        mFileHandler.sendEmptyMessage(WHAT_REQUEST_LIST);
        mCamera = InfotmCamera.getInstance();
        init();
    }
    
    private void init() {
    	
    	   ImageLoaderConfiguration config = new ImageLoaderConfiguration.Builder(
    			   this).threadPriority(Thread.NORM_PRIORITY)
    			   .denyCacheImageMultipleSizesInMemory()
    			   .diskCacheSize(250 * 1024 * 1024).diskCacheFileCount(1000)
    			   .diskCacheFileNameGenerator(new Md5FileNameGenerator()).build();
    			   ImageLoader.getInstance().init(config); 
    		initOption();
    		ImageLoader.getInstance().init(config);
    	
        mLevel0View = findViewById(R.id.ui_level0);
        
        mListView = (ListView) findViewById(R.id.file_list);
        FileListAdapter=new  FileListAdapter(this, InfotmCamera.mVideoList,mediaType,mFileHandler);
        mListView.setAdapter(FileListAdapter);
        
        mListView.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                // TODO Auto-generated method stub
                String selectedFile = (String) ((FileListAdapter)parent.getAdapter()).getItem(position);
                Log.d(TAG, "selected file: " + selectedFile);
                showMoreDialog(selectedFile);
            }
        });
        
        mListView.setOnItemLongClickListener(new OnItemLongClickListener() {

			@Override
			public boolean onItemLongClick(AdapterView<?> arg0, View arg1,
					int arg2, long arg3) {
				// TODO Auto-generated method stub
				FileListAdapter.isSelectMode=true;
				FileListAdapter.notifyDataSetChanged();
				mMenu.removeItem(1);
				 mSelectAllIitem=mMenu.add(Menu.NONE,2,2, getString(R.string.menu_select_all));
				 mSelectAllIitem .setIcon(R.drawable.menu_nochecked)
		        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS | MenuItem.SHOW_AS_ACTION_WITH_TEXT);
				mMenu.add(Menu.NONE, 3,3, "Delete")
		        .setIcon(R.drawable.menu_delete)
		        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS | MenuItem.SHOW_AS_ACTION_COLLAPSE_ACTION_VIEW);

				return true;
			}  	
		});
        

        
        mLevel1View = mListView;
    }
    
    private void playMediaFile(String file) {
        if(file == null || file.isEmpty()) {
            return;
        }
        
        if(!(file.endsWith(".mp4") || file.endsWith(".jpg")|| file.endsWith(".mkv"))) {
            return;
        }
        if(mSelectedMediaType==InfotmCamera.MEDIA_PHOTO){
            Intent intent = new Intent(this, ShowDevicePictureActivity.class);
            intent.putExtra(InfotmCamera.MEDIA_FILE_NAME, file);
            intent.putExtra(InfotmCamera.MEDIA_TYPE, mSelectedMediaType);
            startActivity(intent);
        }else{
        	
        	if(null!=InfotmCamera.mCurrentCameraFormat&&InfotmCamera.mCurrentCameraFormat.contains("1")){
        				if(mSelectedMediaType==InfotmCamera.MEDIA_VIDEO){
        	    		Intent intent = new Intent(Intent.ACTION_VIEW);
        	    		Uri uri = Uri.parse("http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Video/"+file);
        	    		 Log.d(TAG, " file  path: " + uri.getPath());
        	    		intent.setDataAndType(uri ,"video/*");
        	    		startActivity(Intent.createChooser(intent, "打  开"));
        				}else{
            	    		Intent intent = new Intent(Intent.ACTION_VIEW);
            	    		Uri uri = Uri.parse("http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Locked/"+file);
            	    		 Log.d(TAG, " file  path: " + uri.getPath());
            	    		intent.setDataAndType(uri ,"video/*");
            	    		startActivity(Intent.createChooser(intent, "打  开"));
        				}
        			}else{
				        Intent intent = new Intent(this, PlaybackActivity.class);
				        intent.putExtra(InfotmCamera.MEDIA_FILE_NAME, file);
				        intent.putExtra(InfotmCamera.MEDIA_TYPE, mSelectedMediaType);
				        intent.putExtra(InfotmCamera.DEVICE_IP, getIntent().getStringExtra(InfotmCamera.DEVICE_IP));
				        startActivity(intent);
        			}
        	}
    }
    private int mediaType=1;
    private void setUILevel(final int level) {
        mUILevel = level;
        ArrayList<String> list = null;
        
        switch (mSelectedMediaType) {
            case InfotmCamera.MEDIA_VIDEO:
                list = InfotmCamera.mVideoList;
                mediaType=1;
                break;
            case InfotmCamera.MEDIA_VIDEO_LOCKED:
                list = InfotmCamera.mLockedVideoList;
                mediaType=2;
                break;
            case InfotmCamera.MEDIA_PHOTO:
                list = InfotmCamera.mPhotoList;
                mediaType=3;
            default:
                break;
        }
        if(list == null && level == 1) {
            return;
        }
        
        if(level == 1) {
            
            FileListAdapter=new  FileListAdapter(this, list,mediaType,mFileHandler);
            mListView.setAdapter(FileListAdapter);
            FileListAdapter.notifyDataSetChanged();
            
        }
        
        AnimationSet animationSet = new AnimationSet(true);
        AlphaAnimation alphaAnimation = new AlphaAnimation(1, 0);
        alphaAnimation.setDuration(300);
        alphaAnimation.setRepeatCount(0);
        //animationSet.addAnimation(alphaAnimation);
        alphaAnimation.setAnimationListener(new AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                // TODO Auto-generated method stub
            }
            
            @Override
            public void onAnimationRepeat(Animation animation) {
                // TODO Auto-generated method stub
            }
            @Override
            public void onAnimationEnd(Animation animation) {
                // TODO Auto-generated method stub
                Log.d(TAG, "onAnimationEnd");
                mLevel0View.setVisibility(level > 0 ? View.GONE : View.VISIBLE);
                mLevel1View.setVisibility(level > 0 ? View.VISIBLE : View.GONE);
            }
        });
        
//        View animView = level > 0 ? mLevel1View : mLevel0View;
//        animView.startAnimation(alphaAnimation);
        mLevel0View.setVisibility(level > 0 ? View.GONE : View.VISIBLE);
        mLevel1View.setVisibility(level > 0 ? View.VISIBLE : View.GONE);

    }
    
    @Override
    public void onBackPressed() {
        // TODO Auto-generated method stub
    	if(FileListAdapter.isSelectMode){
    		mMenu.removeItem(2);
    		mMenu.removeItem(3);
    		mMenu.add(Menu.NONE,1, 1, "Search")
            .setIcon(R.drawable.ic_sync)
            .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS | MenuItem.SHOW_AS_ACTION_COLLAPSE_ACTION_VIEW);
    		FileListAdapter.isSelectMode=false;
    		FileListAdapter.isSelectAll=false;
    		FileListAdapter.notifyDataSetChanged();
    		FileListAdapter.mSelectData.clear();
    		}else{
        if(mUILevel > 0) {
            mUILevel--;
            setUILevel(mUILevel);
        } else {
            super.onBackPressed();
        }
    		}
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        getActionBar().setDisplayHomeAsUpEnabled(true);
        mMenu=menu;
        menu.add(Menu.NONE,1, 1, "Search")
        .setIcon(R.drawable.ic_sync)
        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS | MenuItem.SHOW_AS_ACTION_COLLAPSE_ACTION_VIEW);
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
            case 1:
                mFileHandler.sendEmptyMessage(WHAT_REQUEST_LIST);
                break;
            case 2:
            	if(FileListAdapter.isSelectAll){
            	  item.setIcon(R.drawable.menu_nochecked);
            	  	FileListAdapter.isSelectAll=false;
            	  	FileListAdapter.mSelectData.clear();
            	}else{
            		item.setIcon(R.drawable.menu_checked);
            		FileListAdapter.isSelectAll=true;
            		FileListAdapter.mSelectData.clear();
            		FileListAdapter.addAllSelectData();
            	}
            	FileListAdapter.notifyDataSetChanged();
                break;
            case 3:
            	if(FileListAdapter.mSelectData.size()>0){
            	String type="Video";
            	if(mSelectedMediaType == InfotmCamera.MEDIA_VIDEO){
            		type="Video";
            		if(FileListAdapter.isSelectAll){
            			deleteCommand(type,"all;;");
            		}else{
            			StringBuilder nameStr=new StringBuilder();;
            			for(String name:FileListAdapter.mSelectData){
            				nameStr.append(name).append(";;");
            			}
            			deleteCommand(type,nameStr.toString());
            		}
            		
            	}else if(mSelectedMediaType == InfotmCamera.MEDIA_VIDEO_LOCKED){
            		type="Locked";
            		if(FileListAdapter.isSelectAll){
            			deleteCommand(type,"all;;");
            		}else{
            			StringBuilder nameStr=new StringBuilder();;
            			for(String name:FileListAdapter.mSelectData){
            				nameStr.append(name).append(";;");
            			}
            			deleteCommand(type,nameStr.toString());
            		}
            	}else if(mSelectedMediaType == InfotmCamera.MEDIA_PHOTO){
            		type="Photo";
            		if(FileListAdapter.isSelectAll){
            			deleteCommand(type,"all;;");
            		}else{
            			StringBuilder nameStr=new StringBuilder();;
            			for(String name:FileListAdapter.mSelectData){
            				nameStr.append(name).append(";;");
            			}
            			deleteCommand(type,nameStr.toString());
            		}
            	}   	
            	}
            	onBackPressed();
            	break;
            default:

        }
        return super.onOptionsItemSelected(item);
    }
    
    public void deleteCommand(String type,String filename){
		final String typeset=type;
		final String name=filename;
        mCamera.sendCommand(new InfotmCamera.CameraOperation() {
            @Override
            public boolean execute() {
				String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                        mCamera.getNextCmdIndex(), "action", 1, 
                        "action.file.delete"+":select;;"+typeset+";;"+name);
                return mCamera.mCommandSender.sendCommand(cmd);
            }
        }, handler);
        mFileHandler.sendEmptyMessage(WHAT_REQUEST_LIST);
    }
    
    public void onFolderSelect(View v) {
        mSelectedMediaType = InfotmCamera.MEDIA_INVALID;
        switch(v.getId()) {
            case R.id.video_folder:
                mSelectedMediaType = InfotmCamera.MEDIA_VIDEO;
                break;
            case R.id.video_lock_folder:
                mSelectedMediaType = InfotmCamera.MEDIA_VIDEO_LOCKED;
                break;
            case R.id.photo_folder:
                mSelectedMediaType = InfotmCamera.MEDIA_PHOTO;
                break;
        }
        if(mSelectedMediaType == InfotmCamera.MEDIA_INVALID)
            return;
        
        setUILevel(1);
    }

    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        mFileHandler.removeMessages(WHAT_REQUEST_LIST);
        unregisterReceiver(mReceiver);
    }
    
    
    //show list item operate dialog
    Dialog mOperateDialog;
    DownloadTask mDownloadTask;
    public void showMoreDialog(String selectedFile){
    	final String file=selectedFile;
    	mOperateDialog=new AlertDialog.Builder(this).create();
    	mOperateDialog.show();
    	Window window = mOperateDialog.getWindow();  
    	window.setContentView(R.layout.media_operate_dialog_layout);
    	mOperateDialog.findViewById(R.id.open_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				playMediaFile(file);
				destroyOperateDialog();
			}
		});
    	mOperateDialog.findViewById(R.id.save_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				try{
					String urlString="";
					String urlString1="";
					String tempTitle="";
					if(mSelectedMediaType==InfotmCamera.MEDIA_VIDEO){
				            urlString="http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Video/"+file;
							 if(file.endsWith(".mkv"))
							 	tempTitle=file.replaceFirst("mkv", "jpg");
							 else
				             tempTitle=file.replaceFirst("mp4", "jpg");
				            urlString1="http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Video/."+tempTitle;
					}else if(mSelectedMediaType==InfotmCamera.MEDIA_VIDEO_LOCKED){
						urlString="http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Locked/"+file;
						if(file.endsWith(".mkv"))
							 	tempTitle=file.replaceFirst("mkv", "jpg");
							 else
		            			 tempTitle=file.replaceFirst("mp4", "jpg");
		            	tempTitle=tempTitle.replaceFirst("LOCK_", "");
						urlString1="http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Locked/."+tempTitle;
					}else if(mSelectedMediaType==InfotmCamera.MEDIA_PHOTO){
						urlString="http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Photo/"+file;
					}
				Toast.makeText(FolderBrowserActivity.this, "开始下载"+file, 2000).show();
				mDownloadTask = new DownloadTask(FolderBrowserActivity.this,file) ;
				mDownloadTask.execute(new URL(urlString)) ;
				if(!"".equals(urlString1)){
					new DownloadTask(FolderBrowserActivity.this,file).execute(new URL(urlString1)) ;
				}

				}catch(Exception e){
					e.printStackTrace();
				}
				
                destroyOperateDialog();
			}
		});
    	mOperateDialog.findViewById(R.id.delete_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				String type="Video";
				if(mSelectedMediaType==InfotmCamera.MEDIA_VIDEO){
					type="Video";
			}else if(mSelectedMediaType==InfotmCamera.MEDIA_VIDEO_LOCKED){
				type="Locked";
			}else if(mSelectedMediaType==InfotmCamera.MEDIA_PHOTO){
				type="Photo";
			}
				final String typeset=type;
                mCamera.sendCommand(new InfotmCamera.CameraOperation() {
                    @Override
                    public boolean execute() {
       				String cmd = CommandSender.commandStringBuilder(InfotmCamera.PIN, 
                                mCamera.getNextCmdIndex(), "action", 1, 
                                "action.file.delete"+":select;;"+typeset+";;"+file +";;");
                        return mCamera.mCommandSender.sendCommand(cmd);
                    }
                }, handler);
                mFileHandler.sendEmptyMessage(WHAT_REQUEST_LIST);
                destroyOperateDialog();
			}
			
		});
    	
    	
    	mOperateDialog.findViewById(R.id.operate_cancel).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				destroyOperateDialog();
			}
		});
    }
    
    public void destroyOperateDialog(){
    	if(mOperateDialog!=null){
    		mOperateDialog.dismiss();
    	}
    }
    
    
    
    //download file
    
    
    private Handler handler = new Handler(){
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case CameraOperation.WHAT_FAIL:
                	Toast.makeText(FolderBrowserActivity.this,  R.string.file_delete_fail, 2000).show();
                    break;
                case CameraOperation.WHAT_SUCCESS:
                	Toast.makeText(FolderBrowserActivity.this,   R.string.file_delete_success, 2000).show();
                    break;
                default:
                    break;
            }
        };
    };
    
}
