package com.infotm.dv;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.Toast;

import com.danale.video.sdk.Danale;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.CollectionType;
import com.danale.video.sdk.platform.constant.GetType;
import com.danale.video.sdk.platform.constant.OnlineType;
import com.danale.video.sdk.platform.constant.PlatformCmd;
import com.danale.video.sdk.platform.constant.ShareActionType;
import com.danale.video.sdk.platform.entity.Device;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.GetDeviceListResult;
import com.infotm.dv.adapter.DeviceAdapter;
import com.infotm.dv.wifiadapter.WifiAdapter;

public class DeviceListOnlineActivity extends Activity implements PlatformResultHandler{

	public static final String TAG = "Danale";
	public static final String[] Items ={"","","",""};
	
	private App app;
	private Context context;
	private Session session;
	private Spinner spinner;
	private Button button;
	private ListView listView;
	
	private GetType getType = GetType.MINE;
	private List<Device> deviceList;
	private DeviceAdapter mDeviceAdapter;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		context = this;
		app = (App) getApplication();
		Items[0]= getString(R.string.my_device);
		
		Items[1]= getString(R.string.concern_device);
		Items[2]= getString(R.string.share_devive);
		Items[3]= getString(R.string.pubilc_device);
		// ��ȡSDK�洢��Session���󣨱����ڵ�¼�ɹ�����ܻ�ȡ��
		session = Danale.getSession();
		
		setContentView(R.layout.activity_device_list_online);

