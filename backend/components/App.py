from flask import Flask, json, request, send_file
from flask_cors import CORS
import os
from Dashboard import Dashboard
from MqttHandler import MqttHandler
from Constants import *

app = Flask(__name__)
CORS(app, allow_headers=["Content-Type"])
json.provider.DefaultJSONProvider.sort_keys = False

dashboard = Dashboard(root_path=app.root_path)
mqtt_handler = MqttHandler(client_id=__name__, dashboard_ref=dashboard)
mqtt_handler.publish(Topics.flask_server.value, Devices.sys) #request limits


@app.route("/api/data", methods=["GET"])
def get_data():
    return dashboard.get_data()


@app.route("/api/limits", methods=["GET"])
def get_limits():
    return dashboard.get_limits()


@app.route('/api/download')
def download_csv():
    csv_file = dashboard.get_csv_file_name()
    if os.path.exists(csv_file):
        return send_file(csv_file, as_attachment=True)
    else:
        return "CSV file not found", 404


@app.route("/api/set/<device>", methods=["POST"])
def control_device(device):
    data = request.json
    isOn = data.get('isOn', False)

    message = ""
    status = ""
    if isOn is True:
        status = "ON"
        message = Devices[device].value + ',' + ESP_TURN_ON_VALUE
    else:
        status = "OFF"
        message = Devices[device].value + ',' + ESP_TURN_OFF_VALUE
    mqtt_handler.publish(Topics.flask_server.value, message)
    print(f"The {device} is {status}!")
    return ('', 204)


@app.route("/api/set/<property>/<limit>", methods=["POST"])
def set_limit(property, limit):
    data = request.json
    value = data.get('value', False)
    dashboard.update_limit(property, limit, value)
    limits = dashboard.get_limits()
    message = f"{Devices.sys.value},{limits['temp']['min']},{limits['temp']['max']},{limits['hum']['min']},{limits['hum']['max']},{limits['moist']['min']},{limits['moist']['max']}"
    mqtt_handler.publish(Topics.flask_server.value, message)
    return ('', 204)


if __name__ == "__main__":
    app.run(debug=False, host='drogeanunicusor.go.ro', port=5000)
