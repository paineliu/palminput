package com.palminput.keyboard.skin;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.io.InputStream;
import java.util.HashMap;

public class SkinResPool {
    private final AssetManager mAssetManager;
    private final int mKeyboardWidth;
    private final String mPathName;

    public SkinResPool(Context context, String pathName, int keyboardWidth) {
        mKeyboardWidth = keyboardWidth;
        mPathName = pathName;
        mAssetManager = context.getResources().getAssets();
    }

    HashMap<String, Bitmap> mapBitmap =  new HashMap<String, Bitmap>();

    public Bitmap getBitmap(String fileName) {

        String fullWidthName = mPathName +  "/" + mKeyboardWidth + "/" + fileName;
        if (mapBitmap.containsKey(fullWidthName)) {
            return mapBitmap.get(fullWidthName);
        }

        String fullName = mPathName +  "/" + fileName;
        if (mapBitmap.containsKey(fullName)) {
            return mapBitmap.get(fullName);
        }

        Bitmap bitmap = loadBitmapFromAssetFilename(fullWidthName);
        if (bitmap != null) {
            mapBitmap.put(fileName, bitmap);
            return bitmap;
        }

        bitmap = loadBitmapFromAssetFilename(fullName);
        if (bitmap != null) {
            mapBitmap.put(fullName, bitmap);
            return bitmap;
        }

        return null;
    }

    private Bitmap loadBitmapFromAssetFilename(String filename) {
        InputStream inputStream = null;
        Bitmap bitmap = null;

        try {
            inputStream = mAssetManager.open(filename);
            bitmap = BitmapFactory.decodeStream(inputStream);

        } catch (Exception e) {
            //e.printStackTrace();
        }

        return bitmap;
    }
}
