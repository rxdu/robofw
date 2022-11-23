# RCCar CAN Communication

The CAN communication between the SBC and the MCU uses the same VESC protocol. The MCU is treated as a fake VESC that relays commands to the real VESC. By default, the MCU and VESC are assigned the following id:

* MCU: 0x60
* VESC: 0x68

The communication data flow is illustrated in the following diagram:

```
RC  ------- (SBUS) ------->
                            \
                             |---> MCU <--- (VESC_CAN 0x68) ---> VESC
                            /                                   /
SBC --- (VESC_CAN 0x60) -->                                    /
     \                                                        /
      <---------------- (VESC_CAN 0x68) <--------------------
```

## SBC --> MCU

1. SetServoPosCmd command
2. SetDutyCycleCmd command
3. SetCurrentCmd command
4. SetCurrentBrake command
5. SetRpmCmd command
6. SetPositionCmd command

