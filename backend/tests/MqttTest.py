import time
import json
import random
import paho.mqtt.publish as publish

mqtt_broker = "localhost"
mqtt_topic = "plant"

while True:
    message = f"{int(time.time())},{random.randint(0, 100)},{random.randint(0, 100)},{random.randint(0, 100)},{random.randint(0, 1)},{random.randint(0, 1)}"
    publish.single(mqtt_topic, payload=message, hostname=mqtt_broker)

    print(f"Published message: {message}")

    time.sleep(1)
