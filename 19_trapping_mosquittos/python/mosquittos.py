# Used examples from
# https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial
# https://pythonforundergradengineers.com/python-arduino-potentiometer.html
# http://www.steves-internet-guide.com/client-connections-python-mqtt/

import paho.mqtt.client as mqtt
import time
import serial

ser = serial.Serial('/dev/cu.usbmodem14101')
ser.flushInput()

# The callback for when the client receives a CONNACK response from the server.

#def on_connect(client, userdata, rc): #for old paho version
def on_connect(client, obj, flags, rc):
    print("Connected with result code "+str(rc))
# Subscribing in on_connect() means that if we lose the connection and
# reconnect then subscriptions will be renewed.
    client.subscribe("humidity/commands")
    
#    client.publish("test", "Greetings from python", 0)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic + " = " + str(msg.payload))

client = mqtt.Client("testClient", False, None, mqtt.MQTTv31, "tcp")
client.username_pw_set(username="ilja",password="inf471c")  #http://www.steves-internet-guide.com/client-connections-python-mqtt/
client.on_connect = on_connect
client.on_message = on_message

client.connect("dev.intellisoft.ee", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
# client.loop_forever()

# i = 0

while True:
    client.loop(1);
    try:
        ser_bytes = ser.readline()
        client.publish("humidity/readings", ser_bytes);
        print(".")
    except:
        print("Error")
        break
    
