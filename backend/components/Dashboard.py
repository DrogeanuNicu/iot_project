import json
from datetime import datetime

class Dashboard():
    def __init__(self, max_size: int = 40):
        self.max_size = max_size
        self.list = list()
        self.are_limits_stored = False
        self.limits = {
            "temp" : {
                "min": 0,
                "max": 0
            },
            "hum" : {
                "min": 0,
                "max": 0
            },
            "moist" : {
                "min": 0,
                "max": 0
            }
        }

    def append_data(self, data:str):
        if len(self.list) > self.max_size:
            self.list.pop(0)

        try:
            trimmed_data = [int(item.strip()) for item in data.split(',')]
            data_dict = {
                "time" : self.__format_unix_time(trimmed_data[0]),
                "temp" : trimmed_data[1],
                "hum"  : trimmed_data[2],
                "moist": trimmed_data[3],
                "fan"  : trimmed_data[4],
                "pump" : trimmed_data[5]
            }
            self.list.append(data_dict)
        except Exception as e:
            print(f"Exception occurred: {e}")


    def get_data(self):
        return self.list

    def get_are_limits_stored(self):
        return self.are_limits_stored

    def store_limits(self, data:str):
        if self.are_limits_stored is True:
            return

        print(f"Storing the limits: {data}")
        try:
            trimmed_data = [int(item.strip()) for item in data.split(',')]
            self.limits["temp"]["min"] = trimmed_data[0]
            self.limits["temp"]["max"] = trimmed_data[1]
            self.limits["hum"]["min"] = trimmed_data[2]
            self.limits["hum"]["max"] = trimmed_data[3]
            self.limits["moist"]["min"] = trimmed_data[4]
            self.limits["moist"]["max"] = trimmed_data[5]

            self.are_limits_stored = True
        except Exception as e:
            print(f"Exception occurred: {e}")

    def get_limits(self):
        return self.limits

    def update_limit(self, property, limit, value):
        try:
            self.limits[property][limit] = value
        except Exception as e:
            print(f"Exception occurred: {e}")

    def __format_unix_time(self, unix_time):
        dt_object = datetime.utcfromtimestamp(unix_time)
        formatted_time = dt_object.strftime('%d/%m/%Y %H:%M:%S')
        return formatted_time
