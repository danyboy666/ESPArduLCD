#include <Arduino.h>

#define LED_PIN 15    // This is the pin the backlight is controlled by, it must be a PWM pin
#define STARTUP_BRIGHTNESS 128  // What backlight brightness to start up with (50% by default).

# define BAUDRATE 9600  // What baudrate to use for the serial port

// lcd width and height
const int LCDW = 16;
const int LCDH = 2;

// include the LCD library code:
#include <LiquidCrystal.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <WiFi.h>
#include <ESP8266WebServer.h>

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "pass"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

#define LCD_PORT 2425
WiFiServer server(LCD_PORT);


/*
 * ethlcd protocol constants
 *
 */

#ifndef ETHLCD_PROTO_H
#define ETHLCD_PROTO_H

#define ETHLCD_SEND_INSTR    0x01
#define ETHLCD_SEND_DATA    0x02
#define ETHLCD_GET_BUTTONS    0x03
#define ETHLCD_SET_BACKLIGHT    0x04
#define ETHLCD_SET_BEEP     0x05
#define ETHLCD_GET_FIRMWARE_VERSION 0x06
#define ETHLCD_GET_PROTOCOL_VERSION 0x07
#define ETHLCD_GET_ENC_REVISION   0x08
#define ETHLCD_CLOSE_CONN   0x09
#define ETHLCD_UNRECOGNIZED_COMMAND 0x0A

#define ETHLCD_BACKLIGHT_ON   0x01
#define ETHLCD_BACKLIGHT_HALF   0x02
#define ETHLCD_BACKLIGHT_OFF    0x03

#endif //ETHLCD_PROTO_H

// initialize the library with the numbers of the interface pins
/*  Note, while all the ardu documentation and schematics show 4-bit operation.
  It seems the library actually supports both 8-bit and 4-bit mode. It attempts
  8-bit, then falls back to 4-bit */
const int rs = 16, en = 5, d4 = 4, d5 = 13, d6 = 12, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void set_backlight(int value) {
  // We can control the backlight via PWM here, range from 0 to 255 values
  // 0 is "off", 255 is "on" (max brightness).
  analogWrite(LED_PIN, value);  // pin 10 is PWM, so we can vary brightness this way
}

void setup() {
  pinMode(LED_PIN, OUTPUT);           // set pin to output
  // We first set the backlight to 50% brightness
  set_backlight(STARTUP_BRIGHTNESS);
  // set up the LCD's number of columns and rows:
  lcd.begin(LCDW, LCDH);
  // set up serial
  lcd.display();
  lcd.clear();
  lcd.write("Ready");

  Serial.begin(BAUDRATE);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println(String("Starting server on port: ") + LCD_PORT);
}

void loop() {

  WiFiClient client = server.available();
   
  int cursor = 0; //current cursor position (emulated)

  if (client) { // if we have a connection

    if (client.connected())
    {
      Serial.println("Client Connected");
    }

    while (client.connected()) { // keep the client open

      while (client.available() > 0) { // while client is available

        int8_t cmd = client.read(); // store the incoming char

        Serial.println(String("Incoming string: ") + cmd);
        client.flush();
        
        switch (cmd) { // switch the incoming char
/*
          case 0x00: // not really sure if this is needed
            while (1) {
              cmd = client.read();
              if (cmd != -1) {
                break;
              }
            }
            lcd.write(cmd);
            break;
*/
          case ETHLCD_SEND_INSTR: // intruction bit 0x01, send command to the LCD

            while (client.available() == 0) {
              delay(1);
            }
            cmd = client.read();
//            Serial.println(String("ETHLCD_SEND_INSTR: ") + cmd);
            lcd.command(cmd);
            client.write(ETHLCD_SEND_INSTR); // reply back to the driver; this is needed, not sure why but it's related the the tcp stuff on server side.
            //cursor = cmd;
            break;

          case ETHLCD_SEND_DATA: // data bit 0x02, we send data to the LCD here
            cmd = client.read();
 //           Serial.println(String("ETHLCD_SEND_DATA: ") + cmd);
            lcd.write(cmd);
            client.write(ETHLCD_SEND_DATA); // reply back to the driver
            cursor++;
            break;

          case ETHLCD_SET_BACKLIGHT: // backlight setup
            cmd = client.read();

            // broken
            /*
            if (cmd == ETHLCD_BACKLIGHT_ON)
            {
              set_backlight(255);
            }
            
              else if (cmd == ETHLCD_BACKLIGHT_HALF)
              {
                set_backlight(128);
              }
              //else set_backlight(0);
              
                else if (cmd == ETHLCD_BACKLIGHT_OFF)
                {
                  //cmd = 1
                  set_backlight(0);
                }

                */

              set_backlight(255); // hard code the backlight for now
//              Serial.println(String("ETHLCD_SET_BACKLIGHT: ") + cmd);
              client.write(ETHLCD_SET_BACKLIGHT); // reply back to the driver
              break;
              
            case ETHLCD_GET_FIRMWARE_VERSION: // not sure if this is needed
//              cmd = client.read();
//              Serial.println(String("case ETHLCD_GET_FIRMWARE_VERSION: ") + cmd);
              client.write(ETHLCD_GET_FIRMWARE_VERSION); // reply back to the driver
              break;

            case ETHLCD_CLOSE_CONN: // not sure if this is needed
              //cmd = client.read();
//              Serial.println(String("case ETHLCD_GET_FIRMWARE_VERSION: ") + cmd);
              client.write(ETHLCD_CLOSE_CONN); // reply back to the driver
              break;
              
            default: // Write back 
              client.write(ETHLCD_UNRECOGNIZED_COMMAND); // lcdproc crash fix
//              Serial.println(String("Default case: ") + cmd);
              //lcd.write(cmd);
              //cursor++;
              break;
              } // end switch cmd
            } //end while client available
        } // end while client connected
        
        client.write(ETHLCD_CLOSE_CONN); // lcdproc crash fix
        client.stop(); // Don't leave the client connected for nothing, lcdproc crash fix
        Serial.println("Client disconnected");
      } // end if client
    }// end main loop
