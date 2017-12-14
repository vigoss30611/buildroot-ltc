package com.infotm.dv;

import com.infotm.dv.wifiadapter.WifiApMan;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.widget.FrameLayout;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 * 
 * @see SystemUiHider
 */
public class WelcomeActivity extends Activity {
	private long animateTime = 1000 ;
	private long delayTime = 600 ;
	private Handler mHandler;
	 private WifiManager mWifiManager;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.welcome_layout);
		mHandler=new Handler();
		if (savedInstanceState == null) {
			FrameLayout title = (FrameLayout) findViewById(R.id.welcome) ;
			title.setAlpha(1f) ;
			title.animate().alpha(1f).setDuration(animateTime).setListener(new AnimatorListener() {
				
				@Override
				public void onAnimationStart(Animator arg0) {
					// TODO Auto-generated method stub					
				}
				
				@Override
				public void onAnimationRepeat(Animator arg0) {
					// TODO Auto-generated method stub					
				}
				
				@Override
				public void onAnimationEnd(Animator arg0) {
					// TODO Auto-generated method stub
					mHandler.postDelayed(new Runnable() {
						
						@Override
						public void run() {
							// TODO Auto-generated method stub
							Intent intent = new Intent(WelcomeActivity.this, FunctionActivity.class) ;
							startActivity(intent) ;
							finish();
						}
					}, delayTime);
				}
				
				@Override
				public void onAnimationCancel(Animator arg0) {
					// TODO Auto-generated method stub
					
				}
			}) ;
			
		}
		mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
		stopAp();
	}
	
	public  void stopAp()
	{
		 WifiApMan apMan = new WifiApMan(this) ;
		apMan.setWifiManager(mWifiManager);
		if(apMan.isWifiApEnabled())
		{ 
		
		   apMan.setWifiApEnabled(apMan.getWifiApConfiguration(),false);
		}
		if (!mWifiManager.isWifiEnabled()) 
		{
            mWifiManager.setWifiEnabled(true);
        }
	}

}
