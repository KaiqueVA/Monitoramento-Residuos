import json
from decimal import Decimal
from datetime import datetime
from data_base import insert_data_to_db
import requests

URL = "http://172.16.51.83:8000/api/contentors/"

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
        "device_addr": devaddr,
        "battery_level": battery,
        "volume": (volume/10),
        "is_tipped_over": is_tipped_over,
        "latitude": latitude,
        "longitude": longitude,
        "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
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
    
    try:
        headers = {'Content-Type': 'application/json'}
        response = requests.post(URL, data=json.dumps(command), headers=headers)  # Envia o payload como JSON
        response.raise_for_status()
        print("Dados enviados com sucesso para a API.")
    except requests.exceptions.RequestException as e:
        print(f"Erro ao enviar dados para a API: {e} Response: {response.text}")

    
    json_data = json.dumps(command)
    
    
    return json_data.encode('utf-8')
