package com.my.ndkdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private TextView tv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv = findViewById(R.id.textViewId);
    }

    public void click1(View view) {
        String str = NDKUtil.stringFromJNI();
        tv.setText(str);
    }

    public void click2(View view) {
        NDKUtil.setJNILogEnable(2);
        tv.setText("你的参数:" + 2);
    }

    public void click3(View view) {
        int type = NDKUtil.getVersionCode();
        tv.setText("动态注册的Native方法 type = " + type);
    }

    //动态注册的Native方法 有参数有返回值
    public void click4(View view) {
        String version = NDKUtil.getVersion(3);
        tv.setText("version = " + version);
    }

    // 动态注册Native方法，并且C回调Java方法
    public void click5(View view) {
        NDKUtil.callJavaString();
        tv.setText("动态注册Native方法，并且C回调Java方法");
    }

}
