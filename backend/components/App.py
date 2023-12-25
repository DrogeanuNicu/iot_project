from flask import Flask
from MqttHandler import MqttHandler

app = Flask(__name__)
import Routes

mqtt_handler = MqttHandler(__name__)

if __name__ == '__main__':
    app.run(debug=False)