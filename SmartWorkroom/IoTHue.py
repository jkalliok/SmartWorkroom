import requests
import json
import time
import socket
import os

url = 'http://192.168.1.97/api/FhcWNuLdWIVVZV8oDxNoO3yePvu7X5-HZknOTqTt'
#IP and Hue API authentication figured out in advance

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(('', 4650))  #receive requests here

#Used to check what lights are available so we can find what to control
def CheckLights():
    x = requests.get(url+"/lights", verify=False)
    xjson = x.json()
    for k in xjson.keys():
        print(k+": "+str(xjson[k]))

#Just for testing. Actual light control happening on ESP-32
def OnOff(id, on):
    body = {'on': on}
    x = requests.put(url+"/lights/"+str(id)+"/state", data=json.dumps(body), verify=False)
    print(x.text)

#This may use hibernate if it is enabled, which it is not for me
def GoSleep():
    os.system("rundll32.exe powrprof.dll,SetSuspendState 0,1,0")

#Going to sleep on any packet received. Could add some security protocol here
while True:
    message, address = server_socket.recvfrom(1024)
    message = message.decode("utf-8")
    print(message)
    GoSleep()

#OnOff(2, False)
#time.sleep(2.0)
#OnOff(2, True)
