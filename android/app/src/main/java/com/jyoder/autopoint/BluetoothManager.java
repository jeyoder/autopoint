package com.jyoder.autopoint;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.ParcelUuid;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

/**
 * Created by james on 12/25/2017.
 */

public class BluetoothManager {

    private static final int REQUEST_ENABLE_BT = 1;

    private MainActivity activity;

    private boolean connected = false;
    private BluetoothDevice connectedDevice;
    private BluetoothSocket socket;

    public BluetoothManager(MainActivity act) {
        this.activity = act;
    }

    private BluetoothDevice findDevice(BluetoothAdapter adapter) {
        /* Check if the device we want is already paired */
        Set<BluetoothDevice> pairedDevices = adapter.getBondedDevices();

        if (pairedDevices.size() > 0) {
            // There are paired devices. Get the name and address of each paired device.
            for (BluetoothDevice device : pairedDevices) {
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address

                if (deviceName.equals("laser")) {
                    System.out.println("Found em boss");
                    return device;
                }
            }
        }

        return null;
    }

    public boolean connect() {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter == null) {
            /* no bluetooth :( */
            System.out.println("get you some bluetooth");
            return false;
        }

        /* enable the blue teeth */
        /* TODO: make this happen independent from MainActivity... */
        if (!adapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            activity.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            return false;
        }

        BluetoothDevice dev = findDevice(adapter);
        if (dev == null) {
            System.out.println("No device :(");
            return false;
        }

        /* got the device, open an rfcomm channel */
        try {
            for(ParcelUuid i : dev.getUuids()) {
                System.out.println(i);
            }

            BluetoothSocket sock = dev.createInsecureRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"));
            sock.connect();
            socket = sock;
            connectedDevice = dev;
            connected = true;
            return true;
        } catch (IOException ex) {
            ex.printStackTrace();
            return false;
        }
    }
}
