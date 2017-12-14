package com.infotm.dv;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.danale.video.sdk.device.constant.PowerFrequency;
import com.danale.video.sdk.device.constant.RecordPlanState;
import com.danale.video.sdk.device.entity.AlarmInfo;
import com.danale.video.sdk.device.entity.Connection;
import com.danale.video.sdk.device.entity.RecordPlan;
import com.danale.video.sdk.device.entity.TimeInfo;
import com.danale.video.sdk.device.entity.WifiInfo;
import com.danale.video.sdk.device.handler.DeviceResultHandler;
import com.danale.video.sdk.device.result.DeviceResult;
import com.danale.video.sdk.device.result.GetAlarmInfoResult;
import com.danale.video.sdk.device.result.GetPowerFrequencyResult;
import com.danale.video.sdk.device.result.GetTimeInfoResult;
import com.danale.video.sdk.device.result.GetWifiInfoResult;
import com.danale.video.sdk.device.result.GetWifiListResult;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.DeviceServiceState;
import com.danale.video.sdk.platform.constant.DeviceServiceType;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.danale.video.sdk.platform.entity.Device;
import com.danale.video.sdk.platform.entity.DeviceService;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.platform.result.GetDeviceServicesListResult;

public class DeviceSettingsActivity extends Activity implements OnClickListener{
	
	private Device device;
	private TextView devAliasTv;
	private Button editAliasBtn,pushMsgOpenBtn,safeSettingBtn,timeSettingBtn,powerSettingBtn,rebootDevBtn,formatSDBtn,recoverBtn,sdPlanManagerBtn,netSettingBtn;
	
	public static void startActivity(Context fromContext,String deviceId){
		Intent intent = new Intent(fromContext, DeviceSettingsActivity.class);
		intent.putExtra("device_id", deviceId);
		fromContext.startActivity(intent);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_device_settings);
		String deviceId = getIntent().getStringExtra("device_id");
		device = ((App)getApplication()).getPlatformDeviceById(deviceId);
		
		devAliasTv = (TextView) findViewById(R.id.dev_alias_content);
		editAliasBtn = (Button) findViewById(R.id.edit_dev_alias_btn);
		pushMsgOpenBtn = (Button) findViewById(R.id.push_msg_open_btn);
		safeSettingBtn = (Button) findViewById(R.id.safe_setting_btn);
		timeSettingBtn = (Button) findViewById(R.id.time_setting_btn);
		powerSettingBtn = (Button) findViewById(R.id.power_setting_btn);
		rebootDevBtn = (Button) findViewById(R.id.reboot_dev_btn);
		formatSDBtn = (Button) findViewById(R.id.format_sd_card_btn);
		recoverBtn = (Button) findViewById(R.id.recover_btn);
		sdPlanManagerBtn = (Button) findViewById(R.id.sd_plan_manager_btn);
		netSettingBtn = (Button) findViewById(R.id.net_setting_btn);
		
		editAliasBtn.setOnClickListener(this);
		pushMsgOpenBtn.setOnClickListener(this);
		safeSettingBtn.setOnClickListener(this);
		timeSettingBtn.setOnClickListener(this);
		powerSettingBtn.setOnClickListener(this);
		rebootDevBtn.setOnClickListener(this);
		formatSDBtn.setOnClickListener(this);
		recoverBtn.setOnClickListener(this);
		sdPlanManagerBtn.setOnClickListener(this);
		netSettingBtn.setOnClickListener(this);
		
		devAliasTv.setText(device.getAlias());
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.edit_dev_alias_btn:
			onClickEditDevAlias();
			break;
			
		case R.id.push_msg_open_btn:
			onClickPushMsgOpen();
			break;

		case R.id.safe_setting_btn:
			onClickSafeSetting();
			break;
			
		case R.id.time_setting_btn:
			onClickTimeSetting();
			break;
			
		case R.id.power_setting_btn:
			onClickPowerSetting();
			break;
			
