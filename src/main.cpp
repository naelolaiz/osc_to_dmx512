#include <Arduino.h>
#include <esp_dmx.h>
#include <array>
#include <WiFi.h>
#include <WebServer.h>

/* Hardware pins for the ESP32 */
int transmitPin = 5;
int receivePin = -1;
int enablePin = -1;

/* DMX port to use */
dmx_port_t dmxPort = 1;

/* Wi-Fi credentials */
//const char* ssid = "YourSSID";      // Replace with your Wi-Fi SSID
//const char* password = "YourPassword";  // Replace with your Wi-Fi Password
#include "credentials.h"

/* Web server running on port 80 */
WebServer server(80);

class NineChannelsModeDmxData
{
public:
    enum class Mode : uint8_t
    {
        LASER_OFF = 0,
        AUTO_MODE = 64,
        SOUND_MODE = 128,
        DMX_MODE = 192
    };

    NineChannelsModeDmxData()
    {
        mData.fill(0);
        setMode(NineChannelsModeDmxData::Mode::DMX_MODE);
    }

    void setMode(NineChannelsModeDmxData::Mode mode)
    {
        mMode = mode;
        mData[1] = static_cast<uint8_t>(mode);
    }

    Mode getMode() const
    {
        return mMode;
    }

    void setPatternValue(uint8_t value)
    {
        mPatternValue = value;
        mData[3] = value * 8;
    }

    uint8_t getPatternValue() const
    {
        return mPatternValue;
    }

    // Zooming (Channel 4)
    void setZoomMode(bool manual)
    {
        mZoomManual = manual;
        updateZoomChannel();
    }

    bool getZoomMode() const
    {
        return mZoomManual;
    }

    void setZoomValue(uint8_t value)
    {
        mZoomValue = value;
        updateZoomChannel();
    }

    uint8_t getZoomValue() const
    {
        return mZoomValue;
    }

    // X Axis Rolling (Channel 5)
    void setXAxisRollingMode(bool manual)
    {
        mXRollingManual = manual;
        updateXAxisRollingChannel();
    }

    bool getXAxisRollingMode() const
    {
        return mXRollingManual;
    }

    void setXAxisRollingValue(uint8_t value)
    {
        mXAxisRollingValue = value;
        updateXAxisRollingChannel();
    }

    uint8_t getXAxisRollingValue() const
    {
        return mXAxisRollingValue;
    }

    // Y Axis Rolling (Channel 6)
    void setYAxisRollingMode(bool manual)
    {
        mYRollingManual = manual;
        updateYAxisRollingChannel();
    }

    bool getYAxisRollingMode() const
    {
        return mYRollingManual;
    }

    void setYAxisRollingValue(uint8_t value)
    {
        mYAxisRollingValue = value;
        updateYAxisRollingChannel();
    }

    uint8_t getYAxisRollingValue() const
    {
        return mYAxisRollingValue;
    }

    // Z Axis Rolling (Channel 7)
    void setZAxisRollingMode(bool manual)
    {
        mZRollingManual = manual;
        updateZAxisRollingChannel();
    }

    bool getZAxisRollingMode() const
    {
        return mZRollingManual;
    }

    void setZAxisRollingValue(uint8_t value)
    {
        mZAxisRollingValue = value;
        updateZAxisRollingChannel();
    }

    uint8_t getZAxisRollingValue() const
    {
        return mZAxisRollingValue;
    }

    // X Axis Moving (Channel 8)
    void setXAxisMovingMode(bool manual)
    {
        mXMovingManual = manual;
        updateXAxisMovingChannel();
    }

    bool getXAxisMovingMode() const
    {
        return mXMovingManual;
    }

    void setXAxisMovingValue(uint8_t value)
    {
        mXAxisMovingValue = value;
        updateXAxisMovingChannel();
    }

    uint8_t getXAxisMovingValue() const
    {
        return mXAxisMovingValue;
    }

    // Y Axis Moving (Channel 9)
    void setYAxisMovingMode(bool manual)
    {
        mYMovingManual = manual;
        updateYAxisMovingChannel();
    }

    bool getYAxisMovingMode() const
    {
        return mYMovingManual;
    }

    void setYAxisMovingValue(uint8_t value)
    {
        mYAxisMovingValue = value;
        updateYAxisMovingChannel();
    }

    uint8_t getYAxisMovingValue() const
    {
        return mYAxisMovingValue;
    }

    const uint8_t* getData() const
    {
        return mData.data();
    }

    size_t getPacketSize() const
    {
        return mData.size();
    }

private:
    std::array<uint8_t, DMX_PACKET_SIZE> mData{};
    Mode mMode;
    uint8_t mPatternValue{0u};

    // Zooming (Channel 4)
    bool mZoomManual{true};
    uint8_t mZoomValue{64u};

    void updateZoomChannel()
    {
        if (mZoomManual)
            mData[4] = 127 - mZoomValue;  // Manual Pattern Size (0-127)
        else
            mData[4] = mZoomValue + 128;  // Loop Zooming In/Out Speed (128-255)
    }

