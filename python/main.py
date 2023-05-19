"""
A high percentage of the code in this script has been obtained checking forums and adapting previously developed
code by other programmers.
The code is far from amazing, but it worked :).
"""
__author__ = "Nicolas Gutierrez"

#%% Libraries needed
import socket
import struct
import os
import time
import csv
import requests
import timeit
import serial
import numpy as np

#%% User options
downsampling_ratio_for_arduino = 16
com_port_arduino = 'COM10'
speed_buffer_size = 5
record_to_csv = False

#%% Definitions
speedbuffer = []
speed = []
ardu = []
i_ardu = 0  # Counter for the downsampler to Arduino

# Create UDP socket.
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Initiate the Serial communications
ser = serial.Serial(com_port_arduino, 115200, timeout=1)

#%% Loop
try:
    sock.bind(("127.0.0.1", 20777))
    sock.settimeout(1)
    if record_to_csv:
       with open('csv_file.csv', mode='w', newline='') as csv_file:
           datawriter = csv.writer(csv_file)
    while True:
        # Receive data.
        try:
            data = sock.recv(512)
        except:
            data = None
#        if not data:
#            break # Lost connection
        if data:
            # Increase the Ardu counter
            i_ardu = i_ardu+1
            
            outsim_pack = struct.unpack('64f', data[0:256])
            if len(speedbuffer) > speed_buffer_size:
                speedbuffer = speedbuffer[1:-1]
            speedbuffer.append(outsim_pack[7]*3.6)
            print("Speed   {}".format(outsim_pack[7]*3.6))
            print("Gear    {}".format(outsim_pack[33]))
            print("RPM     {}".format(outsim_pack[37]*10))
            print("Max RPM {}".format(outsim_pack[63]*10))

            datawriter.writerow(outsim_pack)
           
    #        for id, value in enumerate(outsim_pack):
    #            print ("%d : %s" % (id, value))
            if i_ardu > downsampling_ratio_for_arduino:
                i_ardu = 0
                power = round((outsim_pack[37]/outsim_pack[63])*100)
                speed = np.mean(np.diff(speedbuffer))
                speed = round(((outsim_pack[7]*3.6)+2*speed)/1.3)

                speedtosend = str(np.int(np.abs(speed))).zfill(3)
                revstosend = 50 + (((outsim_pack[37]*10)-1100)/8000)*17
                revstosend = str(np.int(np.abs(revstosend))).zfill(3)
                
                try:
                    ser.flush()
                    print(" ")
                    print('<'+speedtosend+','+revstosend+'>')
                    ser.write(('<'+speedtosend+','+revstosend+'>').encode())
                    stringtosplit = str(ser.readline())
                    print(stringtosplit)
                    #stringtosplit = str(ser.read(size=8))
                    #print("Sent: {}-{} -> Received: {}-{}".format(str(np.int(np.abs(speed))),str(np.int(np.abs(revstosend))),stringtosplit.split(',')[0][2:],stringtosplit.split(',')[1]))
                    ser.flush()
                    #response = requests.get("http://192.168.0.200:8080/fan?f={}".format(power)        
                except:
                    print("Serial failed")
            time.sleep(0.01)
        else:
            try:
                ser.flush()
                print(" ")
                print("Nothing from the game")
                print('<'+'000'+','+'000'+'>')
                ser.write(('<'+'000'+','+'000'+'>').encode())
                stringtosplit = str(ser.readline())
                print(stringtosplit)
                #stringtosplit = str(ser.read(size=8))
                #print("Sent: {}-{} -> Received: {}-{}".format(str(np.int(np.abs(speed))),str(np.int(np.abs(revstosend))),stringtosplit.split(',')[0][2:],stringtosplit.split(',')[1]))
                ser.flush()
                #response = requests.get("http://192.168.0.200:8080/fan?f={}".format(power)        
            except:
                print("Serial failed")

except:
    print('lost connection')
    # Release the socket.
    sock.close()
    ser.close()
