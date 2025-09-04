import logging
import warnings
import comtypes
from flask import Flask, request
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
from obs_volume import *

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR) 
warnings.filterwarnings("ignore", message=r"COMError attempting to get property .* from device .*") #Removes those stupid warnings

app = Flask(__name__)

pot1 = 100
pot2 = 100
pot1_mute_status = False
pot2_mute_status = False

MIC_NAME = "YOUR_MIC_NAME"  # Print the "d" in the for loop to find your mic's exact name. 
SOURCE_NAME = 'YOUR_OBS_SOURCE_NAME' 

def set_volume(name: str, percent: int, mute: bool):
    comtypes.CoInitialize()  
        
    devices = AudioUtilities.GetAllDevices()
    
    for d in devices:
        if d.FriendlyName == name:
            vol = d.EndpointVolume.QueryInterface(IAudioEndpointVolume)
            if percent <= 0 or mute == True:
                vol.SetMute(1, None)  
            else:
                vol.SetMute(0, None)  
                vol.SetMasterVolumeLevelScalar(float(percent) / 100.0, None)

            return True
        
    print("Failed to find mic")
    return False 

@app.route('/data', methods=['POST'])

def receive_data():
    global pot1, pot2, pot1_mute_status, pot2_mute_status
    
    data = request.get_json() 

    pot1 = data['pot1_percentage']
    pot2 = data['pot2_percentage']
    pot1_mute_status = data['button1_state']
    pot2_mute_status = data['button2_state']

    set_volume(MIC_NAME, pot1, pot1_mute_status)
    slider_to_db(pot2, pot2_mute_status, SOURCE_NAME)

    return {"status": "ok"}, 200

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)
