#include <TinyGPS++.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>

// =====================================================
// AQUA SAFETY
// IoT WATER RESCUE & LIFE-SAVIOUR SYSTEM
// =====================================================


// =====================================================
// PIN DEFINITIONS
// =====================================================

#define WATER_SENSOR 34

#define LED_PIN 26
#define BUZZER_PIN 27

#define BLUE_LED 2

// NEO-6M
// GPS TX -> ESP32 GPIO16
// GPS RX -> ESP32 GPIO17

#define GPS_RX 16
#define GPS_TX 17


// =====================================================
// WATER SETTINGS
// =====================================================

int WATER_THRESHOLD = 1200;

// Water must remain detected for 30 seconds
unsigned long ALERT_DELAY = 30000;


// =====================================================
// GPS
// =====================================================

TinyGPSPlus gps;

HardwareSerial gpsSerial(1);

float latitude = 0.0;
float longitude = 0.0;
float altitude = 0.0;

bool gpsValid = false;


// =====================================================
// WATER
// =====================================================

int waterValue = 0;

unsigned long waterStartTime = 0;

bool waterDetected = false;
bool emergencyAlert = false;


// =====================================================
// WIFI ACCESS POINT
// =====================================================

const char* ssid = "water_safty_alarm";
const char* password = "12345678";

WebServer server(80);


// =====================================================
// SAFETY GUARD
// =====================================================

int guardCount = 0;

int guardRSSI = -127;

float guardDistance = -1.0;

bool guardConnected = false;


// =====================================================
// WIFI DISTANCE CALIBRATION
// =====================================================

// Approximate RSSI at 1 metre
const float RSSI_AT_1M = -45.0;

// Environment factor
const float WIFI_PATH_LOSS = 2.7;


// =====================================================
// WEBPAGE
// =====================================================

String webpage = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport"
content="width=device-width, initial-scale=1">

<title>Aqua Safety</title>

<style>

*{
  box-sizing:border-box;
}

body{

  margin:0;

  font-family:Arial,Helvetica,sans-serif;

  background:#eaf5ff;

  text-align:center;

  color:#111;

}

.header{

  background:#064579;

  color:white;

  padding:22px 10px;

  font-size:32px;

  font-weight:bold;

}

.container{

  max-width:600px;

  margin:auto;

  padding:10px;

}

.card{

  background:white;

  margin:12px 0;

  padding:18px;

  border-radius:14px;

  box-shadow:0 3px 12px rgba(0,0,0,0.18);

}

.card h2{

  margin-top:0;

  font-size:25px;

}

.value{

  font-size:30px;

  font-weight:bold;

  margin:10px;

}

.gps{

  font-size:20px;

  line-height:1.8;

}

button{

  background:#1677d2;

  color:white;

  border:none;

  padding:12px 20px;

  border-radius:8px;

  font-size:17px;

  cursor:pointer;

}

button:active{

  transform:scale(0.97);

}

.safe{

  color:green;

  font-size:25px;

  font-weight:bold;

}

.danger{

  color:red;

  font-size:27px;

  font-weight:bold;

}

.waiting{

  color:#555;

  font-size:20px;

}

.guardConnected{

  color:green;

  font-size:23px;

  font-weight:bold;

}

.guardDisconnected{

  color:red;

  font-size:21px;

  font-weight:bold;

}

.distance{

  font-size:30px;

  font-weight:bold;

  margin:8px;

}

.vibrationEnabled{

  color:green;

  font-size:16px;

  font-weight:bold;

  margin-top:10px;

}

a{

  text-decoration:none;

}

</style>

</head>


<body>


<!-- =================================================
     HEADER
================================================= -->

<div class="header">

Aqua Safety

<div style="font-size:18px;margin-top:5px;">

IoT Water Rescue & Life-Saviour System

</div>

</div>


<div class="container">


<!-- =================================================
     WATER LEVEL
================================================= -->

<div class="card">

<h2>Water Level</h2>

<div class="value" id="water">

Loading...

</div>

<div style="font-size:14px;color:#555;">

Sensor value

</div>

</div>


<!-- =================================================
     GPS LOCATION
================================================= -->

<div class="card">

<h2>GPS Location</h2>

<div class="gps" id="gps">

Searching for GPS...

</div>

<br>

<a id="mapLink"
href="#"
target="_blank">

<button>

Open Location in Google Maps

</button>

</a>

</div>


<!-- =================================================
     SAFETY GUARD
================================================= -->

<div class="card">

<h2>Safety Guard</h2>

<div id="guard">

Checking...

</div>

<br>

<button onclick="enableVibration()">

Enable Alert Vibration

</button>

<div id="vibrationStatus"></div>

</div>


<!-- =================================================
     STATUS
================================================= -->

<div class="card">

