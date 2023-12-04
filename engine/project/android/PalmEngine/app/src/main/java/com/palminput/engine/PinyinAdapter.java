package com.palminput.engine;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.palminput.engine.PYProvider;

public class PinyinAdapter extends RecyclerView.Adapter<PinyinAdapter.DataViewHolder> implements View.OnClickListener{
    private Context mContext;
    private PYProvider mProvider;
    private OnItemClickListener mOnItemClickListener = null;
    public PinyinAdapter() {
    }

    public PinyinAdapter(Context mContext, PYProvider pyProvider) {
        this.mContext = mContext;
        this.mProvider = pyProvider;
    }

    @Override
    public void onClick(View v) {
        if (mOnItemClickListener != null) {
            mOnItemClickListener.onItemClick(v,(int)v.getTag());
        }
    }

    @Override
    public DataViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(mContext).inflate(R.layout.item, null);
        view.setLayoutParams(new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        DataViewHolder holder = new DataViewHolder(view);

        view.setOnClickListener(this);
        return holder;
    }

    @Override
    public void onBindViewHolder(DataViewHolder holder, int position) {
        holder.tv_data.setText(mProvider.getCandItemString(position));
        holder.itemView.setTag(position);

    }

    @Override
    public int getItemCount() {
        return mProvider.getCandItemTotal();
    }

    public void setOnItemClickListener(OnItemClickListener listener) {
        this.mOnItemClickListener = listener;
    }

    public interface OnItemClickListener {
        void onItemClick(View view , int position);
    }

    public static class DataViewHolder extends RecyclerView.ViewHolder {
        TextView tv_data;

        public DataViewHolder(View itemView) {
            super(itemView);
            tv_data = itemView.findViewById(R.id.tv_recycle);
        }
    }
}
