import paho.mqtt.client as mqtt
from message_handler import process_message
from data_base import create_table

TOPIC = "LORAWAN"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"Conectado com sucesso ao broker! Código de retorno: {rc}")
        client.subscribe(TOPIC)
        print(f"Inscrito no tópico: {TOPIC}")
    else:
        print(f"Falha ao conectar. Código de retorno: {rc}")

def on_message(client, userdata, msg):
    payload_str = msg.payload.decode("utf-8")
    # print(f"Mensagem completa recebida: {payload_str}")
    
    process_message(payload_str)

def run_mqtt_client(broker_address="52.1.135.87", port=1883, keepalive=60):
    """Função para inicializar e rodar o cliente MQTT."""
    client = mqtt.Client()

    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker_address, port, keepalive)

    client.loop_forever()

if __name__ == "__main__":
    create_table()
    run_mqtt_client()
