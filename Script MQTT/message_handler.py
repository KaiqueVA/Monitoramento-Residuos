import json
from utils import hex_to_ascii, decode

def process_message(payload_str):
    try:
        payload_json = json.loads(payload_str)
        if verify_devAddr(payload_str, "F3E35972"):
            value_devaddr = payload_json.get("devaddr", "")
            value_rssi = payload_json.get("rssi", "")
            hex_data = payload_json.get("data", "")
            if hex_data:
                ascii_resultado = hex_to_ascii(hex_data)
                # print(f"Key data: {ascii_resultado}")
                decoded_message = decode(ascii_resultado, value_devaddr, value_rssi)
                print(f"Mensagem decodificada: {decoded_message}")
            else:
                print("Campo 'data' não encontrado na mensagem.")
        
        
    except json.JSONDecodeError:
        print("Erro ao decodificar a mensagem JSON.")

def verify_devAddr(message, devaddr):
    try:
        payload_json = json.loads(message)
        value_devaddr = payload_json.get("devaddr", "")
        if value_devaddr == devaddr:
            return True
        else:
            return False
    except json.JSONDecodeError:
        return False