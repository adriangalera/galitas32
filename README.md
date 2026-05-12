# galitas32

Home alarm built with ESP32

Features:

- Read door sensor
- Camera to store/send images
- Buzzer to scare burglars
- LED strip with programmable colors
- RFID to lock/unlock the alarm
- Connected to MQTT to interact with the system via telegram, through a Raspberry Pi acting as MQTT Broker and Telegram bot.

This is built using [ESP32-S3-CAM board](https://www.amazon.es/dp/B0F4D8ZY6L?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1). 

Built with [PlatformIO](https://docs.platformio.org/en/latest/)

## Wirings

### Door sensor

```text

ESP32
-------
   Common GND -------------------------------+
                                             |
   D2 (input pin)                            |
        |                                    |
        |                                    |
        +--------------------[ REED SWITCH ]+
                             (magnet closes contact)
```

### LED Strip

```text

External 5V Power Supply
------------------------
    +5V ----------------------------+------------------> LED Strip +5V
                                    |
    GND ----------------------------+------------------> LED Strip GND
                                    |
                                    +------------------> Common GND


ESP32
-------
   D6 (data pin)
        |
        |   330Ω (recommended)
        +----/\/\/\/\-------------------------> DIN (LED Strip Data In)
```

There's an external power source providing 5V for the LED strip. The GND should be common with the ESP32 board.

The LED strip needs to be programmed through the Data In line. Therefore the output pin of the ESP32 will be connected to DIN port in the LED strip. The resistor protects the pin from current surges caused by the LED strip.


### Buzzer

```text
    ESP
    --------
        PIN 21
        |
        |   1kΩ
        +---/\/\/\----+
                        |
                        |
                    B |/
GND ---------------- |   TIP120
                    C |\ 
                        |
                        +--------(-) Buzzer (+)-------- +5V
                        |
                    E  |
                        |
                        GND
```

**Base**: connected with the ESP32 PIN via a 1K resistor for protection.

**Collector**: connected to the negative side of the buzzer. This acts as the switch to close the circuit when the base has signal coming from ESP32.

**Emitter**: connected to GND