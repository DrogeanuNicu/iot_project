from flask import Flask
import paho.mqtt.client as mqtt

from Constants import *

app = Flask(__name__,
            static_folder=FLASK_STATIC_FOLDER_PATH,
            static_url_path=FLASK_STATIC_URL_PATH)

app.config['MQTT_BROKER_URL'] = MQTT_ADDR
app.config['MQTT_BROKER_PORT'] = MQTT_PORT
app.config['MQTT_KEEPALIVE'] = MQTT_KEEPALIVE
app.config['MQTT_TLS_ENABLED'] = MQTT_TLS_ENABLED
app.config['MQTT_CLEAN_SESSION'] = MQTT_CLEAN_SESSION
app.config['MQTT_REFRESH_TIME'] = MQTT_REFRESH_TIME

def get_data_test(arg: str):
    data = {"message": arg}
    return data

@app.route('/api/hello', methods=['GET'])
def get_data():
    return get_data_test("dasasad")

def on_connect(client, userdata, flags, rc, properties=None):
    client.subscribe('#')

def on_message(client, userdata, message):
    print (f'MQTT: {message.topic}, payload: {message.payload.decode()}')

mqtt_client = mqtt.Client(protocol=mqtt.MQTTv5, client_id="flask")
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_ADDR, MQTT_PORT, MQTT_KEEPALIVE)
mqtt_client.loop_start()  # Start the MQTT client loop

if __name__ == '__main__':
    app.run()