    // X Axis Rolling (Channel 5)
    bool mXRollingManual{true};
    uint8_t mXAxisRollingValue{64u};

    void updateXAxisRollingChannel()
    {
        if (mXRollingManual)
            mData[5] = mXAxisRollingValue;  // Manual Rolling (0-127)
        else
            mData[5] = mXAxisRollingValue + 128;  // Rolling Speed (128-255)
    }

    // Y Axis Rolling (Channel 6)
    bool mYRollingManual{true};
    uint8_t mYAxisRollingValue{64u};

    void updateYAxisRollingChannel()
    {
        if (mYRollingManual)
            mData[6] = mYAxisRollingValue;  // Manual Rolling (0-127)
        else
            mData[6] = mYAxisRollingValue + 128;  // Rolling Speed (128-255)
    }

    // Z Axis Rolling (Channel 7)
    bool mZRollingManual{true};
    uint8_t mZAxisRollingValue{64u};

    void updateZAxisRollingChannel()
    {
        if (mZRollingManual)
            mData[7] = mZAxisRollingValue;  // Manual Rolling (0-127)
        else
            mData[7] = mZAxisRollingValue + 128;  // Rolling Speed (128-255)
    }

    // X Axis Moving (Channel 8)
    bool mXMovingManual{true};
    uint8_t mXAxisMovingValue{64u};

    void updateXAxisMovingChannel()
    {
        if (mXMovingManual)
            mData[8] = mXAxisMovingValue;  // Manual Position (0-127)
        else
            mData[8] = mXAxisMovingValue + 128;  // Moving Speed (128-255)
    }

    // Y Axis Moving (Channel 9)
    bool mYMovingManual{true};
    uint8_t mYAxisMovingValue{64u};

    void updateYAxisMovingChannel()
    {
        if (mYMovingManual)
            mData[9] = mYAxisMovingValue;  // Manual Position (0-127)
        else
            mData[9] = mYAxisMovingValue + 128;  // Moving Speed (128-255)
    }
};

NineChannelsModeDmxData dmxData;

/* Handler for the root path */
void handleRoot()
{
    String html = "<html><head><title>DMX Controller</title><script>";

    // JavaScript function to toggle visibility
    html += R"(
    function toggleVisibility(control, isManual) {
        var manualInput = document.getElementById(control + 'ManualValue');
        var speedInput = document.getElementById(control + 'SpeedValue');
        if (isManual) {
            manualInput.style.display = 'block';
            speedInput.style.display = 'none';
        } else {
            manualInput.style.display = 'none';
            speedInput.style.display = 'block';
        }
    }
    )";

    html += "</script></head><body>";
    html += "<h1>DMX Controller Settings</h1>";
    html += "<form action=\"/set\" method=\"POST\">";

    // Mode selection
    html += "Mode: <select name=\"mode\">";
    NineChannelsModeDmxData::Mode currentMode = dmxData.getMode();
    html += "<option value=\"0\"" + String(currentMode == NineChannelsModeDmxData::Mode::LASER_OFF ? " selected" : "") + ">Laser Off</option>";
    html += "<option value=\"64\"" + String(currentMode == NineChannelsModeDmxData::Mode::AUTO_MODE ? " selected" : "") + ">Auto Mode</option>";
    html += "<option value=\"128\"" + String(currentMode == NineChannelsModeDmxData::Mode::SOUND_MODE ? " selected" : "") + ">Sound Mode</option>";
    html += "<option value=\"192\"" + String(currentMode == NineChannelsModeDmxData::Mode::DMX_MODE ? " selected" : "") + ">DMX Mode</option>";
    html += "</select><br><br>";

    // Pattern Value
    html += "Pattern Value (0-31): <input type=\"number\" name=\"patternValue\" min=\"0\" max=\"31\" value=\"" + String(dmxData.getPatternValue()) + "\"><br><br>";

    // Controls
    struct Control {
        String name;
        String displayName;
        bool manualMode;
        uint8_t value;
    };

    // List of controls
    Control controls[] = {
        {"zoom", "Zooming Control", dmxData.getZoomMode(), dmxData.getZoomValue()},
        {"xAxisRolling", "X Axis Rolling Control", dmxData.getXAxisRollingMode(), dmxData.getXAxisRollingValue()},
        {"yAxisRolling", "Y Axis Rolling Control", dmxData.getYAxisRollingMode(), dmxData.getYAxisRollingValue()},
        {"zAxisRolling", "Z Axis Rolling Control", dmxData.getZAxisRollingMode(), dmxData.getZAxisRollingValue()},
        {"xAxisMoving", "X Axis Moving Control", dmxData.getXAxisMovingMode(), dmxData.getXAxisMovingValue()},
        {"yAxisMoving", "Y Axis Moving Control", dmxData.getYAxisMovingMode(), dmxData.getYAxisMovingValue()}
    };

    for (const auto& control : controls)
    {
        html += "<h3>" + control.displayName + "</h3>";
        String manualChecked = control.manualMode ? "checked" : "";
        String speedChecked = !control.manualMode ? "checked" : "";

        html += "<input type=\"radio\" name=\"" + control.name + "Mode\" id=\"" + control.name + "Manual\" value=\"manual\" " + manualChecked + " onclick=\"toggleVisibility('" + control.name + "', true)\"> Manual";
        html += "<input type=\"radio\" name=\"" + control.name + "Mode\" id=\"" + control.name + "Speed\" value=\"speed\" " + speedChecked + " onclick=\"toggleVisibility('" + control.name + "', false)\"> Speed<br>";

        // Manual Input
        html += "<div id=\"" + control.name + "ManualValue\" style=\"display:" + (control.manualMode ? "block" : "none") + ";\">";
        html += control.displayName + " (0-127): ";
        html += "<input type=\"number\" name=\"" + control.name + "ManualValue\" min=\"0\" max=\"127\" value=\"" + String(control.manualMode ? control.value : 0) + "\"><br><br>";
        html += "</div>";

        // Speed Input
        html += "<div id=\"" + control.name + "SpeedValue\" style=\"display:" + (!control.manualMode ? "block" : "none") + ";\">";
        html += control.displayName + " Speed (0-127): ";
        html += "<input type=\"number\" name=\"" + control.name + "SpeedValue\" min=\"0\" max=\"127\" value=\"" + String(!control.manualMode ? control.value : 0) + "\"><br><br>";
        html += "</div>";
    }

    html += "<input type=\"submit\" value=\"Set Parameters\">";
    html += "</form>";

    // Initialize visibility
    html += "<script>";
    for (const auto& control : controls)
    {
        html += "toggleVisibility('" + control.name + "', " + (control.manualMode ? "true" : "false") + ");";
    }
    html += "</script>";

    html += "</body></html>";

    server.send(200, "text/html", html);
}