		listView = (ListView) findViewById(R.id.lv_online);
		listView.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> adapter, View view, int position, long id) {
				Device device = deviceList.get(position);
				if (device.getOnlineType() == OnlineType.OFFLINE){
					Toast.makeText(context,R.string.deivce_outline, Toast.LENGTH_SHORT).show();
					return;
				}
				
				Intent intent = new Intent(context, MainActivity.class);
				
				intent.putExtra("isPlatformDevice", true);
				intent.putExtra("deviceId", device.getDeviceId());
				startActivity(intent);
			}
		});
		
		listView.setOnItemLongClickListener(new OnItemLongClickListener() {

			@Override
			public boolean onItemLongClick(AdapterView<?> parent, View view,
					int position, long id) {
				final Device device = deviceList.get(position);
				final int listid=position;
				AlertDialog moreActionDialog;
				AlertDialog.Builder builder = new AlertDialog.Builder(DeviceListOnlineActivity.this);
				final ArrayList<String> itemsList = new ArrayList<String>();
				itemsList.add(context.getString(R.string.rename));
				itemsList.add(context.getString(R.string.share));
				itemsList.add(context.getString(R.string.delete));
				//itemsList.add(context.getString(R.string.message));
				//itemsList.add(context.getString(R.string.setting));
				
				String[] items = new String[itemsList.size()];
				itemsList.toArray(items);
				builder.setItems(items, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						String actionStr = itemsList.get(which);
						
						if(actionStr.equals(context.getString(R.string.rename))){
							AlertDialog.Builder builder = new AlertDialog.Builder(DeviceListOnlineActivity.this);
							builder.setTitle(R.string.rename);
							LinearLayout layout = new LinearLayout(DeviceListOnlineActivity.this);
							layout.setOrientation(LinearLayout.VERTICAL);
							
							final EditText edit = new EditText(DeviceListOnlineActivity.this);
							edit.setHint(R.string.input_alias_name);
							edit.setPadding(30, 30, 30, 30);
							layout.addView(edit);							
							
							builder.setView(layout);
							builder.setPositiveButton(R.string.next, new DialogInterface.OnClickListener() {

							     @Override
							     public void onClick(DialogInterface dialog, int which) {
							      // TODO Auto-generated method stub
							    	 Session.getSession().setDeviceAlias(0, device.getDeviceId(),edit.getText().toString(), DeviceListOnlineActivity.this);
							    	 device.setAlias(edit.getText().toString());
							    	 app.setPlatformDeviceList(deviceList);
							    	 mDeviceAdapter.notifyDataSetChanged();
							     }
							    });
							builder.create().show();
							
							
						}else if(actionStr.equals(context.getString(R.string.share))){
							AlertDialog.Builder builder = new AlertDialog.Builder(DeviceListOnlineActivity.this);
							builder.setTitle(R.string.share);
							LinearLayout layout = new LinearLayout(DeviceListOnlineActivity.this);
							layout.setOrientation(LinearLayout.VERTICAL);
							
							EditText edit = new EditText(DeviceListOnlineActivity.this);
							edit.setHint(R.string.input_share_name);
							edit.setPadding(30, 30, 30, 30);
							layout.addView(edit);
							
							Button sharePersonBtn = new Button(DeviceListOnlineActivity.this);
							sharePersonBtn.setLayoutParams(new LinearLayout.LayoutParams(android.view.ViewGroup.LayoutParams.MATCH_PARENT, android.view.ViewGroup.LayoutParams.WRAP_CONTENT));
							sharePersonBtn.setText(R.string.share_personal);
							sharePersonBtn.setId(11);
							sharePersonBtn.setOnClickListener(new ShareSettingOnClickListener(device,edit));
							layout.addView(sharePersonBtn);
							
							Button shareSqBtn = new Button(DeviceListOnlineActivity.this);
							shareSqBtn.setLayoutParams(new LinearLayout.LayoutParams(android.view.ViewGroup.LayoutParams.MATCH_PARENT, android.view.ViewGroup.LayoutParams.WRAP_CONTENT));
							shareSqBtn.setText(R.string.share_public);
							shareSqBtn.setId(12);
							shareSqBtn.setOnClickListener(new ShareSettingOnClickListener(device,edit));
							layout.addView(shareSqBtn);
							
							Button cancelSqBtn = new Button(DeviceListOnlineActivity.this);
							cancelSqBtn.setLayoutParams(new LinearLayout.LayoutParams(android.view.ViewGroup.LayoutParams.MATCH_PARENT, android.view.ViewGroup.LayoutParams.WRAP_CONTENT));
							cancelSqBtn.setText(R.string.cancel_share_public);
							cancelSqBtn.setId(13);
							cancelSqBtn.setOnClickListener(new ShareSettingOnClickListener(device,edit));
							layout.addView(cancelSqBtn);
							
							Button cancelAllBtn = new Button(DeviceListOnlineActivity.this);
							cancelAllBtn.setLayoutParams(new LinearLayout.LayoutParams(android.view.ViewGroup.LayoutParams.MATCH_PARENT, android.view.ViewGroup.LayoutParams.WRAP_CONTENT));
							cancelAllBtn.setText(R.string.cancel_all_share);
							cancelAllBtn.setId(14);
							cancelAllBtn.setOnClickListener(new ShareSettingOnClickListener(device, edit));
							layout.addView(cancelAllBtn);
							
							
							builder.setView(layout);
							builder.create().show();
							
							
						}else if(actionStr.equals(context.getString(R.string.message))){
							MsgActivity.startActivity(DeviceListOnlineActivity.this, device.getDeviceId());
						}else if(actionStr.equals(context.getString(R.string.delete))){
							Session.getSession().deleteDevice(0, device.getDeviceId(), DeviceListOnlineActivity.this);
							deviceList.remove(listid);
							app.setPlatformDeviceList(deviceList);
							
							 mDeviceAdapter.notifyDataSetChanged();
						}//else if(actionStr.equals(R.string.setting)){
						//	DeviceSettingsActivity.startActivity(DeviceListOnlineActivity.this, device.getDeviceId());
						//}
					}
				});
				
				moreActionDialog = builder.create();
				moreActionDialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
				moreActionDialog.show();
				return true;
			}
		});
		
		button = (Button) findViewById(R.id.btn_online_add_device);
		button.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				//Intent intent = new Intent(context, AddDeviceActivity.class);
				Intent intent = new Intent(context, ConnectActivity.class);
				startActivity(intent);
			}
		});
		
		spinner = (Spinner) findViewById(R.id.spinner_online);
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(context, android.R.layout.simple_spinner_dropdown_item, Items);
		spinner.setAdapter(adapter);
		spinner.setOnItemSelectedListener(new OnItemSelectedListener() {
			@Override
			public void onItemSelected(AdapterView<?> spinner, View view, int position, long id) {
				if (position == 0){
					getType = GetType.MINE;
					button.setEnabled(true);
				}else if (position == 1){
					getType = GetType.ATTENTION;
					button.setEnabled(false);
				}else if (position == 2){
					getType = GetType.SHARE_WITH_ME;
					button.setEnabled(false);
				}else if (position == 3){
					getType = GetType.PUBLIC;
					button.setEnabled(false);
				}
				// ����û���ѡ�񣬻�ȡ��Ӧ���豸�б�
				session.getDeviceList(0, getType, 1, 20, DeviceListOnlineActivity.this);// ��ҳ��ȡ����1ҳ��ÿҳ20�����
				Toast.makeText(context, getString(R.string.loading)+Items[position], Toast.LENGTH_SHORT).show();
			}

			@Override
			public void onNothingSelected(AdapterView<?> spinner) {
				
			}
		});
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		session.getDeviceList(0, getType, 1, 10, DeviceListOnlineActivity.this);	// ��ҳ��ȡ����1ҳ��ÿҳ10�����
	}
	
	private class ShareSettingOnClickListener implements OnClickListener{

		private Device dev;
		private EditText edit;
		
		public ShareSettingOnClickListener(Device device,EditText edit){
			dev = device;
			this.edit = edit;
		}
		
		@Override
		public void onClick(View v) {
			switch (v.getId()) {
			case 11:
				//˽�ܷ���
				Session.getSession().shareDevice(0, dev.getDeviceId(), ShareActionType.ADD_PRIVATE_SHARE, edit.getText().toString(), new PlatformResultHandler() {
					
					@Override
					public void onSuccess(PlatformResult arg0) {
						Toast.makeText(DeviceListOnlineActivity.this, R.string.set_secret_share_ok, Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
						Toast.makeText(DeviceListOnlineActivity.this, R.string.set_secret_share_fail_network, Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult arg0, int arg1) {
						Toast.makeText(DeviceListOnlineActivity.this, getString(R.string.set_secret_share_fail_error) + arg1, Toast.LENGTH_SHORT).show();
						
					}
				});
				break;
				
			case 12:
				//��������
				Session.getSession().shareDevice(0, dev.getDeviceId(), ShareActionType.SET_PUBLIC_SHARE, null, new PlatformResultHandler() {
					
					@Override
					public void onSuccess(PlatformResult arg0) {
						Toast.makeText(DeviceListOnlineActivity.this, R.string.set_public_share_ok, Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
						Toast.makeText(DeviceListOnlineActivity.this, R.string.set_public_share_fail_netwrok, Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult arg0, int arg1) {
						Toast.makeText(DeviceListOnlineActivity.this, getString(R.string.set_public_share_fail_error)+arg1, Toast.LENGTH_SHORT).show();
						
					}
				});
				break;
				
			case 13:
				//ȡ������
				Session.getSession().shareDevice(0, dev.getDeviceId(), ShareActionType.CANCEL_PUBLIC_SHARE, null, new PlatformResultHandler() {
					
					@Override
					public void onSuccess(PlatformResult arg0) {
						Toast.makeText(DeviceListOnlineActivity.this,R.string.cancel_share_public_ok, Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
						Toast.makeText(DeviceListOnlineActivity.this, R.string.cancel_share_public_fail_netwrok, Toast.LENGTH_SHORT).show();
						
						
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult arg0, int arg1) {
						Toast.makeText(DeviceListOnlineActivity.this,getString(R.string.cancel_share_public_fail_error)+arg1, Toast.LENGTH_SHORT).show();
						
					}
				});
				break;

			case 14:
				//ȡ�����з���
				Session.getSession().shareDevice(0, dev.getDeviceId(), ShareActionType.CANCEL_ALL_SHARE, null, new PlatformResultHandler() {
					
					@Override
					public void onSuccess(PlatformResult arg0) {
						Toast.makeText(DeviceListOnlineActivity.this,R.string.cancel_all_share_ok, Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
						Toast.makeText(DeviceListOnlineActivity.this,R.string.cancel_all_share_fail_netwrok, Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult arg0, int arg1) {
						Toast.makeText(DeviceListOnlineActivity.this, getString(R.string.cancel_all_share_fail_error)+arg1, Toast.LENGTH_SHORT).show();
						
					}
				});
				break;
			default:
				break;
			}
			
		}
		
	}

	@Override
	public void onSuccess(PlatformResult result) {
		if (result.getRequestCommand() == PlatformCmd.getDeviceList){
			// TODO ����ȡ�豸�б?�ɹ��Ĵ���
			GetDeviceListResult ret = (GetDeviceListResult)result;
			deviceList = ret.getDeviceList();
			app.setPlatformDeviceList(deviceList);
			Toast.makeText(context, getString(R.string.now_get)+deviceList.size()+getString(R.string.piece_device), Toast.LENGTH_SHORT).show();

			mDeviceAdapter = new DeviceAdapter(deviceList, this);
			listView.setAdapter(mDeviceAdapter);
		}else if (result.getRequestCommand() == PlatformCmd.logout){
			// TODO ����ȡ�豸�б?�ɹ��Ĵ���
			Toast.makeText(context, R.string.logout_ok, Toast.LENGTH_SHORT).show();
		}else if (result.getRequestCommand() == PlatformCmd.deleteDevice)
	    {
			Toast.makeText(context, R.string.delete_ok, Toast.LENGTH_SHORT).show();
	    }
	}

	@Override
	public void onCommandExecFailure(PlatformResult result, int errorCode) {
		if (result.getRequestCommand() == PlatformCmd.getDeviceList){
			// TODO ����ȡ�豸�б?ʧ�ܵĴ���
			Toast.makeText(context, getString(R.string.get_device_list_fail)+errorCode, Toast.LENGTH_SHORT).show();
		}else if (result.getRequestCommand() == PlatformCmd.logout){
			// TODO ���ǳ���ʧ�ܵĴ���
			Toast.makeText(context, getString(R.string.logout_fail)+errorCode, Toast.LENGTH_SHORT).show();
		}else if (result.getRequestCommand() == PlatformCmd.deleteDevice)
	    {
			Toast.makeText(context, R.string.delete_fail+errorCode, Toast.LENGTH_SHORT).show();
	    }
	}

	@Override
	public void onOtherFailure(PlatformResult result, HttpException exception) {
		exception.printStackTrace();
		Toast.makeText(context,R.string.visit_fail_check_network, Toast.LENGTH_SHORT).show();
		if (result.getRequestCommand() == PlatformCmd.getDeviceList){
			// TODO ��������ԭ���¡���ȡ�豸�б?ʧ�ܵĴ���
		}else if (result.getRequestCommand() == PlatformCmd.logout){
			// TODO ��������ԭ���¡��ǳ���ʧ�ܵĴ���
		}else if (result.getRequestCommand() == PlatformCmd.deleteDevice)
	    {
	    }
	}
	
	@Override
	protected void onDestroy() {
		
		// �����������
		for (Device device: deviceList){
			device.getConnection().destroy(0, null);
		}
		app.setPlatformDeviceList(null);
		
		// ����Ҫ�˳���¼������logou�ӿڣ������̨��һֱ���ֵ�ǰ�˺�����
		session.logout(0, DeviceListOnlineActivity.this);
		
		super.onDestroy();
	}
}
