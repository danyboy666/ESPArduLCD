# ESPArduLCD

ESP8266 based interface for HD44780 LCD to lcdproc and equivalent.

Based on http://manio.skyboo.net/ethlcd/ 
and
https://github.com/ZivaVatra/ArduLCD
 

Credit goes to both of those devs. I ported what already existed for my purposes. 

Main code is checking for :

- ETHLCD_SEND_INSTR	0x01
- ETHLCD_SEND_DATA	0x02
- ETHLCD_SET_BACKLIGHT	0x04

I've added replies client.write(cmd) since the original code was handling the tcp layer this is the way I found to send tcp ack. Not sure if this proper or not but it works and I removed all other cases since i'm only handling the lcdproc requests; 

# lcdproc configuration

This is how to configure LCDd.conf to work with ESPArduLCD

```
Driver=hd44780


## Hitachi HD44780 driver ##
[hd44780]

# Select what type of connection. See documentation for availabe types.
ConnectionType=ethlcd

# Device of the serial, I2C, or SPI interface [default: /dev/lcd]
Device=ip_of_your_esp

# Bitrate of the serial port (0 for interface default)
Speed=9600

# If you have a keypad connected.
# You may also need to configure the keypad layout further on in this file.
Keypad=no

# Specifies the size of the LCD.
# In case of multiple combined displays, this should be the total size.
Size=16x2

# Character map to to map ISO-8859-1 to the LCD's character set
# [default: hd44780_default; legal: hd44780_default, hd44780_euro, ea_ks0073,
# sed1278f_0b, hd44780_koi8_r, hd44780_cp1251, hd44780_8859_5, upd16314 ]
# (hd44780_koi8_r, hd44780_cp1251, hd44780_8859_5 and upd16314 are possible if
# compiled with additional charmaps)
CharMap=hd44780_default

# If your display is slow and cannot keep up with the flow of data from
# LCDd, garbage can appear on the LCDd. Set this delay factor to 2 or 4
# to increase the delays. Default: 1.
#DelayMult=2

# Some displays (e.g. vdr-wakeup) need a message from the driver to that it
# is still alive. When set to a value bigger then null the character in the
# upper left corner is updated every <KeepAliveDisplay> seconds. Default: 0.
#KeepAliveDisplay=0

# If you experience occasional garbage on your display you can use this
# option as workaround. If set to a value bigger than null it forces a
# full screen refresh <RefreshDiplay> seconds. Default: 0.
#RefreshDisplay=5



```

# lcdproc service

You will need to edit your lcdproc service file; I've made some changes to the code so it doesn't keep the client connected for nothing but after another crash i've edited the service to restarted itself.

```
lcdproc.service
```

```
[Unit]
Description=LCD display daemon
Documentation=man:LCDd(8) http://www.lcdproc.org/

[Service]
Type=simple
ExecStart=/usr/sbin/LCDd -s 1 -f -c /etc/LCDd.conf
User=lcdproc
TimeoutStopSec=1s
Restart=on-failure
RestartSec=5
StartLimitInterval=30
StartLimitBurst=5

[Install]
WantedBy=multi-user.target

```


```
sudo systemctl daemon-reload
```


```
tail -f /var/lib/kodi/.kodi/temp/kodi.log
```

```
2021-03-28 15:05:44.790 T:281471839339696  NOTICE: ### [XBMC LCDproc] - Connected to LCDd at 0.0.0.0:13666, Protocol version 0.3 - Geometry 16x2 characters (80x16 pixels, 5x8 pixels per character)
2021-03-28 15:06:19.083 T:281471839339696  NOTICE: ### [XBMC LCDproc] - Empty driver information reply
2021-03-28 15:06:19.194 T:281471570904240  NOTICE: service.xbmc.tts: 1.0.8
2021-03-28 15:06:19.195 T:281471570904240  NOTICE: service.xbmc.tts: Platform: linux2
2021-03-28 15:06:19.882 T:281471570904240  NOTICE: service.xbmc.tts: playSFX() has useCached
2021-03-28 15:06:22.069 T:281471839339696  NOTICE: ### [XBMC LCDproc] - Loading settings from /var/lib/kodi/.kodi/addons/script.xbmc.lcdproc/resources/LCD.xml.defaults
2021-03-28 15:06:22.101 T:281471839339696  NOTICE: ### [XBMC LCDproc] - Loading settings from /var/lib/kodi/.kodi/userdata/LCD.xml
2021-03-28 15:06:23.149 T:281472275559600  NOTICE: service.xbmc.tts: Threaded TTS Started: Speech-Dispatcher
2021-03-28 15:06:23.196 T:281471570904240  NOTICE: service.xbmc.tts: Backend: Speech-Dispatcher
2021-03-28 15:06:23.196 T:281471570904240  NOTICE: service.xbmc.tts: SERVICE STARTED :: Interval: 100ms
2021-03-28 15:06:23.669 T:281472258774192  NOTICE: service.xbmc.tts: Threaded TTS Started: Speech-Dispatcher
2021-03-28 15:06:23.727 T:281471570904240  NOTICE: service.xbmc.tts: Backend: Speech-Dispatcher
2021-03-28 15:06:24.154 T:281472275559600  NOTICE: service.xbmc.tts: Threaded TTS Finished: Speech-Dispatcher


```


In action:

https://www.youtube.com/watch?v=BneBLG7jdJY
