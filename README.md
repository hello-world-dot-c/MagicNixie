# MagicNixie
![MagicNixie running on an NCS314 Nixie Clock](MagicNixiePictureSmall.jpg)

Firmware to use the GRA &amp; AFCH IN-14 Arduino Shield Nixie Tubes Clock modified to use a Wemos D1 WiFi Arduino-compatible board with ESP8266.

[Video to demonstrate the firmware](https://youtu.be/gqnWdiJaWJw)
## Hardware references
[Schematics for the NCS314 Nixie shield that was used for the modification, version 3.4](http://gra-afch.com/content/uploads/2020/12/Scheme-Shield-NCS314-6-v3.4.pdf)

[Wemos D1 R2 Board Schematic](https://www.openhacks.com/uploadsproductos/wemos-d1-r2-schematic.pdf)

[Arduino Uno Pinout](https://diyi0t.com/wp-content/uploads/2019/08/Arduino-Uno-Pinout-1.png)

## Modifying Nixie Clock Shield NCS314 for the use with WeMos D1 R2 board
The ESP8266 on the WeMos D1 R2 has a lot fewer I/O pins than the typical ATmega on the standard Arduino board so several pins are connected to the same signals and can't be used independently from each other.
|NCS314|Arduino|WeMos D1 R2|ESP8266|
|------|-------|-----------|-------|
|SDA |SDA |D2 |GPIO4 |
|SCL |SCL |D1 |GPIO5 |
|SCK |D13 |D5 |GPIO14 |
|MOSI |D11 |D7 |GPIO13 |
|LE |D10 |D8 |GPIO15 |
|PWM1 |D6 |D4 |GPIO2 |
## Conflicts
Thankfully, in the new v3.4 version of NCS314 the PWM2 and PWM3 signals are not used so there is no conflict with them. The only remaining conflict is the IR signal on Arduino D2 pin which is connected to D14 and SDA. To resolve this, I had to isolate the D2 pin on WeMos. One could also remove the IR receiver.
## Free signals
The only free signals still available are D6/GPIO12 (MISO) and maybe D1/GPIO1 (Tx). Tx is normally connected to the debugging USB UART but here also GPS module if connected. So right now this would be a conflict so it is not supported/not free.
