package com.infotm.dv;

import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;

public class FunctionSettingActivity extends BaseActivity {
    TextView mConnectForward,mConnectConvert,mConnectP2p,mConnectDirect,version_tv;
    int isForward=0;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.function_setting_layout);
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
        isForward=mPref.getInt("connect_mode", 1);
		mConnectForward=(TextView)findViewById(R.id.device_connect_modeconnect_mode_forward);
		mConnectConvert=(TextView)findViewById(R.id.device_connect_modeconnect_mode_convert);
		mConnectP2p=(TextView)findViewById(R.id.device_connect_modeconnect_mode_p2p);
		mConnectDirect=(TextView)findViewById(R.id.device_connect_modeconnect_mode_direct);
		version_tv=(TextView)findViewById(R.id.version_tv);
		if(isForward==1){
			mConnectForward.setBackgroundResource(R.drawable.quality_press);
			mConnectConvert.setBackgroundResource(R.drawable.more_function_selector);
			mConnectP2p.setBackgroundResource(R.drawable.more_function_selector);
			mConnectDirect.setBackgroundResource(R.drawable.more_function_selector);
		}else if(isForward==2){
			mConnectForward.setBackgroundResource(R.drawable.more_function_selector);
			mConnectConvert.setBackgroundResource(R.drawable.more_function_selector);
			mConnectP2p.setBackgroundResource(R.drawable.quality_press);
			mConnectDirect.setBackgroundResource(R.drawable.more_function_selector);
		}else if(isForward==3){
			mConnectForward.setBackgroundResource(R.drawable.more_function_selector);
			mConnectConvert.setBackgroundResource(R.drawable.more_function_selector);
			mConnectP2p.setBackgroundResource(R.drawable.more_function_selector);
			mConnectDirect.setBackgroundResource(R.drawable.quality_press);
		}else{
			mConnectForward.setBackgroundResource(R.drawable.more_function_selector);
			mConnectConvert.setBackgroundResource(R.drawable.quality_press);
			mConnectP2p.setBackgroundResource(R.drawable.more_function_selector);
			mConnectDirect.setBackgroundResource(R.drawable.more_function_selector);
		}
		mConnectP2p.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mConnectForward.setBackgroundResource(R.drawable.more_function_selector);
				mConnectConvert.setBackgroundResource(R.drawable.more_function_selector);
				mConnectDirect.setBackgroundResource(R.drawable.more_function_selector);
				mConnectP2p.setBackgroundResource(R.drawable.quality_press);
				Editor e=mPref.edit();
				e.putInt("connect_mode", 2);
				e.commit();
			}
		});
		mConnectForward.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mConnectForward.setBackgroundResource(R.drawable.quality_press);
				mConnectConvert.setBackgroundResource(R.drawable.more_function_selector);
				mConnectDirect.setBackgroundResource(R.drawable.more_function_selector);
				mConnectP2p.setBackgroundResource(R.drawable.more_function_selector);
				Editor e=mPref.edit();
				e.putInt("connect_mode", 1);
				e.commit();
			}
		});
		mConnectConvert.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mConnectForward.setBackgroundResource(R.drawable.more_function_selector);
				mConnectConvert.setBackgroundResource(R.drawable.quality_press);
				mConnectP2p.setBackgroundResource(R.drawable.more_function_selector);
				mConnectDirect.setBackgroundResource(R.drawable.more_function_selector);
				Editor e=mPref.edit();
				e.putInt("connect_mode", 0);
				e.commit();
			}
		});
		mConnectDirect.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mConnectForward.setBackgroundResource(R.drawable.more_function_selector);
				mConnectConvert.setBackgroundResource(R.drawable.more_function_selector);
				mConnectP2p.setBackgroundResource(R.drawable.more_function_selector);
				mConnectDirect.setBackgroundResource(R.drawable.quality_press);
				Editor e=mPref.edit();
				e.putInt("connect_mode", 3);
				e.commit();
			}
		});
		version_tv.setText("V"+getVersionName());
		
	}
	
	
	public void functionSettingClick(View view){

			switch (view.getId()) {
			case R.id.device_operate_script:
				Intent intent = new Intent(FunctionSettingActivity.this, OperateDescriptionActivity.class);
				startActivity(intent);
				break;
			case R.id.device_ssid_set:
				Intent intent2 = new Intent(FunctionSettingActivity.this, SSIDSettingActivity.class);
				startActivity(intent2);
				break;
			default:
				break;
		}
	}
	

	
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        getActionBar().setDisplayHomeAsUpEnabled(true);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }
    
    public String getVersionName()
    {
            // ��ȡpackagemanager��ʵ��
            PackageManager packageManager = getPackageManager();
            // getPackageName()���㵱ǰ��İ�����0�����ǻ�ȡ�汾��Ϣ
            PackageInfo packInfo=null;
            String version="";
			try {
				packInfo = packageManager.getPackageInfo(getPackageName(),0);
			} catch (NameNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if(null!=packInfo){
				version = packInfo.versionName;
			}
            return version;
    }
}
