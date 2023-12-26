import paho.mqtt.client as mqtt

from Dashboard import Dashboard

MQTT_ADDR = "localhost"
MQTT_PORT = 1883
MQTT_KEEPALIVE = 5
MQTT_TLS_ENABLED = False
MQTT_CLEAN_SESSION = True
MQTT_REFRESH_TIME = 1.0

class MqttHandler(mqtt.Client):
    def __init__(self, client_id:str, dashboard_ref:Dashboard):
        self.dashboard_ref = dashboard_ref

        super().__init__(protocol=mqtt.MQTTv5, client_id=client_id)
        self.on_connect = self.on_connect
        self.on_message = self.on_message
        self.connect(MQTT_ADDR, MQTT_PORT, MQTT_KEEPALIVE)
        self.loop_start()

    def on_connect(self, client, userdata, flags, rc, properties=None):
        client.subscribe('#')

    def on_message(self, client, userdata, message):
        topic = message.topic
        payload = message.payload.decode()

        print(f'MQTT: topic={topic} payload={payload}')
        self.dashboard_ref.append_data(topic, payload)
