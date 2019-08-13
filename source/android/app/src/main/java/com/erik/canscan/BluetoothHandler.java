package com.erik.canscan;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

public class BluetoothHandler implements Runnable{
    BluetoothSocket BTSocket;
    static String TAG = "CanScan:BTHandler";

   public BluetoothHandler(BluetoothDevice selectedDevice)
    {
        String result = serialReceiveCallback("foobar");
        try {
            BTSocket = selectedDevice.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
        } catch (IOException e) {
            Log.e(TAG,"wordt hem niet bro");
            return;
        }
        try {
            // Connect to the remote device through the socket. This call blocks
            // until it succeeds or throws an exception.
            BTSocket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and return.
            Log.e(TAG,"niet verbonden");
            try {
                BTSocket.close();
            } catch (IOException closeException) {
                Log.e(TAG, "Could not close the client socket", closeException);
            }
            return;
        }

        Log.e(TAG, "Hah binnnen");
        Thread messageListener = new Thread(this);
        messageListener.run();

    }
    public void run() {
        int bufferSize = 1024;
        byte[] buffer = new byte[bufferSize];
        try {
            InputStream instream = BTSocket.getInputStream();
            int bytesRead = -1;
            String message = "";
            while (true) {
                message = "";
                bytesRead = instream.read(buffer);
                if (bytesRead != -1) {
                    while ((bytesRead==bufferSize)&&(buffer[bufferSize-1] != 0)) {
                        message = message + new String(buffer, 0, bytesRead);
                        bytesRead = instream.read(buffer);
                    }
                    message = message + new String(buffer, 0, bytesRead - 1);
                    Log.d(TAG,"Got message: " + message);
                    BTSocket.getInputStream();
                }
            }
        } catch (IOException e) {
            Log.d("BLUETOOTH_COMMS", e.getMessage());
        }
    }
    boolean sendMessage(String data){
        try
        {
            BTSocket.getOutputStream().write(data.getBytes());
        } catch (IOException e) {
            return false;
        }
        return true;
    }
//    boolean sendString(String data);
//    void registerCallback(std::function<void(std::string)> callback);
    public native String serialReceiveCallback(String message);
}
