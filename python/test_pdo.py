import canopen
import os
import time
import pprint

pp = pprint.PrettyPrinter(indent=4)

network = canopen.Network()
network.connect(channel='can0', bustype='socketcan')

# This will attempt to read an SDO from nodes 1 - 127
network.scanner.search()
# We may need to wait a short while here to allow all nodes to respond
time.sleep(0.05)
for node_id in network.scanner.nodes:
    print("Found node %d!" % node_id)

node = network.add_node(8, './Hipnuc_IMU_TaskSlave.eds')

node.tpdo.read()
node.rpdo.read()

print("cob id: {}, name: {}".format(hex(node.tpdo[1].cob_id), node.tpdo[1].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[2].cob_id), node.tpdo[2].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[3].cob_id), node.tpdo[3].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[4].cob_id), node.tpdo[4].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[5].cob_id), node.tpdo[5].name))

# node.tpdo[1].clear()
# node.tpdo[1].add_variable('CAN_ACC', 'Number of Entries', 2)
# node.tpdo[1].add_variable('CAN_ACC', 'CAN_ACC 1', 2)
# node.tpdo[1].add_variable('CAN_ACC', 'CAN_ACC 2', 2)
# node.tpdo[1].add_variable('CAN_ACC', 'CAN_ACC 3', 2)
# node.tpdo[1].trans_type = 254
# node.tpdo[1].enabled = True

# for i in range(500):
while(True):
    node.tpdo[1].wait_for_reception()
    # print("raw: {}, data: {}, {}, {}".format(
    #     node.tpdo[1].data,
    #     node.tpdo[1]['CAN_ACC.CAN_ACC 1'].raw,
    #     node.tpdo[1]['CAN_ACC.CAN_ACC 2'].raw,
    #     node.tpdo[1]['CAN_ACC.CAN_ACC 3'].raw))
    print("data: {}, {}, {}".format(
        node.tpdo[1]['CAN_ACC.CAN_ACC 1'].phys,
        node.tpdo[1]['CAN_ACC.CAN_ACC 2'].phys,
        node.tpdo[1]['CAN_ACC.CAN_ACC 3'].phys))
    # print("variables: {}".format(node.tpdo[1].map))
    # speed = node.tpdo['Application Status.Actual Speed'].phys
    # f.write('%s\n' % speed)
    # print("Speed: %d" % speed)
    # print("received")
    # print("data: %d", node.tpdo[1]['CAN_ACC 1'].phys)
    # print("Button press counter: {}".format(map['CAN_ACC 1'].raw))
    # print("values: %f" % map['CAN_ACC 1'].phys) 
    # map.add_variable('INTEGER16 value')  
    # map.add_variable('INTEGER16 value')  
    # print("values: %f, %f, %f" % map['INTEGER16 value'].raw, map['INTEGER16 value'].raw, map['INTEGER16 value'].raw)

# # Using a callback to asynchronously receive values
# # Do not do any blocking operations here!
# def print_speed(message):
#     print('%s received' % message.name)
#     for var in message:
#         print('%s = %d' % (var.name, var.raw))

# node.tpdo[0].add_callback(print_speed)
# time.sleep(5)

# Stop transmission of RxPDO
# node.tpdo[1].stop()

network.disconnect()
