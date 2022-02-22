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

node = network.add_node(10, '../src/app/tbot/objdict/robofw.eds')

name = node.sdo['Manufacturer device name']
print("Device name: '{}'".format(name.raw))

node.tpdo.read()
node.rpdo.read()

print("cob id: {}, name: {}".format(hex(node.tpdo[1].cob_id), node.tpdo[1].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[2].cob_id), node.tpdo[2].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[3].cob_id), node.tpdo[3].name))
print("cob id: {}, name: {}".format(hex(node.tpdo[4].cob_id), node.tpdo[4].name))

# # Using a callback to asynchronously receive values
# # Do not do any blocking operations here!
def print_tpdo1(message):
    print('%s received' % message.name)
    for var in message:
        print('%s = %d' % (var.name, var.raw))

node.tpdo[1].add_callback(print_tpdo1)
node.tpdo[2].add_callback(print_tpdo1)
node.tpdo[3].add_callback(print_tpdo1)
node.tpdo[4].add_callback(print_tpdo1)

while(True):
    time.sleep(1)
