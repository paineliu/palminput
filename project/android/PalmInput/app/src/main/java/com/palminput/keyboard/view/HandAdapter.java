package com.palminput.keyboard.view;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.palminput.keyboard.R;
import com.palminput.keyboard.engine.HWProvider;
import com.palminput.keyboard.engine.PYProvider;

public class HandAdapter extends RecyclerView.Adapter<HandAdapter.DataViewHolder> implements View.OnClickListener{
    private Context mContext;
    private HWProvider mProvider;
    static public int mBkColor;
    static public int mTextColor;

    private OnItemClickListener mOnItemClickListener = null;
    public HandAdapter() {
    }

    public HandAdapter(Context mContext, HWProvider provider, int bkColor, int textColor) {
        this.mContext = mContext;
        this.mProvider = provider;
        HandAdapter.mBkColor = bkColor;
        HandAdapter.mTextColor = textColor;
    }

    @Override
    public void onClick(View v) {
        if (mOnItemClickListener != null) {
            //注意这里使用getTag方法获取position
            mOnItemClickListener.onItemClick(v,(int)v.getTag());
        }
    }
    //用于创建ViewHolder
    @Override
    public DataViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(mContext).inflate(R.layout.item, null);
        //使用代码设置宽高（xml布局设置无效时）
        view.setLayoutParams(new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        DataViewHolder holder = new DataViewHolder(view);

        view.setOnClickListener(this);
        return holder;
    }

    //绑定数据
    @Override
    public void onBindViewHolder(DataViewHolder holder, int position) {
        holder.tv_data.setText(mProvider.getCandItemString(position));
        holder.tv_data.setBackgroundColor(HandAdapter.mBkColor);
        holder.tv_data.setTextColor(HandAdapter.mTextColor);
        holder.itemView.setTag(position);

    }

    //数据总数
    @Override
    public int getItemCount() {
        return mProvider.getCandItemTotal();
    }

    //define interface
    public void setOnItemClickListener(OnItemClickListener listener) {
        this.mOnItemClickListener = listener;
    }

    public interface OnItemClickListener {
        void onItemClick(View view , int position);
    }
    //创建ViewHolder
    public static class DataViewHolder extends RecyclerView.ViewHolder {
        TextView tv_data;

        public DataViewHolder(View itemView) {
            super(itemView);
            tv_data = itemView.findViewById(R.id.tv_recycle);
        }
    }
}