<h2>Status</h2>

<div id="status">

Checking...

</div>

</div>


</div>


<script>


// =====================================================
// GUARD PHONE VIBRATION
// =====================================================

let vibrationEnabled = false;

let dangerVibrationStarted = false;


// =====================================================
// ENABLE VIBRATION
// =====================================================

function enableVibration(){

  if("vibrate" in navigator){

    // Small test vibration
    navigator.vibrate(300);

    vibrationEnabled = true;

    document.getElementById("vibrationStatus").innerHTML =

      "<div class='vibrationEnabled'>"
      +
      "Alert vibration enabled"
      +
      "</div>";

  }

  else{

    document.getElementById("vibrationStatus").innerHTML =

      "<div style='color:red;margin-top:10px;'>"
      +
      "Vibration not supported"
      +
      "</div>";

  }

}


// =====================================================
// START DANGER VIBRATION
// =====================================================

function startDangerVibration(){

  if(!vibrationEnabled){
    return;
  }

  if(dangerVibrationStarted){
    return;
  }

  dangerVibrationStarted = true;


  // Repeating emergency vibration

  navigator.vibrate([
    1000,
    500,
    1000,
    500,
    1000,
    500,
    1000
  ]);

}


// =====================================================
// STOP DANGER VIBRATION
// =====================================================

function stopDangerVibration(){

  dangerVibrationStarted = false;

  if("vibrate" in navigator){

    navigator.vibrate(0);

  }

}


// =====================================================
// UPDATE DATA
// =====================================================

function updateData(){

  fetch("/data")

  .then(response => response.json())

  .then(data => {


    // =================================================
    // WATER
    // =================================================

    document.getElementById("water").innerHTML =
      data.water;


    // =================================================
    // GPS
    // =================================================

    if(data.gpsValid == 1){

      document.getElementById("gps").innerHTML =

        "Latitude: " + data.lat +
        "<br>" +

        "Longitude: " + data.lng +
        "<br>" +

        "Altitude: " + data.alt + " m";


      document.getElementById("mapLink").href =

        "https://www.google.com/maps/search/?api=1&query="
        +
        data.lat
        +
        ","
        +
        data.lng;

    }

    else{

      document.getElementById("gps").innerHTML =

        "<span class='waiting'>"
        +
        "Searching for GPS..."
        +
        "</span>";

      document.getElementById("mapLink").href = "#";

    }


    // =================================================
    // SAFETY GUARD
    // =================================================

    if(data.guardConnected == 1){

      document.getElementById("guard").innerHTML =

        "<span class='guardConnected'>"
        +
        "GUARD CONNECTED"
        +
        "</span>"

        +

        "<br><br>"

        +

        "Devices: "
        +
        data.guardCount

        +

        "<br>"

        +

        "Signal: "
        +
        data.guardRSSI
        +
        " dBm"

        +

        "<br><br>"

        +

        "<div class='distance'>"
        +
        data.guardDistance
        +
        " m"
        +
        "</div>";

    }

    else{

      document.getElementById("guard").innerHTML =

        "<span class='guardDisconnected'>"
        +
        "NO GUARD CONNECTED"
        +
        "</span>";

    }


    // =================================================
    // STATUS + VIBRATION
    // =================================================

    if(data.alert == 1){

      document.getElementById("status").innerHTML =

        "<span class='danger'>"
        +
        "WATER DANGER"
        +
        "</span>";


      // Vibrate connected guard phone
      startDangerVibration();

    }


    else if(data.waterDetected == 1){

      stopDangerVibration();


      document.getElementById("status").innerHTML =

        "<span style='color:orange;font-size:22px;font-weight:bold;'>"
        +
        "Water Detected - Monitoring"
        +
        "</span>";

    }


    else{

      stopDangerVibration();


      document.getElementById("status").innerHTML =

        "<span class='safe'>"
        +
        "SAFE"
        +
        "</span>";

    }

  })

  .catch(error => {

    console.log(error);

  });

}


// =====================================================
// UPDATE EVERY SECOND
// =====================================================

setInterval(updateData,1000);


// =====================================================
// FIRST UPDATE
// =====================================================

updateData();


</script>


</body>

</html>

)rawliteral";


// =====================================================
// UPDATE SAFETY GUARD
// =====================================================

