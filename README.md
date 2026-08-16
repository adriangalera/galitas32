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

## Requirements

The physical mechanisms must work regardless of the network connection, i.e: door sensor, LED strip, camera, RFID reader and speaker. 

When the network is enabled, it will act as an enhacement.

> `*` optional steps (the alarm will still work without them)

### Startup procedure

1. Setup read sensor
2. Setup LED strip
3. Setup camera
4. Setup RFID reader
5. Setup speaker
6. `*`Setup Wifi
7. `*`Setup MQTT
8. Determine the lock status: armed/disarmed

### Door sensor

When the door is open:

- The LED strip is powered, the color depends on the lock status:
    - Armed: blinking red/blue like a policen siren
    - Disarmed: welcoming color: light orange or something like this

- Camera takes pictures for 30 seconds one picture each second:
    - Armed: send the pictures via MQTT (to telegram) and store the images in the SD
    - Disarmed: store the images in the SD

- Speaker: 
    - Armed: powered and making noises
    - Disarmed: do nothing

- `*` MQTT:
    - Armed: While open send message about door opened
    - Disarmed: While open send message about door opened

When the door is closed:

- LED strip: powered down
- Camera: stop take pictures task (if running)
- Speaker: powered down
- MQTT: stop sending message task


### RFID reader

The RFID read will be used to toogle the status and deactivate the alerting devices when the alarm is armed:

When the RFID tag is detected in the reader:
- Disarmed: 
    - Set the alarm in armed state
    - Send a message about the state toggling `*`

- Armed:
    - Set the alarm in disarmed state
    - Send a message about the state toggling `*`
    - The alerting devices will see the disarmed state


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

## Case

The 3D design of the case it is AI-enabled. Make sure to install:

```bash
brew install admesh
pip3 install trimesh rtree scipy
```
