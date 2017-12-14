package com.infotm.dv.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

public class PanelView extends View {
    private int mWidth;
    private int mHeight;
 
    private int mPercent=0;
 
    //刻度宽度
    private float mTikeWidth;
 
    //第二个弧的宽度
    private int mScendArcWidth;
 
    //最小圆的半径
    private int mMinCircleRadius;
 
    //文字矩形的宽
    private int mRectWidth;
 
    //文字矩形的高
    private int mRectHeight;
 
 
    //文字内容
    private String mText = "0km/H";
 
    //文字的大小
    private int mTextSize;
 
    //设置文字颜色
    private int mTextColor;
    private int mArcColor;
 
    //小圆和指针颜色
    private int mMinCircleColor;
 
    //刻度的个数
    private int mTikeCount=12;
 
    private Context mContext;
 
    public PanelView(Context context) {
        this(context, null);
    }
 
    public PanelView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }
 
    public PanelView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        mArcColor = Color.RED;
        mMinCircleColor =Color.GREEN;
        mTikeCount = 12;
        mTextSize = 36;
        mText ="0 Km/H";
        mScendArcWidth = 50;
    }
 
 
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int heightSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(widthMeasureSpec);
        if (widthMode == MeasureSpec.EXACTLY) {
            mWidth = widthSize;
        }else {
           // mWidth = PxUtils.dpToPx(200,mContext);
        }
 
 
      //  if (heightMode == MeasureSpec.EXACTLY) {
            mHeight = heightSize;
  //      }else {
        //    mHeight = PxUtils.dpToPx(200,mContext);3
     //   }
        Log.e("wing",mWidth+"");
        setMeasuredDimension(mWidth, mHeight);
    }
    
    
					@Override
					protected void onDraw(Canvas canvas) {
						// TODO Auto-generated method stub
						super.onDraw(canvas);
						
						Paint p = new Paint();
						   int strokeWidth = 4;
						   p.setStrokeWidth(strokeWidth);
						   p.setAntiAlias(true);
						   p.setStyle(Paint.Style.STROKE);
						   //p.setColor(Color.argb(255, 00, 102, 153));
						   p.setColor(Color.BLUE);
						   //最外面线条
						   canvas.drawArc(new RectF(strokeWidth, strokeWidth, mWidth - strokeWidth, mWidth - strokeWidth), 145, 250, false, p);
						   
						   p.setColor(Color.GREEN);
						   RectF secondRectF = new RectF(strokeWidth + 50, strokeWidth + 50, mWidth - strokeWidth - 50, mWidth - strokeWidth - 50);
					        float secondRectWidth = mWidth - strokeWidth - 50 - (strokeWidth + 50);
					        float secondRectHeight = mHeight - strokeWidth - 50 - (strokeWidth + 50);
					        float percent = mPercent ;
					        
					        //充满的圆弧的度数    -5是大小弧的偏差
					        float fill = percent ;
					 
					        //空的圆弧的度数
					        float empty = 250 - fill;
//					        Log.e("wing", fill + "");
					 
					        if(percent==0){
					            p.setColor(Color.GRAY);
					        }
					        //画粗弧突出部分左端
					        p.setStrokeWidth(40);
					        canvas.drawArc(secondRectF,135,11,false,p);
					        
					        canvas.drawArc(secondRectF, 145, fill, false, p);
					        
					        p.setColor(Color.GRAY);
					        //画弧胡的未充满部分
					        canvas.drawArc(secondRectF, 145 + fill, empty, false, p);
					        
					      //画粗弧突出部分右端
					        if(percent == 1){
					            p.setColor(Color.GREEN);
					        }
					        canvas.drawArc(secondRectF,144+fill+empty,10,false,p);
					        
					        p.setColor(Color.RED);
					        
					        
					      //绘制小圆外圈
					      p.setStrokeWidth(6);
					      canvas.drawCircle(mWidth / 2, mHeight / 2, 30, p);
					      
					      
					    //绘制小圆内圈
					      
					      p.setColor(Color.BLUE);
					      p.setStrokeWidth(10);
					      mMinCircleRadius = 15;
					      canvas.drawCircle(mWidth / 2, mHeight / 2, mMinCircleRadius, p);
					      
					      
					    //绘制刻度！
					      
					      p.setColor(Color.BLUE);
					      //绘制第一条最上面的刻度
					      mTikeWidth = 18;
					      p.setStrokeWidth(4);
					       
					      canvas.drawLine(mWidth / 2, 0, mWidth / 2, mTikeWidth, p);
					      //旋转的角度
					      float rAngle = 250f / mTikeCount;
					      //通过旋转画布 绘制右面的刻度
					      for (int i = 0; i < mTikeCount / 2; i++) {
					          canvas.rotate(rAngle, mWidth / 2, mHeight / 2);
					          canvas.drawLine(mWidth / 2, 2, mWidth / 2, mTikeWidth, p);
					      }
					       
					      //现在需要将将画布旋转回来
					      canvas.rotate(-rAngle * mTikeCount / 2, mWidth / 2, mHeight / 2);
					      
					    //通过旋转画布 绘制左面的刻度
					      for (int i = 0; i < mTikeCount / 2; i++) {
					          canvas.rotate(-rAngle, mWidth / 2, mHeight / 2);
					          canvas.drawLine(mWidth / 2, 2, mWidth / 2, mTikeWidth, p);
					      }
					       
					       
					      //现在需要将将画布旋转回来
					      canvas.rotate(rAngle * mTikeCount / 2, mWidth / 2, mHeight / 2);
					      
					      
					    //绘制指针
					      
					      p.setColor(Color.BLUE);
					      p.setStrokeWidth(10);
					       
					       
					       
					      //按照百分比绘制刻度
					      canvas.rotate(( 250 * percent - 250/2), mWidth / 2, mHeight / 2);
					       
					      canvas.drawLine(mWidth / 2, (mHeight / 2 - secondRectHeight / 2) + mScendArcWidth / 2 + 2, mWidth / 2, mHeight / 2 - mMinCircleRadius, p);
					       
					      //将画布旋转回来
					      canvas.rotate(-( 250 * percent - 250/2), mWidth / 2, mHeight / 2);
					      
					      
					    //绘制矩形
					      p.setStyle(Paint.Style.FILL);
					      p.setColor(Color.RED);
					      mRectWidth = 100;
					      mRectHeight = 50;
					   
					      //文字矩形的最底部坐标
					      float rectBottomY = mHeight/2 + secondRectHeight/3+mRectHeight;
					      canvas.drawRect(mWidth/2-mRectWidth/2,mHeight/2 + secondRectHeight/3,mWidth/2+mRectWidth/2,rectBottomY,p);
					   
					   
					      p.setTextSize(mTextSize);
					      mTextColor = Color.BLUE;
					      p.setColor(mTextColor);
					      float txtLength = p.measureText(mText);
					      canvas.drawText(mText,(mWidth-txtLength)/2,rectBottomY + 40,p);
					   
					      super.onDraw(canvas);
					}
					
					
					/**
					    * 设置百分比
					    * @param percent
					    */
					   public void setPercent(int percent) {
					       mPercent = percent;
					       invalidate();
					   }
					 
					   /**
					    * 设置文字
					    * @param text
					    */
					   public void setText(String text){
					       mText = text;
					       invalidate();
					   }
					 
					   /**
					    * 设置圆弧颜色
					    * @param color
					    */
					 
					   public void setArcColor(int color){
					       mArcColor = color;
					 
					       invalidate();
					   }
					 
					 
					   /**
					    * 设置指针颜色
					    * @param color
					    */
					   public void setPointerColor(int color){
					       mMinCircleColor = color;
					 
					       invalidate();
					   }
					 
					   /**
					    * 设置文字大小
					    * @param size
					    */
					   public void setTextSize(int size){
					       mTextSize = size;
					 
					       invalidate();
					   }
					 
					   /**
					    * 设置粗弧的宽度
					    * @param width
					    */
					   public void setArcWidth(int width){
					       mScendArcWidth = width;
					 
					       invalidate();
					   }
}