void updateGuardStatus()
{

  wifi_sta_list_t stationList;


  // Reset

  guardConnected = false;

  guardCount = 0;

  guardRSSI = -127;

  guardDistance = -1.0;


  // Get connected Wi-Fi devices

  esp_err_t result =
    esp_wifi_ap_get_sta_list(&stationList);


  if(result != ESP_OK)
  {
    return;
  }


  guardCount = stationList.num;


  // No device connected

  if(guardCount == 0)
  {
    return;
  }


  // =================================================
  // FIND STRONGEST SIGNAL
  // =================================================

  int strongestRSSI = -127;


  for(int i = 0; i < stationList.num; i++)
  {

    int currentRSSI =
      stationList.sta[i].rssi;


    if(currentRSSI > strongestRSSI)
    {

      strongestRSSI =
        currentRSSI;

    }

  }


  guardRSSI = strongestRSSI;

  guardConnected = true;


  // =================================================
  // RSSI DISTANCE ESTIMATION
  // =================================================

  guardDistance =

    pow(
      10.0,

      (RSSI_AT_1M - guardRSSI)
      /
      (10.0 * WIFI_PATH_LOSS)
    );


  if(guardDistance < 0.1)
  {
    guardDistance = 0.1;
  }


  if(guardDistance > 9999)
  {
    guardDistance = 9999;
  }

}


// =====================================================
// HOME PAGE
// =====================================================

void handleRoot()
{

  server.send(
    200,
    "text/html",
    webpage
  );

}


// =====================================================
// REAL-TIME DATA
// =====================================================

void handleData()
{

  String json = "{";


  // Water

  json += "\"water\":";
  json += String(waterValue);


  // Latitude

  json += ",";

  json += "\"lat\":";
  json += String(latitude, 6);


  // Longitude

  json += ",";

  json += "\"lng\":";
  json += String(longitude, 6);


  // Altitude

  json += ",";

  json += "\"alt\":";
  json += String(altitude, 2);


  // GPS valid

  json += ",";

  json += "\"gpsValid\":";
  json += gpsValid ? "1" : "0";


  // Water detected

  json += ",";

  json += "\"waterDetected\":";
  json += waterDetected ? "1" : "0";


  // Emergency

  json += ",";

  json += "\"alert\":";
  json += emergencyAlert ? "1" : "0";


  // Guard connected

  json += ",";

  json += "\"guardConnected\":";
  json += guardConnected ? "1" : "0";


  // Connected devices

  json += ",";

  json += "\"guardCount\":";
  json += String(guardCount);


  // RSSI

  json += ",";

  json += "\"guardRSSI\":";
  json += String(guardRSSI);


  // Distance

  json += ",";

  json += "\"guardDistance\":";

  if(guardDistance >= 0)
  {

    json +=
      String(
        guardDistance,
        1
      );

  }

  else
  {

    json += "-1";

  }


  json += "}";


  server.send(
    200,
    "application/json",
    json
  );

}


// =====================================================
// SETUP
// =====================================================

void setup()
{

  Serial.begin(115200);


  // =================================================
  // OUTPUTS
  // =================================================

  pinMode(
    LED_PIN,
    OUTPUT
  );

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  pinMode(
    BLUE_LED,
    OUTPUT
  );


  // OFF initially

  digitalWrite(
    LED_PIN,
    LOW
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  digitalWrite(
    BLUE_LED,
    LOW
  );


  // =================================================
  // GPS
  // =================================================

  gpsSerial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX,
    GPS_TX
  );


  // =================================================
  // WIFI ACCESS POINT
  // =================================================

  WiFi.mode(WIFI_AP);


  WiFi.softAP(
    ssid,
    password
  );


  // =================================================
  // SERIAL STARTUP
  // =================================================

  Serial.println();

  Serial.println(
    "================================"
  );

  Serial.println(
    "        AQUA SAFETY"
  );

  Serial.println(
    "================================"
  );

  Serial.println();


  Serial.print(
    "WiFi: "
  );

  Serial.println(
    ssid
  );


  Serial.print(
    "Password: "
  );

  Serial.println(
    password
  );


  Serial.print(
    "Open: http://"
  );

  Serial.println(
    WiFi.softAPIP()
  );


  Serial.println();


  Serial.println(
    "GPS starting..."
  );

  Serial.println(
    "Water monitoring starting..."
  );

  Serial.println(
    "Guard monitoring starting..."
  );


  // =================================================
  // WEB SERVER
  // =================================================

  server.on(
    "/",
    handleRoot
  );


  server.on(
    "/data",
    handleData
  );


  server.begin();


  Serial.println();

  Serial.println(
    "Web server started."
  );

}


// =====================================================
// LOOP
// =====================================================

