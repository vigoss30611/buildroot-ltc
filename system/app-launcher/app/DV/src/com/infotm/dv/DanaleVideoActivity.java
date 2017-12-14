/*package com.infotm.dv;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder.AudioSource;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.danale.video.jni.DanaDevSession.AudioInfo;
import com.danale.video.sdk.device.constant.OperationState;
import com.danale.video.sdk.device.constant.Orientation;
import com.danale.video.sdk.device.constant.PTZ;
import com.danale.video.sdk.device.constant.RecordListGetType;
import com.danale.video.sdk.device.entity.BoundaryInfo;
import com.danale.video.sdk.device.entity.Connection;
import com.danale.video.sdk.device.entity.Connection.LiveAudioReceiver;
import com.danale.video.sdk.device.entity.Connection.OnConnectionErrorListener;
import com.danale.video.sdk.device.entity.LanDevice;
import com.danale.video.sdk.device.entity.RecordInfo;
import com.danale.video.sdk.device.extend.DeviceExtendByteResultHandler;
import com.danale.video.sdk.device.extend.DeviceExtendDispatcher;
import com.danale.video.sdk.device.handler.DeviceResultHandler;
import com.danale.video.sdk.device.result.DeviceResult;
import com.danale.video.sdk.device.result.GetOrientaionResult;
import com.danale.video.sdk.device.result.GetRecordListResult;
import com.danale.video.sdk.http.exception.HttpException;
import com.danale.video.sdk.platform.base.PlatformResult;
import com.danale.video.sdk.platform.constant.CollectionType;
import com.danale.video.sdk.platform.constant.DeviceType;
import com.danale.video.sdk.platform.entity.Device;
import com.danale.video.sdk.platform.entity.Session;
import com.danale.video.sdk.platform.handler.PlatformResultHandler;
import com.danale.video.sdk.player.DanalePlayer;
import com.danale.video.view.opengl.DanaleGlSurfaceView;

public class DanaleVideoActivity extends Activity implements
		DanalePlayer.OnPlayerStateChangeListener, OnClickListener {

	private static final String HORIZONTAL = "ˮƽ";
	private static final String VERTICAL = "��ֱ";
	private static final String UPRIGHT = "����";
	private static final String TURN_180 = "180��";

	private App app;
	private Context context;
	private ProgressBar progressBar;

	private Connection connection;
	private LanDevice device;
	private DanalePlayer danalePlayer;
	private DanaleGlSurfaceView glSurfaceView;
	private SurfaceView surfaceView;

	private int channel = 1;
	private int videoQuality = 30;

	private Button startOrStopVideoBtn, startOrStopAudioBtn,
			startOrStopTalkBtn, screenShotBtn, gainQualityBtn,
			gainScreenOrientationBtn, setQualityBtn, setScreenOrientationBtn,
			ptzControlBtn, attentionBtn, sdRecordBtn, transferBtn, setBcBtn;

	private boolean isVideoOpened;
	private boolean isAudioOpened;
	private boolean isTalkOpened;

	private AudioTrack audioTrack;
	private AudioRecord audioRecord;

	private boolean isPlatDevice;
	private CollectionType collectionType;

	public static void startActivity(Context fromActivity, String deviceId,
			boolean isLocal) {
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		findViews();
		initData();
	}

	private void findViews() {
		setContentView(R.layout.activity_danale_video);
		glSurfaceView = (DanaleGlSurfaceView) findViewById(R.id.gl_sv);
		surfaceView = (SurfaceView) findViewById(R.id.sv);
		progressBar = (ProgressBar) findViewById(R.id.pb);
		startOrStopVideoBtn = (Button) findViewById(R.id.start_or_stop_video_btn);
		startOrStopAudioBtn = (Button) findViewById(R.id.start_or_stop_audio_btn);
		startOrStopTalkBtn = (Button) findViewById(R.id.start_or_stop_talk_btn);
		screenShotBtn = (Button) findViewById(R.id.screenshot_btn);
		gainQualityBtn = (Button) findViewById(R.id.gain_video_quality_btn);
		gainScreenOrientationBtn = (Button) findViewById(R.id.gain_video_screen_orientation_btn);
		setQualityBtn = (Button) findViewById(R.id.set_video_quality_btn);
		setScreenOrientationBtn = (Button) findViewById(R.id.set_video_screen_orientation_btn);
		ptzControlBtn = (Button) findViewById(R.id.ptz_control_btn);
		attentionBtn = (Button) findViewById(R.id.attention_btn);
		sdRecordBtn = (Button) findViewById(R.id.sd_record_btn);
		transferBtn = (Button) findViewById(R.id.transfer_btn);
		setBcBtn = (Button) findViewById(R.id.set_bc_btn);

		startOrStopVideoBtn.setOnClickListener(this);
		startOrStopAudioBtn.setOnClickListener(this);
		startOrStopTalkBtn.setOnClickListener(this);
		screenShotBtn.setOnClickListener(this);
		gainQualityBtn.setOnClickListener(this);
		gainScreenOrientationBtn.setOnClickListener(this);
		setQualityBtn.setOnClickListener(this);
		setScreenOrientationBtn.setOnClickListener(this);
		ptzControlBtn.setOnClickListener(this);
		attentionBtn.setOnClickListener(this);
		sdRecordBtn.setOnClickListener(this);
		transferBtn.setOnClickListener(this);
		setBcBtn.setOnClickListener(this);
	}

	private void initData() {
		context = this;
		app = (App) getApplication();

		Intent intent = getIntent();
		isPlatDevice = intent.getBooleanExtra("isPlatformDevice", true);
		if (isPlatDevice) {
			device = app.getPlatformDeviceById(intent
					.getStringExtra("deviceIp"));
			connection = device.getConnection();

			collectionType = app.getPlatformDeviceById(
					intent.getStringExtra("deviceIp")).getCollectionType();
			if (collectionType == CollectionType.COLLECT) {
				attentionBtn.setText("ȡ���ղ�");
			} else {
				attentionBtn.setText("�ղ�");
			}
		} else {
			//device = app.getLanDeviceById(intent.getStringExtra("deviceIp"));
			device = app.getLanDeviceByIp(intent.getStringExtra("deviceIp"));
			connection = device.getConnection();
			connection.setLocalAccessAuth("admin", "admin");

			attentionBtn.setVisibility(View.GONE);
		}

		// ��Ƶ�ռ���
		int bufferSizeInBytes = AudioRecord.getMinBufferSize(8000,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
		audioRecord = new AudioRecord(AudioSource.VOICE_COMMUNICATION, 8000,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
				bufferSizeInBytes);

		// bufferSizeInBytes = AudioRecord.getMinBufferSize(8000,
		// AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
		// mAudioRecord = new AudioRecord(AudioSource.VOICE_COMMUNICATION, 8000,
		// AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
		// bufferSizeInBytes);

		glSurfaceView.init();
		// ����DanalePlayer
		danalePlayer = new DanalePlayer(this,surfaceView, glSurfaceView);

		// ���ò���״̬����
		danalePlayer.setOnPlayerStateChangeListener(this);
		// ��ʼ���ţ��������ȿ�ʼ���ţ�Ȼ���ڷ�ָ����������õ�һ֡����������ʾ��
		// ��һ���������Ƿ�Ӳ��,�ڶ��������ǲ����豸����
		danalePlayer.preStart(true, DeviceType.IPC);
		// ����ָ�������Ƶ
		connection.startLiveVideo(0, channel, videoQuality, danalePlayer,
				new DeviceResultHandler() {

					@Override
					public void onSuccess(DeviceResult result) {
						isVideoOpened = true;
						startOrStopVideoBtn.setText("�ر���Ƶ");
						startOrStopVideoBtn.setEnabled(true);
						Toast.makeText(context, "��Ƶ�����ɹ������ڽ�����Ƶ����",
								Toast.LENGTH_SHORT).show();

					}

					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						progressBar.setVisibility(View.GONE);
						isVideoOpened = false;
						startOrStopVideoBtn.setText("������Ƶ");
						startOrStopVideoBtn.setEnabled(true);
						Toast.makeText(context, "������Ƶʧ��", Toast.LENGTH_SHORT)
								.show();
					}
				});
		startOrStopVideoBtn.setEnabled(false);
		// connection.startLiveVideo(0, channel, videoQuality,new
		// Connection.RawLiveVideoReceiver() {
		//
		// @Override
		// public void onReceive(int channel, int format, long timeStamp,
		// boolean isKeyFrame,
		// byte[] data) {
		// danalePlayer.handleData(channel, format, timeStamp, isKeyFrame,
		// data);
		//
		// }
		// }, new DeviceResultHandler() {
		//
		// @Override
		// public void onSuccess(DeviceResult arg0) {
		// isVideoOpened = true;
		// startOrStopVideoBtn.setText("�ر���Ƶ");
		// startOrStopVideoBtn.setEnabled(true);
		// Toast.makeText(context, "��Ƶ�����ɹ������ڽ�����Ƶ����",
		// Toast.LENGTH_SHORT).show();
		// }
		//
		// @Override
		// public void onFailure(DeviceResult arg0, int arg1) {
		// progressBar.setVisibility(View.GONE);
		// isVideoOpened = false;
		// startOrStopVideoBtn.setText("������Ƶ");
		// startOrStopVideoBtn.setEnabled(true);
		// Toast.makeText(context, "������Ƶʧ��", Toast.LENGTH_SHORT).show();
		// }
		// });

		// ���������жϵĻص�
		connection
				.setOnConnectionErrorListener(new OnConnectionErrorListener() {

					@Override
					public void onConnectionError() {
						// �����ڴ˴�����stop��Ƶ������,����start��Ƶ

					}
				});
	}

	@Override
	public void onVideoPlaying(int channel) {
		progressBar.setVisibility(View.GONE);
		isVideoOpened = true;
		Toast.makeText(context, "���ڲ�����Ƶ", Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onVideoSizeChange(int channel, int videoWidth, int videoHeight) {
		Toast.makeText(context, "��Ƶ��С�����仯:" + videoWidth + "x" + videoHeight,
				Toast.LENGTH_SHORT).show();
	}

	public void onVideoTimeout() {
		Toast.makeText(context, "���ų�ʱ����һ��ʱ����û���յ���Ƶ����", Toast.LENGTH_SHORT)
				.show();
		danalePlayer.stop();
		connection.stopLiveVideo(0, channel, danalePlayer,
				new DeviceResultHandler() {

					@Override
					public void onSuccess(DeviceResult result) {
						finish();
					}

					@Override
					public void onFailure(DeviceResult result, int errorCode) {
						finish();
					}
				});
	}

	@Override
	public void onBackPressed() {
		if (audioTrack != null) {
			audioTrack.stop();
			audioTrack.release();
		}

		if (audioRecord != null) {
			audioRecord.stop();
			audioRecord.release();
		}
		OperationState state = connection.getVideoOptState();
		if (state == OperationState.STARTED) {
			progressBar.setVisibility(View.VISIBLE);
			danalePlayer.stop();

			connection.stopLiveVideo(0, channel, danalePlayer,
					new DeviceResultHandler() {

						@Override
						public void onSuccess(DeviceResult result) {
							finish();

						}

						@Override
						public void onFailure(DeviceResult result, int errorCode) {
							finish();
						}
					});
		} else {
			finish();
		}
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.start_or_stop_video_btn:
			onClickStartOrStopVideoBtn();
			break;

		case R.id.start_or_stop_audio_btn:
			onClickStartOrStopAudioBtn();
			break;

		case R.id.start_or_stop_talk_btn:
			onClickStartOrStopTalkBtn();
			break;

		case R.id.screenshot_btn:
			onClickScreenShotBtn();
			break;

		case R.id.gain_video_quality_btn:
			onClickGainVideoQuality();
			break;

		case R.id.gain_video_screen_orientation_btn:
			onClickGainVideoScreenOrientation();
			break;

		case R.id.set_video_quality_btn:
			onClickSetVideoQuality();
			break;

		case R.id.set_video_screen_orientation_btn:
			onClickSetVideoScreenOrientation();
			break;

		case R.id.ptz_control_btn:
			onClickPtzControl();
			break;

		case R.id.attention_btn:
			onClickAttention();
			break;

		case R.id.sd_record_btn:
			onClickSdRecord();
			Toast.makeText(this, "���������", Toast.LENGTH_SHORT).show();
			break;

		case R.id.transfer_btn:
			onClickTransferData();
			break;

		case R.id.set_bc_btn:
			onClickSetBcBtn();
			break;

		default:
			break;
		}

	}

	private void onClickSetBcBtn() {
		List<BoundaryInfo> list = new ArrayList<BoundaryInfo>();
		BoundaryInfo info = new BoundaryInfo();
		info.setAcrossArea(new int[]{2,4,6,10});
		info.setVertexs(new int[][]{{50,50},{50,50},{50,50},{50,50}});
		list.add(info);
		
		
		device.getConnection().setBoundaryInfos(1, 1, list, new DeviceResultHandler() {
			
			@Override
			public void onSuccess(DeviceResult arg0) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void onFailure(DeviceResult arg0, int arg1) {
				// TODO Auto-generated method stub
				
			}
		});
	}

	private void onClickSdRecord() {
		// timeStamp:�Ը�ʱ��Ϊ��׼ʱ��, getType:ѡ���׼ʱ����ǰ��ȡ��������ȡ num:һ�β�����30�� (����ֵ��˳������)
		device.getConnection().getRecordList(0, 1, System.currentTimeMillis(),
				RecordListGetType.NEXT, 20, new DeviceResultHandler() {

					@Override
					public void onSuccess(DeviceResult arg0) {
						List<RecordInfo> list = ((GetRecordListResult) arg0)
								.getRecordInfo();

						
						 * ¼�񲥷Ųο�����
						 * //��list�л�ȡ���ŵ�¼��,��ʵstartTime��ʾ�ö�¼��ʼ��ʱ��,length��ʾ¼���ʱ��
						 * ,�����ֶ����ƺ�ʱֹͣ����
						 * //¼�񲥷ŵ�������:1.��ȡ��ĳʱ�����ָ��������¼���¼,�����������ּ���ȡ
						 * ,�����ȡ��timestamp��ǰһ�����һ����¼�Ľ���ʱ���1��,���������޷��ȡ
						 * 2.��������ӿڲ���¼��,����ĳ��¼��ʱ����㿪ʼʱ��ͽ���ʱ��,�����ŵ�����ʱ��ʱ���ֶ�stop
						 * 3.RecordAction������ͣ/����¼��
						 * device.getConnection().recordPlayByVideoRaw(0, 1,
						 * list.get(0).getStartTime(), danalePlayer, new
						 * LiveAudioReceiver() {
						 * 
						 * @Override public void onReceiveAudio(byte[] arg0) {
						 * //��������
						 * 
						 * } } , new DeviceResultHandler() {
						 * 
						 * @Override public void onSuccess(DeviceResult arg0) {
						 * //�ɹ�
						 * 
						 * }
						 * 
						 * @Override public void onFailure(DeviceResult arg0,
						 * int arg1) { // ʧ��
						 * 
						 * } });
						 * 
						 * //�÷�����������¼�����ͣ�Լ���������
						 * device.getConnection().recordAction(0, 1,
						 * RecordAction.PAUSE, new DeviceResultHandler() {
						 * 
						 * @Override public void onSuccess(DeviceResult arg0) {
						 * // TODO Auto-generated method stub
						 * 
						 * }
						 * 
						 * @Override public void onFailure(DeviceResult arg0,
						 * int arg1) { // TODO Auto-generated method stub
						 * 
						 * } });
						 * 
						 * //�÷�������ֹͣ����¼�� device.getConnection().recordStop(0, 1,
						 * new DeviceResultHandler() {
						 * 
						 * @Override public void onSuccess(DeviceResult arg0) {
						 * // TODO Auto-generated method stub
						 * 
						 * }
						 * 
						 * @Override public void onFailure(DeviceResult arg0,
						 * int arg1) { // TODO Auto-generated method stub
						 * 
						 * } });
						 
					}

					@Override
					public void onFailure(DeviceResult arg0, int arg1) {
						// ʧ��
					}

				});
	}

	private void onClickAttention() {
		if (collectionType == CollectionType.NOT_COLLECT) {
			Session.getSession().setDeviceCollection(0, device.getDeviceId(),
					CollectionType.COLLECT, new PlatformResultHandler() {

						@Override
						public void onSuccess(PlatformResult arg0) {
							showToast("�ղسɹ�");
							((Device) device)
									.setCollectionType(CollectionType.COLLECT);

						}

						@Override
						public void onOtherFailure(PlatformResult arg0,
								HttpException arg1) {
							showToast("�ղ�ʧ��,�������");

						}

						@Override
						public void onCommandExecFailure(PlatformResult arg0,
								int arg1) {
							showToast("�ղ�ʧ��--������:" + arg1);
						}
					});
		} else if (collectionType == CollectionType.COLLECT) {
			Session.getSession().setDeviceCollection(0, device.getDeviceId(),
					CollectionType.NOT_COLLECT, new PlatformResultHandler() {

						@Override
						public void onSuccess(PlatformResult arg0) {
							showToast("ȡ���ղسɹ�");
							((Device) device)
									.setCollectionType(CollectionType.NOT_COLLECT);

						}

						@Override
						public void onOtherFailure(PlatformResult arg0,
								HttpException arg1) {
							showToast("ȡ���ղ�ʧ��,�������");

						}

						@Override
						public void onCommandExecFailure(PlatformResult arg0,
								int arg1) {
							showToast("ȡ���ղ�ʧ��--������:" + arg1);
						}
					});
		}

	}

	private void onClickPtzControl() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("��̨����(�����淭ת�ı�ʱ,���Ӧ״̬������̨����)");
		LinearLayout layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.HORIZONTAL);
		layout.setGravity(Gravity.CENTER);

		Button leftBtn = new Button(this);
		leftBtn.setText("����");
		leftBtn.setId(11);
		leftBtn.setLayoutParams(new LinearLayout.LayoutParams(0,
				LayoutParams.WRAP_CONTENT, 1.0f));
		leftBtn.setOnTouchListener(new TouchListener());
		layout.addView(leftBtn);

		Button rightBtn = new Button(this);
		rightBtn.setLayoutParams(new LinearLayout.LayoutParams(0,
				LayoutParams.WRAP_CONTENT, 1.0f));
		rightBtn.setText("����");
		rightBtn.setId(12);
		rightBtn.setOnTouchListener(new TouchListener());
		layout.addView(rightBtn);

		Button topBtn = new Button(this);
		topBtn.setLayoutParams(new LinearLayout.LayoutParams(0,
				LayoutParams.WRAP_CONTENT, 1.0f));
		topBtn.setText("����");
		topBtn.setId(13);
		topBtn.setOnTouchListener(new TouchListener());
		layout.addView(topBtn);

		Button downBtn = new Button(this);
		downBtn.setLayoutParams(new LinearLayout.LayoutParams(0,
				LayoutParams.WRAP_CONTENT, 1.0f));
		downBtn.setText("����");
		downBtn.setId(14);
		downBtn.setOnTouchListener(new TouchListener());
		layout.addView(downBtn);

		builder.setView(layout);
		builder.setPositiveButton(android.R.string.ok,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {

					}
				});
		builder.setNegativeButton(android.R.string.cancel,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub

					}
				});
		builder.create().show();

	}

	private class TouchListener implements OnTouchListener {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			int id = v.getId();
			if (event.getAction() == MotionEvent.ACTION_UP) {
				// ��ֹ̨ͣ
				connection.ptzCtrl(0, channel, PTZ.STOP, null);
			} else if (event.getAction() == MotionEvent.ACTION_DOWN) {
				switch (id) {
				case 11:
					connection.ptzCtrl(0, channel, PTZ.MOVE_LEFT, null);
					break;

				case 12:
					connection.ptzCtrl(0, channel, PTZ.MOVE_RIGHT, null);
					break;

				case 13:
					connection.ptzCtrl(0, channel, PTZ.MOVE_UP, null);
					break;

				case 14:
					connection.ptzCtrl(0, channel, PTZ.MOVE_DOWN, null);
					break;

				default:
					break;
				}
			}

			return false;
		}

	}

	*//**
	 * ������Ƶ��ת��ť�¼�
	 * 
	 *//*
	private void onClickSetVideoScreenOrientation() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("��Ƶ���淭ת����");
		LinearLayout layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.VERTICAL);
		TextView msgTv = new TextView(this);
		msgTv.setText(HORIZONTAL + ":" + Orientation.HORIZONTAL.getNum()
				+ "  ; " + VERTICAL + ":" + Orientation.VERTICAL.getNum()
				+ " ; " + UPRIGHT + ":" + Orientation.UPRIGHT.getNum() + " ; "
				+ TURN_180 + ":" + Orientation.TURN180.getNum());
		msgTv.setPadding(10, 10, 10, 10);
		layout.addView(msgTv);
		final EditText edit = new EditText(this);
		layout.addView(edit);
		builder.setView(layout);
		builder.setPositiveButton(android.R.string.ok,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						connection.setOrientation(0, channel, Orientation
								.getOrientation(Integer.valueOf(edit.getText()
										.toString())),
								new DeviceResultHandler() {

									@Override
									public void onSuccess(DeviceResult arg0) {
										showToast("���û��淭ת�ɹ�");

									}

									@Override
									public void onFailure(DeviceResult arg0,
											int arg1) {
										showToast("���û��淭תʧ��--������:" + arg1);

									}
								});
					}
				});
		builder.setNegativeButton(android.R.string.cancel,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub

					}
				});
		builder.create().show();

	}

	*//**
	 * ������Ƶ������ť�¼�
	 *//*
	private void onClickSetVideoQuality() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("��Ƶ��������");
		LinearLayout layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.VERTICAL);
		TextView msgTv = new TextView(this);
		msgTv.setText("������0-100֮�����ֵ");
		msgTv.setPadding(10, 10, 10, 10);
		layout.addView(msgTv);
		final EditText edit = new EditText(this);
		layout.addView(edit);
		builder.setView(layout);
		builder.setPositiveButton(android.R.string.ok,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						connection.setVideoQuality(0, channel,
								Integer.valueOf(edit.getText().toString()),
								new DeviceResultHandler() {

									@Override
									public void onSuccess(DeviceResult arg0) {
										showToast("������Ƶ�����ɹ�");
									}

									@Override
									public void onFailure(DeviceResult arg0,
											int arg1) {
										showToast("������Ƶ����ʧ��--������:" + arg1);

									}
								});
					}
				});
		builder.setNegativeButton(android.R.string.cancel,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub

					}
				});
		builder.create().show();
	}

	*//**
	 * �����Ƶ��ת������ť�¼�
	 *//*
	private void onClickGainVideoScreenOrientation() {
		connection.getOrientation(0, channel, new DeviceResultHandler() {

			@Override
			public void onSuccess(DeviceResult result) {
				Orientation orientation = ((GetOrientaionResult) result)
						.getOrientation();
				String orientationStr = "";
				if (orientation == Orientation.HORIZONTAL) {
					orientationStr = HORIZONTAL;
				} else if (orientation == Orientation.VERTICAL) {
					orientationStr = VERTICAL;
				} else if (orientation == Orientation.UPRIGHT) {
					orientationStr = UPRIGHT;
				} else if (orientation == Orientation.TURN180) {
					orientationStr = TURN_180;
				}

				showToast("��Ƶ��ת����Ϊ:  " + orientation.getNum() + "--"
						+ orientationStr);
			}

			@Override
			public void onFailure(DeviceResult result, int arg1) {
				showToast("��ȡ��Ƶ��ת����ʧ��--������:" + arg1);

			}
		});

	}

	*//**
	 * ��ȡ��Ƶ������ť�¼�
	 *//*
	private void onClickGainVideoQuality() {
		int videoQuality = connection.getConnectionInfo()
				.getDefaultVideoQuality();
		showToast("��Ƶ����Ϊ: " + videoQuality);
	}

	*//**
	 * ��ͼ��ť�¼�
	 *//*
	private void onClickScreenShotBtn() {
		if (isVideoOpened) {
			// TODO ��bitmap���浽ָ����·����
			String filePath = null;
			if (Environment.getExternalStorageState().equals(
					Environment.MEDIA_MOUNTED)
					|| !Environment.isExternalStorageRemovable()) {
				filePath = Environment.getExternalStorageDirectory()
						.getAbsolutePath() + File.separator + "DanaleShot.png";
				File file = new File(filePath);
				if (!file.exists()) {
					try {
						file.createNewFile();
						Toast.makeText(this, filePath, Toast.LENGTH_LONG)
								.show();
						danalePlayer.screenShot(filePath, false);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		} else {
			showToast("������Ƶ����ʱ��ͼ");
		}

	}

	*//**
	 * ������رնԽ���ť�¼�
	 *//*
	private void onClickStartOrStopTalkBtn() {
		if (isTalkOpened == true) {
			startOrStopTalkBtn.setEnabled(false);
			connection.stopTalkBack(0, channel, new DeviceResultHandler() {

				@Override
				public void onSuccess(DeviceResult arg0) {
					isTalkOpened = false;
					startOrStopTalkBtn.setEnabled(true);
					startOrStopTalkBtn.setText("�����Խ�");
					showToast("�رնԽ��ɹ�");
					if (audioRecord != null) {
						audioRecord.stop();
					}
				}

				@Override
				public void onFailure(DeviceResult arg0, int arg1) {
					isTalkOpened = true;
					startOrStopTalkBtn.setEnabled(true);
					startOrStopTalkBtn.setText("�رնԽ�");
					showToast("�رնԽ�ʧ��--������:" + arg1);
				}
			});
		} else {
			startOrStopTalkBtn.setEnabled(false);
			connection.startTalkBack(0, channel, new DeviceResultHandler() {

				@Override
				public void onSuccess(DeviceResult arg0) {
					isTalkOpened = true;
					startOrStopTalkBtn.setEnabled(true);
					startOrStopTalkBtn.setText("�رնԽ�");
					showToast("�����Խ��ɹ�");
					// ¼���߳�
					new AsyncTask<Void, Void, Void>() {

						@Override
						protected Void doInBackground(Void... params) {
							audioRecord.startRecording();
							byte[] audioData = new byte[320];
							while (isTalkOpened) {
								audioRecord
										.read(audioData, 0, audioData.length);
								connection.sendTalkBackData(channel, audioData);
							}

							audioRecord.stop();
							return null;
						}
					}.execute();
				}

				@Override
				public void onFailure(DeviceResult arg0, int arg1) {
					isTalkOpened = false;
					startOrStopTalkBtn.setEnabled(true);
					startOrStopTalkBtn.setText("�����Խ�");
					showToast("�����Խ�ʧ��--������:" + arg1);
				}
			});
		}

	}

	*//**
	 * ������ر���Ƶ��ť�¼�
	 *//*
	private void onClickStartOrStopAudioBtn() {
		if (isAudioOpened == true) {
			startOrStopAudioBtn.setEnabled(false);
			connection.stopLiveAudio(0, channel, new DeviceResultHandler() {

				@Override
				public void onSuccess(DeviceResult arg0) {
					isAudioOpened = false;
					startOrStopAudioBtn.setText("������Ƶ");
					startOrStopAudioBtn.setEnabled(true);
					if (audioTrack != null) {
						audioTrack.stop();
					}
					showToast("�ر���Ƶ�ɹ�");
				}

				@Override
				public void onFailure(DeviceResult arg0, int arg1) {
					isAudioOpened = true;
					startOrStopAudioBtn.setText("�ر���Ƶ");
					startOrStopAudioBtn.setEnabled(true);
					showToast("�ر���Ƶʧ��--������:" + arg1);

				}
			});
		} else {
			startOrStopAudioBtn.setEnabled(false);
			final AudioInfo info = new AudioInfo();
			connection.startLiveAudio(0, channel, info,
					new LiveAudioReceiver() {

						// ������Ƶ���ݵĻص�����
						@Override
						public void onReceiveAudio(byte[] data) {
							if (isAudioOpened == true && audioTrack != null && audioTrack.getState() == AudioTrack.STATE_INITIALIZED) {
								audioTrack.write(data, 0, data.length);
							}
						}

					}, new DeviceResultHandler() {

						@Override
						public void onSuccess(DeviceResult arg0) {
							isAudioOpened = true;
							startOrStopAudioBtn.setText("�ر���Ƶ");
							startOrStopAudioBtn.setEnabled(true);

							if (audioTrack != null) {
								audioTrack.stop();
								audioTrack.release();
								audioTrack = null;
							}

							Log.d("audio", "info samplerate = " + info.sampleRate + "; sample bit = " + info.sampleBit + "; track = " + info.getAudioTrack());
							int bufferSizeInBytes = AudioTrack.getMinBufferSize(
									info.sampleRate,
									info.getAudioTrack() == com.danale.video.jni.DanaDevSession.AudioTrack.STEREO ? AudioFormat.CHANNEL_OUT_STEREO
											: AudioFormat.CHANNEL_OUT_MONO,
									info.sampleBit == 8 ? AudioFormat.ENCODING_PCM_8BIT
											: AudioFormat.ENCODING_PCM_16BIT);
							audioTrack = new AudioTrack(
									AudioManager.STREAM_MUSIC,
									info.sampleRate,
									info.getAudioTrack() == com.danale.video.jni.DanaDevSession.AudioTrack.STEREO ? AudioFormat.CHANNEL_OUT_STEREO
											: AudioFormat.CHANNEL_OUT_MONO,
									info.sampleBit == 8 ? AudioFormat.ENCODING_PCM_8BIT
											: AudioFormat.ENCODING_PCM_16BIT,
									bufferSizeInBytes, AudioTrack.MODE_STREAM);
							audioTrack.play();
							showToast("������Ƶ�ɹ�");
						}

						@Override
						public void onFailure(DeviceResult arg0, int arg1) {
							isAudioOpened = false;
							startOrStopAudioBtn.setText("������Ƶ");
							startOrStopAudioBtn.setEnabled(true);
							showToast("������Ƶʧ��--������:" + arg1);

						}
					});
		}

	}

	*//**
	 * ������ر���Ƶ��ť�¼�
	 *//*
	private void onClickStartOrStopVideoBtn() {
		if (isVideoOpened == true) {
			startOrStopVideoBtn.setEnabled(false);
			progressBar.setVisibility(View.VISIBLE);
			// ֹͣ��Ƶ����
			connection.stopLiveVideo(0, channel, danalePlayer,
					new DeviceResultHandler() {

						@Override
						public void onSuccess(DeviceResult arg0) {
							progressBar.setVisibility(View.GONE);
							isVideoOpened = false;
							startOrStopVideoBtn.setText("������Ƶ");
							startOrStopVideoBtn.setEnabled(true);
							danalePlayer.stop();
							danalePlayer.drawBlack();
							showToast("�ر���Ƶ�ɹ�");
						}

						@Override
						public void onFailure(DeviceResult arg0, int arg1) {
							progressBar.setVisibility(View.GONE);
							isVideoOpened = true;
							startOrStopVideoBtn.setText("�ر���Ƶ");
							startOrStopVideoBtn.setEnabled(true);
							showToast("�ر���Ƶʧ��--������:" + arg1);
						}
					});
		} else {
			startOrStopVideoBtn.setEnabled(false);
			progressBar.setVisibility(View.VISIBLE);
			// ������Ƶ����
			connection.startLiveVideo(0, channel, videoQuality, danalePlayer,
					new DeviceResultHandler() {

						@Override
						public void onSuccess(DeviceResult arg0) {
							isVideoOpened = true;
							startOrStopVideoBtn.setText("�ر���Ƶ");
							startOrStopVideoBtn.setEnabled(true);
							danalePlayer.preStart(true, DeviceType.IPC);
							showToast("������Ƶ�ɹ�");
						}

						@Override
						public void onFailure(DeviceResult arg0, int arg1) {
							progressBar.setVisibility(View.GONE);
							isVideoOpened = false;
							startOrStopVideoBtn.setText("������Ƶ");
							startOrStopVideoBtn.setEnabled(true);
							danalePlayer.stop();
							danalePlayer.drawBlack();
							showToast("������Ƶʧ��--������:" + arg1);
						}
					});
		}
	}

	*//**
	 * ͸������
	 *//*
	public void onClickTransferData() {
		byte[] data = new byte[100];
		// data��д���Զ������ݣ��мǴ�С��Ҫ����896;
		DeviceExtendByteResultHandler handler = new DeviceExtendByteResultHandler() {

			@Override
			public void onSuccess(byte[] arg0) {
				*//**
				 * �м���Ϊֻ��һ��͸������ͨ��,������Ϊ�Զ���,����sdk�޷������˵���Ӧ����;ͨ��͸��ͨ�����ص����ݴ˴��������,
				 * ��Ҫʹ���߸����Լ������data�� �ж��Ƿ����Լ���Ҫ������,���ȷ���Լ�Ҫ�����ݷ��غ�,
				 * ��removeDeviceExtendByteResultHandler;
				 *//*

				// .....�ж�arg0�Ƿ����Լ���Ҫ������

				// ������ص����Զ�������������Ҫremove�����handler���ͷ��ڴ�

				DeviceExtendDispatcher.getInstance()
						.removeIndistinctDeviceExtendResultHandler(this);

			}
		};
		connection.transferByteDatas(channel, data, handler);
	}

	private void showToast(String text) {
		Toast.makeText(DanaleVideoActivity.this, text, Toast.LENGTH_SHORT)
				.show();
	}

	private void showEditDialog(String title, String message, String hint) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(title);
		LinearLayout layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.VERTICAL);
		TextView msgTv = new TextView(this);
		msgTv.setText(message);
		layout.addView(msgTv);
		EditText edit = new EditText(this);
		edit.setHint(hint);
		layout.addView(edit);
		builder.setView(layout);
		builder.setPositiveButton(android.R.string.ok,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub

					}
				});
		builder.setNegativeButton(android.R.string.cancel,
				new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub

					}
				});
	}

	@Override
	public void onVideoTimout() {
		// ���ų�ʱ
		// ��������:��stop��Ƶ��start
	}

}
*/