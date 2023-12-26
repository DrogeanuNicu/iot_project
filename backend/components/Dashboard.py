import json
from datetime import datetime

class Dashboard():
    def __init__(self, max_size: int = 20):
        self.max_size = max_size
        self.list = list()

    def append_data(self, topic:str, data:str):
        if len(self.list) > self.max_size:
            self.list.pop(0)
        data_dict = json.loads(data)
        data_dict["time"] = self.__format_unix_time(data_dict["time"])
        self.list.append(data_dict)

    def get_data(self):
        return self.list


    def __format_unix_time(self, unix_time):
        dt_object = datetime.utcfromtimestamp(unix_time)
        formatted_time = dt_object.strftime('%d/%m/%Y %H:%M:%S')
        return formatted_time
