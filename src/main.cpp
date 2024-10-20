#include <Arduino.h>
#include <esp_dmx.h>
#include <array>

/* First, lets define the hardware pins that we are using with our ESP32. We
  need to define which pin is transmitting data and which pin is receiving data.
  DMX circuits also often need to be told when we are transmitting and when we
  are receiving data. We can do this by defining an enable pin. */
int transmitPin = 5; //17;
int receivePin = -1; //5; //16;
int enablePin = -1; //6; //21;
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
//byte mData[DMX_PACKET_SIZE];

/* This variable will allow us to update our packet and print to the Serial
  Monitor at a regular interval. */
unsigned long lastUpdate = millis();
class ThreeChannelsModeDmxData
{
  public:
  enum class Mode : uint8_t
  {
    LASER_OFF=0,
    SOUND_MODE = 101,
    AUTO_MODE = 156
  };
  ThreeChannelsModeDmxData()
  {
    mData.fill(0);
    setMode(Mode::AUTO_MODE);
    setManualPatternSize(80);
  }
  void setMode(Mode mode)
  {
    mMode=mode;
    mData[1] = static_cast<uint8_t>(mode);
  }
  void setManualPatternSize(uint8_t value)
  {
    mManualPatternSize=value;
    mData[3] = value;
  }
  const uint8_t *getData() const
  {
    return mData.data();
  }
  uint8_t getPacketSize() const
  {
    return mData.size();
  }
  private:
  // ch 1: Zooming (0-127: manual pattern size, 128-255: loop zooming in/out)
  uint8_t mManualPatternSize{0u}; // 0-127, manual pattern size
  uint8_t mLoopZoomingInOut{128u}; // 128-255, loop zooming in/out
  // ch 3: Mode (0-100: laser off, 101-155: Sound Mode, 156-255 Auto mode with rising speed 0-100)
//  uint8_t mMode{0u}; // 0-100, laser off
  Mode mMode;
  std::array<uint8_t,DMX_PACKET_SIZE> mData{};

};
class NineChannelsModeDmxData
{
  public:
  enum class Mode : uint8_t
  {
    LASER_OFF=0,
    AUTO_MODE = 64,
    SOUND_MODE = 128,
    DMX_MODE = 192
  };
  NineChannelsModeDmxData()
  {
    mData.fill(0);
    setMode(NineChannelsModeDmxData::Mode::DMX_MODE);
    setPatternValue(5);
    setManualPatternSize(100);
    //setVerticalMovingSpeed(5);

  }
  void setMode(NineChannelsModeDmxData::Mode mode)
  {
    mMode=mode;
    mData[1] = static_cast<uint8_t>(mode);
  }
  void setPatternValue(uint8_t value)
  {
    mPatternValue=value;
    mData[3] = value*8;
  }
  void setManualPatternSize(uint8_t value)
  {
    mManualPatternSize=value;
    mData[4] = value;
  }
  void setLoopZoomingInOut(uint8_t value)
  {
    mLoopZoomingInOut=value+128;
    mData[4] = mLoopZoomingInOut;
  }
  void setXAxisRolling(uint8_t value)
  {
    mXAxisRolling=value;
    mData[5] = value;
  }
  void setXRollingSpeed(uint8_t value)
  {
    mXRollingSpeed=value+128;
    mData[5] = mXRollingSpeed;
  }
  void setYAxisRolling(uint8_t value)
  {
    mYAxisRolling=value;
    mData[6] = value;
  }
  void setYRollingSpeed(uint8_t value)
  {
    mYRollingSpeed=value+128;
    mData[6] = mYRollingSpeed;
  }
  void setZAxisRolling(uint8_t value)
  {
    mZAxisRolling=value;
    mData[7] = value;
  }
  void setZRollingSpeed(uint8_t value)
  {
    mZRollingSpeed=value+128;
    mData[7] = mZRollingSpeed;
  }
  void setXAxisManualPosition(uint8_t value)
  {
    mXAxisManualPosition=value;
    mData[8] = value;
  }
  void setHorizontalMovingSpeed(uint8_t value)
  {
    mHorizontalMovingSpeed=value+128;
    mData[8] = mHorizontalMovingSpeed;
  }
  void setYAxisManualPosition(uint8_t value)
  {
    mYAxisManualPosition=value;
    mData[9] = value;
  }
  void setVerticalMovingSpeed(uint8_t value)
  {
    mVerticalMovingSpeed=value+128;
    mData[9] = mVerticalMovingSpeed;
  }

  const uint8_t *getData() const
  {
    return mData.data();
  }
  uint8_t getPacketSize() const
  {
    return mData.size();
  }
private:
  std::array<uint8_t,DMX_PACKET_SIZE> mData{};
  // ch 1: mode select
  Mode mMode; 
  // ch 3: pattern select
  uint8_t mPatternValue{0u}; // 0-31 -> 0-255
  // ch 4: zooming
  uint8_t mManualPatternSize{0u}; // 0-127, manual pattern size
  uint8_t mLoopZoomingInOut{128u}; // 128-255, loop zooming in/out
  // ch 5: X axis rolling
  uint8_t mXAxisRolling{0u}; // 0-127, manual rolling
  uint8_t mXRollingSpeed{128u}; // 128-255, rolling speed
  // ch 6: Y axis rolling
  uint8_t mYAxisRolling{0u}; // 0-127, manual rolling
  uint8_t mYRollingSpeed{128u}; // 128-255, rolling speed
  // ch 7: Z axis rolling
  uint8_t mZAxisRolling{0u}; // 0-127, manual rolling
  uint8_t mZRollingSpeed{128u}; // 128-255, rolling speed
  // ch 8: X axis moving
  uint8_t mXAxisManualPosition{0u}; // 0-127, horizontal manual position
  uint8_t mHorizontalMovingSpeed{128u}; // 128-255, horizontal moving speed
  // ch 9: Y axis moving
  uint8_t mYAxisManualPosition{0u}; // 0-127, vertical manual position
  uint8_t mVerticalMovingSpeed{128u}; // 128-255, vertical moving speed
  
};

void setup() {
  /* Start the serial connection back to the computer so that we can log
    messages to the Serial Monitor. Lets set the baud rate to 115200. */
  Serial.begin(115200);
//  memset(data, 0, DMX_PACKET_SIZE);
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
//ThreeChannelsModeDmxData dmxData;
NineChannelsModeDmxData dmxData;
void loop() {
  #if 0
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
  #endif
  for (uint8_t patternValue = 0; patternValue < 32; ++patternValue)
  {
    dmxData.setPatternValue(patternValue);
    for (uint8_t manualPatternSize = 0; manualPatternSize < 255; ++manualPatternSize)
    {
      auto patternSize = manualPatternSize<128?manualPatternSize:255-manualPatternSize;
      dmxData.setManualPatternSize(patternSize);
      dmx_write(dmxPort, dmxData.getData(), 512);//dmxData.getPacketSize());
      dmx_send(dmxPort);
      dmx_wait_sent(dmxPort, DMX_TIMEOUT_TICK);
      delay(60);
    }
    delay(500);
  }
#if 0
      dmx_write(dmxPort, dmxData.getData(), 512);
      dmx_send(dmxPort);
      dmx_wait_sent(dmxPort, DMX_TIMEOUT_TICK);
      delay(1000);
#endif

}