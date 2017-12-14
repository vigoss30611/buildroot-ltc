package com.infotm.dv;

import java.io.File;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ListView;

import com.infotm.dv.R.drawable;
import com.infotm.dv.adapter.FileListAdapter;
import com.infotm.dv.adapter.LocalFileListAdapter;
import com.infotm.dv.camera.InfotmCamera;
import com.nostra13.universalimageloader.cache.disc.naming.Md5FileNameGenerator;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.assist.ImageScaleType;

public class LocalMediaFileActivity extends BaseActivity {
	private static String TAG="FunctionLocalMediaActivity";
	ListView mFileList;
	LocalFileListAdapter mLocalFileListAdapter;
	List<String> fileListName=new ArrayList<String>();;
	File appDir ;
	String deviceName;
	String currentFileName;
	private Menu mMenu;
	private Handler mHandler;
	private CheckBox selectAllCB;
	private RelativeLayout selectAllRl;
	private TextView mNoMediaDataView;
	
	public void init(){
		   ImageLoaderConfiguration config = new ImageLoaderConfiguration.Builder(
				   this).threadPriority(Thread.NORM_PRIORITY)
				   .denyCacheImageMultipleSizesInMemory()
				   .diskCacheSize(250 * 1024 * 1024).diskCacheFileCount(1000)
				   .diskCacheFileNameGenerator(new Md5FileNameGenerator()).build();
				   ImageLoader.getInstance().init(config); 
			initOption();
			ImageLoader.getInstance().init(config);
		}
		
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
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.function_locat_media_layout);
		deviceName=getIntent().getStringExtra("device");
		currentFileName=getIntent().getStringExtra("filename");
		getActionBar().setTitle(currentFileName);
		mNoMediaDataView=(TextView) findViewById(R.id.no_media_data);		
		init();
		selectAllCB=(CheckBox) findViewById(R.id.select_all_cb);
		selectAllCB.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
            	if(mLocalFileListAdapter.isSelectAll){
              	mLocalFileListAdapter.isSelectAll=false;
              	mLocalFileListAdapter.mSelectData.clear();
              	}else{
              		mLocalFileListAdapter.isSelectAll=true;
              		mLocalFileListAdapter.mSelectData.clear();
              		mLocalFileListAdapter.addAllSelectData();
              	}
            	mLocalFileListAdapter.notifyDataSetChanged();
			}
		});
		selectAllRl=(RelativeLayout) findViewById(R.id.select_all_rl);
		mHandler=new Handler(){
			public void handleMessage(android.os.Message msg) {
				 switch (msg.what) {
				 				case LocalFileListAdapter.REFLASH_SELECT_ALL_UI:
				 						if(mLocalFileListAdapter.isSelectAll){
				 								selectAllCB.setChecked(true);
				 							}else{
				 								selectAllCB.setChecked(false);
				 							}
				 						break;
				 					default:
				 							break;
				 }
				 }
						};
		scanFile();
		mFileList=(ListView) findViewById(R.id.local_file_list);
		if("Video".equals(currentFileName)){
		      mLocalFileListAdapter=new LocalFileListAdapter(this, fileListName,1,mHandler,deviceName);
		}else if("Locked".equals(currentFileName)){
			mLocalFileListAdapter=new LocalFileListAdapter(this, fileListName,2,mHandler,deviceName);
		}else{
			mLocalFileListAdapter=new LocalFileListAdapter(this, fileListName,3,mHandler,deviceName);
		}
		mFileList.setAdapter(mLocalFileListAdapter);
		
		mFileList.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View arg1, int position,
					long arg3) {
				// TODO Auto-generated method stub
                String selectedFile = (String) ((LocalFileListAdapter)parent.getAdapter()).getItem(position);
                Log.d(TAG, "selected file: " + selectedFile);
                showMoreDialog(selectedFile);
			}
		}

		);
		
		mFileList.setOnItemLongClickListener(new OnItemLongClickListener() {

			@Override
			public boolean onItemLongClick(AdapterView<?> arg0, View arg1,
					int arg2, long arg3) {
				// TODO Auto-generated method stub
				mLocalFileListAdapter.isSelectMode=true;
				mLocalFileListAdapter.notifyDataSetChanged();
				mMenu.add(Menu.NONE, 1,1, "Delete")
		        .setIcon(R.drawable.menu_delete)
		        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS | MenuItem.SHOW_AS_ACTION_COLLAPSE_ACTION_VIEW);
				selectAllRl.setVisibility(View.VISIBLE);
				selectAllCB.setChecked(false);
				return true;
			}
		});
	}

    //show list item operate dialog
    Dialog mOperateDialog;
    public void showMoreDialog(String selectedFile){
    	final String file=selectedFile;
    	mOperateDialog=new AlertDialog.Builder(this).create();
    	mOperateDialog.show();
    	Window window = mOperateDialog.getWindow();  
    	window.setContentView(R.layout.local_media_operate_dialog_layout);
    	mOperateDialog.findViewById(R.id.open_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				playMediaFile(file);
				destroyOperateDialog();
			}


		});
    	
    	
    	
    	mOperateDialog.findViewById(R.id.send_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				sendMediaFile(file);
				destroyOperateDialog();
			}


		});
    	mOperateDialog.findViewById(R.id.delete_media).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				deleteMediaFile(file);
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
    
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        getActionBar().setDisplayHomeAsUpEnabled(true);
        mMenu=menu;
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
            case 1:
            	for(String fileName: mLocalFileListAdapter.mSelectData){
            		deleteMediaFile( fileName);
            	}
            	onBackPressed() ;
            	break;
            default:

        }
        return super.onOptionsItemSelected(item);
    }
    
    private void  scanFile(){
		appDir = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +currentFileName) ;
		String[] fileName=appDir.list();
		fileListName.clear();
		for(int i=0;i<fileName.length;i++){
			Log.i(TAG, fileName[i]);
			if(!fileName[i].startsWith(".")){
			fileListName.add(fileName[i]);
			}
		}
		if(fileListName.size()>0){
			mNoMediaDataView.setVisibility(View.GONE);
		}else{
			mNoMediaDataView.setVisibility(View.VISIBLE);
		}
    }
    
	private void playMediaFile(String file) {
		// TODO Auto-generated method stub
		Intent intent = new Intent(Intent.ACTION_VIEW);
		Uri uri = Uri.parse("file://"+Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +currentFileName+File.separator +file);
		 Log.d(TAG, " file  path: " + uri.getPath());
		if("Photo".equals(currentFileName)){
			intent.setDataAndType(uri ,"image/*");
		}else{
		intent.setDataAndType(uri ,"video/*");
		}
		startActivity(Intent.createChooser(intent, "打  开"));
	}
	
	private void sendMediaFile(String file) {
		// TODO Auto-generated method stub
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.putExtra(Intent.EXTRA_SUBJECT, "分  享");  
		Uri uri = Uri.parse("file://"+Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +currentFileName+File.separator +file);
		 Log.d(TAG, " file  path: " + uri.getPath());
		if("Photo".equals(currentFileName)){
			intent.setDataAndType(uri ,"image/*");
		}else{
		intent.setDataAndType(uri ,"video/*");
		}
		startActivity(Intent.createChooser(intent, "分享"));
	}
	
	private void deleteMediaFile(String fileName) {
		// TODO Auto-generated method stub
		File file=new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +currentFileName+File.separator +fileName);
		file.delete();
		File file1=null;
		String tempTitle="";
		if("Video".equals(currentFileName)){
			if(fileName.endsWith(".mkv"))
				tempTitle="."+fileName.replaceFirst("mkv", "jpg");
			else
			 tempTitle="."+fileName.replaceFirst("mp4", "jpg");
			file1=new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +currentFileName+File.separator +tempTitle);
		}else if("Locked".equals(currentFileName)){
        	if(fileName.endsWith(".mkv"))
				tempTitle="."+fileName.replaceFirst("mkv", "jpg");
			else
			 tempTitle="."+fileName.replaceFirst("mp4", "jpg");
        	tempTitle="."+tempTitle.replaceFirst("LOCK_", "");
			file1=new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +currentFileName+File.separator +tempTitle);
		}
		if(null!=file1)
		file1.delete();
		scanFile();
		mLocalFileListAdapter.notifyDataSetChanged();
	}
	
    @Override
    public void onBackPressed() {
        // TODO Auto-generated method stub
    	if(mLocalFileListAdapter.isSelectMode){
    		mMenu.removeItem(1);
    		mLocalFileListAdapter.isSelectMode=false;
    		mLocalFileListAdapter.isSelectAll=false;
    		mLocalFileListAdapter.notifyDataSetChanged();
    		mLocalFileListAdapter.mSelectData.clear();
    		selectAllRl.setVisibility(View.GONE);
    		}else{
            super.onBackPressed();
    		}
    }
}
