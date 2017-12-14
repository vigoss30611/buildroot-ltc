package com.infotm.dv;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class PhoneLocalMediaDeviceActivity extends BaseActivity {

	String deviceName;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_phone_file_browser);
		deviceName=getIntent().getStringExtra("device");
		getActionBar().setTitle(R.string.phone_playback_media_str);
	}
	
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
        }
        return true;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        getActionBar().setDisplayHomeAsUpEnabled(true);
        return super.onCreateOptionsMenu(menu);
    }
    
    
  public void  onFolderSelect(View v){
		Intent intent= new Intent(PhoneLocalMediaDeviceActivity.this, PhoneLocalMediaFileActivity.class) ;
		intent.putExtra("device", deviceName);
	  switch(v.getId()){
	    case R.id.video_folder:
                   intent.putExtra("filename", "Video");
		  break;
	  case R.id.video_lock_folder:
		            intent.putExtra("filename", "Locked");
		  break;
	  case R.id.photo_folder:
		            intent.putExtra("filename", "Photo");
		  break;
	  }  
		startActivity(intent) ;
  }

}
