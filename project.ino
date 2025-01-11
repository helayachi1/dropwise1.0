// DROPWISE APP | Made with love by Elayachi Hamrit 2025.


#include <Wire.h>
#include <WiFi.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "ESPAsyncWebServer.h"

// Pin definitions
const int RELY_1_PIN = 13; // ZONE 1
const int RELY_2_PIN = 12; // ZONE 2
const int RELY_3_PIN = 14; // ZONE 3 
const int RELY_4_PIN = 27; // WATER FILLING PUMP

const int WATER_LEVEL_SENSORE_PIN = 34;
const int BUZZER_PIN = 5;
const int DHT11_SENSORE_PIN = 4;
const int BUILT_IN_LED = 2;

// Global variables
int WATER_LEVEL;
int HUMDITY_VALUE;
int TEMPERATURE_VALUE;

// Systeme Health 
int relay1Status;
int relay2Status;
int relay3Status;
int relay4Status;

// WiFi Information
const char* wifi_ssid = "Galaxy A14";
const char* wifi_pass = "1234512345";
String wifi_rssi;  
String wifi_ip;
String wifi_status;
bool AlarmAllowed = true;
bool AutoWaterFillingAllowed = true;

// Create web server object on port 80;
AsyncWebServer server(80);

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD
DHT dht(DHT11_SENSORE_PIN, DHT11);  // DHT sensor setup

// APP 
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.7.2/css/all.min.css" integrity="sha512-Evv84Mr4kqVGRNSgIGL/F/aIDqQb7xQ2vcrdIwxfjThSH8CSR7PBEakCr51Ck+w+/U6swU2Im1vVX0SVk9ABhg==" crossorigin="anonymous" referrerpolicy="no-referrer" />

    <style>
        @import url('https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;500;600;700&display=swap');

        /* Global Styles */
        * {
            padding: 0;
            margin: 0;
            box-sizing: border-box;
            font-family: "Poppins", sans-serif;
            scroll-behavior: smooth;
        }

        body {
            background-color: #f9f9f9;
        }

        h2 {
            font-weight: 500;
            padding-top: 1rem;
            font-size: 1.2rem;
            color: #696868;
        }

        a {
            text-decoration: none;
        }


        .wrapper {
            display: grid;
            max-width: 1200px;
            margin: 0 auto;
            grid-template-columns: clamp(1rem, 4vw, 3rem) 1fr clamp(1rem, 4vw, 3rem) ;
            grid-template-areas: ". navbar ."
                                 ". env ."
                                 ". connections ."
                                 ". health ."
                                " . sensors ."
                                ". controllers .";

            gap: 1rem;
            padding-bottom: 2rem;
        }
        

        .brand {
            padding: 2rem;
            color: #383636;
        }

        .navbar {
            display: flex;
            justify-content: space-between;
            align-items: center;
            grid-area: navbar;
            padding: 1rem 0rem;
            position: sticky;
            width: 100%;
            top: 0;
            background-color: #f9f9f9;

        }        

        .menu {
            display: flex;
            gap: 3rem;
        }

        .link {
            color: #383636;
            font-weight: 500;
        }

        .link:hover {
            color: #4da9f6;
        }
    
.device-status {
    display: flex;
    padding: 1rem;
    overflow: hidden;
    flex-wrap: wrap;
    justify-content: space-between; /* Distribute child elements evenly */
    width: 100%; /* Make sure it uses the full available width */
    background-color: #fbfbfb;
    border-radius: 8px;
}

.device-status p {
    flex: 1; /* This ensures that the content inside each status element fills available space */
    min-width: 120px; /* Ensure each status has a minimum width */
}

.fa-solid {
    padding-right: 0.7rem;
}

.system-health-wrapper {
    background-color: #fff;
    padding: 2rem;
    border-radius: 12px;
    display: flex;
    flex-direction: column;
    gap: 1rem;
    background-color: #fff;
    grid-area: health;

}


.system-health-actions {
    display: flex;
    justify-content: end;
}


