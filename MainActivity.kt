package com.example.omnibotcontrol

import android.annotation.SuppressLint
import android.app.Application
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Intent
import android.os.Bundle
import android.view.MotionEvent
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.ImageButton
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintSet
import com.google.gson.Gson
import io.github.controlwear.virtual.joystick.android.JoystickView
import io.github.controlwear.virtual.joystick.android.JoystickView.OnMoveListener
import java.io.IOException
import java.util.*




class MainActivity : AppCompatActivity() {
    var bluetoothSocket: BluetoothSocket? = null
    lateinit var bluetoothBondedDevices: Set<BluetoothDevice>
    lateinit var bluetoothAdapter: BluetoothAdapter

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val button_connect: Button = findViewById(R.id.button_connect)
        val button_disconnect: Button = findViewById(R.id.button_disconnect)
        val text_connection: TextView = findViewById(R.id.textView_bt_status)
        val button_up: ImageButton = findViewById(R.id.button_up)
        val button_down: ImageButton = findViewById(R.id.button_down)
        val button_right: ImageButton = findViewById(R.id.button_right)
        val button_left: ImageButton = findViewById(R.id.button_left)
        val button_upright: ImageButton = findViewById(R.id.button_upright)
        val button_downright: ImageButton = findViewById(R.id.button_downright)
        val button_downleft: ImageButton = findViewById(R.id.button_downleft)
        val button_upleft: ImageButton = findViewById(R.id.button_upleft)
        val button_spincw: ImageButton = findViewById(R.id.button_spincw)
        val button_spinccw: ImageButton = findViewById(R.id.button_spinccw)
        val textView_angle: TextView = findViewById(R.id.textView_angle)
        val textView_strength: TextView = findViewById(R.id.textView_strength)
        val joystickView: JoystickView = findViewById(R.id.joystickView)
        val button_armControl: Button = findViewById(R.id.button_armControl)

        val button_cz1up: Button = findViewById(R.id.button_cz1up)
        val button_cz1down: Button = findViewById(R.id.button_cz1down)
        val button_cz2up: Button = findViewById(R.id.button_cz2up)
        val button_cz2down: Button = findViewById(R.id.button_cz2down)
        val button_chwup: Button = findViewById(R.id.button_chwup)
        val button_chwdown: Button = findViewById(R.id.button_chwdown)
        val button_home: Button = findViewById(R.id.button_home)
        val editTextNumber_cz1: EditText = findViewById(R.id.editTextNumber_cz1)
        val editTextNumber_cz2: EditText = findViewById(R.id.editTextNumber_cz2)
        val editTextNumber_chw: EditText = findViewById(R.id.editTextNumber_chw)
        val button_setPosition: Button = findViewById(R.id.button_setPosition)
        val button_back: Button = findViewById(R.id.button_back)
        val textView_0: TextView = findViewById(R.id.textView)
        val textView_angleLabel: TextView = findViewById(R.id.textView1)
        val textView_strengthLabel: TextView = findViewById(R.id.textView2)
        val textView_1: TextView = findViewById(R.id.textView3)
        val textView_2: TextView = findViewById(R.id.textView4)
        val textView_3: TextView = findViewById(R.id.textView5)
        val textView_4: TextView = findViewById(R.id.textView6)
        val textView_5: TextView = findViewById(R.id.textView7)
        val textView_cz1: TextView = findViewById(R.id.textView_cz1)  //TODO wyświetlanie pozycji
        val textView_cz2: TextView = findViewById(R.id.textView_cz2)  //TODO wyświetlanie pozycji
        val textView_chw: TextView = findViewById(R.id.textView_chw)  //TODO wyświetlanie pozycji

        joystickView.setOnMoveListener(OnMoveListener { angle, strength ->
            val joystick = true;
            textView_angle.text = angle.toString()
            textView_strength.text = strength.toString()
            sendCommand(bluetoothSocket, "joystick,${angle.toString()},${strength.toString()}")
        }, 100) //refresh rate = 10ms

        button_connect.setOnClickListener {
            bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
            if (bluetoothAdapter == null)
            {
                text_connection.text = "Bluetooth is not supported on this device"
            }
            if (!bluetoothAdapter!!.isEnabled)
            {
                text_connection.text = "Please enable bluetooth"
            }
            bluetoothBondedDevices = bluetoothAdapter!!.bondedDevices
            if(bluetoothBondedDevices.isNotEmpty())
            {
                for(device: BluetoothDevice in bluetoothBondedDevices)
                {
                    if(device.name == "OmniBot")
                    {
                        try {
                            bluetoothSocket = device.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"))
                            BluetoothAdapter.getDefaultAdapter().cancelDiscovery()
                            bluetoothSocket!!.connect()
                            text_connection.text = "Connected"
                            break
                        } catch(e: IOException) {text_connection.text = "Connection failed"}
                    }
                }
            }
        }
        button_disconnect.setOnClickListener {
            if (bluetoothSocket != null)
            {
                try {
                    bluetoothSocket!!.close()
                    bluetoothSocket = null
                    text_connection.text = "Not connected"
                } catch(e: IOException) {e.printStackTrace()}
            }
        }

