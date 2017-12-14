package com.infotm.dv;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.util.Log;
import android.widget.RemoteViews;
import android.widget.Toast;

import com.infotm.dv.camera.InfotmCamera;

public class DownloadTask  extends AsyncTask<URL, Long, Boolean>{

	String mFileName ;
	Context mContext ;
	WifiLock mWifiLock ;
	String mIp ;
	Handler mHandler;
	boolean mCancelled ;
	PowerManager.WakeLock mWakeLock ;
   private  int oldProcess;
   private NotificationManager manager;
   private Notification notification;
   private RemoteViews contentView;
   private String mfile;


	//private ProgressDialog mProgressDialog ;
	

	DownloadTask(Context context,String file) {

		mContext = context ;
		manager = (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
	    notification = new Notification(R.drawable.ic_launcher, "下载进度",System.currentTimeMillis());
	    contentView = new RemoteViews(mContext.getPackageName(), R.layout.download_notify_layout);
	   contentView.setProgressBar(R.id.pb, 100, 0, false);
	   contentView.setTextViewText(R.id.tv,  file+"下载进度");
	   notification.contentView = contentView;
	    manager.notify(0, notification);
	    mfile=file;
	}

	@Override
	protected void onPreExecute() {

		Log.i("DownloadTask", "onPreExecute") ;

		WifiManager wm = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE) ;
		mWifiLock = wm.createWifiLock(WifiManager.WIFI_MODE_FULL, "DownloadTask") ;
		mWifiLock.acquire() ;

		PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE) ;
		mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "DownloadTask") ;
		mWakeLock.acquire() ;

		mCancelled = false ;
		//showProgress(mContext) ;

		super.onPreExecute() ;
	}

	@Override
	protected Boolean doInBackground(URL... urls) {

		Log.i("DownloadTask", "doInBackground " + urls[0]) ;

		try {
			mIp = urls[0].getHost() ;

			HttpURLConnection urlConnection = (HttpURLConnection) urls[0].openConnection() ;
			urlConnection.setRequestMethod("GET") ;
			urlConnection.setConnectTimeout(3000) ;
			urlConnection.setReadTimeout(10000) ;
			urlConnection.setUseCaches(false) ;
			urlConnection.setDoInput(true) ;
			urlConnection.setRequestProperty("Accept-Encoding", "identity");
			urlConnection.connect() ;
			
			InputStream inputStream = urlConnection.getInputStream() ;

			mFileName = urls[0].getFile().substring(urls[0].getFile().lastIndexOf(File.separator) + 1) ;
			File appDir =null;
			
			String ssid="media";
				if(mfile.contains("LOCK")){
					appDir = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+ File.separator +ssid+File.separator +"Locked") ;
				}else if(mfile.endsWith(".mp4")||mfile.endsWith(".mkv")){
					appDir = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +ssid+File.separator +"Video") ;
				}else if(mfile.endsWith(".jpg")){
					appDir = new File(Environment.getExternalStorageDirectory().toString()+ File.separator +"Infotm"+File.separator +ssid+File.separator +"Photo") ;
				}

			
			if (!appDir.exists()) {
				appDir.mkdirs() ;
			}
			
			File file = new File(appDir, mFileName) ;

			Log.i("DownloadTask", "Path:"+appDir.getPath() + File.separator + mFileName) ;

			if (file.exists()) {
				file.delete() ;
			}

			file.createNewFile() ;
			FileOutputStream fileOutput = new FileOutputStream(file) ;

			byte[] buffer = new byte[1024] ;
			int bufferLength = 0 ;
			try {
				Log.i("progress", "Content-Length: " + Long.valueOf(urlConnection.getContentLength()));
				while ((bufferLength = inputStream.read(buffer)) > 0) {
					publishProgress(Long.valueOf(urlConnection.getContentLength()), file.length());
					//Log.i("Write", "Write " + bufferLength);
					fileOutput.write(buffer, 0, bufferLength) ;
					if (mCancelled) {
						urlConnection.disconnect();
						break ;
					}
				}
			} finally {

			}
			if (mCancelled && file.exists()) {
				file.delete() ;
				return false ;
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace() ;
			return false ;
		}

		return true ;
	}

	@Override
	protected void onProgressUpdate(Long... values) {

		//if (mProgressDialog != null) {
			int max = values[0].intValue()/1024 ;
			int current=values[1].intValue()/1024;
			int progress =current*100/max;
			//Log.i("download","max:"+max+"KB----------process:"+current+"KB");
			//Log.i("download","progress:"+progress);
			if(oldProcess!=progress){
				Log.d("download","update  progress:"+progress);
				oldProcess=progress;
			   contentView.setProgressBar(R.id.pb, 100, progress, false);
			   notification.contentView = contentView;
			    manager.notify(0, notification);
			}

			//mProgressDialog.setTitle("下载文件： " + mFileName) ;
/*			if (max != -1) {
				if (max > 1024) {
					max /= 1024 ;
					progress /= 1024 ;
					unit = "KB" ;
				}
				
				if (max > 1024) {
					max /= 1024 ;
					progress /= 1024 ;
					unit = "MB" ;
				}
			} else {
				progress /= (1024 * 1024);
				max = progress * 2;
				unit = "KB" ;
			}
			Log.i("Progress", "Progress----------> " + progress);*/
			//mProgressDialog.setMax((int)max) ;
			//mProgressDialog.setProgress((int)progress) ;
			//mProgressDialog.setProgressNumberFormat("%1d/%2d " + unit) ;
		//}
		super.onProgressUpdate(values) ;
	}

	private void cancelDownload() {

		mCancelled = true ;
		
		/*for (FileNode fileNode : sSelectedFiles) {
			fileNode.mSelected = false ;
		}
		sSelectedFiles.clear() ;
		mFileListAdapter.notifyDataSetChanged() ;
	*/}

	@Override
	protected void onCancelled() {

		Log.i("DownloadTask", "onCancelled") ;

/*		if (mProgressDialog != null) {
			mProgressDialog.dismiss() ;
			mProgressDialog = null ;
		}*/
		//sDownloadTask = null ;

		mWakeLock.release() ;
		mWifiLock.release() ;

		super.onCancelled() ;
	}

	@Override
	protected void onPostExecute(Boolean result) {

		Log.i("DownloadTask", "onPostExecute " + mFileName + " " + (mCancelled ? "CANCELLED"
				: result ? "SUCCESS" : "FAIL")) ;
		Toast.makeText(mContext, mFileName+"下载成功 ！", 2000).show();
		manager.cancel(0);
		mWakeLock.release() ;
		mWifiLock.release() ;
		super.onPostExecute(result) ;
	}

}
