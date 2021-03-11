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

I've added replys client.write(cmd) since the original code was handling the tcp layer this is the way I found to send tcp ack. Not sure if this proper or not but it works and I removed all other cases since i'm only handling the lcdproc requests; 

# lcdproc configuration

This is how to configure LCDd.conf to work with ESPArduLCD

```
Driver=hd44780


## Hitachi HD44780 driver ##
[hd44780]

# Select what type of connection. See documentation for availabe types.
ConnectionType=ethlcd

# Device of the serial, I2C, or SPI interface [default: /dev/lcd]
Device=ip_of_you_esp

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

You will need to edit your lcdproc service file; I,ve made some changes to the code so it doesn't keep the client connected for nothing but after another crash i've edited the service to restarted itself.

```
/etc/systemd/system/multi-user.target.wants/lcdproc.service
```

```
[Unit]
Description=LCD display daemon
Documentation=man:LCDd(8) http://www.lcdproc.org/

[Service]
Restart=always
User=root
ExecStart=/usr/sbin/LCDd -s 1 -f -c /etc/LCDd.conf

[Install]
WantedBy=multi-user.target
```

```
[code]2021-03-11 08:28:38.824 T:281471654798512 WARNING: ### [XBMC LCDproc] - Reply to 'noop' was ''
2021-03-11 08:28:38.824 T:281471654798512   ERROR: ### [XBMC LCDproc] - noop failed in IsConnected(), aborting!
2021-03-11 08:28:41.187 T:281471654798512  NOTICE: ### [XBMC LCDproc] - Connected to LCDd at 0.0.0.0:13666, Protocol version 0.3 - Geometry 16x2 characters (80x16 pixels, 5x8 pixels per character)
2021-03-11 08:28:41.218 T:281471654798512  NOTICE: ### [XBMC LCDproc] - Empty driver information reply
2021-03-11 08:28:41.387 T:281471654798512  NOTICE: ### [XBMC LCDproc] - Loading settings from /var/lib/kodi/.kodi/addons/script.xbmc.lcdproc/resources/LCD.xml.defaults
2021-03-11 08:28:41.394 T:281471654798512  NOTICE: ### [XBMC LCDproc] - Loading settings from /var/lib/kodi/.kodi/userdata/LCD.xml
[/code]
```

In action:

https://www.youtube.com/watch?v=BneBLG7jdJY
