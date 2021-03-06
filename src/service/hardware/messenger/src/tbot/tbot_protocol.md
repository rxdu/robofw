# TBot Communication Protocol

## 1. Computer --> Robot

### PWM command

* Left PWM: uint8_t
* Right PWM: uint8_t

|  ID   | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
| :---: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| 0x101 |  left  | right  |        |        |        |        |        |        |

### Motor command

* Left RPM: int32_t
* Right RPM: int32_t

|  ID   | Byte 0  | Byte 1  | Byte 2  | Byte 3  |  Byte 4  |  Byte 5  |  Byte 6  |  Byte 7  |
| :---: | :-----: | :-----: | :-----: | :-----: | :------: | :------: | :------: | :------: |
| 0x102 | left_B0 | left_B1 | left_B2 | left_B3 | right_B0 | right_B1 | right_B2 | right_B3 |

### Motion command

* Linear X: float (4-byte)
* Angular Z: float (4-byte)

|  ID   |  Byte 0  |  Byte 1  |  Byte 2  |  Byte 3  |  Byte 4  |  Byte 5  |  Byte 6  |  Byte 7  |
| :---: | :------: | :------: | :------: | :------: | :------: | :------: | :------: | :------: |
| 0x103 | lin_x_B0 | lin_x_B1 | lin_x_B2 | lin_x_B3 | ang_z_B0 | ang_z_B1 | ang_z_B2 | ang_z_B3 |

## 2. Robot --> Computer

### RC state

* Throttle channel: int16_t
* Steering channel: int16_t

|  ID   |   Byte 0    |   Byte 1    |   Byte 2    |   Byte 3    | Byte 4  | Byte 5  | Byte 6 | Byte 7 |
| :---: | :---------: | :---------: | :---------: | :---------: | :-----: | :-----: | :----: | :----: |
| 0x201 | throttle_B0 | throttle_B1 | steering_B0 | steering_B1 | var0_B0 | var0_B1 |  sw0   |  sw1   |

### Encoder raw

* Left RPM: int32_t
* Right RPM: int32_t

|  ID   | Byte 0  | Byte 1  | Byte 2  | Byte 3  |  Byte 4  |  Byte 5  |  Byte 6  |  Byte 7  |
| :---: | :-----: | :-----: | :-----: | :-----: | :------: | :------: | :------: | :------: |
| 0x211 | left_B0 | left_B1 | left_B2 | left_B3 | right_B0 | right_B1 | right_B2 | right_B3 |

### Encoder filtered

* Left RPM: int32_t
* Right RPM: int32_t

|  ID   | Byte 0  | Byte 1  | Byte 2  | Byte 3  |  Byte 4  |  Byte 5  |  Byte 6  |  Byte 7  |
| :---: | :-----: | :-----: | :-----: | :-----: | :------: | :------: | :------: | :------: |
| 0x212 | left_B0 | left_B1 | left_B2 | left_B3 | right_B0 | right_B1 | right_B2 | right_B3 |