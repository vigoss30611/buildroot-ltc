package com.infotm.dv;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import com.danale.video.sdk.platform.entity.Session;
import com.infotm.dv.adapter.ImagePaperAdapter;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;


public class FunctionActivity extends Activity {
	private ViewPager mViewPaper;
	private ImagePaperAdapter mAdapter;
	private ArrayList<ImageView> imageList=new ArrayList<ImageView>();
	private LayoutInflater mInflater;
	private boolean misScrolled;
	private ImageView[]  mBottomImages;
	private LinearLayout mDotLi;
	private int mCurrIndex;
	private Timer timer;
	private SharedPreferences mPref;
	//��ʱ�ֲ�ͼƬ����Ҫ�����߳������޸� UI
	private Handler mHandler = new Handler() {
	    public void handleMessage(Message msg) {
	        switch (msg.what) {
	            case 1:
	                if (msg.arg1 != 0) {
	                	mViewPaper.setCurrentItem(msg.arg1);
	                } else {
	                    //false ����ĩҳ������ҳ�ǣ�����ʾ��ҳ����Ч����
	                	mViewPaper.setCurrentItem(msg.arg1, false);
	                }
	                break;
	        }
	    }
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.function_layout);
		mInflater=LayoutInflater.from(this);
		mViewPaper=(ViewPager) findViewById(R.id.advertise_show);
		mDotLi= (LinearLayout) findViewById(R.id.dot_li);
		ImageView imge1=(ImageView) mInflater.inflate(R.layout.image_layout, null);
		imge1.setImageDrawable(getResources().getDrawable(R.drawable.top_bg));
		imageList.add(imge1);
		ImageView imge2=(ImageView) mInflater.inflate(R.layout.image_layout, null);
		imge2.setImageDrawable(getResources().getDrawable(R.drawable.top_bg1));
		imageList.add(imge2);
		mAdapter=new ImagePaperAdapter(imageList);
		mViewPaper.setAdapter(mAdapter);
		mViewPaper.setOnPageChangeListener(new OnPageChangeListener() {
			
			@Override
			public void onPageSelected(int position) {
				// TODO Auto-generated method stub
                // һ������ͼƬ������Բ�㣬��ע���Ǵ�0��ʼ��
                int total = mBottomImages.length;
                for (int j = 0; j < total; j++) {
                    if (j == position) {
                        mBottomImages[j].setBackgroundResource(R.drawable.dot_focused);
                    } else {
                        mBottomImages[j].setBackgroundResource(R.drawable.dot_normal);
                    }
                }

                //����ȫ�ֱ�����currentIndexΪѡ��ͼ��� index
                mCurrIndex = position;
			}
			
			@Override
			public void onPageScrolled(int arg0, float arg1, int arg2) {
				// TODO Auto-generated method stub
			}
			
			@Override
			public void onPageScrollStateChanged(int state) {
				// TODO Auto-generated method stub
				switch (state) {
				case ViewPager.SCROLL_STATE_DRAGGING:
					misScrolled=false;
					break;
				case ViewPager.SCROLL_STATE_SETTLING:
					misScrolled=true;
					break;
				case ViewPager.SCROLL_STATE_IDLE:
					if (mViewPaper.getCurrentItem() == mViewPaper.getAdapter().getCount() - 1 && !misScrolled) {
						
					}
					misScrolled=true;
					break;					
				default:
					break;
				}
				
			}
		});
		
		mBottomImages = new ImageView[imageList.size()];
        for (int i = 0; i < mBottomImages.length; i++){
            ImageView imageView = new ImageView(FunctionActivity.this);
            LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(20, 20);
            params.setMargins(5, 0, 5, 0);
            imageView.setLayoutParams(params);
            if (i == 0) {
                imageView.setBackgroundResource(R.drawable.dot_focused);
            } else {
                imageView.setBackgroundResource(R.drawable.dot_normal);
            }
            mBottomImages[i] = imageView;
            //��ָʾ���õ�ԭ��ͼƬ����ײ�����ͼ��
            mDotLi.addView(mBottomImages[i]);
        
        }
        
        // �����Զ��ֲ�ͼƬ��5s��ִ�У�������5s
                timer=new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        Message message = new Message();
                        message.what = 1;
                        if (mCurrIndex == mBottomImages.length - 1) {
                        	mCurrIndex = -1;
                        }
                        message.arg1 = mCurrIndex + 1;
                        mHandler.sendMessage(message);
                    }
                }, 5000, 5000);
	}
	
	public void functionClick(View v){
		switch (v.getId()) {
		case R.id.function_record:
			mPref = PreferenceManager.getDefaultSharedPreferences(this);
			int isForward=mPref.getInt("connect_mode", 1);
			Intent intent1;
			if(isForward==2)
			{
			    intent1 = new Intent(FunctionActivity.this, LoginActivity.class) ;
			    startActivity(intent1) ;
			}
			else if(isForward==3)
			{
				intent1 = new Intent(FunctionActivity.this, UsbTetherSettings.class) ;
			    startActivity(intent1) ;
			}
			else
		    {
				 intent1 = new Intent(FunctionActivity.this, ConnectActivity.class) ;
				 startActivity(intent1) ;
			}
			
			break;
		case R.id.function_setting:
			Intent intent2 = new Intent(FunctionActivity.this, FunctionSettingActivity.class) ;
			startActivity(intent2) ;
			break;
		case R.id.function_phone_media:
			Intent intent3= new Intent(FunctionActivity.this, FunctionLocalMediaActivity.class) ;
			startActivity(intent3) ;
		break;
		case R.id.function_phone_compass:
			Intent intent4= new Intent(FunctionActivity.this, CompassActivity.class) ;
			startActivity(intent4) ;
		break;  
		case R.id.function_phone_gpsspeed:
			Intent intent5= new Intent(FunctionActivity.this, GPSSpeedActivity.class) ;
			startActivity(intent5) ;
		break;
		case R.id.function_phone_roadtrack:
			Intent intent6= new Intent(FunctionActivity.this, RoadTrackActivity.class) ;
			startActivity(intent6) ;
		break;
		
		default:
			break;
		}
	}
	
}
