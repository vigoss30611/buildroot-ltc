package com.infotm.dv;
    import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.widget.ImageView;
import android.widget.TextView;
      
    /** 
     * 电子罗盘 方向传感器 
     */  
    public class CompassActivity extends Activity implements SensorEventListener {  
        private ImageView imageView;  
        private TextView tvDegree;
        private float currentDegree = 0f;  
      
        public void onCreate(Bundle savedInstanceState) {  
            super.onCreate(savedInstanceState);  
            setContentView(R.layout.compass_layout);  
            imageView = (ImageView) findViewById(R.id.compass_imageView);  
            tvDegree=(TextView) findViewById(R.id.text_or);
      
            // 传感器管理器  
            SensorManager sm = (SensorManager) getSystemService(SENSOR_SERVICE);  
            // 注册传感器(Sensor.TYPE_ORIENTATION(方向传感器);SENSOR_DELAY_FASTEST(0毫秒延迟);  
            // SENSOR_DELAY_GAME(20,000毫秒延迟)、SENSOR_DELAY_UI(60,000毫秒延迟))  
            sm.registerListener(CompassActivity.this,  
                    sm.getDefaultSensor(Sensor.TYPE_ORIENTATION),  
                    SensorManager.SENSOR_DELAY_FASTEST);  
      
        }  
        //传感器报告新的值(方向改变)  
        public void onSensorChanged(SensorEvent event) {  
            if (event.sensor.getType() == Sensor.TYPE_ORIENTATION) {  
                float degree = event.values[0];  
                  int degreeInt=(int)degree;
                  if(degreeInt<=20||degreeInt>=340){
                	  tvDegree.setText("北 "+degreeInt);
                  }else if(degreeInt>20&&degreeInt<70){
                	  tvDegree.setText("东北  "+degreeInt);
                  }else if(degreeInt>=70&&degreeInt<=110){
                	  tvDegree.setText("东  "+degreeInt);
                  }else if(degreeInt>110&&degreeInt<160){
                	  tvDegree.setText("东南  "+degreeInt);
                  }else if(degreeInt>=160&&degreeInt<=200){
                	  tvDegree.setText("南  "+degreeInt);
                  }else if(degreeInt>200&&degreeInt<250){
                	  tvDegree.setText("西南  "+degreeInt);
                  }else if(degreeInt>=250&&degreeInt<=290){
                	  tvDegree.setText("西  "+degreeInt);
                  }else if(degreeInt>290&&degreeInt<340){
                	  tvDegree.setText("西北  "+degreeInt);
                  }

                RotateAnimation ra = new RotateAnimation(currentDegree, -degree,  
                        Animation.RELATIVE_TO_SELF, 0.5f,  
                        Animation.RELATIVE_TO_SELF, 0.5f);  
                //旋转过程持续时间  
                ra.setDuration(200);  
                //罗盘图片使用旋转动画  
                imageView.startAnimation(ra);  
                  
                currentDegree = -degree;  
            }  
        }  
        //传感器精度的改变  
        public void onAccuracyChanged(Sensor sensor, int accuracy) {  
      
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




