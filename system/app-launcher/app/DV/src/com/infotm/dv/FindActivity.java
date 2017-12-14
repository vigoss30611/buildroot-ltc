package com.infotm.dv;

import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.entity.CountryCode;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.GetCountryCodesResult;

public class FindActivity extends Activity implements OnClickListener{

	private Context context = this;
	private EditText et_find_username, et_find_password, et_find_validation;
	private Button btn_find_send, btn_find_ok;
	private TextView country_code_tv;
	private CountryCode curCountryCode;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		findViews();
		setListener();
	}
	
	private void findViews(){
		setContentView(R.layout.activity_find);
		et_find_username = (EditText) findViewById(R.id.et_find_username);
		et_find_password = (EditText) findViewById(R.id.et_find_password);
		et_find_validation = (EditText) findViewById(R.id.et_find_validation);
		btn_find_send = (Button) findViewById(R.id.btn_find_send);
		btn_find_ok = (Button) findViewById(R.id.btn_find_ok);
		country_code_tv = (TextView) findViewById(R.id.phone_code_tv);
	}
	
	private void setListener(){
		btn_find_send.setOnClickListener(this);
		btn_find_ok.setOnClickListener(this);
		country_code_tv.setOnClickListener(this);
	}
	
	@Override
	public void onClick(View v) {
		if (v == btn_find_ok){
			onClickfind();
		}else if (v == btn_find_send){
			onClickSendValidation();
		}else if(v == country_code_tv){
			
		}
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		requestCountryCode();
	}
	
	
	private void requestCountryCode(){
		Session.getCountryCodes(0, 1, 30,new PlatformResultHandler() {
			
			@Override
			public void onSuccess(PlatformResult arg0) {
				AlertDialog.Builder builder = new AlertDialog.Builder(FindActivity.this);
				builder.setTitle(R.string.set_country_code);
				
				final List<CountryCode> list = ((GetCountryCodesResult)arg0).getCountryCodeList();
				final String[] items = new String[list.size()];
				for(int i = 0; i < list.size(); i ++){
					items[i] = list.get(i).getCountryCode() + "(" + list.get(i).getPhoneCode() + ")";
				}
				
				builder.setSingleChoiceItems(items, -1, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, final int which) {
						FindActivity.this.runOnUiThread(new Runnable() {
							
							@Override
							public void run() {
								country_code_tv.setText(items[which]);
								curCountryCode = list.get(which);
							}
						});
						dialog.dismiss();
					}
				});
				builder.setCancelable(false);
				builder.create().show();
			}
			
			@Override
			public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
				Toast.makeText(FindActivity.this, R.string.get_country_error, Toast.LENGTH_SHORT).show();
				
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult arg0, int arg1) {
				Toast.makeText(FindActivity.this, getString(R.string.get_country_error)+arg1, Toast.LENGTH_SHORT).show();
				
			}
		});
	}
	/**
	 * ��ȡ��֤��
	 */
	private void onClickSendValidation(){
		final String userName = et_find_username.getText().toString();
		if (TextUtils.isEmpty(userName)){
			Toast.makeText(context, R.string.input_name_find, Toast.LENGTH_SHORT).show();
			et_find_username.requestFocus();
			return;
		}
		
		Session.forgetPassword(0, userName, new PlatformResultHandler() {
			@Override
			public void onSuccess(PlatformResult result) {
				Toast.makeText(context, userName+getString(R.string.activation_code_send), Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult result, int errorCode) {
				Toast.makeText(context, getString(R.string.error_code)+errorCode, Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onOtherFailure(PlatformResult result, HttpException e) {
				Toast.makeText(context, R.string.getpwdfail_network_except, Toast.LENGTH_SHORT).show();
				e.printStackTrace();
			}
		});
	}
	
	/**
	 * ȷ����֤�룬���޸�����
	 */
	private void onClickfind(){
		final String userName = et_find_username.getText().toString();
		final String password = et_find_password.getText().toString();
		final String validationCode = et_find_validation.getText().toString();
		
		if (TextUtils.isEmpty(userName)){
			Toast.makeText(context,R.string.input_name, Toast.LENGTH_SHORT).show();
			et_find_username.requestFocus();
			return;
		}
		
		if (TextUtils.isEmpty(validationCode)){
			Toast.makeText(context, R.string.input_validationcode, Toast.LENGTH_SHORT).show();
			et_find_validation.requestFocus();
			return;
		}
		
		if (TextUtils.isEmpty(password)){
			Toast.makeText(context, R.string.input_new_pwd, Toast.LENGTH_SHORT).show();
			et_find_password.requestFocus();
			return;
		}
		
		
		Session.checkForgetPassword(0, userName, password, validationCode,curCountryCode.getPhoneCode(), new PlatformResultHandler() {
			@Override
			public void onSuccess(PlatformResult result) {
				Toast.makeText(context, userName+getString(R.string.new_pwd_set_ok), Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult result, int errorCode) {
				Toast.makeText(context, getString(R.string.error_code)+errorCode, Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onOtherFailure(PlatformResult result, HttpException e) {
				Toast.makeText(context, R.string.new_pwd_set_fail_netwrok_except, Toast.LENGTH_SHORT).show();
				e.printStackTrace();
			}
		});
	}
}