void loop()
{

  // =================================================
  // 1. READ GPS CONTINUOUSLY
  // =================================================

  while(
    gpsSerial.available() > 0
  )
  {

    char c =
      gpsSerial.read();


    gps.encode(c);

  }


  // =================================================
  // 2. UPDATE GPS
  // =================================================

  if(
    gps.location.isValid()
  )
  {

    latitude =
      gps.location.lat();


    longitude =
      gps.location.lng();


    altitude =
      gps.altitude.meters();


    gpsValid = true;

  }


  // =================================================
  // 3. READ WATER SENSOR
  // =================================================

  waterValue =
    analogRead(
      WATER_SENSOR
    );


  // =================================================
  // 4. WATER DETECTION
  // =================================================

  if(
    waterValue >= WATER_THRESHOLD
  )
  {

    // First detection

    if(!waterDetected)
    {

      waterDetected = true;


      waterStartTime =
        millis();


      Serial.println();

      Serial.println(
        "WATER DETECTED!"
      );


      Serial.println(
        "Starting 30 second confirmation..."
      );

    }


    // =================================================
    // 5. EMERGENCY AFTER 30 SECONDS
    // =================================================

    if(
      millis() - waterStartTime
      >= ALERT_DELAY
    )
    {

      if(!emergencyAlert)
      {

        emergencyAlert = true;


        Serial.println();

        Serial.println(
          "================================"
        );

        Serial.println(
          "     !!! WATER EMERGENCY !!!"
        );

        Serial.println(
          "================================"
        );


        // GPS information

        if(gpsValid)
        {

          Serial.print(
            "Latitude: "
          );

          Serial.println(
            latitude,
            6
          );


          Serial.print(
            "Longitude: "
          );

          Serial.println(
            longitude,
            6
          );


          Serial.print(
            "Altitude: "
          );

          Serial.print(
            altitude,
            2
          );

          Serial.println(
            " m"
          );

        }


        // Guard information

        if(guardConnected)
        {

          Serial.print(
            "Guard RSSI: "
          );

          Serial.print(
            guardRSSI
          );

          Serial.print(
            " dBm | Distance: "
          );

          Serial.print(
            guardDistance,
            1
          );

          Serial.println(
            " m"
          );

        }

        else
        {

          Serial.println(
            "No Guard Connected"
          );

        }

      }

    }

  }


  // =================================================
  // 6. WATER REMOVED
  // =================================================

  else
  {

    if(waterDetected)
    {

      Serial.println();

      Serial.println(
        "Water removed."
      );

      Serial.println(
        "System reset."
      );

    }


    waterDetected = false;

    emergencyAlert = false;

    waterStartTime = 0;

  }


  // =================================================
  // 7. ALERT OUTPUTS
  // =================================================

  if(emergencyAlert)
  {

    digitalWrite(
      LED_PIN,
      HIGH
    );


    digitalWrite(
      BUZZER_PIN,
      HIGH
    );


    digitalWrite(
      BLUE_LED,
      HIGH
    );

  }

  else
  {

    digitalWrite(
      LED_PIN,
      LOW
    );


    digitalWrite(
      BUZZER_PIN,
      LOW
    );


    digitalWrite(
      BLUE_LED,
      LOW
    );

  }


  // =================================================
  // 8. UPDATE GUARD EVERY SECOND
  // =================================================

  static unsigned long lastGuardUpdate = 0;


  if(
    millis() - lastGuardUpdate >= 1000
  )
  {

    lastGuardUpdate =
      millis();


    updateGuardStatus();

  }


  // =================================================
  // 9. SERIAL MONITOR
  // =================================================

  static unsigned long lastPrint = 0;


  if(
    millis() - lastPrint >= 2000
  )
  {

    lastPrint =
      millis();


    Serial.print(
      "Water: "
    );

    Serial.print(
      waterValue
    );


    Serial.print(
      " | GPS: "
    );


    if(gpsValid)
    {

      Serial.print(
        latitude,
        6
      );


      Serial.print(
        ", "
      );


      Serial.print(
        longitude,
        6
      );


      Serial.print(
        " | Alt: "
      );


      Serial.print(
        altitude,
        2
      );


      Serial.print(
        "m"
      );

    }

    else
    {

      Serial.print(
        "Waiting for GPS..."
      );

    }


    // Guard

    Serial.print(
      " | Guard: "
    );


    if(guardConnected)
    {

      Serial.print(
        "CONNECTED"
      );


      Serial.print(
        " | RSSI: "
      );


      Serial.print(
        guardRSSI
      );


      Serial.print(
        " dBm"
      );


      Serial.print(
        " | Distance: "
      );


      Serial.print(
        guardDistance,
        1
      );


      Serial.print(
        "m"
      );

    }

    else
    {

      Serial.print(
        "NOT CONNECTED"
      );

    }


    // Status

    Serial.print(
      " | Status: "
    );


    if(emergencyAlert)
    {

      Serial.println(
        "WATER DANGER"
      );

    }

    else if(waterDetected)
    {

      Serial.println(
        "WATER DETECTED"
      );

    }

    else
    {

      Serial.println(
        "SAFE"
      );

    }

  }


  // =================================================
  // 10. WEB SERVER
  // =================================================

  server.handleClient();

}