/* Handler for setting parameters */
void handleSet()
{
    if (server.method() == HTTP_POST)
    {
        // Mode
        if (server.hasArg("mode"))
        {
            int modeValue = server.arg("mode").toInt();
            NineChannelsModeDmxData::Mode mode = static_cast<NineChannelsModeDmxData::Mode>(modeValue);
            dmxData.setMode(mode);
        }

        // Pattern Value
        if (server.hasArg("patternValue"))
        {
            int patternValue = server.arg("patternValue").toInt();
            dmxData.setPatternValue(patternValue);
        }

        // Controls
        struct Control {
            String name;
            std::function<void(bool)> setMode;
            std::function<void(uint8_t)> setValue;
        };

        Control controls[] = {
            {"zoom", [&](bool manual){ dmxData.setZoomMode(manual); }, [&](uint8_t value){ dmxData.setZoomValue(value); }},
            {"xAxisRolling", [&](bool manual){ dmxData.setXAxisRollingMode(manual); }, [&](uint8_t value){ dmxData.setXAxisRollingValue(value); }},
            {"yAxisRolling", [&](bool manual){ dmxData.setYAxisRollingMode(manual); }, [&](uint8_t value){ dmxData.setYAxisRollingValue(value); }},
            {"zAxisRolling", [&](bool manual){ dmxData.setZAxisRollingMode(manual); }, [&](uint8_t value){ dmxData.setZAxisRollingValue(value); }},
            {"xAxisMoving", [&](bool manual){ dmxData.setXAxisMovingMode(manual); }, [&](uint8_t value){ dmxData.setXAxisMovingValue(value); }},
            {"yAxisMoving", [&](bool manual){ dmxData.setYAxisMovingMode(manual); }, [&](uint8_t value){ dmxData.setYAxisMovingValue(value); }}
        };

        for (auto& control : controls)
        {
            if (server.hasArg(control.name + "Mode"))
            {
                bool manual = server.arg(control.name + "Mode") == "manual";
                control.setMode(manual);
                if (manual && server.hasArg(control.name + "ManualValue"))
                {
                    int value = server.arg(control.name + "ManualValue").toInt();
                    control.setValue(value);
                }
                else if (!manual && server.hasArg(control.name + "SpeedValue"))
                {
                    int value = server.arg(control.name + "SpeedValue").toInt();
                    control.setValue(value);
                }
            }
        }

        server.sendHeader("Location", "/");
        server.send(303);
    }
    else
    {
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting DMX Controller");

    /* Connect to Wi-Fi */
    WiFi.begin(ssid, password);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    /* Set up the web server */
    server.on("/", handleRoot);
    server.on("/set", handleSet);
    server.begin();
    Serial.println("HTTP server started");

    /* Install the DMX driver */
    dmx_config_t config = DMX_CONFIG_DEFAULT;
    dmx_personality_t personalities[] = {};
    int personality_count = 0;
    dmx_driver_install(dmxPort, &config, personalities, personality_count);

    /* Set the DMX hardware pins */
    dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);
}

void loop()
{
    server.handleClient();

    /* Send the DMX data with current parameters */
    dmx_write(dmxPort, dmxData.getData(), dmxData.getPacketSize());
    dmx_send(dmxPort);
    dmx_wait_sent(dmxPort, DMX_TIMEOUT_TICK);
    delay(30);
}
