from enum import Enum


ESP_TURN_ON_VALUE = '1'
ESP_TURN_OFF_VALUE = '0'


# TIMESTAMP_DIFF_SECS = 21600
TIMESTAMP_DIFF_SECS = 60


class Topics(str, Enum):
    flask_server = "server"
    plant = "plant"
    limits = "limits"

class Devices(str, Enum):
    fan ="0"
    pump = "1"
    sys = "2"
