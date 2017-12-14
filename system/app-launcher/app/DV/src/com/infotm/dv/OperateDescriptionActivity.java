package com.infotm.dv;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import com.infotm.dv.adapter.DescripterPaperAdapter;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 * 
 * @see SystemUiHider
 */
public class OperateDescriptionActivity extends Activity {
	private ViewPager mViewPaper;
	private DescripterPaperAdapter mAdapter;
	private ArrayList<LinearLayout> imageList=new ArrayList<LinearLayout>();
	private LayoutInflater mInflater;
	private boolean misScrolled;
	private ImageView[]  mBottomImages;
	private LinearLayout mDotLi;
	private int mCurrIndex;
	private Timer timer;
	
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
		mInflater=LayoutInflater.from(this);
		setContentView(R.layout.operate_description_layout);
		mViewPaper=(ViewPager) findViewById(R.id.advertise_show);
		mDotLi= (LinearLayout) findViewById(R.id.dot_li);
		LinearLayout imge1=(LinearLayout) mInflater.inflate(R.layout.descripter_layout1, null);
		imageList.add(imge1);
		LinearLayout imge2=(LinearLayout) mInflater.inflate(R.layout.descripter_layout2, null);
		imageList.add(imge2);
		LinearLayout imge3=(LinearLayout) mInflater.inflate(R.layout.descripter_layout3, null);
		imageList.add(imge3);
		LinearLayout imge4=(LinearLayout) mInflater.inflate(R.layout.descripter_layout4, null);
		imageList.add(imge4);
		LinearLayout imge5=(LinearLayout) mInflater.inflate(R.layout.descripter_layout5, null);
		imageList.add(imge5);
		LinearLayout imge6=(LinearLayout) mInflater.inflate(R.layout.descripter_layout6, null);
		imageList.add(imge6);
		LinearLayout imge7=(LinearLayout) mInflater.inflate(R.layout.descripter_layout7, null);
		imageList.add(imge7);
		LinearLayout imge8=(LinearLayout) mInflater.inflate(R.layout.descripter_layout8, null);
		imageList.add(imge8);
		/*		ImageView imge2=(ImageView) mInflater.inflate(R.layout.image_layout, null);
		imge2.setImageDrawable(getResources().getDrawable(R.drawable.advertise_2));
		imageList.add(imge2);
		ImageView imge3=(ImageView) mInflater.inflate(R.layout.image_layout, null);
		imge3.setImageDrawable(getResources().getDrawable(R.drawable.advertise_3));
		imageList.add(imge3);*/
		mAdapter=new DescripterPaperAdapter(imageList);
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
							//Toast.makeText(OperateDescriptionActivity.this, "end!", 3000).show();
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
            ImageView imageView = new ImageView(OperateDescriptionActivity.this);
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
/*        timer=new Timer();
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
        }, 5000, 5000);*/
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

}
