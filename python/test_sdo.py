import canopen
import os

ZEPHYR_BASE = os.environ['ZEPHYR_BASE']
# EDS = os.path.join(ZEPHYR_BASE, 'samples', 'subsys', 'canbus', 'canopen',
#                    'objdict', 'objdict.eds')
EDS = '/home/rdu/Data/rdu/Workspace/weston_robot/wrdev_ws/robofw/src/app/tbot/objdict/objdict.eds'
NODEID = 10

network = canopen.Network()

network.connect()

node = network.add_node(NODEID, EDS)
name = node.sdo['Manufacturer device name']

print("Device name: '{}'".format(name.raw))

network.disconnect()