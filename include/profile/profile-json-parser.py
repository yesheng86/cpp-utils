import json
import sys

def main():

    with open("profile.json", "r") as read_file:
        profile = json.load(read_file)

    data_list = profile["data"]
    msg_table = profile["message_lookup"]

    count_table = []
    duration_sum_table = []
    duration_max_table = []
    duration_min_table = []
    for i in range(len(msg_table)):
        duration_sum_table.append(0)
        duration_max_table.append(0)
        duration_min_table.append(sys.maxsize)
        count_table.append(0)

    for data in data_list:
        duration_sum_table[data[0]] += data[2]
        count_table[data[0]] += 1
        if data[2] > duration_max_table[data[0]]:
            duration_max_table[data[0]] = data[2]
        if data[2] < duration_min_table[data[0]]:
            duration_min_table[data[0]] = data[2]

    for i in range(len(msg_table)):
        print(str(i) + ". Messge:" + str(msg_table[i]) + ", count:" + str(count_table[i])
        + ", avg duration:" + str(duration_sum_table[i] / count_table[i])
        + ", max:" + str(duration_max_table[i]) + ", min:" + str(duration_min_table[i]))

main()