.all-statuses {
    display: grid;
    gap: 1rem;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); /* Adjust grid columns */
}
    
    .fa-sun {
        color: #ff7a7a;
    }

    .fa-cloud {
        color: #8e8ed7;
    }
        

        .connection_info {
            display: flex;
            flex-wrap: wrap;
            gap: 1rem;
            justify-content: space-between;
            align-items: center;
            padding: 2rem 2rem;
            font-size: 0.9rem;
            background-color: #ffffff;
            border-radius: 12px;
            grid-area: connections;


        }

        .connection_info .info {
            display: flex;
            flex-wrap: wrap;
            flex-grow: 1;
            align-items: center;
            gap: 1rem;
            background-color: #fbfbfb;
            padding: 1rem;
            border-radius: 12px;
        }

        svg {
            height: 20px;
        }



        .controlers {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 1rem;
            grid-area: controllers;
        }

        .controler_card {
            padding: 1rem;
            background-color: #ffffff;
            border-radius: 12px;
        }

        .info {
            display: flex;
            gap: 1rem;
            align-items: center;
            background-color: #fbfbfb;
            padding: 1rem;
            border-radius: 12px;
        }


        .info_title {
            font-weight: bold;
            color: #ccc;
        }


        .env_infos {
            display: grid;
            grid-template-columns: repeat(auto-fit,  minmax(200px, 1fr));
            gap: 1.2rem;
            background-color: #fff;
            border-radius: 12px;
            grid-area: env;
            padding: 1rem;
        }     

        

        .controler_card {
            padding: 1rem 2rem;
            background-color: #ffffff;
            border-radius: 12px;
        }

        .controler_info {
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .controler_zone_product {
            color: #696868;
        }

        .controler_zone_name {
            font-size: 1.3rem;
            font-weight: 500;
        }

    
        .device-name {
            font-weight: 600;
        }

        .status {
                transition: color 0.5s ease, background-color 0.5s ease; /* Smooth transition */
        }

        .controler_switches {
            margin-top: 1rem;
        }

        .switch-btn {
            padding: 0.8rem 2rem;
            width: 100px;
            border-radius: 12px;
            border: none;
            cursor: pointer;
            width: 100%;
            font-size: 0.9rem;
            color: #fff;
            background-color: #9376ed;
        }

       .sensors_container {
            display: grid;
            gap: 1rem;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            grid-area: sensors;
       }

       .sensore_card {
        background-color: #fff;
        padding: 2rem;
        overflow: hidden;
        border-radius: 12px;
       }

       .sensore_card_value, .sensore_card_ {
        font-size: 5rem;
        font-weight: 600;
       }

       .sensore_card_value_container {
        display: flex;
        align-items: center;
        gap: 0.4rem;
       }


       .sensore_card_unite {
        font-size: 5rem;
        font-weight: 600;
        color: #ccc;
       }

      
    </style>
</head>

<body>

    <div class="wrapper">
        <div class="navbar">
            <h1 id="brand">Dropwise</h1>

            <div class="menu">
                <a class="link " href="facebook.com/dropwise"></i>Follow us</a>  
                <a class="link aboutlink" href="mailto:iamelayachi.h@gmail.com"></i>Contact</a> 
 
            </div>

           
        </div>

 <div class="env_infos">

                

                <div class="info">
                <i class="fa-solid fa-sun"></i>
                    <p class="info_title">Temperature</p>
                    <p id="temperature">[TEMP]</p>
                </div>

                <div class="info">
                    <i class="fa-solid fa-cloud"></i>
                    <p class="info_title">Humidity</p>
                    <p id="humidity">[HUMID]</p>
                </div>

            </div>
       
         <div class="connection_info">

            <div class="info connection_location">
                <i class="fa-solid fa-location-dot"></i>
                <p id="location">Algeria, Setif</p>
            </div>

            <div class="info connection_ssid">
                <i class="fa-solid fa-wifi"></i>
                <p id="wifi_ssid">{{W_SSID}}</p>
            </div>

            <div class="info connection_rssi">
                <i class="fa-solid fa-signal"></i>
                <p id="wifi_rssi">{{W_RSSI}}</p><span> db/ms</span>

            </div>


            <div class="info connection_ip">
                <i class="fa-solid fa-network-wired"></i>
                <p id="ip_adress">{{W_IP}}</p>
            </div>
        </div>




    <div class="system-health-wrapper">        

        <div class="all-statuses">
                <div class="device-status">
                    <p class="device-name">Pump 1 <span style="color: #b5b4b4"> - Zone 1</span> </p>
                    <p id="rely1Status" class="status">{{R1_STATUS}}</p>
                </div>

                <div class="device-status">
                    <p class="device-name">Pump 2 <span style="color: #b5b4b4"> - Zone 2</span></p>
                    <p id="rely2Status" class="status">{{R2_STATUS}}</p>
                </div>

                <div class="device-status">
                    <p class="device-name">Pump 3 <span style="color: #b5b4b4"> - Zone 3</span></p>
                    <p id="rely3Status" class="status">{{R3_STATUS}}</p>
                </div>
                
                <div class="device-status">
                    <p class="device-name">Water Filling </p>
                    <p id="rely4Status" class="status">{{R4_STATUS}}</p> <span style="color: blue" id="awf-command-type" class="commandType">{{AW_FILLING_STATUS}}</span>
                </div>
            </div>
               
    </div>  
                

        <div class="sensors_container">

        <div class="sensore_card water_level">
                <p class="sensore_card_title">Water Level<span style="color: #b5b4b4"> - Tank 1</span></p>
                <div class="sensore_card_value_container">
                    <p id="waterlevel" class="sensore_card_value">[WATERLEVEL]</p>
                    <p class="sensore_card_unite">%</p>
                </div>
        </div>


        </div>


<div class="controlers">

            <div class="controler_card">
                <div class="controler_info">
                    <p class="controler_zone_name">Zone 1</p>
                    <p class="controler_zone_product">Potato</p>
                </div>
                <div class="controler_switches">
                    <button id="zone1RelayButton" class="switch-btn " data-state="off">Press to start</button>
                </div>
            </div>


            <div class="controler_card">
                <div class="controler_info">
                    <p class="controler_zone_name">Zone 2</p>
                    <p class="controler_zone_product">Onion</p>
                </div>
                <div class="controler_switches">
                    <button id="zone2RelayButton" class="switch-btn " data-state="off">Press to start</button>
                </div>
            </div>

             <div class="controler_card">
                <div class="controler_info">
                    <p class="controler_zone_name">Zone 3</p>

                    <p class="controler_zone_product">Gerden</p>

                </div>
                <div class="controler_switches">
                    <button id="zone3RelayButton" class="switch-btn " data-state="off">Press to start</button>
                </div>
            </div>


            <div class="controler_card">
                <div class="controler_info">
                    <p class="controler_zone_name">Alarm</p>

                </div>
                <div class="controler_switches">
                    <button id="alarmButton" class="switch-btn " data-state="off">Press to activate</button>
                </div>
            </div>

            <div class="controler_card">
                <div class="controler_info">
                    <p class="controler_zone_name">Auto Water Filling</p>
                </div>
                <div class="controler_switches">
                    <button id="waterFillingButton" class="switch-btn " data-state="off">Press to activate</button>
                </div>
            </div>
       
</div>
    </div>

<script type="text/javascript">
 // Ensure DOM is fully loaded before adding event listeners
window.onload = function () {
           
            // Select all the buttons with the class "switch-btn"
            let zone1RelayController = document.getElementById("zone1RelayButton");
            let zone2RelayController = document.getElementById("zone2RelayButton");
            let zone3RelayController = document.getElementById("zone3RelayButton");
            let alarmController = document.getElementById("alarmButton");
            let waterFillingController = document.getElementById("waterFillingButton");
            let nowDate = document.getElementById('.date');
            // Relay Status 

            let zone1RelayStatus = document.getElementById('rely1Status');
            let zone2RelayStatus = document.getElementById('rely2Status');
            let zone3RelayStatus = document.getElementById('rely3Status');
            let autoWaterFillingStatus = document.getElementById('rely4Status');

      



            zone1RelayController.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();
                
                let zone1RelayStatusContent = zone1RelayStatus.innerText.trim();

                if (zone1RelayStatusContent == "Inactive") {
                    xhttp.open("GET", "zone/1/on", true);
                    zone1RelayController.setAttribute('data-state', 'on');
                    zone1RelayStatus.style.color = "green";
                    zone1RelayController.style.backgroundColor = "red";
                    zone1RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/1/off", true);
                   zone1RelayController.setAttribute('data-state', 'off');
                   zone1RelayStatus.style.color = "red";

                   zone1RelayController.style.backgroundColor = "green";
                   zone1RelayController.innerText = "Start";
                }

                xhttp.send();
            });


          

          

            zone2RelayController.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();
                
                let zone2RelayStatusContent = zone2RelayStatus.innerText.trim();

                if (zone2RelayStatusContent == "Inactive") {
                    xhttp.open("GET", "zone/2/on", true);
                    zone2RelayController.setAttribute('data-state', 'on');
                    zone2RelayStatus.style.color = "green";
                    zone2RelayController.style.backgroundColor = "red";
                    zone2RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/2/off", true);
                   zone2RelayController.setAttribute('data-state', 'off');
                   zone2RelayStatus.style.color = "red";

                zone2RelayController.style.backgroundColor = "green";
                zone2RelayController.innerText = "Start";
                }

                xhttp.send();
            });




            zone3RelayController.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();
                
                let zone3RelayStatusContent = zone3RelayStatus.innerText.trim();

                if (zone3RelayStatusContent == "Inactive") {
                    xhttp.open("GET", "zone/3/on", true);
                    zone3RelayController.setAttribute('data-state', 'on');
                    zone3RelayStatus.style.color = "green";
                    zone3RelayController.style.backgroundColor = "red";
                    zone3RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/3/off", true);
                   zone3RelayController.setAttribute('data-state', 'off');
                   zone3RelayStatus.style.color = "red";

                   zone3RelayController.style.backgroundColor = "green";
                   zone3RelayController.innerText = "Start";
                }

                xhttp.send();
            });



          
            


            alarmController.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();

                let alarmControllerAtrr = alarmController.getAttribute('data-state');
                
                if (alarmControllerAtrr == "off") {
                    xhttp.open("GET", "alarm/activate", true);
                    alarmController.setAttribute('data-state', 'on');
                    alarmController.style.backgroundColor = "red";
                    alarmController.innerText = "Disactivate";

                } else {
                   xhttp.open("GET", "alarm/disactivate", true);
                    alarmController.setAttribute('data-state', 'off');
                    alarmController.style.backgroundColor = "green";
                    alarmController.innerText = "Activate";

                }

                xhttp.send();
            });
        
};
         

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 3000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 5000 ) ;


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("waterlevel").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/waterlevel", true);
  xhttp.send();
}, 1000 ) ;

 // Fetch SSID every 15 seconds
