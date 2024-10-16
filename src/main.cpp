#include <Arduino.h>
/*
#include <Adafruit_NeoPixel.h> //Libreria necesaria, instalarla desde el gestor de librerias

Adafruit_NeoPixel LED_RGB(1, 8, NEO_GRBW + NEO_KHZ800);  // Creamos el objeto que manejará el led rgb

void setup() {
  LED_RGB.begin();            // Inicia el objeto que hemos creado asociado a la librería NeoPixel 
  LED_RGB.setBrightness(150); // Para el brillo del led
}

void loop() {
  LED_RGB.setPixelColor(0, uint32_t(LED_RGB.Color(255,   0,   0))); // Encendemos el led rojo
  LED_RGB.show(); // Enciende el color
  delay(1000);    // Pausa de un segundo
  
  LED_RGB.setPixelColor(0, uint32_t(LED_RGB.Color(  0, 255,   0))); // Encendemos el led verde
  LED_RGB.show(); // Enciende el color
  delay(1000);    // Pausa de un segundo
  
  LED_RGB.setPixelColor(0, uint32_t(LED_RGB.Color(  0,   0, 255))); // Encendemos el led azul
  LED_RGB.show(); // Enciende el color
  delay(1000);    // Pausa de un segundo

}


*/
#include <esp_dmx.h>

/* First, lets define the hardware pins that we are using with our ESP32. We
  need to define which pin is transmitting data and which pin is receiving data.
  DMX circuits also often need to be told when we are transmitting and when we
  are receiving data. We can do this by defining an enable pin. */
int transmitPin = 4; //17;
int receivePin = 5; //16;
int enablePin = 6; //21;
/* Make sure to double-check that these pins are compatible with your ESP32!
  Some ESP32s, such as the ESP32-WROVER series, do not allow you to read or
  write data on pins 16 or 17, so it's always good to read the manuals. */

/* Next, lets decide which DMX port to use. The ESP32 has either 2 or 3 ports.
  Port 0 is typically used to transmit serial data back to your Serial Monitor,
  so we shouldn't use that port. Lets use port 1! */
dmx_port_t dmxPort = 1;

/* Now we want somewhere to store our DMX data. Since a single packet of DMX
  data can be up to 513 bytes long, we want our array to be at least that long.
  This library knows that the max DMX packet size is 513, so we can fill in the
  array size with `DMX_PACKET_SIZE`. */
byte data[DMX_PACKET_SIZE];

/* This variable will allow us to update our packet and print to the Serial
  Monitor at a regular interval. */
unsigned long lastUpdate = millis();

void setup() {
  /* Start the serial connection back to the computer so that we can log
    messages to the Serial Monitor. Lets set the baud rate to 115200. */
  Serial.begin(115200);
  delay(1000); // Give us time to open the Serial Monitor.
  Serial.println("Starting DMX Controller");


  /* Now we will install the DMX driver! We'll tell it which DMX port to use,
    what device configuration to use, and what DMX personalities it should have.
    If you aren't sure which configuration to use, you can use the macros
    `DMX_CONFIG_DEFAULT` to set the configuration to its default settings.
    Because the device is being setup as a DMX controller, this device won't use
    any DMX personalities. */
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_personality_t personalities[] = {};
  int personality_count = 0;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);

  /* Now set the DMX hardware pins to the pins that we want to use and setup
    will be complete! */
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);
}

void loop() {
  /* Get the current time since boot in milliseconds so that we can find out how
    long it has been since we last updated data and printed to the Serial
    Monitor. */
  unsigned long now = millis();

  if (now - lastUpdate >= 1000) {
    /* Increment every byte in our packet. Notice we don't increment the zeroeth
      byte, since that is our DMX start code. Then we must write our changes to
      the DMX packet. */
    for (int i = 1; i < DMX_PACKET_SIZE; i++) {
      data[i]++;
    }
    dmx_write(dmxPort, data, DMX_PACKET_SIZE);

    /* Log our changes to the Serial Monitor. */
    Serial.printf("Sending DMX 0x%02X\n", data[1]);
    lastUpdate = now;
  }

  /* Now we can transmit the DMX packet! */
  dmx_send_num(dmxPort, DMX_PACKET_SIZE);

  /* We can do some other work here if we want. */

  /* If we have no more work to do, we will wait until we are done sending our
    DMX packet. */
  dmx_wait_sent(dmxPort, DMX_TIMEOUT_TICK);
}