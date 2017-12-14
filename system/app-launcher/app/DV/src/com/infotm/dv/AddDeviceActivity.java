package com.infotm.dv;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.danale.video.sdk.Danale;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.AddType;
import com.danale.video.sdk.platform.entity.DeviceAddState;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.GetDeviceAddStateResult;

public class AddDeviceActivity extends Activity implements OnClickListener{

	private EditText et_add_device_id, et_add_device_name;
	private Button btn_add_device_state, btn_add_device_add;
	private TextView tv_add_device_content;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		findViews();
		setListener();
	}
	
	private void findViews(){
		setContentView(R.layout.activity_add_device);
		et_add_device_id = (EditText) findViewById(R.id.et_add_device_id);
		et_add_device_name = (EditText) findViewById(R.id.et_add_device_name);
		btn_add_device_state = (Button) findViewById(R.id.btn_add_device_state);
		btn_add_device_add = (Button) findViewById(R.id.btn_add_device_add);
		tv_add_device_content = (TextView) findViewById(R.id.tv_add_device_content);
	}
	
	private void setListener(){
		btn_add_device_state.setOnClickListener(this);
		btn_add_device_add.setOnClickListener(this);
	}

	@Override
	public void onClick(View v) {
		if (v == btn_add_device_state){
			onClickCheckAddState();
		}else if (v == btn_add_device_add){
			onClickAddDevice();
		}
	}
	
	private void onClickCheckAddState(){
		String deviceId = et_add_device_id.getText().toString();
		if (deviceId.length() != 32){
			Toast.makeText(this, R.string.UID_ERROR, Toast.LENGTH_SHORT).show();
			return;
		}
		
		List<String> deviceIds = new ArrayList<String>();
		deviceIds.add(deviceId);
		Danale.getSession().getDeviceAddState(0, deviceIds, 1, 1, new PlatformResultHandler() {
			
			@Override
			public void onSuccess(PlatformResult result) {
				GetDeviceAddStateResult ret = (GetDeviceAddStateResult) result;
				DeviceAddState state = ret.getDeviceState().get(0);
				
				StringBuilder sb = new StringBuilder();
				sb.append(state.getDeviceId()).append('\n');
				sb.append(state.getOnlineType()).append('\n');
				if (state.getAddType() == AddType.NO_ADDED){
					sb.append(R.string.UID_not_add).append('\n');
				}else{
					sb.append(R.string.UID_have_add).append('\n');
					sb.append(state.getOwnerName()).append('\n');
					sb.append(state.getOwnerLikeName()).append('\n');
				}
				tv_add_device_content.setText(sb.toString());
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult result, int errorCode) {
				if (errorCode == 1208 || errorCode == 203){
					Toast.makeText(AddDeviceActivity.this, R.string.UID_not_exist, Toast.LENGTH_SHORT).show();
				}else{
					Toast.makeText(AddDeviceActivity.this, getString(R.string.check_fail)+errorCode, Toast.LENGTH_SHORT).show();
				}
			}
			
			@Override
			public void onOtherFailure(PlatformResult result, HttpException e) {
				Toast.makeText(AddDeviceActivity.this,R.string.check_fail_netwrok, Toast.LENGTH_SHORT).show();
			}
		});
	}

	private void onClickAddDevice(){
		String deviceId = et_add_device_id.getText().toString();
		String likeName = et_add_device_name.getText().toString();
		if (deviceId.length() != 32){
			Toast.makeText(this,R.string.UID_ERROR, Toast.LENGTH_SHORT).show();
			return;
		}
		
		Danale.getSession().addDevice(0, deviceId, likeName, new PlatformResultHandler() {
			
			@Override
			public void onSuccess(PlatformResult result) {
				Toast.makeText(AddDeviceActivity.this,R.string.UID_add_ok_refresh, Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult result, int errorCode) {
				if (errorCode == 1208 || errorCode == 203){
					Toast.makeText(AddDeviceActivity.this, R.string.UID_not_exist, Toast.LENGTH_SHORT).show();
				}else if (errorCode == 204){
					Toast.makeText(AddDeviceActivity.this, R.string.UID_add_by_you, Toast.LENGTH_SHORT).show();
				}else if (errorCode == 1207){
					Toast.makeText(AddDeviceActivity.this, R.string.UID_add_by_other, Toast.LENGTH_SHORT).show();
				}else if (errorCode == 1215){
					Toast.makeText(AddDeviceActivity.this, R.string.UID_not_inline, Toast.LENGTH_SHORT).show();
				}else{
					Toast.makeText(AddDeviceActivity.this, getString(R.string.UID_add_fail)+errorCode, Toast.LENGTH_SHORT).show();
				}
			}
			
			@Override
			public void onOtherFailure(PlatformResult result, HttpException e) {
				Toast.makeText(AddDeviceActivity.this,R.string.UID_add_fail_netwrok, Toast.LENGTH_SHORT).show();
			}
		});
	}
	
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == 0 && resultCode == 0 && data != null){
			String deviceId = data.getStringExtra("deviceId");
			et_add_device_id.setText(deviceId);
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
}