setInterval(function () {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("wifi_ssid").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/ssid", true);
            xhttp.send();
}, 3000);  

// Fetch RSSI every 15 seconds
setInterval(function () {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("wifi_rssi").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/rssi", true);
            xhttp.send();
}, 3000);  


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rely1Status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/relay/1/status", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("awf-command-type").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/waterfilling/status", true);
  xhttp.send();
}, 1000 ) ;


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rely2Status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/relay/2/status", true);
  xhttp.send();
}, 1000 ) ;


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rely3Status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/relay/3/status", true);
  xhttp.send();
}, 1000 ) ;


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rely4Status").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/relay/4/status", true);
  xhttp.send();
}, 1000 );




// Fetch IP Address every 15 seconds
setInterval(function () {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ip_adress").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/ip", true);
            xhttp.send();
}, 5000);



 </script>

</body>


</html>

)rawliteral";

void setup_gpio_pins() {
  // Set GPIO pins for relays as output
  pinMode(RELY_1_PIN, OUTPUT);
  pinMode(RELY_2_PIN, OUTPUT);
  pinMode(RELY_3_PIN, OUTPUT);
  pinMode(RELY_4_PIN, OUTPUT);

  digitalWrite(RELY_1_PIN, HIGH);
  digitalWrite(RELY_2_PIN, HIGH);
  digitalWrite(RELY_3_PIN, HIGH);
  digitalWrite(RELY_4_PIN, HIGH);

  // Set GPIO pin for water level sensor as input
  pinMode(WATER_LEVEL_SENSORE_PIN, INPUT);

  // Set GPIO pin for buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);

  // Set GPIO pin for DHT11 sensor as input
  pinMode(DHT11_SENSORE_PIN, INPUT);

  // Set GPIO pin for built-in LED as output
  pinMode(BUILT_IN_LED, OUTPUT);
}

