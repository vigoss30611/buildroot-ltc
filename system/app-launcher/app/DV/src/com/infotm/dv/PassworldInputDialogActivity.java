
package com.infotm.dv;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.InputType;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;


public class PassworldInputDialogActivity extends Activity{
	EditText mPasswordStr;
	TextView mSetPasswordVisible;
	boolean passwordIsVisible=false;
	
	  @Override
	   protected void onCreate(Bundle savedInstanceState) {
		  // TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.device_password_input_dialog_layout);
		mPasswordStr = (EditText) findViewById(R.id.password_et);
	    mSetPasswordVisible = (TextView) findViewById(R.id.password_setvisible);
    	findViewById(R.id.ssid_cancel).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				finish();
			}
		});
    	findViewById(R.id.ssid_ok).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				String password=mPasswordStr.getText().toString();
		    	if(password==null||"".equals(password)){
		    		Toast.makeText(PassworldInputDialogActivity.this, "请输入设备密码!", 2000).show();
		    		return;
		        	}
		    	if(password.length()<8){
		    		Toast.makeText(PassworldInputDialogActivity.this, "请输入8位以上密码!", 2000).show();
		    		return;
		    	}
                Intent intent = new Intent();
                //把返回数据存入Intent
                intent.putExtra("password",password);
                PassworldInputDialogActivity.this.setResult(2, intent);
				finish();
			}
		});
    	mSetPasswordVisible.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(passwordIsVisible){
					passwordIsVisible=false;
					mPasswordStr.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
					mSetPasswordVisible.setBackgroundResource(R.drawable.password_invisible);
				}else{
					passwordIsVisible=true;
					mPasswordStr.setInputType(InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
					mSetPasswordVisible.setBackgroundResource(R.drawable.password_visible);
				}
				
			}
		});
	  }
	  
}
