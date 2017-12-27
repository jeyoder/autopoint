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

public class BluetoothManager extends Thread{
    /* The BluetoothManager runs a loop on its own thread, following
     * a "state machine" that responds to events and external commands.
     */

    public enum State {
        IDLE,
        WAIT_FOR_BLUETOOTH,
        CONNECTING,
        CONNECTED,
        CONNECTION_ERROR
    };
    private volatile State state = State.IDLE;

    public State getCurrentState() {
        return state;
    }

    public enum Command {
        CONNECT,
        DIE
    }
    protected volatile Command command = null;

    private static final int REQUEST_ENABLE_BT = 1;

    private MainActivity activity;

    private BluetoothDevice connectedDevice;
    private BluetoothSocket socket;

    public BluetoothManager(MainActivity act) {
        this.activity = act;
    }

    /* Search for the laser device on the given BluetoothAdapter.
     * Returns null if not found.
     */
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

    public synchronized void startConnecting() {
        command = Command.CONNECT;
        this.notify();
    }

    public synchronized void kill() {
        command = Command.DIE;
        this.notify();
    }

    public void run() {
        while(true) {
           if(state == State.IDLE) {
               // Spin in IDLE until we're told otherwise
               if(command == Command.CONNECT) {
                   BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
                   if (adapter == null) {
                    /* no bluetooth :( */
                       System.out.println("get you some bluetooth");
                       state = State.CONNECTION_ERROR;
                   }

                /* enable the blue teeth */
                   if (!adapter.isEnabled()) {
                       Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                       activity.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                       state = State.WAIT_FOR_BLUETOOTH;
                   } else {
                       state = State.CONNECTING;
                   }
               }
           } else if (state == State.WAIT_FOR_BLUETOOTH) {
                BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
                if (adapter.isEnabled()) {
                    state = State.CONNECTING;
                }
           } else if (state == State.CONNECTING) {
               BluetoothDevice dev = findDevice(BluetoothAdapter.getDefaultAdapter());
               if (dev == null) {
                   System.out.println("No device :(");
                   state = State.CONNECTION_ERROR;
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
                   state = State.CONNECTED;
               } catch (IOException ex) {
                   ex.printStackTrace();
                   state = State.CONNECTION_ERROR;
               }
           } else if (state == State.CONNECTED) {
                String pkt = "$1:69:420:69\n";
                try {
                    socket.getOutputStream().write(pkt.getBytes());
                } catch (IOException ex) {
                    state = State.CONNECTION_ERROR;
                }
           }

           if(command == Command.DIE) {
               if(state == State.CONNECTED) {
                   try {
                       socket.close();
                   } catch (IOException ex) {
                   }
               }

               System.out.println("BluetoothManager is dying.......");
               return;
           }

           try{
               synchronized (this) {
                   this.wait(100);
               }
           } catch (InterruptedException ex) {
           }
        }
    }
}
