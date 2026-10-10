# AQUA SAFETY – LIFESAVIOUR

<p align="center">
  <strong>IoT-Based Water Rescue and Life-Saviour System</strong>
</p>

---

## 1. Objective

The objective of Aqua Safety is to develop an IoT-based water safety prototype that detects continuous water contact, activates a local emergency alarm, and displays GPS location information to support rescue operations.

The main objectives are:

- To detect water contact using a water sensor.
- To activate a buzzer and red LED after 20 seconds of continuous water detection.
- To obtain latitude, longitude, and altitude using a GPS module.
- To provide a web dashboard for real-time monitoring.
- To share the safety guard's GPS location and calculate the approximate distance between the user and guard.

## 2. Requirements

### Hardware Requirements

1. ESP32-WROOM-32 development board
2. Water sensor
3. NEO-6M GPS module
4. Buzzer
5. Red LED with a current-limiting resistor
6. Jumper wires
7. Breadboard or PCB
8. Battery and suitable power supply/charging circuit
9. Waterproof enclosure (recommended)

### Software Requirements

1. Arduino IDE
2. ESP32 board support package
3. TinyGPSPlus library
4. Wi-Fi and WebServer libraries
5. Mobile phone or computer with a web browser

### Pin Configuration

| Component | ESP32 Pin |
|---|---|
| Water sensor | GPIO 34 |
| Red emergency LED | GPIO 25 |
| Buzzer | GPIO 27 |
| GPS RX | GPIO 16 |
| GPS TX | GPIO 17 |

**Note:** GPS wiring is crossed between the modules: GPS TX connects to ESP32 RX (GPIO 16), and GPS RX connects to ESP32 TX (GPIO 17).

## 3. How It Works

1. **System Initialization:** The ESP32 initializes the water sensor, GPS module, Wi-Fi access point, and web server.
2. **Water Detection:** The water sensor measures water contact and sends readings to the ESP32.
3. **Sensor Filtering:** Multiple sensor readings are averaged to reduce noise and false signals.
4. **Emergency Confirmation:** When water contact is confirmed, a 20-second timer starts.
5. **Alert Activation:** If water detection continues for 20 seconds, the buzzer and red LED activate.
6. **Alert Reset:** When the sensor confirms that water is no longer detected, the alarm is deactivated.
7. **GPS Tracking:** The GPS module provides latitude, longitude, and altitude when a valid GPS fix is available.
8. **Web Dashboard:** The ESP32 hosts a dashboard displaying water readings, approximate depth, GPS coordinates, alert status, and guard connection information.
9. **Guard Location:** A safety guard can share their phone's GPS location through the dashboard. The system calculates the distance when both locations are valid and current.

### Dashboard Access

| Setting | Value |
|---|---|
| Wi-Fi Name (SSID) | `water_safty_alarm` |
| Password | `12345678` |
| Dashboard URL | `http://192.168.4.1` |

## 4. Advantages

1. Provides an immediate local warning through a buzzer and red LED after the confirmation period.
2. Uses sensor averaging to reduce noise.
3. Includes a 20-second confirmation period to help prevent false alarms.
4. Displays GPS coordinates when satellite reception is available.
5. Provides a Google Maps location link.
6. Offers a web dashboard accessible from a connected phone or computer.
7. Uses affordable and commonly available electronic components.
8. Can be expanded with additional communication and monitoring features.

## 5. Disadvantages

1. Water-sensor readings can vary with water conductivity, sensor placement, and calibration.
2. The displayed water depth is an estimate based on sensor readings, not a direct measurement of actual depth.
3. GPS accuracy may decrease indoors, underwater, or near obstructions.
4. The local Wi-Fi connection has a limited range.
5. The system does not automatically contact emergency services unless a separate communication service is implemented.
6. Guard distance depends on receiving valid GPS coordinates from both the device and the guard's phone.
7. Battery life and waterproofing require proper design and testing.
8. The prototype is not a replacement for certified life-saving equipment or trained rescue personnel.

## 6. Applications

1. Water safety monitoring demonstrations.
2. Swimming pool safety prototypes.
3. Flood and water-rescue research projects.
4. Monitoring people near rivers, lakes, and other water bodies.
5. Academic projects involving IoT, GPS, and embedded systems.

## 7. Result

The Aqua Safety prototype demonstrates water-contact detection, sensor filtering, a 20-second emergency confirmation timer, a local buzzer and red LED alert, GPS data display, and a Wi-Fi-based web dashboard.

The actual performance of the system depends on sensor calibration, GPS reception, stable power, and correct hardware connections. Each function should be tested individually and documented before claiming successful operation in real rescue situations.

## 8. Future Enhancements

1. Integrate a long-range communication system for remote emergency notifications.
2. Add a suitable depth sensor for actual water-depth measurement.
3. Implement battery-level monitoring and low-battery alerts.
4. Add secure safety-guard authentication.
5. Improve the waterproof enclosure and wearable design.
6. Conduct extensive testing under different water and environmental conditions.

## 9. Project Team

- **Gajendra Rajpoot**
- **Nishant Hardaha**

## 10. Conclusion

Aqua Safety is an IoT-based water safety prototype designed to demonstrate how water sensing, GPS tracking, local alarms, and a web dashboard can support water-rescue monitoring. It provides a foundation for further development into a more reliable and comprehensive safety system. Additional communication features, hardware protection, and field testing are necessary before it can be considered suitable for real-life emergency use.