        button_up.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "forward")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_down.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "backward")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_right.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "right")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_left.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "left")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_upright.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "upright")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_downright.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "downright")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_downleft.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "downleft")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_upleft.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "upleft")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_spincw.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "spincw")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_spinccw.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "spinccw")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_armControl.setOnClickListener{
            button_cz1up.visibility = View.VISIBLE
            button_cz1down.visibility = View.VISIBLE
            button_cz2up.visibility = View.VISIBLE
            button_cz2down.visibility = View.VISIBLE
            button_chwup.visibility = View.VISIBLE
            button_chwdown.visibility = View.VISIBLE
            button_home.visibility = View.VISIBLE
            button_setPosition.visibility = View.VISIBLE
            button_back.visibility = View.VISIBLE
            editTextNumber_cz1.visibility = View.VISIBLE
            editTextNumber_cz2.visibility = View.VISIBLE
            editTextNumber_chw.visibility = View.VISIBLE
            textView_0.visibility = View.VISIBLE
            textView_1.visibility = View.VISIBLE
            textView_2.visibility = View.VISIBLE
            textView_3.visibility = View.VISIBLE
            textView_4.visibility = View.VISIBLE
            textView_5.visibility = View.VISIBLE
            //textView_cz1.visibility = View.VISIBLE
            //textView_cz2.visibility = View.VISIBLE
            //textView_chw.visibility = View.VISIBLE

            button_up.visibility = View.INVISIBLE
            button_down.visibility = View.INVISIBLE
            button_left.visibility = View.INVISIBLE
            button_right.visibility = View.INVISIBLE
            button_upright.visibility = View.INVISIBLE
            button_downright.visibility = View.INVISIBLE
            button_downleft.visibility = View.INVISIBLE
            button_upleft.visibility = View.INVISIBLE
            button_spincw.visibility = View.INVISIBLE
            button_spinccw.visibility = View.INVISIBLE
            button_armControl.visibility = View.INVISIBLE
            button_connect.visibility = View.INVISIBLE
            button_disconnect.visibility = View.INVISIBLE
            textView_angle.visibility = View.INVISIBLE
            textView_strength.visibility = View.INVISIBLE
            text_connection.visibility = View.INVISIBLE
            joystickView.visibility = View.INVISIBLE
            textView_angleLabel.visibility = View.INVISIBLE
            textView_strengthLabel.visibility = View.INVISIBLE

        }

        button_cz1up.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "cz1up")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_cz1down.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "cz1down")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_cz2up.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "cz2up")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_cz2down.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "cz2down")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_chwup.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "chwup")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_chwdown.setOnTouchListener {v, event ->
            if (event.action == MotionEvent.ACTION_DOWN)
            {
                sendCommand(bluetoothSocket, "chwdown")
            }
            if (event.action == MotionEvent.ACTION_UP)
            {
                sendCommand(bluetoothSocket, "200")
            }
            true
        }
        button_home.setOnClickListener {
            sendCommand(bluetoothSocket, "home")
        }
        button_setPosition.setOnClickListener {
            sendCommand(bluetoothSocket, "arm,${editTextNumber_cz1.text},${editTextNumber_cz2.text},${editTextNumber_chw.text}")
        }
        button_back.setOnClickListener {
            button_cz1up.visibility = View.INVISIBLE
            button_cz1down.visibility = View.INVISIBLE
            button_cz2up.visibility = View.INVISIBLE
            button_cz2down.visibility = View.INVISIBLE
            button_chwup.visibility = View.INVISIBLE
            button_chwdown.visibility = View.INVISIBLE
            button_home.visibility = View.INVISIBLE
            button_setPosition.visibility = View.INVISIBLE
            button_back.visibility = View.INVISIBLE
            editTextNumber_cz1.visibility = View.INVISIBLE
            editTextNumber_cz2.visibility = View.INVISIBLE
            editTextNumber_chw.visibility = View.INVISIBLE
            textView_0.visibility = View.INVISIBLE
            textView_1.visibility = View.INVISIBLE
            textView_2.visibility = View.INVISIBLE
            textView_3.visibility = View.INVISIBLE
            textView_4.visibility = View.INVISIBLE
            textView_5.visibility = View.INVISIBLE
            //textView_cz1.visibility = View.INVISIBLE
            //textView_cz2.visibility = View.INVISIBLE
            //textView_chw.visibility = View.INVISIBLE

            button_up.visibility = View.VISIBLE
            button_down.visibility = View.VISIBLE
            button_left.visibility = View.VISIBLE
            button_right.visibility = View.VISIBLE
            button_upright.visibility = View.VISIBLE
            button_downright.visibility = View.VISIBLE
            button_downleft.visibility = View.VISIBLE
            button_upleft.visibility = View.VISIBLE
            button_spincw.visibility = View.VISIBLE
            button_spinccw.visibility = View.VISIBLE
            button_armControl.visibility = View.VISIBLE
            button_connect.visibility = View.VISIBLE
            button_disconnect.visibility = View.VISIBLE
            textView_angle.visibility = View.VISIBLE
            textView_strength.visibility = View.VISIBLE
            text_connection.visibility = View.VISIBLE
            joystickView.visibility = View.VISIBLE
            textView_angleLabel.visibility = View.VISIBLE
            textView_strengthLabel.visibility = View.VISIBLE
        }

    }
    fun sendCommand(bluetoothSocket: BluetoothSocket?, command: String)
    {
        if (bluetoothSocket != null)
        {
            try {
                bluetoothSocket!!.outputStream.write(command.toByteArray())
            } catch(e: IOException) {e.printStackTrace()}
        }
    }
}
