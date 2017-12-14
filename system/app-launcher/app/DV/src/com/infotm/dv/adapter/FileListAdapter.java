package com.infotm.dv.adapter;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import com.infotm.dv.FolderBrowserActivity;
import com.infotm.dv.R;
import com.infotm.dv.R.drawable;
import com.infotm.dv.R.id;
import com.infotm.dv.R.layout;
import com.infotm.dv.camera.InfotmCamera;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ImageView;
import android.widget.TextView;
   
public class FileListAdapter extends BaseAdapter implements OnClickListener {

        private Context mContext;
        private LayoutInflater inflater;
        private List<String> mData;
        private Handler mhandler;
        private int mMediaType=1;
        public boolean isSelectMode=false;
        public boolean isSelectAll=false;
        public List<String>  mSelectData=new ArrayList<String>();
        public FileListAdapter(Context mContext,List<String> data,int mediaType,Handler handler) {
            this.mContext = mContext;
            inflater = LayoutInflater.from(mContext);
            mMediaType=mediaType;
            mData=data;
            mhandler=handler;
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
                convertView = inflater.inflate(R.layout.folder_item1_layout, null);
                viewHolder.img = (ImageView) convertView.findViewById(R.id.text1);
                viewHolder.title = (TextView) convertView.findViewById(R.id.text2);
                viewHolder.box = (CheckBox) convertView.findViewById(R.id.text3);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            final String title=mData.get(position);
            if(mMediaType==1){
            	//viewHolder.img.setBackgroundResource(R.drawable.ic_video);
            	
            	String tempTitle;
            	if(title.contains(".mkv"))
            	{
            		tempTitle=title.replaceFirst("mkv", "jpg");
            	}else
            	{
            		tempTitle=title.replaceFirst("mp4", "jpg");
            	}
            	ImageLoader.getInstance().displayImage("http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Video/."+tempTitle, viewHolder.img,FolderBrowserActivity.options);
            	//viewHolder.img.setImageResource(R.drawable.ic_video);
            }else if(mMediaType==2){
            	//viewHolder.img.setBackgroundResource(R.drawable.ic_video_lock);
            	String tempTitle;
            	if(title.contains(".mkv"))
            	{
            		tempTitle=title.replaceFirst("mkv", "jpg");
            	}else
            	{
            		tempTitle=title.replaceFirst("mp4", "jpg");
            	}
            	tempTitle=tempTitle.replaceFirst("LOCK_", "");
            	ImageLoader.getInstance().displayImage("http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Locked/."+tempTitle, viewHolder.img,FolderBrowserActivity.options);
            	//viewHolder.img.setImageResource(R.drawable.ic_video_lock);
            }else{
            	//viewHolder.img.setBackgroundResource(R.drawable.ic_photo);
            	ImageLoader.getInstance().displayImage("http://"+InfotmCamera.currentIP+"/mmc/DCIM/100CVR/Photo/"+title, viewHolder.img,FolderBrowserActivity.options);
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
							mhandler.sendEmptyMessage(FolderBrowserActivity.REFLASH_SELECT_ALL_UI);
						}
					}else{
						mSelectData.remove(title);
						isSelectAll=false;
						mhandler.sendEmptyMessage(FolderBrowserActivity.REFLASH_SELECT_ALL_UI);
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