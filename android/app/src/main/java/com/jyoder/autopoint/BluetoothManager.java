package com.jyoder.autopoint;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;

/**
 * Created by james on 12/25/2017.
 */

public class BluetoothManager {

    private static final int REQUEST_ENABLE_BT = 1;

    private MainActivity activity;

    public BluetoothManager(MainActivity act) {
        this.activity = act;
    }

    public void connect() {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter == null) {
            /* no bluetooth :( */
            return;
        }

        /* enable the blue teeth */
        if (!adapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            activity.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }


    }
}
