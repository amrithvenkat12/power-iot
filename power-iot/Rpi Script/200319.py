import numpy as np
from matplotlib import pyplot as plt
import smbus
import sys
import getopt
import time
from datetime import datetime
import pigpio
import RPi.GPIO as GPIO
import mysql.connector


##GPIO.setmode(GPIO.BCM)
##GPIO.setwarnings(False)
##relayPin=17

##GPIO.setup(relayPin,GPIO.OUT)




i2c_bus = smbus.SMBus(1)
OMRON_1=0x0a 					# 7 bit I2C address of Omron MEMS Temp Sensor D6T-44L
OMRON_BUFFER_LENGTH=35				# Omron data buffer size
 	# initialize the temperature data list
tempa=[0.0]*16
tempb=[0.0]*16*20
la=[0.0]*16
lb=[0.0]*16
means=[0.0]*16


la=np.array(la)
la=la.reshape(4,4)

lb=np.array(lb)
lb=lb.reshape(4,4)
present=0

# intialize the pigpio library and socket connection to the daemon (pigpiod)
pi = pigpio.pi()              # use defaults
#version = pi.get_pigpio_version()
#print ('PiGPIO version = '+str(version))
handle = pi.i2c_open(1, 0x0a) # open Omron D6T device at address 0x0a on bus 1

# initialize the device based on Omron's appnote 1
result=i2c_bus.write_byte(OMRON_1,0x4c);
#print ('write result = '+str(result))


def plot(in_np_arr):
    inp=in_np_arr.reshape(4,4)
    plt.imshow(inp)
##    plt.set_cmap('coolwarm_r')
    plt.axis('off')
    plt.show()
    return;
    
def readdata(red):
    temperature_data=[0]*OMRON_BUFFER_LENGTH
    for x in range(0, len(temperature_data)):
  # print x
   # Read all data  tem
  # temperature_data[x]=i2c_bus.read_byte(OMRON_1)
        (bytes_read, temperature_data) = pi.i2c_read_device(handle, len(temperature_data))

# Display data 
#print ('Bytes read from Omron D6T: '+str(bytes_read))
#print ('Data read from Omron D6T : ')
    #for x in range(bytes_read):
     #  print(temperature_data[x]),
#print 'done'
    ptat=(temperature_data[0]+(temperature_data[1]*256))*0.1
#ptat=(ptat*9/5)+32
    #print( 'ptat:', ptat)

#print ('vals:')
    for i in range(0,16):
        tempa[i]=(temperature_data[(i*2)+2]+(temperature_data[(i*2)+3]*256))*0.1
        #print(tempa[i])
    f=open("dat11.csv", "a+")
    for ap in range (0,16):
        f.write(str(tempa[ap])+",")
    #f.write(str(tempa[ap])+"\n")
    f.close()
    return;

def filtermean():
    la[0,0]=np.mean(xb[0:2,0:2])
    la[0,1]=np.mean(xb[0:2,0:3])
    la[0,2]=np.mean(xb[0:2,1:4])
    la[0,3]=np.mean(xb[0:2,2:4])
    la[1,0]=np.mean(xb[0:3,0:2])
    la[1,1]=np.mean(xb[0:3,0:3])
    la[1,2]=np.mean(xb[0:3,1:4])
    la[1,3]=np.mean(xb[0:3,2:4])
    la[2,0]=np.mean(xb[1:4,0:2])
    la[2,1]=np.mean(xb[1:4,0:3])
    la[2,2]=np.mean(xb[1:4,1:4])
    la[2,3]=np.mean(xb[1:4,2:4])
    la[3,0]=np.mean(xb[2:4,0:2])
    la[3,1]=np.mean(xb[2:4,0:3])
    la[3,2]=np.mean(xb[2:4,1:4])
    la[3,3]=np.mean(xb[2:4,2:4])


##    for i in range(0,3):
##        for j in range(0,3):
##            if ((i or j)==0):
##                la[i,j]=np.mean(xb[])
    return;

def filtermed():
    lb[0,0]=np.median(xb[0:2,0:2])
    lb[0,1]=np.median(xb[0:2,0:3])
    lb[0,2]=np.median(xb[0:2,1:4])
    lb[0,3]=np.median(xb[0:2,2:4])
    lb[1,0]=np.median(xb[0:3,0:2])
    lb[1,1]=np.median(xb[0:3,0:3])
    lb[1,2]=np.median(xb[0:3,1:4])
    lb[1,3]=np.median(xb[0:3,2:4])
    lb[2,0]=np.median(xb[1:4,0:2])
    lb[2,1]=np.median(xb[1:4,0:3])
    lb[2,2]=np.median(xb[1:4,1:4])
    lb[2,3]=np.median(xb[1:4,2:4])
    lb[3,0]=np.median(xb[2:4,0:2])
    lb[3,1]=np.median(xb[2:4,0:3])
    lb[3,2]=np.median(xb[2:4,1:4])
    lb[3,3]=np.median(xb[2:4,2:4])
    return;


def saveto():
    tt=str(datetime.now())
    fa= open("mean.csv","a+")
    fa.write(tt+',')
    for i in range (0,20):
        fa.write(str(means[i])+',')
    fa.write(str(means[i])+'\n')
    fa.close()
    print('to mean csv')
    return;

try:
    while(1):
        f = open("dat11.csv", "w")
        f.truncate()
        print("file truncation done")
        for i in range (0,20):
            #print(i)
            readdata(i)
        print('20 values taken')    
        f=open("dat11.csv", "r")#open file
        tempb=f.read() #read string data from file data.txt
        tempb=tempb.split(",") #split the strings at delimiter ','
        tempb.pop() #pop out the last value ','
        
        #xa=np.array(tempa)
        xb=np.array(tempb,np.float) #assign and convert tempc to nparray with each element as float
        n_xb=np.size(xb)
        xc=xb.reshape(int(n_xb/16),16) #reshape nparray nb as nc 20*n, 16
        print(n_xb)
        means=np.mean(xc,axis=1)
        saveto()
        
        #xl=xa.reshape(4,4)
        #print(xl)
        #filtermean()
        #filtermed()
        #aa=np.mean(xb)
        #print(aa)
        
##        mydb = mysql.connector.connect(host="localhost",user="root",passwd="P@ssw0rd",database="myDBPDO")
##        mycursor = mydb.cursor()
##        if present==1:
##            mycursor.execute("UPDATE pythsql SET isPresent=1")
##            print("on")
##        else:
##            mycursor.execute("UPDATE pythsql SET isPresent=1")
##            print("off")
##        mydb.commit()

        
##        GPIO.output(relayPin,GPIO.HIGH)
        
   
    

    
except KeyboardInterrupt:
    print("keyboard interrupt")
##    f = open("dat11.csv", "w")
##    f.truncate()
##    print("file truncation done")
    pi.i2c_close(handle)
    pi.stop()
    GPIO.cleanup()
    print("GPIO cleanup")
    print("exit")
    sys.exit()