void connectToWiFi() {
    WiFi.begin(wifi_ssid, wifi_pass);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("Connecting ...");
        wifi_status = "OFFLINE";
    }
    
    Serial.println("\nConnected to WiFi");
    wifi_status = "ONLINE";
    wifi_rssi = String(WiFi.RSSI());  // Get the RSSI value
    wifi_ip = WiFi.localIP().toString();

    Serial.print(wifi_ip);
}

// Beep buzzer for water level alert
void beep_buzzer(int times, int every_ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(every_ms);
    digitalWrite(BUZZER_PIN, LOW);
    delay(every_ms);
  }
}

// Function to handle water level and sensor data
String hundelWaterLevel() {
  int WATER_LEVEL_SENSORE_VALUE = analogRead(WATER_LEVEL_SENSORE_PIN);
  int WATER_LEVEL_PERC = map(WATER_LEVEL_SENSORE_VALUE, 0, 4095, 0, 100); // Updated to 4095 for ESP32 resolution
  WATER_LEVEL = constrain(WATER_LEVEL_PERC, 0, 100);
  return String(WATER_LEVEL);
}

String hundelRely1Status() {
    
    int current_status = digitalRead(RELY_1_PIN);
    
    if (current_status == LOW) {
      return "Active";
    } else {
      return "Inactive";
    }

};

