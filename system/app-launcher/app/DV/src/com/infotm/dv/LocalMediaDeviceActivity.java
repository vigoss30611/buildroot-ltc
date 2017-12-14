package com.infotm.dv;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
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

import com.infotm.dv.adapter.FileListAdapter;
import com.infotm.dv.adapter.LocalFileListAdapter;
import com.infotm.dv.camera.InfotmCamera;

public class LocalMediaDeviceActivity extends BaseActivity {
	private static String TAG="FunctionLocalMediaActivity";
	
	ListView mFileList;
	LocalFileListAdapter mLocalFileListAdapter;
	List<String> fileListName=new ArrayList<String>();;
	File appDir ;
	String deviceName;
	private Menu mMenu;
	private Handler mHandler;
	private CheckBox selectAllCB;
	private RelativeLayout selectAllRl;
	private TextView mNoMediaDataView;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.function_locat_media_layout);
		deviceName=getIntent().getStringExtra("device");
		getActionBar().setTitle(deviceName);
		mNoMediaDataView=(TextView) findViewById(R.id.no_media_data);
		scanFile();
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
		mFileList=(ListView) findViewById(R.id.local_file_list);
		mLocalFileListAdapter=new LocalFileListAdapter(this, fileListName,0,mHandler,null);
		mFileList.setAdapter(mLocalFileListAdapter);
		
		mFileList.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View arg1, int position,
					long arg3) {
				// TODO Auto-generated method stub
                String selectedFile = (String) ((LocalFileListAdapter)parent.getAdapter()).getItem(position);
                Log.d(TAG, "selected file: " + selectedFile);
                
    			Intent intent= new Intent(LocalMediaDeviceActivity.this, LocalMediaFileActivity.class) ;
    			intent.putExtra("device", deviceName);
    			intent.putExtra("filename", selectedFile);
    			startActivity(intent) ;
			}
		});
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
		appDir = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName) ;
		String[] fileName=appDir.list();
		fileListName.clear();
		for(int i=0;i<fileName.length;i++){
			Log.i(TAG, fileName[i]);
			fileListName.add(fileName[i]);
		}
		if(fileListName.size()>0){
			mNoMediaDataView.setVisibility(View.GONE);
		}else{
			mNoMediaDataView.setVisibility(View.VISIBLE);
		}
    }
	private void deleteMediaFile(String fileName) {
		// TODO Auto-generated method stub
		File file=new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +deviceName+File.separator +fileName);
		deleteDir(file);
		scanFile();
		mLocalFileListAdapter.notifyDataSetChanged();
	}
	
	    private static boolean deleteDir(File dir) {
	        if (dir.isDirectory()) {
	            String[] children = dir.list();
	            for (int i=0; i<children.length; i++) {
	                boolean success = deleteDir(new File(dir, children[i]));
	                if (!success) {
	                    return false;
	                }
	            }
	        }
	        // 目录此时为空，可以删除
	        return dir.delete();
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
