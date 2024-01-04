import json
import csv
import os
from datetime import datetime, timedelta
from Constants import *

class Dashboard():
    def __init__(self, root_path: str, max_size: int = 40):
        self.max_size = max_size
        self.list = list()
        self.are_limits_stored = False
        self.last_unix_timestamp = datetime.utcfromtimestamp(0)
        self.csv_file = os.path.join(root_path, '..', "data", "data.csv")
        self.field_names = ["time", "temp", "hum", "moist", "fan", "pump"]
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

        if not os.path.exists(self.csv_file):
            with open(self.csv_file, 'w', newline='') as csvfile:
                writer = csv.DictWriter(csvfile, fieldnames=self.field_names)
                writer.writeheader()
        else:
            with open(self.csv_file, 'r', newline='') as csvfile:
                reader = csv.reader(csvfile)
                next(reader, None) # Skip the header
                lines = list(reader)[-self.max_size:]
                for line in lines:
                    data_dict = {
                        self.field_names[0]: line[0],
                        self.field_names[1]: line[1],
                        self.field_names[2]: line[2],
                        self.field_names[3]: line[3],
                        self.field_names[4]: line[4],
                        self.field_names[5]: line[5]
                    }
                    self.list.append(data_dict)


    def append_data(self, data:str):
        try:
            trimmed_data = [int(item.strip()) for item in data.split(',')]
            dt_object = datetime.utcfromtimestamp(trimmed_data[0])
            formatted_time = dt_object.strftime('%d/%m/%Y %H:%M:%S')
            data_dict = {
                self.field_names[0]: formatted_time,
                self.field_names[1]: trimmed_data[1],
                self.field_names[2]: trimmed_data[2],
                self.field_names[3]: trimmed_data[3],
                self.field_names[4]: trimmed_data[4],
                self.field_names[5]: trimmed_data[5]
            }

            if dt_object - self.last_unix_timestamp >= timedelta(seconds=TIMESTAMP_DIFF_SECS):
                if len(self.list) > self.max_size:
                    self.list.pop(0)
                self.list.append(data_dict)
                self.last_unix_timestamp = dt_object
                self.__append_to_csv(data_dict)
            else:
                if len(self.list) > 0:
                    self.list[-1] = data_dict

        except Exception as e:
            print(f"Exception occurred: {e}")


    def get_data(self):
        return self.list

    def get_csv_file_name(self):
        return self.csv_file

    def get_are_limits_stored(self):
        return self.are_limits_stored

    def store_limits(self, data:str):
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

    def __append_to_csv(self, data_dict):
        with open(self.csv_file, 'a', newline='') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=self.field_names)
            writer.writerow(data_dict)

