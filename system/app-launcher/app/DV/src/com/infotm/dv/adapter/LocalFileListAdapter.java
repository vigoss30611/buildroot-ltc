package com.infotm.dv.adapter;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

import com.infotm.dv.FolderBrowserActivity;
import com.infotm.dv.R;
import com.infotm.dv.camera.InfotmCamera;
import com.nostra13.universalimageloader.core.ImageLoader;
   
public class LocalFileListAdapter extends BaseAdapter implements OnClickListener {
	   public  static final  int REFLASH_SELECT_ALL_UI = 100;
        private Context mContext;
        private LayoutInflater inflater;
        private List<String> mData;
        private int mType;
        
        public boolean isSelectMode=false;
        public boolean isSelectAll=false;
        public List<String>  mSelectData=new ArrayList<String>();
        private Handler mhandler;
        private String mDeviceName;
        
        public LocalFileListAdapter(Context mContext,List<String> data,int type,Handler handler,String deviceName) {
            this.mContext = mContext;
            inflater = LayoutInflater.from(mContext);
            mData=data;
            mType=type;
            mhandler=handler;
            mDeviceName=deviceName;
        }
        
        public void addAllSelectData(){
        	mSelectData.addAll(mData);
        }
        
        @Override
        public int getCount() {
            return mData.size();
        }

        @Override
        public Object getItem(int position) {
            return mData.get(position);
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder viewHolder = null;
            if(convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.folder_item2_layout, null);
                viewHolder.img = (ImageView) convertView.findViewById(R.id.text1);
                viewHolder.title = (TextView) convertView.findViewById(R.id.text2);
                viewHolder.box = (CheckBox) convertView.findViewById(R.id.text3);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            final String title=mData.get(position);
			String tempTitle=null;
            	if(mType==0){
            	viewHolder.img.setImageResource(R.drawable.file_icon);
            	}else  if(mType==1){
            	   if(title.contains(".mkv"))
	            	{
	            		tempTitle=title.replaceFirst("mkv", "jpg");
	            	}else
	            	{
	            		tempTitle=title.replaceFirst("mp4", "jpg");
	            	}
                	ImageLoader.getInstance().displayImage("file://"+Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +mDeviceName+File.separator +"Video"+File.separator +"."+tempTitle, viewHolder.img,FolderBrowserActivity.options);
                	viewHolder.img.setImageResource(R.drawable.ic_video);
            	}else if(mType==2){
                	if(title.contains(".mkv"))
	            	{
	            		tempTitle=title.replaceFirst("mkv", "jpg");
	            	}else
	            	{
	            		tempTitle=title.replaceFirst("mp4", "jpg");
	            	}
                	tempTitle=tempTitle.replaceFirst("LOCK_", "");
                	ImageLoader.getInstance().displayImage("file://"+Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +mDeviceName+File.separator +"Locked"+File.separator+"."+tempTitle, viewHolder.img,FolderBrowserActivity.options);
                	viewHolder.img.setImageResource(R.drawable.ic_video_lock);
                }else{
                	ImageLoader.getInstance().displayImage("file://"+Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +mDeviceName+File.separator +"Photo"+File.separator +title, viewHolder.img,FolderBrowserActivity.options);
                	//viewHolder.img.setImageResource(R.drawable.ic_photo);
                }
            

            viewHolder.title.setText(title);
            if(isSelectMode){
                viewHolder.box.setVisibility(View.VISIBLE);

                final boolean checked=mSelectData.contains(title);
                if(checked){
             	   viewHolder.box.setChecked(true);
                }else{
             	   viewHolder.box.setChecked(false);
                }
                viewHolder.box.setOnClickListener(new OnClickListener() {
					
					@Override
					public void onClick(View arg0) {
						// TODO Auto-generated method stub
						if(!checked){
						    mSelectData.add(title);
							if(mSelectData.size()==mData.size()){
								isSelectAll=true;
								mhandler.sendEmptyMessage(REFLASH_SELECT_ALL_UI);
							}
						}else{
							mSelectData.remove(title);
							isSelectAll=false;
							mhandler.sendEmptyMessage(REFLASH_SELECT_ALL_UI);
						}
					}
				});
             }else {
             	viewHolder.box.setVisibility(View.GONE);
             }
            
            return convertView;
        }
        
        
        
        class ViewHolder {
        	ImageView img;
            TextView title;
            CheckBox box;
        }

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			
		}


}