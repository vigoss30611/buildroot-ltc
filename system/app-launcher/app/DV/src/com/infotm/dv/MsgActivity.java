package com.infotm.dv;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.danale.video.comm.constant.PushMsgType;
import com.danale.video.comm.entity.PushMsg;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.SystemMsgType;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.entity.SystemMsg;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.GetPushMsgListResult;
import com.danale.video.sdk.platform.result.GetSystemMsgListResult;

public class MsgActivity extends Activity implements OnClickListener{
	private static final int SYS_MSG = 1;
	private static final int ALARM_MSG = 2;
	
	private Button sysMsgBtn,alarmMsgBtn;
	private ListView msgListView;
	
	private List<SystemMsg> sysMsgList = new ArrayList<SystemMsg>();
	private List<PushMsg> alarmMsgList = new ArrayList<PushMsg>();
	
	private String devId;
	public static void startActivity(Context fromContext,String deviceId){
		Intent intent = new Intent(fromContext, MsgActivity.class);
		intent.putExtra("device_id", deviceId);
		fromContext.startActivity(intent);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_msg);
		
		sysMsgBtn = (Button) findViewById(R.id.sys_msg_btn);
		alarmMsgBtn = (Button) findViewById(R.id.alarm_msg_btn);
		msgListView = (ListView) findViewById(R.id.msg_content_listview);
		
		devId = getIntent().getStringExtra("device_id");
		
