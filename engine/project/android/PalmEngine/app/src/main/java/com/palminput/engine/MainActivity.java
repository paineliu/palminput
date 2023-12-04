package com.palminput.engine;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.palminput.engine.PYOptions;
import com.palminput.engine.PYProvider;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    PYProvider mPYProvider = new PYProvider();
    PinyinAdapter mRVAdapter;
    RecyclerView mRV;
    TextView   mComp;
    TextView   mEditBox;
    int mCommitTimes = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mPYProvider.create(this.getAssets(), "pinyin/", this.getFilesDir().toString());
//        PYOptions pyOptions = mPYProvider.getOptions();
//        pyOptions.mFuzzyFlags = PYOptions.PY_FUZZY_JP_S_SH | PYOptions.PY_FUZZY_JP_C_CH | PYOptions.PY_FUZZY_JP_Z_ZH;
//        pyOptions.mPredictEnable = true;
//        mPYProvider.setOptions(pyOptions);

        mEditBox = this.findViewById(R.id.tvEditBox);

        mComp = this.findViewById(R.id.tvComp);

        mRVAdapter = new PinyinAdapter(this, mPYProvider);
        mRV = this.findViewById(R.id.rvCand);

        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.HORIZONTAL);
        mRV.setLayoutManager(manager);

        mRV.setAdapter(mRVAdapter);

        mRVAdapter.setOnItemClickListener(new PinyinAdapter.OnItemClickListener(){
            @Override
            public void onItemClick(View view , int position){
                selItem(position);
            }
        });

        this.findViewById(R.id.bt_a).setOnClickListener(this);
        this.findViewById(R.id.bt_b).setOnClickListener(this);
        this.findViewById(R.id.bt_c).setOnClickListener(this);
        this.findViewById(R.id.bt_d).setOnClickListener(this);
        this.findViewById(R.id.bt_e).setOnClickListener(this);
        this.findViewById(R.id.bt_f).setOnClickListener(this);
        this.findViewById(R.id.bt_g).setOnClickListener(this);
        this.findViewById(R.id.bt_h).setOnClickListener(this);
        this.findViewById(R.id.bt_i).setOnClickListener(this);
        this.findViewById(R.id.bt_j).setOnClickListener(this);
        this.findViewById(R.id.bt_k).setOnClickListener(this);
        this.findViewById(R.id.bt_l).setOnClickListener(this);
        this.findViewById(R.id.bt_m).setOnClickListener(this);
        this.findViewById(R.id.bt_n).setOnClickListener(this);
        this.findViewById(R.id.bt_o).setOnClickListener(this);
        this.findViewById(R.id.bt_p).setOnClickListener(this);
        this.findViewById(R.id.bt_q).setOnClickListener(this);
        this.findViewById(R.id.bt_r).setOnClickListener(this);
        this.findViewById(R.id.bt_s).setOnClickListener(this);
        this.findViewById(R.id.bt_t).setOnClickListener(this);
        this.findViewById(R.id.bt_u).setOnClickListener(this);
        this.findViewById(R.id.bt_v).setOnClickListener(this);
        this.findViewById(R.id.bt_w).setOnClickListener(this);
        this.findViewById(R.id.bt_x).setOnClickListener(this);
        this.findViewById(R.id.bt_y).setOnClickListener(this);
        this.findViewById(R.id.bt_z).setOnClickListener(this);

        this.findViewById(R.id.bt_del).setOnClickListener(this);
        this.findViewById(R.id.bt_enter).setOnClickListener(this);
        this.findViewById(R.id.bt_reset).setOnClickListener(this);
    }


    private void selItem(int id)
    {
        if (mPYProvider.selCandItem(id) == 1) {
            String editString = mEditBox.getText().toString() + mPYProvider.getCommitString();
            mEditBox.setText(editString);

            // 提交超过一定次数调用保存
            mCommitTimes++;
            if (mCommitTimes >= 5) {
                mPYProvider.save();
                mCommitTimes = 0;
            }

            mPYProvider.reset();
        }

        updateViews();
    }

    private void updateViews() {
        mRV.scrollToPosition(0);
        mRVAdapter.notifyDataSetChanged();
        String strComp = mPYProvider.getCompString();
        mComp.setText(strComp);
    }

    @Override
    public void onClick(View v) {
        Button bt = (Button) v;
        if (bt.getId() == R.id.bt_del){
            if (mPYProvider.getCommitString().length() > 0) {
                mPYProvider.addKey('\b');
            }
            else if (mEditBox.getText().length() > 0) {
                mEditBox.setText(mEditBox.getText().subSequence(0, mEditBox.getText().length() - 1));
            }
        }
        else if (bt.getId() == R.id.bt_reset) {
            mEditBox.setText("");
            mPYProvider.reset();
        }
        else if (bt.getId() == R.id.bt_enter) {
            String editString = mEditBox.getText().toString() + mPYProvider.getCommitString();
            mEditBox.setText(editString);
            mPYProvider.reset();
        }
        else {
            int key = bt.getText().charAt(0);
            mPYProvider.addKey(key);
        }

        updateViews();
    }

    @Override
    protected void onDestroy() {
        mPYProvider.destroy();
        super.onDestroy();
    }
}
