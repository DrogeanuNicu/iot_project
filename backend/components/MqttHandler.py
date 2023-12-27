import paho.mqtt.client as mqtt

from Dashboard import Dashboard
from Constants import *

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
        if topic == Topics.flask_server.value:
            print(f'MQTT sent: topic={topic} payload={payload}')
        else:
            print(f'MQTT recv: topic={topic} payload={payload}')
            if topic == Topics.limits.value:
                self.dashboard_ref.store_limits(payload)
            elif topic == Topics.plant.value:
                if self.dashboard_ref.get_are_limits_stored is False:
                    self.publish(Topics.flask_server.value, Devices.sys) #request limits
                self.dashboard_ref.append_data(payload)
            else:
                print("Unknown MQTT topic!")