		sysMsgBtn.setOnClickListener(this);
		alarmMsgBtn.setOnClickListener(this);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		switchBtn(SYS_MSG);
	}
	
	private void switchBtn(int msgType){
		if(msgType == SYS_MSG){
			sysMsgBtn.setEnabled(false);
			alarmMsgBtn.setEnabled(true);
		}else if(msgType == ALARM_MSG){
			sysMsgBtn.setEnabled(true);
			alarmMsgBtn.setEnabled(false);
		}
		requestData(msgType);
	}
	
	private void requestData(int msgType){
		if(msgType == SYS_MSG){
			sysMsgList.clear();
			msgListView.setAdapter(new SysMsgAdapter());
			
			//��ȡϵͳ��Ϣ
			Session.getSession().getSystemMsgList(0, devId, 1, 30, new PlatformResultHandler() {
				
				@Override
				public void onSuccess(PlatformResult arg0) {
					sysMsgList = ((GetSystemMsgListResult)arg0).getSystemMsgList();
					if(sysMsgList.size() == 0){
						Toast.makeText(MsgActivity.this, "ϵͳ��Ϣ����Ϊ0", Toast.LENGTH_SHORT).show();
					}else{
						((BaseAdapter)msgListView.getAdapter()).notifyDataSetChanged();
					}
				}
				
				@Override
				public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
					Toast.makeText(MsgActivity.this, "����ϵͳ��Ϣʧ��,�������", Toast.LENGTH_SHORT).show();
					
				}
				
				@Override
				public void onCommandExecFailure(PlatformResult arg0, int arg1) {
					Toast.makeText(MsgActivity.this, "����ϵͳ��Ϣʧ��--������:"+arg1, Toast.LENGTH_SHORT).show();
					
				}
			});
		}else if(msgType == ALARM_MSG){
			alarmMsgList.clear();
			msgListView.setAdapter(new AlarmMsgAdater());
			
			//��ȡ������Ϣ
			Session.getSession().getPushMsgList(0, devId, 1, 30, new PlatformResultHandler() {
				
				@Override
				public void onSuccess(PlatformResult arg0) {
					alarmMsgList = ((GetPushMsgListResult)arg0).getPushMsgList();
					if(alarmMsgList.size() == 0){
						Toast.makeText(MsgActivity.this, "�澯��Ϣ����Ϊ0", Toast.LENGTH_SHORT).show();
					}else{
						((BaseAdapter)msgListView.getAdapter()).notifyDataSetChanged();
					}
				}
				
				@Override
				public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
					Toast.makeText(MsgActivity.this, "���ظ澯��Ϣʧ��,�������", Toast.LENGTH_SHORT).show();
					
				}
				
				@Override
				public void onCommandExecFailure(PlatformResult arg0, int arg1) {
					Toast.makeText(MsgActivity.this, "���ظ澯��Ϣʧ��--������:"+arg1, Toast.LENGTH_SHORT).show();
					
				}
			});
		}
	}
	
	/**
	 * ϵͳ��Ϣ������
	 * @author Administrator
	 *
	 */
	private class SysMsgAdapter extends BaseAdapter{

		@Override
		public int getCount() {
			return sysMsgList.size();
		}

		@Override
		public Object getItem(int position) {
			return sysMsgList.get(position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			SysMsgHolder holder = null;
			if(convertView == null){
				holder = new SysMsgHolder();
				convertView = LayoutInflater.from(MsgActivity.this).inflate(R.layout.item_sys_msg, null);
				holder.sysMsgTv = (TextView) convertView.findViewById(R.id.sys_msg_content_tv);
				convertView.setTag(holder);
			}else{
				holder = (SysMsgHolder) convertView.getTag();
			}
			SystemMsg msg = sysMsgList.get(position);
			//��ô�����Ϣ���û�
			String shareUser = msg.getShareMsg().getUserName();
			String devName = msg.getShareMsg().getDeviceAlias();
			String devId = msg.getShareMsg().getDeviceId();
			long createTime = msg.getCreateTime();
			String content = "";
			//���������Ϣ���
			if(msg.getSystemMsgType() == SystemMsgType.CANEL_SHARE){
				content = shareUser + "����ȡ�����豸" + devName + "(" + devId +")�ķ���,ʱ��("+String.valueOf(createTime)+")";
			}else if(msg.getSystemMsgType() == SystemMsgType.ADD_SHARE){
				content = shareUser + "�������豸" + devName + "(" + devId +")����,ʱ��("+String.valueOf(createTime)+")";
			}
			holder.sysMsgTv.setText(content);
			
			return convertView;
		}
		
	}
	
	private final class SysMsgHolder{
		TextView sysMsgTv;
	}
	
	/**
	 * �澯��Ϣ������
	 * @author Administrator
	 *
	 */
	private class AlarmMsgAdater extends BaseAdapter{

		@Override
		public int getCount() {
			return alarmMsgList.size();
		}

		@Override
		public Object getItem(int position) {
			return alarmMsgList.size();
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			AlarmMsgHolder holder = null;
			if(convertView == null){
				holder = new AlarmMsgHolder();
				convertView = LayoutInflater.from(MsgActivity.this).inflate(R.layout.item_alarm_msg, null);
				holder.alarmMsgContent = (TextView) convertView.findViewById(R.id.alarm_msg_content_tv);
				convertView.setTag(holder);
			}else{
				holder = (AlarmMsgHolder) convertView.getTag();
			}
			
			PushMsg pushMsg = alarmMsgList.get(position);
			String deviceId = pushMsg.getDeviceId();
			String body = pushMsg.getMsgBody();
			long createTime = pushMsg.getAlarmTime();
			String type = null;
			if(pushMsg.getMsgType() == PushMsgType.MOTION){
				holder.alarmMsgContent.setText("�豸("+deviceId+")"+"   �������" + ",ʱ��:" + createTime);
			}else if(pushMsg.getMsgType() == PushMsgType.SOUND){
				holder.alarmMsgContent.setText("�豸("+deviceId+")"+"   �������" + ",ʱ��:" + createTime);
			}else{
				holder.alarmMsgContent.setText("�豸("+deviceId+")��������,ʱ��:" + createTime);
			}
			return convertView;
		}
		
	}
	
	private final class AlarmMsgHolder{
		public TextView alarmMsgContent;
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.sys_msg_btn:
			switchBtn(SYS_MSG);
			break;
			
		case R.id.alarm_msg_btn:
			switchBtn(ALARM_MSG);
			break;

		default:
			break;
		}
		
	}
}
