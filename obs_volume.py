import obsws_python as obs

def slider_to_db(slider_value, muted, source_name, host='YOUR_COMPUTER_IP', port=4455, password='YOUR_OBS_WEBSOCKET_PASSWORD'):
    if muted or slider_value <= 0:
        volume_db = -60.0
    elif slider_value >= 100:
        volume_db = 0.0
    else:
        volume_db = -60.0 + (slider_value * 0.6)

    client = None
    
    try:
        client = obs.ReqClient(host=host, port=port, password=password, timeout=3)
        client.set_input_volume(name=source_name, vol_db=volume_db)
        return True
    
    except Exception:
        print("Failed to set volume in OBS")
        return False
    
    finally:
        if client:
            try:
                client.disconnect()
            except Exception:
                pass