String hundelRely2Status() {
    
    int current_status = digitalRead(RELY_2_PIN);
    
    if (current_status == LOW) {
      return "Active";
    } else {
      return "Inactive";
    }
  
  };


String hundelRely3Status() {
    
    int current_status = digitalRead(RELY_3_PIN);
    
    if (current_status == LOW) {
      return "Active";
    } else {
      return"Inactive";
    }
};


String hundelAutoWaterFillingStatus() {
  if (AutoWaterFillingAllowed) {
    return "Auto";
  } else {
    return "Manual";
  }
};
 
String hundelRely4Status() {
    
    int current_status = digitalRead(RELY_4_PIN);
    
    if (current_status == LOW) {
      return "Active";
    } else {
      return "Inactive";
    }
};

void hundelWaterFilling() {
  if (AutoWaterFillingAllowed && WATER_LEVEL < 10) {
    if (AlarmAllowed) {
      beep_buzzer(3, 500);  // Trigger buzzer if water level is low
    }
    digitalWrite(RELY_4_PIN, LOW);
  } else if (WATER_LEVEL > 30) {
    digitalWrite(RELY_4_PIN, HIGH);
  }
}

String hundelSSID() {
  return String(wifi_ssid);
};

String hundelRSSI() {
  return String(wifi_rssi);
};

void activateBuzzerAlarm() {
    AlarmAllowed = true;
};

void disactivateBuzzerAlarm() {
    AlarmAllowed = false;
};


void activateAutoWaterFilling() {
  AutoWaterFillingAllowed = true;
  digitalWrite(RELY_4_PIN, LOW);
};

void disactivateAutoWaterFilling() {
  AutoWaterFillingAllowed = false;
  digitalWrite(RELY_4_PIN, HIGH);
};

void turnOnZone1() {
  digitalWrite(RELY_1_PIN, LOW);
};

void turnOffZone1() {
  digitalWrite(RELY_1_PIN, HIGH);
};


void turnOnZone2() {
  digitalWrite(RELY_2_PIN, LOW);
};

void turnOffZone2() {
  digitalWrite(RELY_2_PIN, HIGH);
};


void turnOnZone3() {
  digitalWrite(RELY_3_PIN, LOW);
};

void turnOffZone3() {
  digitalWrite(RELY_3_PIN, HIGH);
};

String hundelIP() {
  return String(wifi_ip);
}

// Handle humidity and temperature readings
String hundelHumidity() {
  HUMDITY_VALUE = dht.readHumidity();
  if (isnan(HUMDITY_VALUE) ) {
    Serial.println("Failed to read from DHT sensor!");
  }
  return String(HUMDITY_VALUE);
}

