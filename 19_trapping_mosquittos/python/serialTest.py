import serial

ser = serial.Serial('/dev/cu.usbmodem14101')
ser.flushInput()

while True:
    try:
        ser_bytes = ser.readline()
        print(ser_bytes)
        #decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
        #print(decoded_bytes)
    except:
        print("Keyboard Interrupt")
        break