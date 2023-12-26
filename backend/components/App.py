from flask import Flask, json
from Dashboard import Dashboard
from MqttHandler import MqttHandler

app = Flask(__name__)
json.provider.DefaultJSONProvider.sort_keys = False

dashboard = Dashboard()
mqtt_handler = MqttHandler(client_id=__name__, dashboard_ref=dashboard)

@app.route('/api/data', methods=['GET'])
def get_data():
    return dashboard.get_data()

if __name__ == '__main__':
    app.run(debug=False)
