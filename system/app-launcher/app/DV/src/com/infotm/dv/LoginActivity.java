package com.infotm.dv;


import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.danale.video.comm.entity.AppType;
import com.danale.video.sdk.http.exception.HttpClientException;
import com.danale.video.sdk.http.exception.HttpClientException.ClientException;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.http.exception.HttpException.ExceptionType;
import com.danale.video.sdk.http.exception.HttpNetException;
import com.danale.video.sdk.http.exception.HttpNetException.NetException;
import com.danale.video.sdk.http.exception.HttpServerException;
import com.danale.video.sdk.http.exception.HttpServerException.ServerException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.PlatformCmd;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.LoginResult;

public class LoginActivity extends Activity implements OnClickListener{

	public static final String TAG = "Danale";
	
	private Button btn_login_online;
	//private Button btn_login_local;
	private Button btn_regist;
	private Button btn_find;
	
	private EditText et_username;
	private EditText et_password;
	private SharedPreferences mPref;
	private Context context = this;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		findViews();
		setListener();
		initData();
	}
	
	private void findViews(){
		setContentView(R.layout.activity_login);
		btn_login_online = (Button) findViewById(R.id.btn_login_online);
	//	btn_login_local = (Button) findViewById(R.id.btn_login_local);
		btn_regist = (Button) findViewById(R.id.btn_regist);
		btn_find = (Button) findViewById(R.id.btn_find);
		et_username = (EditText) findViewById(R.id.et_username);
		et_password = (EditText) findViewById(R.id.et_password);
	}
	
	private void setListener(){
		btn_login_online.setOnClickListener(this);
		//btn_login_local.setOnClickListener(this);
		btn_regist.setOnClickListener(this);
		btn_find.setOnClickListener(this);
	}
	
	private void initData(){
		et_username.setText("");
		et_password.setText("");
		mPref = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
		String username=mPref.getString("saved-username", null);
		if(username!=null)
		{
			et_username.setText(username);
		}
		String userpass = mPref.getString("saved-userpass", null);
		if(username!=null)
		{
			et_password.setText(userpass);
		}
	}

	@Override
	public void onClick(View v) {
		if (v == btn_login_online){
			onLoginOnline();
		//}else if (v == btn_login_local){
		//	onClickLoginLocal();
		}
	     else if (v == btn_regist){
			onClickRegist();
		}else if (v == btn_find){
			onClickFind();
		}
	}
	
	// ��¼
	private void onLoginOnline(){
		Toast.makeText(context, R.string.logining, Toast.LENGTH_SHORT).show();
		
		String username = et_username.getText().toString().trim();
		String userpass = et_password.getText().toString().trim();
		
		// ��¼�ӿڣ��첽��
		Session.login(1001, username, userpass, AppType.ANDROID, null, null, new PlatformResultHandler() {
			
			@Override
			public void onSuccess(PlatformResult result) {
				// ����ִ�гɹ�ʱ����
				
				// ������platformResultHandler������ʹ��RequestId�����������Ӧ��ʹ��RequestCommand�����������Ӧ���ָ��
				// ��������platformResultHandler������Ҫ�����κ��жϣ�ֱ�Ӵ��?�ؽ��
				if (result.getRequestId() != 1001 || result.getRequestCommand() != PlatformCmd.login){
					return;
				}
				
				// ǿת����������ýӿ���һ��(Login+Result)
				LoginResult ret = (LoginResult)result;
				
				// ��¼���ȡsession�����ܽ��к�������������
				Session session = ret.getSession();
				Log.d(TAG, "login ok:"+session.getUsername());
				
				 mPref = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
		            Editor mEditor = mPref.edit();
		            mEditor.putString("saved-username", session.getUsername());
		            mEditor.putString("saved-userpass", session.getPassword());
		            mEditor.commit();
		            
				Intent intent = new Intent(context, DeviceListOnlineActivity.class);
				startActivity(intent);
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult result, int errorCode) {
				// ������ִ��ʧ��ʱ���ã���ʱ��������ʧ��ԭ��ͨ��errorCode���أ����磺�˺Ų����ڡ��������ȣ�����errorCodeֵ���ж�
				Toast.makeText(context, getString(R.string.loginfail_name_pass_error)+errorCode, Toast.LENGTH_LONG).show();
			}
			
			@Override
			public void onOtherFailure(PlatformResult result, HttpException exception) {
				
				// �����޷����͵�������ʱ���ã��������类�Ͽ���û�н��뻥��������쳣���ͻ����쳣�������HTTP����500��
				// �����쳣(HttpNetException)�������������쳣(HttpServerException)���ͻ����쳣(HttpClientException)
				
				// ��Ҫ��������쳣���ͣ���Ҫ�������ж�
				ExceptionType type = exception.getType();
				if (type == ExceptionType.net){
					
					HttpNetException httpNetException = (HttpNetException) exception;
					NetException netException = httpNetException.getExceptionType();
					if (netException == NetException.NetworkDisabled){
						Log.d(TAG, getString(R.string.network_disable)+netException.reason);
					}else if (netException == NetException.NetworkError){
						Log.d(TAG, getString(R.string.network_error)+netException.reason);
					}else if (netException == NetException.UnReachable){
						Log.d(TAG, getString(R.string.network_unreach)+netException.reason);
					}
					
					Toast.makeText(context, R.string.network_fail, Toast.LENGTH_SHORT).show();
					
				}else if (type == ExceptionType.server){
					
					
					HttpServerException httpServerException = (HttpServerException) exception;
					ServerException serverException = httpServerException.getExceptionType();
					if (serverException == ServerException.ServerInner){
						Log.d(TAG, getString(R.string.server_error)+serverException.reason);
					}else if (serverException == ServerException.ServerReject){
						Log.d(TAG, getString(R.string.server_no_serve)+serverException.reason);
					}else if (serverException == ServerException.RedirectTooMany){
						Log.d(TAG, getString(R.string.server_redirectTooMany)+serverException.reason);
					}
					
					Toast.makeText(context, getString(R.string.server_fail), Toast.LENGTH_SHORT).show();
					
				}else if (type == ExceptionType.client){
					
					
					HttpClientException httpClientException = (HttpClientException) exception;
					ClientException clientException = httpClientException.getExceptionType();
					if (clientException == ClientException.UrlIsNull){
						Log.d(TAG, getString(R.string.url_error)+clientException.reason+getString(R.string.url_impossible));
					}else if (clientException == ClientException.OtherException){
						Log.d(TAG, getString(R.string.SDK_error)+clientException.reason);
					}
					
					Toast.makeText(context, R.string.cliect_exception, Toast.LENGTH_SHORT).show();
				}
			}
		});
	}
	
	/**
	 * ���ص�¼
	 */
	private void onClickLoginLocal(){
		Intent intent = new Intent(context, DeviceListLocalActivity.class);
		startActivity(intent);
	}
	
	/**
	 * ע��
	 */
	private void onClickRegist(){
		Intent intent = new Intent(context, RegistActivity.class);
		startActivity(intent);
	}
	
	/**
	 * �һ�����
	 */
	private void onClickFind(){
		Intent intent = new Intent(context, FindActivity.class);
		startActivity(intent);
	}
}
