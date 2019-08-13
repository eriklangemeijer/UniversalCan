package com.erik.canscan;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener{
    static String TAG = "CanScan";
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    List<String> things;
    BluetoothDevice selectedDevice;
    BluetoothAdapter BTAdapter;
    Set<BluetoothDevice> pairedDevices;
    BluetoothSocket BTSocket = null;
    BluetoothHandler handler;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        BTAdapter = BluetoothAdapter.getDefaultAdapter();
        // Phone does not support Bluetooth so let the user know and exit.
        if (BTAdapter == null) {
            new AlertDialog.Builder(this)
                    .setTitle("Not compatible")
                    .setMessage("Your device does not support Bluetooth")
                    .setPositiveButton("Exit", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            System.exit(0);
                        }
                    })
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .show();
        }
        Spinner spin = findViewById(R.id.sample_text);
        spin.setOnItemSelectedListener(this);
        refreshBluetoothList();
        Button btnConnect = findViewById(R.id.connect_button);
        btnConnect.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //Toast.makeText(getApplicationContext(), "Selected User: "+selectedDevice.getName() ,Toast.LENGTH_SHORT).show();
                handler = new BluetoothHandler(selectedDevice);

            }
        });
        Button btnRefresh = findViewById(R.id.refresh_button);
        btnRefresh.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                refreshBluetoothList();
            }
        });
        nativeMain();
    }

    private void refreshBluetoothList()
    {
        if (!BTAdapter.isEnabled()) {
            Intent enableBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBT, 1);
        }
        pairedDevices = BTAdapter.getBondedDevices();
        // Example of a call to a native method
        things = new ArrayList<String>();
        for (BluetoothDevice device : pairedDevices) {
            things.add(device.getName());
        }
        Spinner spin = (Spinner)findViewById(R.id.sample_text);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, things);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spin.setAdapter(adapter);

    }

    @Override
    public void onItemSelected(AdapterView<?> arg0, View arg1, int position, long id) {
        selectedDevice = (BluetoothDevice)pairedDevices.toArray()[position];
    }
    @Override
    public void onNothingSelected(AdapterView<?> arg0) {
        // TODO - Custom Code
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String nativeMain();
    
}
