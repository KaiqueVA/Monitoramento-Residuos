import json
from utils import hex_to_ascii, decode

def process_message(payload_str):
    """Processa a mensagem recebida em formato JSON e traduz o campo 'data'."""
    try:
        payload_json = json.loads(payload_str)
        value_devaddr = payload_json.get("devaddr", "")
        hex_data = payload_json.get("data", "")
        if hex_data:
            ascii_resultado = hex_to_ascii(hex_data)
            decoded_message = decode(ascii_resultado, value_devaddr)
            print(f"Mensagem decodificada: {decoded_message}")
        else:
            print("Campo 'data' não encontrado na mensagem.")
    except json.JSONDecodeError:
        print("Erro ao decodificar a mensagem JSON.")
