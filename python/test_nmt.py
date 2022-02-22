import canopen
import os
import time

# ZEPHYR_BASE = os.environ['ZEPHYR_BASE']
# EDS = os.path.join(ZEPHYR_BASE, 'samples', 'subsys', 'canbus', 'canopen',
#                    'objdict', 'objdict.eds')
EDS = '../src/app/tbot/objdict/robofw.eds'
NODEID = 10

network = canopen.Network()
network.connect(channel='can0', bustype='socketcan')

# # This will attempt to read an SDO from nodes 1 - 127
# network.scanner.search()
# # We may need to wait a short while here to allow all nodes to respond
# time.sleep(0.05)
# for node_id in network.scanner.nodes:
#     print("Found node %d!" % node_id)

node = network.add_node(NODEID, EDS)

# Green indicator LED will flash slowly
# node.nmt.state = 'STOPPED'
# time.sleep(5)

# Green indicator LED will flash faster
node.nmt.state = 'PRE-OPERATIONAL'
time.sleep(5)

# # Green indicator LED will be steady on
# node.nmt.state = 'OPERATIONAL'
# time.sleep(5)

# # Node will reset communication
# node.nmt.state = 'RESET COMMUNICATION'
# node.nmt.wait_for_heartbeat()

# # Node will reset
# node.nmt.state = 'RESET'
# node.nmt.wait_for_heartbeat()

network.disconnect()