String hundelTemperature() {
  TEMPERATURE_VALUE = dht.readTemperature();
  if (isnan(TEMPERATURE_VALUE)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  return String(TEMPERATURE_VALUE);
}

String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMP"){
    return hundelTemperature();
  } else if(var == "HUMID"){
    return hundelHumidity();
  } else if(var == "WATERLEVEL"){
    return hundelWaterLevel();
  }  else if(var == "W_IP"){
    return hundelIP();
  }  else if(var == "W_RSSI"){
    return hundelRSSI();
  }  else if(var == "W_SSID"){
    return hundelSSID();
  } else if(var == "R1_STATUS"){
    return hundelRely1Status();
  } else if(var == "R2_STATUS"){
    return hundelRely2Status();
  } else if(var == "R3_STATUS"){
    return hundelRely3Status();
  } else if(var == "R4_STATUS"){
    return hundelRely4Status();
  } else if(var == "AW_FILLING_STATUS"){
    return hundelAutoWaterFillingStatus();
  } 
    return String();
}

// Info Page Update
void lcd_info_page() {
  if (wifi_status == "ONLINE") {
 lcd.clear();  // Clear screen before displaying updated info
  lcd.setCursor(0, 0);
  lcd.print("DROPWISE ");
  lcd.print("W: ");
  lcd.print(WATER_LEVEL);
  lcd.print("% ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  } else {
    lcd.clear();
    lcd.print("SYSTEME ");
    lcd.setCursor(0, 1);
    lcd.print("IS OFFLINE :/");
  }
  
}

// Welcome Page
void lcd_welcome_screen() {
  lcd.clear();
  lcd.setCursor(0, 0);  // Move the cursor to the first row
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);  // Move the cursor to the second row
  lcd.print("Dropwise 1.0");

  delay(4000);  // Wait for 4 seconds (4000 milliseconds)

  lcd.clear();  // Clear the screen after 4 seconds
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  setup_gpio_pins();
  Wire.begin();

  // Setup lcd
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd_welcome_screen();
  lcd_info_page();

  // ROUTES 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html, processor);
  });


  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelTemperature().c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelHumidity().c_str());
  });

  server.on("/waterlevel", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelWaterLevel().c_str());
  });

  server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelIP().c_str());
  });

    server.on("/ssid", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelSSID().c_str());
  });

  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelRSSI().c_str());
  });

  server.on("/zone/1/on", HTTP_GET, [](AsyncWebServerRequest *request){
    turnOnZone1();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response

  });

  server.on("/alarm/activate", HTTP_GET, [](AsyncWebServerRequest *request){
    activateBuzzerAlarm();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response

  });

  server.on("/alarm/disactivate", HTTP_GET, [](AsyncWebServerRequest *request){
    disactivateBuzzerAlarm();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response

  });

   server.on("/zone/1/off", HTTP_GET, [](AsyncWebServerRequest *request){
    turnOffZone1();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

  server.on("/zone/2/off", HTTP_GET, [](AsyncWebServerRequest *request){
    turnOffZone2();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

  server.on("/zone/2/on", HTTP_GET, [](AsyncWebServerRequest *request){
    turnOnZone2();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

  
  server.on("/zone/3/on", HTTP_GET, [](AsyncWebServerRequest *request){
    turnOnZone3();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

  
  server.on("/zone/3/off", HTTP_GET, [](AsyncWebServerRequest *request){
    turnOffZone3();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

  
  server.on("/relay/1/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelRely1Status().c_str());
  });

  server.on("/relay/2/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelRely2Status().c_str());
  });

  server.on("/relay/3/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelRely3Status().c_str());
  });

  server.on("/relay/4/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelRely4Status().c_str());
  });


  server.on("/waterfilling/on", HTTP_GET, [](AsyncWebServerRequest *request){
    activateAutoWaterFilling(); 
    request->send(200, "text/plain", "");  // Send an empty response 
  });

  
  server.on("/waterfilling/off", HTTP_GET, [](AsyncWebServerRequest *request){
    disactivateAutoWaterFilling(); 
    request->send(200, "text/plain", "");  // Send an empty response
  });

   server.on("/waterfilling/status", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/plain", hundelAutoWaterFillingStatus().c_str());
  });
  

  // Start server 
  server.begin();
}


void loop() {
  // Handle water level sensor and sensor readings
  hundelWaterLevel();
  hundelTemperature();
  hundelHumidity();
  hundelWaterFilling();
  lcd_info_page();  // Update LCD with current info
  delay(1000);  
}


