package com.infotm.dv;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import com.danale.video.sdk.Danale;
import com.danale.video.sdk.device.constant.DeviceCmd;
import com.danale.video.sdk.device.entity.Connection;
import com.danale.video.sdk.device.entity.LanDevice;
import com.danale.video.sdk.device.handler.DeviceResultHandler;
import com.danale.video.sdk.device.result.DeviceResult;
import com.danale.video.sdk.device.result.SearchLanDeviceResult;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.infotm.dv.camera.InfotmCamera;
import com.infotm.dv.connect.NetService;

public class DeviceListLocalActivity extends Activity implements DeviceResultHandler{
	
	private App app;
	private Context context;
	private Button button;
	private ListView listView;
	
	private List<LanDevice> deviceList;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		app = (App) getApplication();
		context = this;
		
		setContentView(R.layout.activity_device_list_local);

		listView = (ListView) findViewById(R.id.lv_local);
		listView.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> adapter, View view, int position, long id) {
				LanDevice device = deviceList.get(position);
				String deviceId=device.getDeviceId();
				Danale.getSession().addDevice(0, deviceId, deviceId, new PlatformResultHandler() {
			
					@Override
					public void onSuccess(PlatformResult result) {
						Toast.makeText(DeviceListLocalActivity.this,R.string.UID_add_ok_refresh, Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult result, int errorCode) {
						if (errorCode == 1208 || errorCode == 203){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_not_exist, Toast.LENGTH_SHORT).show();
						}else if (errorCode == 204){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_add_by_you, Toast.LENGTH_SHORT).show();
						}else if (errorCode == 1207){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_add_by_other, Toast.LENGTH_SHORT).show();
						}else if (errorCode == 1215){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_not_inline, Toast.LENGTH_SHORT).show();
						}else{
							Toast.makeText(DeviceListLocalActivity.this, getString(R.string.UID_add_fail)+errorCode, Toast.LENGTH_SHORT).show();
						}
					}

					@Override
					public void onOtherFailure(PlatformResult arg0,
							HttpException arg1) {
						// TODO Auto-generated method stub
						Toast.makeText(DeviceListLocalActivity.this,R.string.UID_add_fail_netwrok, Toast.LENGTH_SHORT).show();
					}
				});
				Intent intent = new Intent(context, MainActivity.class);
				intent.putExtra("isPlatformDevice", false);
				intent.putExtra("deviceId", device.getDeviceId());
				intent.putExtra("deviceIp", device.getIp());
				startActivity(intent);
				InfotmCamera.apIP = device.getIp();
		       // startService(new Intent(getApplicationContext(), NetService.class));
				finish();
				
			}
		});
		
		button = (Button) findViewById(R.id.btn_local_refresh);
		button.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				searchLanDevice();
			}
		});
		
		searchLanDevice();
	}
	
	private void searchLanDevice(){
		Log.i("DeviceListLocalActivity","searchLanDevice");
		Connection.searchLanDevice(0, DeviceType.ALL, this);
		 button.setText(R.string.device_search);
		button.setEnabled(false);
	}

	@Override
	protected void onDestroy() {
		
		// 销毁所有连接
		for (LanDevice device: deviceList){
			device.getConnection().destroy(0, null);
		}
		app.setLanDeviceList(null);

		super.onDestroy();
	}

	@Override
	public void onSuccess(DeviceResult result) {
		if (result.getRequestCommand() == DeviceCmd.searchLanDevice){
			SearchLanDeviceResult ret = (SearchLanDeviceResult) result;
			deviceList = ret.getLanDeviceList();
			app.setLanDeviceList(deviceList);
			Toast.makeText(context, getResources().getString(R.string.search_result)+deviceList.size()+getResources().getString(R.string.search_count), Toast.LENGTH_SHORT).show();
			
			List<String> deviceNames = new ArrayList<String>();
			for (LanDevice device: deviceList){
				String name = device.getIp()+":"+device.getDeviceId();
				deviceNames.add(name);
			}
			ArrayAdapter<String> adapter = new ArrayAdapter<String>(context, android.R.layout.simple_list_item_1, deviceNames);
			listView.setAdapter(adapter);
			if(deviceList.size()==0)
		    {
				try {
	                Thread.sleep(1000);
	            } catch (InterruptedException e) {
	                // TODO Auto-generated catch block
	                e.printStackTrace();
	            }
				searchLanDevice();
		    }else if(deviceList.size()==1)
			{
				LanDevice device=deviceList.get(0);
				String deviceId=device.getDeviceId();
				Danale.getSession().addDevice(0, deviceId, deviceId, new PlatformResultHandler() {
			
					@Override
					public void onSuccess(PlatformResult result) {
						Toast.makeText(DeviceListLocalActivity.this,R.string.UID_add_ok_refresh, Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult result, int errorCode) {
						if (errorCode == 1208 || errorCode == 203){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_not_exist, Toast.LENGTH_SHORT).show();
						}else if (errorCode == 204){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_add_by_you, Toast.LENGTH_SHORT).show();
						}else if (errorCode == 1207){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_add_by_other, Toast.LENGTH_SHORT).show();
						}else if (errorCode == 1215){
							Toast.makeText(DeviceListLocalActivity.this, R.string.UID_not_inline, Toast.LENGTH_SHORT).show();
						}else{
							Toast.makeText(DeviceListLocalActivity.this, getString(R.string.UID_add_fail)+errorCode, Toast.LENGTH_SHORT).show();
						}
					}

					@Override
					public void onOtherFailure(PlatformResult arg0,
							HttpException arg1) {
						// TODO Auto-generated method stub
						Toast.makeText(DeviceListLocalActivity.this,R.string.UID_add_fail_netwrok, Toast.LENGTH_SHORT).show();
					}
				});
				Intent intent = new Intent(context, MainActivity.class);
				intent.putExtra("isPlatformDevice", false);
				intent.putExtra("deviceId", device.getDeviceId());
				intent.putExtra("deviceIp", device.getIp());
				startActivity(intent);
				InfotmCamera.apIP = device.getIp();
		       // startService(new Intent(getApplicationContext(), NetService.class));
				finish();
			}else
			{
			   button.setText(R.string.device_refresh);
			   button.setEnabled(true);
			}
		}
	}
	
	@Override
	public void onFailure(DeviceResult result, int errorCode) {
		if (result.getRequestCommand() == DeviceCmd.searchLanDevice){
			// TODO “搜索局域网设备”失败的处理
	        Toast.makeText(context, getResources().getString(R.string.search_noresult) +errorCode, Toast.LENGTH_SHORT).show();
			//button.setEnabled(true);
	        searchLanDevice();
		}
	}

	
}
