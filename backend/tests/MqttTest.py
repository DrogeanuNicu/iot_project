import time
import json
import random
import paho.mqtt.publish as publish

mqtt_broker = "localhost"
mqtt_topic = "plant"

while True:
    message = {
        "time": int(time.time()),
        "temp": random.randint(0, 100),
        "hum": random.randint(0, 100),
        "moist": random.randint(0, 100),
        "fan": random.randint(0, 1),
        "pump": random.randint(0, 1),
    }

    json_message = json.dumps(message)
    publish.single(mqtt_topic, payload=json_message, hostname=mqtt_broker)

    print(f"Published message: {json_message}")

    time.sleep(1)
