# INF471C - Connected Objects and the Internet of Things (IoT)
# MQTT: Trapping mosquitoes
# Ilja Tihhanovski
 

# Used examples from
# https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial
# https://pythonforundergradengineers.com/python-arduino-potentiometer.html
# http://www.steves-internet-guide.com/client-connections-python-mqtt/
# https://mntolia.com/mqtt-python-with-paho-mqtt-client/

import paho.mqtt.client as mqtt
import time
import serial

# Open connection to Arduino using serial port (hardcoded)
ser = serial.Serial('/dev/cu.usbmodem14101')
ser.flushInput()

# On connect we output message about connection result code and subscribe to our topic
def on_connect(client, obj, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("humidity/commands")
    
# The callback for when a PUBLISH message is received from the server.
# Here we just output the message
def on_message(client, userdata, msg):
    print("Message from broker: " + msg.topic + " = " + str(msg.payload))

client = mqtt.Client("humidityClient", True, None, mqtt.MQTTv31, "tcp")         # Instantiate a client with name 
client.username_pw_set(username="ilja",password="inf471c")                      # Authenticate, see http://www.steves-internet-guide.com/client-connections-python-mqtt/
client.on_connect = on_connect                                                  # Attach callbacks
client.on_message = on_message

client.connect("dev.intellisoft.ee", 1883, 60)                                  # Connect to broker

client.loop_start()                                                             # non-blocking - best of three available options
while True:
    try:
        ser_bytes = ser.readline()                                              # Read data sent from Arduino
        client.publish("humidity/readings", ser_bytes);                         # Just publish data as, but we could process it here
        print(ser_bytes)                                                        # Output data to console
    except:
        client.loop_stop()                                                      # On error (user presses Ctrl+C) stop the client
        break
