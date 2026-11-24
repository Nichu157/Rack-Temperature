# Server Rack Temperature Display (RL78/G13 + DS18B20 + Modbus RTU)

Firmware for a small temperature display unit that monitors a server rack. It reads up to three
DS18B20 1-Wire digital thermometers, cycles their readings on a half-inch 7-segment display,
sounds a buzzer and switches a digital output when a set point is exceeded, and exposes everything
to a BMS or PLC over Modbus RTU.

## Features

- Up to **3 DS18B20 sensors** on one 1-Wire bus, each addressed by its unique ROM code
- **Half-inch 7-segment display** with an indicator LED per sensor (`---` when none are configured)
- **4-key keypad** for set point, per-sensor offsets and Modbus device ID
- **Alarm**: buzzer + digital output turn on while any sensor is above the set point
- **Settings saved in data flash** (Renesas PFDL), so they survive power cycles
- **Modbus RTU slave** over UART0
- Runs on **12 V DC**

## Hardware

| Part | Details |
|------|---------|
| MCU | Renesas RL78/G13, R5F100AC |
| Sensors | DS18B20 (1-Wire) x 3 |
| Display | Half-inch 7-segment, multiplexed |
| Comms | UART0 (Modbus RTU) |
| Inputs | 4 push buttons (K1–K4) |

## Modbus register map

Settings: RTU, 9600 baud, 8 data bits, no parity, 1 stop bit. The device ID is set from the keypad.
All analog values are multiplied by 10 (e.g. `253` = 25.3 °C).

| Register | Address | Function code | Access | Range |
|----------|---------|---------------|--------|-------|
| Set point | 1000 | 0x03 / 0x06 | R/W | 15.0 – 35.0 °C |
| Offset, sensors 1–3 | 1001–1003 | 0x03 / 0x06 | R/W | -5.0 – +5.0 °C |
| Temperature, sensors 1–3 | 2000–2002 | 0x04 | R | 0.0 – 50.0 °C |
| Alarm status, sensors 1–3 | 3000–3002 | 0x02 | R | 1 = alarm, 0 = normal |
| Error status, sensors 1–3 | 3003–3005 | 0x02 | R | 1 = error, 0 = normal |

## Pairing sensors (configuration mode)

1. Power off and connect **one** sensor.
2. Hold **ENTER + EXIT** while powering on. The display shows `Con`.
3. Press **ENTER**. `t1` appears; use UP/DOWN to pick the slot (`t1`–`t3`).
4. Press **ENTER** again. `upd` flashes briefly, which means the sensor's ROM code is saved.
5. Power off, label the sensor, and repeat for the others.
6. Connect all paired sensors together and power on normally.

## Project layout

```
cg_src/            Code Generator peripheral drivers + r_main.c (the real main loop)
ds18b20.c/.h       1-Wire bit-banging, ROM search and temperature conversion
display.c/.h       7-segment multiplexing
keypad.c/.h        Key scanning and menu state machine
flash.c/.h         Settings storage in data flash (uses PFDL)
modbus.c/.h        Modbus RTU slave framing and CRC
modbus_user.c/.h   Register map that links Modbus addresses to variables
variables.c        Shared globals
incrl78/, librl78/ Renesas Pattern Flash Data Library (PFDL)
Code_Downloader/   Renesas Flash Programmer project for flashing
```

> `main.c` in the project root is an unused template. The program's entry point is `cg_src/r_main.c`.

## Building

1. Install Renesas **CS+ for CA,CX** (CubeSuite+), which includes the CA78K0R compiler.
2. Open `_1_Wire_for_Hardy_Racks.mtpj`.
3. Build (F7). The output `.mot` file is written to `DefaultBuild/`.
4. Flash it with Renesas Flash Programmer (project in `Code_Downloader/`) or an E1/E2 Lite debugger.

## References

- [RL78/G13 User's Manual: Hardware (R01UH0146EJ)](https://www.renesas.com/document/mah/rl78g13-users-manual-hardware)
- [DS18B20 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS18B20.pdf)
- [Modbus over Serial Line specification](https://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf)
