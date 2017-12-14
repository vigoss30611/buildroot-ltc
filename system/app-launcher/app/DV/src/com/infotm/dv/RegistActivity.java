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
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.UserType;
import com.danale.video.sdk.platform.entity.CountryCode;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.GetCountryCodesResult;

public class RegistActivity extends Activity implements OnClickListener, OnCheckedChangeListener{

	private Context context = this;
	private RadioGroup rg_regist_type;
	private EditText et_regist_username, et_regist_password, et_regist_code;
	private TextView country_code_tv;
	private Button btn_regist_send, btn_regist_ok;
	private CountryCode curCountryCode;
	/**
	 * �û����ͣ��ʼ����ֻ�
	 */
	private UserType userType;
	
	/**
	 * ����ע�����
	 * 		�뿪���߽����޸�Ϊ�Լ��ĳ���ע����룬������������Щ�û���ͨ��ó���ע��ģ�����ͳ��������û�
	 */
	private String manufacturerCode = "";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		findViews();
		setListener();
		initData();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
	}
	
	private void findViews(){
		setContentView(R.layout.activity_regist);
		rg_regist_type = (RadioGroup) findViewById(R.id.rg_regist_type);
		et_regist_username = (EditText) findViewById(R.id.et_regist_username);
		et_regist_password = (EditText) findViewById(R.id.et_regist_password);
		et_regist_code = (EditText) findViewById(R.id.et_regist_code_active);
		btn_regist_send = (Button) findViewById(R.id.btn_regist_send);
		btn_regist_ok = (Button) findViewById(R.id.btn_regist_ok);
		country_code_tv = (TextView) findViewById(R.id.phone_code_tv);
	}
	
	private void setListener(){
		rg_regist_type.setOnCheckedChangeListener(this);
		btn_regist_send.setOnClickListener(this);
		btn_regist_ok.setOnClickListener(this);
		country_code_tv.setOnClickListener(this);
	}
	
	private void initData(){
		rg_regist_type.check(R.id.rb_regist_email);
	}
	
	@Override
	public void onClick(View v) {
		if (v == btn_regist_ok){
			onClickRegist();
		}else if (v == btn_regist_send){
			onClickSendValidation();
		}
	}

	@Override
	public void onCheckedChanged(RadioGroup group, int checkedId) {
		if (checkedId == R.id.rb_regist_email){
			onClickEmail();
		}else if (checkedId == R.id.rb_regist_mobile){
			onClickPhone();
			requestCountryCode();
		}
	}
	
	private void onClickEmail(){
		userType = UserType.EMAIL;
		et_regist_username.setHint(R.string.input_email);
		et_regist_username.setText("");
		country_code_tv.setVisibility(View.GONE);
	}
	
	private void onClickPhone(){
		userType = UserType.PHONE;
		et_regist_username.setHint(R.string.input_phone);
		et_regist_username.setText("");
		country_code_tv.setVisibility(View.VISIBLE);
	}
	
	/**
	 * �ֻ�ע��ʱ��������
	 */
	private void requestCountryCode(){
		Session.getCountryCodes(0, 1, 200,new PlatformResultHandler() {
			
			@Override
			public void onSuccess(PlatformResult arg0) {
				AlertDialog.Builder builder = new AlertDialog.Builder(RegistActivity.this);
				builder.setTitle(R.string.set_country_code);
				
				final List<CountryCode> list = ((GetCountryCodesResult)arg0).getCountryCodeList();
				final String[] items = new String[list.size()];
				for(int i = 0; i < list.size(); i ++){
					items[i] = list.get(i).getCountryCode() + "(" + list.get(i).getPhoneCode() + ")";
				}
				
				builder.setSingleChoiceItems(items, -1, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, final int which) {
						RegistActivity.this.runOnUiThread(new Runnable() {
							
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
				Toast.makeText(RegistActivity.this, R.string.get_country_error, Toast.LENGTH_SHORT).show();
				
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult arg0, int arg1) {
				Toast.makeText(RegistActivity.this, R.string.get_country_error+arg1, Toast.LENGTH_SHORT).show();
				
			}
		});
	}
	
	/**
	 * ����ע����Ϣ����ȡ������
	 */
	private void onClickSendValidation(){
		final String userName = et_regist_username.getText().toString();
		final String password = et_regist_password.getText().toString();
		
		if (userType == UserType.PHONE){
			if (!TextUtils.isDigitsOnly(userName) || userName.length() != 11){
				Toast.makeText(context, R.string.input_phone_error, Toast.LENGTH_SHORT).show();
				return;
			}
			Session.regist(0, userName, password, userType, manufacturerCode,curCountryCode.getPhoneCode(), new PlatformResultHandler() {
				@Override
				public void onSuccess(PlatformResult result) {
					Toast.makeText(context, userName+getString(R.string.activation_code_send), Toast.LENGTH_SHORT).show();
				}
				
				@Override
				public void onCommandExecFailure(PlatformResult result, int errorCode) {
					Toast.makeText(context, getString(R.string.regist_fail_error) + errorCode, Toast.LENGTH_SHORT).show();
				}
				
				@Override
				public void onOtherFailure(PlatformResult result, HttpException e) {
					Toast.makeText(context,R.string.regist_fail_network_exception, Toast.LENGTH_SHORT).show();
					e.printStackTrace();
				}
			});
		}else if (userType == UserType.EMAIL){
			if (!userName.contains("@")){
				Toast.makeText(context, R.string.input_email_fail, Toast.LENGTH_SHORT).show();
				return;
			}
			Session.regist(0, userName, password, userType, manufacturerCode, new PlatformResultHandler() {
				@Override
				public void onSuccess(PlatformResult result) {
					Toast.makeText(context, userName+getString(R.string.activation_code_send), Toast.LENGTH_SHORT).show();
				}
				
				@Override
				public void onCommandExecFailure(PlatformResult result, int errorCode) {
					Toast.makeText(context, getString(R.string.regist_fail_error) + errorCode, Toast.LENGTH_SHORT).show();
				}
				
				@Override
				public void onOtherFailure(PlatformResult result, HttpException e) {
					Toast.makeText(context, R.string.regist_fail_network_exception, Toast.LENGTH_SHORT).show();
					e.printStackTrace();
				}
			});
		}
		
		
		
	}
	
	/**
	 * ȷ�ϼ�����
	 */
	private void onClickRegist(){
		final String userName = et_regist_username.getText().toString();
		final String activeCode = et_regist_code.getText().toString();
		
		if (TextUtils.isEmpty(activeCode)){
			Toast.makeText(context, R.string.input_activation_code, Toast.LENGTH_SHORT).show();
			et_regist_code.requestFocus();
			return;
		}
		
		Session.checkRegist(0, userName, activeCode, new PlatformResultHandler() {
			@Override
			public void onSuccess(PlatformResult result) {
				Toast.makeText(context, userName+getString(R.string.regist_success), Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult result, int errorCode) {
				Toast.makeText(context, getString(R.string.regist_fail_error)+errorCode, Toast.LENGTH_SHORT).show();
			}
			
			@Override
			public void onOtherFailure(PlatformResult result, HttpException e) {
				Toast.makeText(context, R.string.regist_fail_network_exception, Toast.LENGTH_SHORT).show();
				e.printStackTrace();
			}
		});
	}
}