		case R.id.reboot_dev_btn:
			onClickRebootDev();
			break;
			
		case R.id.format_sd_card_btn:
			onClickFormatSdCard();
			break;
			
		case R.id.recover_btn:
			onClickRecoverBtn();
			break;
			
		case R.id.sd_plan_manager_btn:
			onClickSdPlanManager();
			break;
			
		case R.id.net_setting_btn:
			onClickNetSetting();
			break;
		default:
			break;
		}
		
	}

	/**
	 * sd卡录像计划管理
	 */
	private void onClickSdPlanManager() {
		AlertDialog.Builder builder = new Builder(this);
		builder.setTitle("添加SD卡计划");
		final boolean[] wks = new boolean[]{false,false,false,false,false,false,false};
		View view  = View.inflate(this, R.layout.dialog_sd_plan, null);
		final EditText etBeginTime = (EditText) view.findViewById(R.id.et_begin_time);
		final EditText etEndTime = (EditText) view.findViewById(R.id.et_end_time);
		
		CheckBox cbSunday = (CheckBox) view.findViewById(R.id.cb_sunday);
		CheckBox cbMonday = (CheckBox) view.findViewById(R.id.cb_monday);
		CheckBox cbTuesday = (CheckBox) view.findViewById(R.id.cb_tuesday);
		CheckBox cbWednesday = (CheckBox) view.findViewById(R.id.cb_wednesday);
		CheckBox cbThursday = (CheckBox) view.findViewById(R.id.cb_thursday);
		CheckBox cbFriday = (CheckBox) view.findViewById(R.id.cb_friday);
		CheckBox cbSaturday = (CheckBox) view.findViewById(R.id.cb_saturday);
		
		cbSunday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[0] = isChecked; 
			}
		});
		cbMonday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[1] = isChecked; 
			}
		});
		cbTuesday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[2] = isChecked; 
			}
		});
		cbWednesday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[3] = isChecked; 
			}
		});
		cbThursday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[4] = isChecked; 
			}
		});
		cbFriday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[5] = isChecked; 
			}
		});
		cbSaturday.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				wks[6] = isChecked; 
			}
		});
		
		builder.setView(view);
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
		});
		
		builder.setPositiveButton("保存", new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				RecordPlan recordPlan = new RecordPlan(); 
				recordPlan.setStartTime(etBeginTime.getText().toString());
				recordPlan.setEndTime(etEndTime.getText().toString());
				recordPlan.setStatus(RecordPlanState.OPEN);
				recordPlan.setWeek(wks);
				doSetRecordPlan(recordPlan);
			}

			private void doSetRecordPlan(RecordPlan recordPlan) {
				Connection conn = device.getConnection();
				int channel = device.getDeviceType() == DeviceType.IPC ? 1 : 0;
				conn.setRecordPlan(0, channel, recordPlan, new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult result) {
						Toast.makeText(DeviceSettingsActivity.this, "设置录像计划成功", Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						Toast.makeText(DeviceSettingsActivity.this, "设置录像计划失败,错误码:" + errorCode, Toast.LENGTH_SHORT).show();
					}
				});
			}
		});
		
		builder.show();
		
	}

	private void onClickRecoverBtn() {
		AlertDialog.Builder builder = new Builder(this);
		builder.setTitle("注意");
		builder.setMessage("确定出厂设置吗?");
		builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				doFactoryReset();
			}
			private void doFactoryReset() {
				int channel = device.getDeviceType() == DeviceType.IPC ? 1 : 0;
				Connection conn = device.getConnection();
				conn.factoryReset(0, channel, new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult result) {
						Toast.makeText(DeviceSettingsActivity.this, "恢复出厂设置成功", Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						Toast.makeText(DeviceSettingsActivity.this, "恢复出厂设置失败,错误码:" + errorCode, Toast.LENGTH_SHORT).show();
					}
				});
			}
		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {}
		});
		builder.show();
	}

	private void onClickFormatSdCard() {
		AlertDialog.Builder builder = new Builder(this);
		builder.setTitle("注意");
		builder.setMessage("确定格式化SD卡吗?");
		builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				doFormatSdcard();
			}
			private void doFormatSdcard() {
				int channel = device.getDeviceType() == DeviceType.IPC ? 1 : 0;
				Connection conn = device.getConnection();
				conn.formatSdcard(0, channel, new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult result) {
						Toast.makeText(DeviceSettingsActivity.this, "格式化SD卡成功", Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						Toast.makeText(DeviceSettingsActivity.this, "格式化SD卡失败,错误码:" + errorCode, Toast.LENGTH_SHORT).show();
					}
				});
			}
		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {}
		});
		builder.show();
	}

	private void onClickRebootDev() {
		AlertDialog.Builder builder = new Builder(this);
		builder.setTitle("注意");
		builder.setMessage("确定重启吗?");
		builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				doReboot();
			}
			private void doReboot() {
				int channel = device.getDeviceType() == DeviceType.IPC ? 1 : 0;
				Connection conn = device.getConnection();
				conn.reboot(0, channel, new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult result) {
						Toast.makeText(DeviceSettingsActivity.this, "重启成功", Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						Toast.makeText(DeviceSettingsActivity.this, "重启失败,错误码:" + errorCode, Toast.LENGTH_SHORT).show();
					}
				});
			}
		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {}
		});
		builder.show();
	}

	private void onClickPowerSetting() {
		final Connection conn = device.getConnection();
		final int channel = device.getDeviceType() == DeviceType.IPC ? 1 : 0;

		//获取电源频率
		conn.getPowerFrequency(0, channel, new DeviceResultHandler() {
			
			@Override
			public void onSuccess(DeviceResult result) {
				GetPowerFrequencyResult powerRes = (GetPowerFrequencyResult)result;
				PowerFrequency powerFrequency = powerRes.getPowerFrequency();
				requestSetPowerFrequency(powerFrequency);
			}

			@Override
			public void onFailure(DeviceResult result, int errorCode) {
				Toast.makeText(DeviceSettingsActivity.this, "获取电源频率失败,错误码:" + errorCode, Toast.LENGTH_SHORT).show();
			}
			
			private void requestSetPowerFrequency(PowerFrequency powerFrequency) {
				int choiceItem = powerFrequency == PowerFrequency._50HZ ? 0 : 1; 
				AlertDialog.Builder builder = new Builder(DeviceSettingsActivity.this);
				builder.setTitle("选择电源频率");
				builder.setSingleChoiceItems(new String[]{"50HZ","60HZ"}, choiceItem ,new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.dismiss();
						switch (which) {
						case 0:
							doSetProwerFrequency(PowerFrequency._50HZ);
							break;

						case 1:
							doSetProwerFrequency(PowerFrequency._60HZ);
							break;
							
						default:
							break;
						}
					}
				});
				builder.show();
			}
			//设置电源频率
			private void doSetProwerFrequency(PowerFrequency powerFrequency) {
				conn.setPowerFrequency(1, channel, powerFrequency, new DeviceResultHandler() {
					
					@Override
					public void onSuccess(DeviceResult result) {
						Toast.makeText(DeviceSettingsActivity.this, "设置电源频率成功", Toast.LENGTH_SHORT).show();
					}
					
					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						Toast.makeText(DeviceSettingsActivity.this, "设置电源频率失败,错误码:" + errorCode, Toast.LENGTH_SHORT).show();
					}
				});
			}
		});
	}
	
	private void onClickNetSetting(){
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("设备网络设置");
		builder.setSingleChoiceItems(new String[]{"局域网设置","无线网络设置"}, -1, new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				switch (which) {
				case 0:
					//局域网设置
					//DeviceLanSettingActivity.startActivity(DeviceSettingsActivity.this, device.getDeviceId());
					break;
					
				case 1:
					//无线网络设置
					
					final List<WifiInfo> infoList = new ArrayList<WifiInfo>();
					//获得设备当前的wifi信息
					device.getConnection().getWifiInfo(0, 1, new DeviceResultHandler() {
						
						@Override
						public void onSuccess(DeviceResult arg0) {
							final WifiInfo curWifiInfo = ((GetWifiInfoResult)arg0).getWifiInfo();
							
							//获取设备周边的wifi信息
							device.getConnection().getWifiList(0, 1, new DeviceResultHandler() {
								
								@Override
								public void onSuccess(DeviceResult arg0) {
									List<WifiInfo> wifiInfoList = ((GetWifiListResult)arg0).getWifiList();
									for(WifiInfo info : wifiInfoList){
										if(info.getSsid().equals(curWifiInfo.getSsid())){
											wifiInfoList.remove(info);
											break;
										}
									}
									infoList.add(curWifiInfo);
									infoList.addAll(1, wifiInfoList);
									
									String[] items = new String[infoList.size()];
									for(int i = 0; i < infoList.size(); i++){
										if(i == 0){
											items[i] = infoList.get(i).getSsid() + "  　　当前使用中";
										}else{
											items[i] = infoList.get(i).getSsid();
										}
									}
									
									AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
									builder.setTitle("ＷiFi设置");
									builder.setSingleChoiceItems(items, 0, new DialogInterface.OnClickListener() {
										
										@Override
										public void onClick(DialogInterface dialog, int which) {
											dialog.dismiss();
											final int position = which;
											AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
											builder.setTitle("输入wifi密码");
											final EditText edit = new EditText(DeviceSettingsActivity.this);
											edit.setHint("请输入密码");
											edit.setPadding(15, 15, 15, 15);
											builder.setView(edit);
											builder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
												
												@Override
												public void onClick(DialogInterface dialog, int which) {
													WifiInfo info = infoList.get(position);
													info.setAuthKey(edit.getText().toString());
													
													//设置设备的wifi
													device.getConnection().setWifiInfo(0, 1, info, new DeviceResultHandler() {
														
														@Override
														public void onSuccess(DeviceResult arg0) {
															Toast.makeText(DeviceSettingsActivity.this, "设置wifi成功", Toast.LENGTH_SHORT).show();
															
														}
														
														@Override
														public void onFailure(DeviceResult arg0, int arg1) {
															Toast.makeText(DeviceSettingsActivity.this, "设置wifi失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
															
														}
													});
													
												}
											});
											
											builder.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
												
												@Override
												public void onClick(DialogInterface dialog, int which) {
													// TODO Auto-generated method stub
													
												}
											});
											builder.create().show();
											
											
											
										}
									});
									builder.create().show();
								}
								
								@Override
								public void onFailure(DeviceResult arg0, int arg1) {
									Toast.makeText(DeviceSettingsActivity.this, "获取设备周边Ｗifi信息失败", Toast.LENGTH_SHORT).show();
									
								}
							});
							
						}
						
						@Override
						public void onFailure(DeviceResult arg0, int arg1) {
							Toast.makeText(DeviceSettingsActivity.this, "获取设备当前的Ｗifi信息失败", Toast.LENGTH_SHORT).show();
							
						}
					});
					
					break;

				default:
					break;
				}
				
			}
		});
		builder.create().show();
	}

	/**
	 * 设置设备时间信息
	 */
	private void onClickTimeSetting() {
		device.getConnection().getTimeInfo(0, 1, new DeviceResultHandler() {
			
			@Override
			public void onSuccess(DeviceResult arg0) {
				Toast.makeText(DeviceSettingsActivity.this, "获取设备时间信息成功", Toast.LENGTH_SHORT).show();
				TimeInfo timeInfo = ((GetTimeInfoResult)arg0).getTimeInfo();
				AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
				builder.setTitle("设置设备时间");
				final Context context = DeviceSettingsActivity.this;
				LinearLayout layout = new LinearLayout(context);
				layout.setOrientation(LinearLayout.VERTICAL);
				
				LinearLayout timeLayout = new LinearLayout(context);
				timeLayout.setOrientation(LinearLayout.HORIZONTAL);
				TextView devTimeTv = new TextView(context);
				devTimeTv.setText("设备时间:  ");
				timeLayout.addView(devTimeTv);
				final EditText devTimeEdit = new EditText(context);
				devTimeEdit.setText(timeInfo.getNowTime()+"");
				timeLayout.addView(devTimeEdit);
				layout.addView(timeLayout);
				
				LinearLayout localLayout = new LinearLayout(context);
				localLayout.setOrientation(LinearLayout.HORIZONTAL);
				TextView locatTv = new TextView(context);
				locatTv.setText("设备时区：　　");
				localLayout.addView(locatTv);
				final EditText devLocalEdit = new EditText(context);
				devLocalEdit.setText(timeInfo.getTimeZone().toString());
				localLayout.addView(devLocalEdit);
				layout.addView(localLayout);
				
				
				LinearLayout ntp1Layout = new LinearLayout(context);
				ntp1Layout.setOrientation(LinearLayout.HORIZONTAL);
				TextView ntp1Tv = new TextView(context);
				ntp1Tv.setText("Ntp1:  ");
				ntp1Layout.addView(ntp1Tv);
				final EditText ntp1Edit = new EditText(context);
				ntp1Edit.setText(timeInfo.getNtpServer1().toString());
				ntp1Layout.addView(ntp1Edit);
				layout.addView(ntp1Layout);
				
				LinearLayout ntp2Layout = new LinearLayout(context);
				ntp2Layout.setOrientation(LinearLayout.HORIZONTAL);
				TextView ntp2Tv = new TextView(context);
				ntp2Tv.setText("Ntp2:  ");
				ntp2Layout.addView(ntp2Tv);
				final EditText ntp2Edit = new EditText(context);
				ntp2Edit.setText(timeInfo.getNtpServer2().toString());
				ntp2Layout.addView(ntp2Edit);
				layout.addView(ntp2Layout);
				
				builder.setView(layout);
				builder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						TimeInfo info = new TimeInfo();
						info.setNowTime(Long.valueOf(devTimeEdit.getText().toString()));
						info.setNtpServer1(ntp1Edit.getText().toString());
						info.setNtpServer2(ntp2Edit.getText().toString());
						info.setTimeZone(devLocalEdit.getText().toString());
						device.getConnection().setTimeInfo(0, 1, info, new DeviceResultHandler() {
							
							@Override
							public void onSuccess(DeviceResult arg0) {
								Toast.makeText(context, "设置设备时间成功", Toast.LENGTH_SHORT).show();
								
							}
							
							@Override
							public void onFailure(DeviceResult arg0, int arg1) {
								Toast.makeText(context, "设置设备时间失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
								
							}
						});
						
					}
				});
				builder.setNegativeButton(android.R.string.ok, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub
						
					}
				});
				builder.create().show();
			}
			
			@Override
			public void onFailure(DeviceResult arg0, int arg1) {
				Toast.makeText(DeviceSettingsActivity.this, "获取设备时间信息失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
				
			}
		});
	}

	private void onClickSafeSetting() {
		AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
		builder.setTitle("安全设置");
		String[] items = new String[]{"移动侦测","声音侦测"};
		builder.setSingleChoiceItems(items, -1, new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				switch (which) {
				case 0:
					//移动侦测
					
					//获取当前设备移动侦测状态
					device.getConnection().getAlarmInfo(0, 1, new DeviceResultHandler() {
						
						@Override
						public void onSuccess(DeviceResult arg0) {
							final AlarmInfo alarmInfo = ((GetAlarmInfoResult)arg0).getAlarmInfo();
							AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
							builder.setTitle("移动侦测");
							
							
							//对应0,1,2,3
							String[] items = new String[]{"关闭","低","中","高"};
							builder.setSingleChoiceItems(items, alarmInfo.getMotionDetect(), new DialogInterface.OnClickListener() {
								
								@Override
								public void onClick(DialogInterface dialog, int which) {
									
									//修改侦测状态时,请用之前获取的alarmInfo对象设置,这样就不会重置其他状态
									alarmInfo.setMotionDetect(which);
									device.getConnection().setAlarmInfo(0, 1, alarmInfo, new DeviceResultHandler() {
										
										@Override
										public void onSuccess(DeviceResult arg0) {
											Toast.makeText(DeviceSettingsActivity.this, "设置移动侦测状态成功", Toast.LENGTH_SHORT).show();
											
										}
										
										@Override
										public void onFailure(DeviceResult arg0, int arg1) {
											Toast.makeText(DeviceSettingsActivity.this, "设置移动侦测状失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
											
										}
									});
									
								}
							});
							builder.create().show();
							
						}
						
						@Override
						public void onFailure(DeviceResult arg0, int arg1) {
							Toast.makeText(DeviceSettingsActivity.this, "获取设备移动侦测状态失败--错误码:" + arg1, Toast.LENGTH_SHORT);
							
						}
					});
					
					break;
				case 1:
					//声音侦测
					
					//获取声音侦测状态
						device.getConnection().getAlarmInfo(0, 1, new DeviceResultHandler() {
						
						@Override
						public void onSuccess(DeviceResult arg0) {
							final AlarmInfo alarmInfo = ((GetAlarmInfoResult)arg0).getAlarmInfo();
							AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
							builder.setTitle("声音侦测");
							
							
							//对应0,1,2,3
							String[] items = new String[]{"关闭","低","中","高"};
							builder.setSingleChoiceItems(items, alarmInfo.getSoundDetect(), new DialogInterface.OnClickListener() {
								
								@Override
								public void onClick(DialogInterface dialog, int which) {
									//修改侦测状态时,请用之前获取的alarmInfo对象设置,这样就不会重置其他状态
									alarmInfo.setSoundDetect(which);
									device.getConnection().setAlarmInfo(0, 1, alarmInfo, new DeviceResultHandler() {
										
										@Override
										public void onSuccess(DeviceResult arg0) {
											Toast.makeText(DeviceSettingsActivity.this, "设置声音侦测状态成功", Toast.LENGTH_SHORT).show();
											
										}
										
										@Override
										public void onFailure(DeviceResult arg0, int arg1) {
											Toast.makeText(DeviceSettingsActivity.this, "设置声音侦测状态失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
											
										}
									});
									
								}
							});
							builder.create().show();
						}
						
						@Override
						public void onFailure(DeviceResult arg0, int arg1) {
							Toast.makeText(DeviceSettingsActivity.this, "获取设备声音侦测状态失败--错误码:" + arg1, Toast.LENGTH_SHORT);
							
						}
					});
					break;
				default:
					break;
				}
				
			}
		});
		builder.create().show();
		
	}

	/**
	 * 通知消息控制
	 */
	private void onClickPushMsgOpen() {
		//首先获取该设备通知消息状态
		Session.getSession().getDeviceServicesList(0, device.getDeviceId(), new PlatformResultHandler() {
			
			@Override
			public void onSuccess(PlatformResult arg0) {
				List<DeviceService> devServiceList = ((GetDeviceServicesListResult)arg0).getDeviceServiceList();
				if(devServiceList != null &&  devServiceList.size() != 0){
					DeviceService deviceService = devServiceList.get(0);
					if(deviceService.getDeviceId().equals(device.getDeviceId())){
						DeviceServiceState serviceState = deviceService.getServiceState();
						
						AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
						builder.setTitle("选择通知消息状态");
						String[] stateItems = new String[2];
						stateItems[0] = "关闭";
						stateItems[1] = "开启";
						int checkedItem = 0;
						if(serviceState == DeviceServiceState.OPEN){
							checkedItem = 1;
						}else if(serviceState == DeviceServiceState.CLOSE){
							checkedItem = 0;
						}
						builder.setSingleChoiceItems(stateItems, checkedItem, new DialogInterface.OnClickListener() {
							
							@Override
							public void onClick(DialogInterface dialog, int which) {
								DeviceServiceState state = DeviceServiceState.CLOSE;
								if(which == 0){
									state = DeviceServiceState.CLOSE;
								}else if(which == 1){
									state = DeviceServiceState.OPEN;
								}
								Session.getSession().setDeviceService(0, device.getDeviceId(), DeviceServiceType.PUSH, state, new PlatformResultHandler() {
									
									@Override
									public void onSuccess(PlatformResult arg0) {
										Toast.makeText(DeviceSettingsActivity.this, "设置推送状态成功", Toast.LENGTH_SHORT).show();
										
									}
									
									@Override
									public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
										Toast.makeText(DeviceSettingsActivity.this, "设置推送状态失败,网络错误", Toast.LENGTH_SHORT).show();
										
									}
									
									@Override
									public void onCommandExecFailure(PlatformResult arg0, int arg1) {
										Toast.makeText(DeviceSettingsActivity.this, "设置推送状态失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
										
									}
								});
							}
						});
						builder.create().show();
						
					}else{
						Toast.makeText(DeviceSettingsActivity.this, "未获取到该设备通知状态", Toast.LENGTH_SHORT).show();
					}
				}else{
					Toast.makeText(DeviceSettingsActivity.this, "未获取到该设备通知状态", Toast.LENGTH_SHORT).show();
				}
				
			}
			
			@Override
			public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
				Toast.makeText(DeviceSettingsActivity.this, "获取设备通知状态失败,网络错误", Toast.LENGTH_SHORT).show();
				
			}
			
			@Override
			public void onCommandExecFailure(PlatformResult arg0, int arg1) {
				Toast.makeText(DeviceSettingsActivity.this, "获取设备通知状态失败--错误码:"+arg1, Toast.LENGTH_SHORT).show();
				
			}
		});
		
	}

	/**
	 * 修改设备名称
	 */
	private void onClickEditDevAlias() {
		AlertDialog.Builder builder = new AlertDialog.Builder(DeviceSettingsActivity.this);
		builder.setTitle("修改设备名称");
		final EditText edit = new EditText(DeviceSettingsActivity.this);
		edit.setHint("请输入设备名称");
		edit.setPadding(15, 15, 15, 15);
		builder.setView(edit);
		builder.setPositiveButton(android.R.string.ok,new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				final String alias = edit.getText().toString();
				Session.getSession().setDeviceAlias(0, device.getDeviceId(), alias, new PlatformResultHandler() {
					
					@Override
					public void onSuccess(PlatformResult arg0) {
						Toast.makeText(DeviceSettingsActivity.this, "修改设备名称成功", Toast.LENGTH_SHORT).show();
						devAliasTv.setText(alias);
						
					}
					
					@Override
					public void onOtherFailure(PlatformResult arg0, HttpException arg1) {
						Toast.makeText(DeviceSettingsActivity.this, "修改设备名称成功，网络错误", Toast.LENGTH_SHORT).show();
						
					}
					
					@Override
					public void onCommandExecFailure(PlatformResult arg0, int arg1) {
						Toast.makeText(DeviceSettingsActivity.this, "修改设备名称成功--错误码:"+arg1, Toast.LENGTH_SHORT).show();
						
					}
				});
				
			}
		});
		
		builder.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				// TODO Auto-generated method stub
				
			}
		});
		builder.create().show();
	}
	
	
}
