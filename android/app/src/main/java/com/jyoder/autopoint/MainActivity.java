package com.jyoder.autopoint;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.constraint.ConstraintLayout;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.text.Layout;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    private LinearLayout contentLayout;
    private View connectionView;
    private View controlView;
    private View targetingView;
    private View settingsView;

    private Button connectButton;

    private BluetoothManager bluetooth;

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_connection:
                    switchContents(connectionView);
                    break;
                case R.id.navigation_control:
                    switchContents(controlView);
                    break;
                case R.id.navigation_settings:
                    switchContents(settingsView);
                    break;
                case R.id.navigation_targeting:
                    switchContents(targetingView);
                    break;
            }
            return false;
        }
    };

    protected void switchContents(View view) {
        contentLayout.removeAllViews();
        contentLayout.addView(view);
    }

    protected void tryBluetoothConnect() {
        boolean success = bluetooth.connect();

        if(success) {
            Toast.makeText(this, "You are connected now k", Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(this, "FAILURE :(", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final MainActivity activity = this;

        bluetooth = new BluetoothManager(this);

        setContentView(R.layout.activity_main);

        contentLayout = findViewById(R.id.contents);
        connectionView = getLayoutInflater().inflate(R.layout.connection, null);
     //   controlView = getLayoutInflater().inflate(R.layout.control, null);
     //   settingsView = getLayoutInflater().inflate(R.layout.settings, null);
     //   targetingView = getLayoutInflater().inflate(R.layout.targeting, null);

        switchContents(connectionView);

        connectButton = findViewById(R.id.connect_button);
        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                activity.tryBluetoothConnect();
            }
        });

        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);

    }

}
