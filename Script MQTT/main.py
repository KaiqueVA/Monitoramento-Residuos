import paho.mqtt.client as mqtt
from message_handler import process_message

TOPIC = "LORAWAN"

def on_connect(client, userdata, flags, rc):
    """Callback executado quando o cliente se conecta ao broker."""
    if rc == 0:
        print(f"Conectado com sucesso ao broker! Código de retorno: {rc}")
        client.subscribe(TOPIC)
        print(f"Inscrito no tópico: {TOPIC}")
    else:
        print(f"Falha ao conectar. Código de retorno: {rc}")

def on_message(client, userdata, msg):
    """Callback executado quando uma mensagem é recebida no tópico."""
    payload_str = msg.payload.decode("utf-8")
    
    process_message(payload_str)

def run_mqtt_client(broker_address="0.0.0.0", port=1883, keepalive=60):
    """Função para inicializar e rodar o cliente MQTT."""
    client = mqtt.Client()

    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker_address, port, keepalive)

    client.loop_forever()

if __name__ == "__main__":
    run_mqtt_client()
