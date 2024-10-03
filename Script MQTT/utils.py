import json
from decimal import Decimal
from datetime import datetime
from data_base import insert_data_to_db

def hex_to_ascii(hex_str):
    try:
        bytes_obj = bytes.fromhex(hex_str)
        ascii_str = bytes_obj.decode("ASCII")
        return ascii_str
    except ValueError:
        return "Erro ao converter hexadecimal para ASCII"

def decode(message, devaddr, rssi):
    battery = int(Decimal(message[:3]))
    volume = int(Decimal(message[3:7]))
    latitude = float(Decimal(message[7:17]))
    longitude = float(Decimal(message[17:27]))
    
    is_tipped_over = message[27] == '1'

    command = {
        "DeviceAddr": devaddr,
        "BatteryLevel": battery,
        "Volume": (volume/10),
        "IsTippedOver": is_tipped_over,
        "Latitude": latitude,
        "Longitude": longitude,
        "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "rssi": rssi
    }
    
    insert_data_to_db(
        devaddr=devaddr, 
        battery=battery, 
        volume=volume / 10, 
        is_tipped_over=is_tipped_over, 
        latitude=latitude, 
        longitude=longitude, 
        timestamp=command["timestamp"], 
        rssi=rssi
    )

    
    json_data = json.dumps(command)
    
    
    return json_data.encode('utf-8')
