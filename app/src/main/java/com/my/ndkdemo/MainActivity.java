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
        NDKUtil.setJNILogEnable(1);
        tv.setText("type为1有日志，为0无日志");
    }

    public void click3(View view) {
        int verison = NDKUtil.getVersionCode();
        tv.setText("verison = " + verison);
    }

    //动态注册的Native方法 有参数有返回值
    public void click4(View view) {
        String version = NDKUtil.getVersion(3);
        tv.setText("version = " + version);
    }

    // 动态注册Native方法，并且C回调Java方法
    public void click5(View view) {
        NDKUtil.callJavaVoid();
        tv.setText("带参数不带返回值");
    }

    // 动态注册Native方法，并且C回调Java方法
    public void click6(View view) {
        String resutl = NDKUtil.callJavaString();
        tv.setText("带参数带返回值:" + resutl);
    }
}
