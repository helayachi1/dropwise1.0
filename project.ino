// DROPWISE | Made with love by Elayachi Hamrit & Aziz Abd Elbassit 2025.

// LIBS 
// Includes the Wire library for I2C communication. Required for communication with I2C devices like sensors or displays.
#include <Wire.h>  
// Includes the WiFi library for connecting to Wi-Fi networks and performing network tasks on the ESP32.
#include <WiFi.h>
 // Includes the DHT library for interacting with DHT series temperature and humidity sensors (e.g., DHT11, DHT22).  
#include <DHT.h> 
// Includes the LiquidCrystal_I2C library for controlling an LCD screen over I2C protocol (used for displays like 16x2 LCD).
#include <LiquidCrystal_I2C.h>  
// Includes the ESPAsyncWebServer library for setting up a non-blocking, asynchronous web server on the ESP32.
#include "ESPAsyncWebServer.h" 
 // Includes the HTTPClient library for making HTTP requests (e.g., GET, POST) from the ESP32 to a server. 
#include <HTTPClient.h> 

// Pin definitions
// OUTPUTS 
const int RELY_1_PIN = 13; // ZONE 1 EV 1 P
const int RELY_2_PIN = 12; // ZONE 2 EV 2 P
const int RELY_3_PIN = 14; // ZONE 3 EV 3 P
const int RELY_4_PIN = 27; // WATER FILLING PUMP (P)
const int RELY_5_PIN = 26; // ZONE 1 EV 1 S 
const int RELY_6_PIN = 18; // IRRIGATION M1
const int RELY_7_PIN = 19; // IRRIGATION M2
const int RELY_8_PIN = 23; 

const int BUZZER_PIN = 5;
const int BUILT_IN_LED = 2;

int defaultIrrigtionMotor = RELY_6_PIN;

// INPUTS 
const int WATER_LEVEL_SENSORE_PIN = 34; 
const int DHT11_SENSORE_PIN = 4;
const int RAIN_SENSOR_PIN = 35;
const int WATER_FLOW_SENSORE_1 = 32;
const int WATER_FLOW_SENSORE_2 = 33;

// Global variables
int WATER_LEVEL;
int HUMDITY_VALUE;
int TEMPERATURE_VALUE;

// Boolean to track rain status
bool isRaining = false;

// WiFi Information
const char* wifi_ssid = "Galaxy A14";
const char* wifi_pass = "1234512345";
const String duckdns_domain = "dropwise1.duckdns.org";  // Your DuckDNS subdomain (e.g., "yourname.duckdns.org")
const String duckdns_token = "6727c058-79e7-41b9-a555-b2ca57362c69";  

String wifi_ip;
String wifi_status;

// Enable buzzer of it's true
bool AlarmAllowed = true;

bool isIrrigationActive = false;

// Enable auto water filling of it's true!
bool AutoWaterFillingAllowed = true;

bool flowSensor1Status = false;
bool flowSensor2Status = false;
volatile int pulseCount1 = 0; // count the number of pulses s1
volatile int pulseCount2 = 0; // count the number of pulses s2
const float MIN_FLOW_RATE = 2.0;

unsigned long previousMillis = 0;
unsigned long errorCheckPreviousMillis; 
const unsigned long errorCheckInterval = 5000;  // Interval for error check (5 seconds)

unsigned long zone1IrrigationPreviousMillis = 0; 
unsigned long zone2IrrigationPreviousMillis = 0; 
unsigned long zone3IrrigationPreviousMillis = 0; 

const long interval = 1000;
float flowRate1 = 0.0;
float flowRate2 = 0.0;

int isZone1Active = false;
int isZone2Active = false;
int isZone3Active = false;
int isWaterFillingActive = false;

bool errorOnEV1 = false;
bool errorOnEV2 = false;

// Create web server object on port 80;
AsyncWebServer server(80);

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD
DHT dht(DHT11_SENSORE_PIN, DHT11);  // DHT sensor setup


// APP 
const char dropwise_app[] PROGMEM = R"rawliteral(
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
            grid-template-columns: 3vw 1fr 3vw;
            grid-template-areas: 

                                ". navbar ."
                                ". network_details_header ."
                                ". connections ."
                                ". env_infos_header ."
                                 ". env ."
                                 ". sensores_header ."
                                 ". sensors ."
                                 ". system-health-header ."
                                 ". health ."
                                 ". schuduling_header ."
                                 ". scheduler_wrapper ."
                                 ". activation_header ."
                                 ". controllers .";

            gap: 1rem;
            padding-bottom: 2rem;
        }   

        .header {
            background-color: red;
            display: flex;
            align-items: center;
            background-color: #fbfbfb;
            border-left: 10px solid #9376ed;
            padding: 1rem 2rem;
        }

        .activation_header {
            grid-area: activation_header;
        }

        .schuduling_header {
            grid-area: schuduling_header;
        }

        .system-health-header {
            grid-area: system-health-header;
        }

       .sensores_header {
        grid-area: sensores_header;
       }
        

        .env_infos_header {
            grid-area: env_infos_header;
        }

        .network_details_header {
            grid-area: network_details_header;
        }

        .scheduler_wrapper {
            grid-area: scheduler_wrapper;
            background-color: #fff;
            padding: 2rem;
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 1rem;                    
        }

        .SelectorWrapper {
            background-color: #fbfbfb;
            padding: 1rem 1rem;
            display: flex;
            gap: 1rem;
            align-items: center;
        }

        #scheduler_submit {
            justify-self: self-end;           
        }

         select, input {
            width: 150px;
            outline: none;
            border: none;
            color: #9376ed;
            font-size: 1.1rem;
            background: none;
            border-bottom: 1px solid #9376ed;
        }
        
        label {
            font-weight: 500;
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
            padding: 2rem 0rem;
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
    }

.device-status p {
    flex: 1; /* This ensures that the content inside each status element fills available space */
    min-width: 120px; /* Ensure each status has a minimum width */
}

.fa-solid {
    padding-right: 0.2rem;
}
    
.system-health-wrapper {
    background-color: #fff;
    padding: 2rem;
    
    display: flex;
    flex-direction: column;
    gap: 1rem;
    background-color: #fff;
    grid-area: health;

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

    .fa-cloud-rain {
        color: #4242e1;
    }
        
        .connection_info {
            display: flex;
            flex-wrap: nowrap;
            gap: 0.5rem;
            justify-content: space-between;
            align-items: center;
            padding: 1rem;
            font-size: 0.9rem;
            background-color: #ffffff;
             top: 6REM;
            grid-area: connections;
              overflow-x: auto;   /* Enable horizontal scrolling */
           

        }

        .connection_info .info {
            display: flex;
            align-items: center;
            flex-grow: 1;
            gap: 0.5rem;
            background-color: #fbfbfb;
            padding: 1rem;
            width: fit-content;      
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
            
        }

        .info {
            display: flex;
            gap: 1rem;
            align-items: center;
            background-color: #fbfbfb;
            padding: 1rem;
            
        }


        .info_title {
            font-weight: bold;
            color: #ccc;
        }


        .env_infos {
            display: grid;
            grid-template-columns: repeat(auto-fit,  minmax(200px, 1fr));
            gap: 1rem;
            background-color: #fff;
            
            grid-area: env;
            padding: 1rem;
        }     

    
        .controler_card {
            padding: 1rem 2rem;
            background-color: #ffffff;
            
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
            border: none;
            cursor: pointer;
            width: 100%;
            font-size: 0.9rem;
            color: #fff;
            background-color: #9376ed;
        }

        input::placeholder {
            color: #b5b4b4;
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

       label {
        font-size: 1.3rem;
        color: #696868;
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
            </div>
        </div>

        <div class="header env_infos_header">
            <h2>Environmental Information</h2>
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


                <div class="info">
                    <i class="fa-solid fa-cloud-rain"></i>
                    <p class="info_title">Rain</p>
                    <p id="rain">[RAIN]</p>
                </div>

            </div>


        <div class="header network_details_header">
            <h2>Network Details</h2>
        </div>
       
         <div class="connection_info">
           

            <div class="info connection_ssid">
                <i class="fa-solid fa-wifi"></i>
                <p id="wifi_ssid">{{W_SSID}}</p>
            </div>

            <div class="info connection_rssi">
                <i class="fa-solid fa-signal"></i>
                <p id="wifi_rssi">{{W_RSSI}}</p>


            </div>

            <div class="info connection_ip">
                <i class="fa-solid fa-network-wired"></i>
                <p id="ip_adress">{{W_IP}}</p>
            </div>
        </div>

        <div class="header system-health-header">
            <h2>System Overview</h2>
        </div>

    <div class="system-health-wrapper">        

        <div class="all-statuses">
                <div class="device-status">
                    <p class="device-name">Pump 1 <span style="color: #b5b4b4"> - Zone 1</span> </p>
                    <p id="rely1Status" class="status">{{R1_STATUS}}</p>
                    <p id="ev_1_error" style="color: red;">[EV1_E]</p>  
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
                    <p id="rely4Status" class="status">{{R4_STATUS}} <span id="wf_source">{{WF_SOURCE}}</span> </p> <span style="color: blue" id="awf-command-type" class="commandType">{{AW_FILLING_STATUS}}</span> 
                </div>

                 <div class="device-status">
                    <p class="device-name">Default Motor</p>
                    <p id="irrigation_motor" class="status">[DEF_MOTOR]</p>
                    <p id="ev_2_error" style="color: red;">[EV2_E]</p>
                </div>
                
            </div>
               
    </div>  
                
            <div class="header sensores_header">
            <h2>Sensor Readings</h2>
        </div>


        <div class="sensors_container">

        <div class="sensore_card water_level">
                <p class="sensore_card_title">Water Level<span style="color: #b5b4b4"> - Tank 1</span></p>
                <div class="sensore_card_value_container">
                    <p id="waterlevel" class="sensore_card_value">[WATERLEVEL]</p>
                    <p class="sensore_card_unite">%</p>
                </div>
        </div>


        <div class="sensore_card water_level">
                <p class="sensore_card_title">Flow Sensor<span style="color: #b5b4b4"> - Water tank (l/min)</span></p>
                <div class="sensore_card_value_container">
                    <p id="flow_r_1" class="sensore_card_value">[FLOW_R_1]</p>
                    <p class="sensore_card_unite">L</p>

                </div>
        </div>

        <div class="sensore_card water_level">
                <p class="sensore_card_title">Flow Sensor<span style="color: #b5b4b4"> - Zone 1 (l/min)</span></p>
                <div class="sensore_card_value_container">
                    <p id="flow_r_2" class="sensore_card_value">[FLOW_R_2]</p>
                    <p class="sensore_card_unite">L</p>

                </div>
        </div>
      


        </div>


        <div class="header schuduling_header">
            <h2>Irrigation Scheduling</h2>
        </div>


           <form class="scheduler_wrapper" method="get" action="/schedule_arrigation">
    <div class="SelectorWrapper">
        <label for="zoneSelectorList">Select Zone</label>
        <select id="zoneSelectorList" name="zone">
            <option value="1">Zone 1</option>
            <option value="2">Zone 2</option>
            <option value="3">Zone 3</option>
        </select>
    </div>

    <div class="SelectorWrapper">
        <label for="timeSelectorInput">Duration</label>
        <input required id="timeSelectorInput" type="number" name="duration" placeholder="in seconds">
    </div>

    <button id="scheduler_button" class="switch-btn" type="submit">Press to set timer</button>
</form>

 <div class="header activation_header">
            <h2>Zone Activation  </h2>
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

               <div class="controler_card">
                <div class="controler_info">
                    <p class="controler_zone_name">Set Motor 2 as default</p>
                </div>
                <div class="controler_switches">
                    <button id="def_motor_selector" class="switch-btn " data-state="off">Press to set</button>
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
            let defMotorSelector = document.getElementById("def_motor_selector");
            // Relay Status 
            let zone1RelayStatus = document.getElementById('rely1Status');
            let zone2RelayStatus = document.getElementById('rely2Status');
            let zone3RelayStatus = document.getElementById('rely3Status');
            let autoWaterFillingStatus = document.getElementById('rely4Status');
            let schedulerButton = document.getElementById('scheduler_button');

            

            schedulerButton.addEventListener('click', (event) => {

                let zone1RelayStatusContent = zone1RelayStatus.innerText.trim();
                let zone2RelayStatusContent = zone2RelayStatus.innerText.trim();
                let zone3RelayStatusContent = zone3RelayStatus.innerText.trim();
                let selectedZone = document.getElementById('zoneSelectorList').value;


                if (zone1RelayStatusContent == 'Active' && selectedZone == '1'  ) {
                     alert("Irrigation cannot be started as Zone 1 is currently active");
                    event.preventDefault();
                } else if (zone2RelayStatusContent == 'Active' && selectedZone == '2' ) {
                     alert("Irrigation cannot be started as Zone 2 is currently active");
                } else if (zone3RelayStatusContent == 'Active' && selectedZone == '3' ) {
                     alert("Irrigation cannot be started as Zone 3 is currently active");
                };

            });

            zone1RelayController.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();
                
                let zone1RelayStatusContent = zone1RelayStatus.innerText.trim();

                if (zone1RelayStatusContent == "Inactive") {
                    xhttp.open("GET", "zone/1/on", true);
                    zone1RelayController.setAttribute('data-state', 'on');
                    zone1RelayController.style.backgroundColor = "red";
                    zone1RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/1/off", true);
                   zone1RelayController.setAttribute('data-state', 'off');

                   zone1RelayController.style.backgroundColor = "#61e350";
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
                    zone2RelayController.style.backgroundColor = "red";
                    zone2RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/2/off", true);
                   zone2RelayController.setAttribute('data-state', 'off');

                zone2RelayController.style.backgroundColor = "#61e350";
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
                    zone3RelayController.style.backgroundColor = "red";
                    zone3RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/3/off", true);
                   zone3RelayController.setAttribute('data-state', 'off');
                   zone3RelayController.style.backgroundColor = "#61e350";
                   zone3RelayController.innerText = "Start";
                }

                xhttp.send();
            });





    waterFillingController.addEventListener('click', () => {
    let xhttp = new XMLHttpRequest();
    let waterFillingControllerAttr = waterFillingController.getAttribute('data-state');

    if (waterFillingControllerAttr === "off") {
        xhttp.open("GET", "/waterfilling/on", true);
        waterFillingController.setAttribute('data-state', 'on');
        waterFillingController.style.backgroundColor = "red";
        waterFillingController.innerText = "Disactivate";
    } else {
        xhttp.open("GET", "/waterfilling/off", true);
        waterFillingController.setAttribute('data-state', 'off');
        waterFillingController.style.backgroundColor = "#61e350";
        waterFillingController.innerText = "Activate";
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
                    alarmController.style.backgroundColor = "#61e350";
                    alarmController.innerText = "Activate";

                }

                xhttp.send();
            });

            /////////////////////////:::
            defMotorSelector.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();

                let motorSelectorAtrr = defMotorSelector.getAttribute('data-state');
                
                if (motorSelectorAtrr == "off") {
                    xhttp.open("GET", "defmotor/select/2", true);
                    defMotorSelector.setAttribute('data-state', 'on');
                    defMotorSelector.style.backgroundColor = "red";
                    defMotorSelector.innerText = "Reset Motor 1 as Default";

                } else {
                   xhttp.open("GET", "defmotor/select/1", true);
                    defMotorSelector.setAttribute('data-state', 'off');
                    defMotorSelector.style.backgroundColor = "#61e350";
                    defMotorSelector.innerText = "Set Motor 2 as Default";

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
      document.getElementById("irrigation_motor").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/defarrigmotor", true);
  xhttp.send();
}, 1000 ) ;




setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("flow_r_1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/flowrate/1", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("flow_r_2").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/flowrate/2", true);
  xhttp.send();
}, 1000 ) ;


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rain").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/rain", true);
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
      document.getElementById("wf_source").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/waterfilling/source", true);
  xhttp.send();
}, 2000 ) ;


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



setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ev_1_error").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/error/ev/1", true);
  xhttp.send();
},5000 );


setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ev_2_error").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/error/ev/2", true);
  xhttp.send();
},5000 );



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

void IRAM_ATTR pulseCounter1() {
  pulseCount1++;  // Increment pulse count for sensor 1
}

void IRAM_ATTR pulseCounter2() {
  pulseCount2++;  // Increment pulse count for sensor 2
}

void setup_gpio_pins() {

  // Set GPIO pins for relays as output
  pinMode(RELY_1_PIN, OUTPUT);
  pinMode(RELY_2_PIN, OUTPUT);
  pinMode(RELY_3_PIN, OUTPUT);
  pinMode(RELY_4_PIN, OUTPUT);
  pinMode(RELY_6_PIN, OUTPUT);
  pinMode(RELY_7_PIN, OUTPUT);
  pinMode(RELY_8_PIN, OUTPUT);

  digitalWrite(RELY_1_PIN, HIGH);
  digitalWrite(RELY_2_PIN, HIGH);
  digitalWrite(RELY_3_PIN, HIGH);
  digitalWrite(RELY_4_PIN, HIGH);
  digitalWrite(RELY_6_PIN, HIGH);
  digitalWrite(RELY_7_PIN, HIGH);
  digitalWrite(RELY_8_PIN, HIGH);

  // Set GPIO pin for water level sensor as input
  pinMode(WATER_LEVEL_SENSORE_PIN, INPUT);

  // Set GPIO pin for buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);

  // Set GPIO pin for DHT11 sensor as input
  pinMode(DHT11_SENSORE_PIN, INPUT);

  // Set GPIO PIN for Rain sensor as input 
  pinMode(RAIN_SENSOR_PIN, INPUT);

  // Set GPIO PIN for water_flow sensors as input 
  pinMode(WATER_FLOW_SENSORE_1, INPUT);
  pinMode(WATER_FLOW_SENSORE_2, INPUT);

  pinMode(RAIN_SENSOR_PIN, INPUT);

  // Set GPIO pin for built-in LED as output
  pinMode(BUILT_IN_LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(WATER_FLOW_SENSORE_1), pulseCounter1, FALLING);  // Trigger on falling edge
  attachInterrupt(digitalPinToInterrupt(WATER_FLOW_SENSORE_2), pulseCounter2, FALLING);  // Trigger on falling edge
  
  previousMillis = millis();

}

void lcd_ProgressBar(int seconds) {
  int totalTime = seconds * 1000;  // Convert seconds to milliseconds
  int increment = totalTime / 16;  // 16 steps to fill the progress bar (16 columns)

  lcd.setCursor(0, 1);  // Move to the second line
  lcd.print("                  ");  // Empty progress bar
  
  for (int i = 0; i < 16; i++) {
    delay(increment);  // Wait for the next step
    lcd.setCursor(i + 1, 1);  // Move the cursor to the next position
    lcd.print("-");  // Fill the progress bar with #
  }

  lcd.setCursor(0, 1);  // Move to the beginning of the progress bar
  lcd.print("[-----------------]");  // Complete progress bar

  // Optional: Display the message when loading is complete
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Ready!");
  delay(1000);  // Wait for 1 second before doing anything else
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



void hundelWaterFlowing() {
  int wf_sensore_1_status = digitalRead(WATER_FLOW_SENSORE_1);
  int wf_sensore_2_status = digitalRead(WATER_FLOW_SENSORE_2);

  // Check if water is flowing
  if (wf_sensore_1_status == HIGH ) {  // Assuming LOW indicates water flow
    flowSensor1Status = true;
  } else {
    flowSensor1Status = false;
  }

   if (wf_sensore_2_status == HIGH) {  // Assuming LOW indicates water flow
    flowSensor2Status = true; 
  } else {
    flowSensor2Status = false;
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

String waterFlow1SensorStatus() {
  
  if (flowSensor1Status == true) {
    return "Active";
  } else {
    return "Inactive";
  } 
}

String hundelEv1Error() {
  if (errorOnEV1 == true) {
    return "Error Detected on Zone 1 EV";
  } else {
    return "";
  }
}

String hundelEv2Error() {
  if (errorOnEV2 == true) {
    if (defaultIrrigtionMotor == 18) {
      return "Main Water Pump Error";
    } else {
      return "Secondary Water Pump Error";
    }
  } else {
    return "";
  }
}

String waterFlow2SensorStatus() {
  
  if (flowSensor2Status == true) {
    return "Active";
  } else {
    return "Inactive";
  } 
}

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

  
  // Only proceed if AutoWaterFilling is allowed
  if (AutoWaterFillingAllowed) {
    // If the water level is very low (below 10%)
    if (WATER_LEVEL < 10) {
      if (AlarmAllowed) {
        beep_buzzer(3, 500);  // Trigger buzzer if water level is low
      }
    
      digitalWrite(RELY_4_PIN, LOW); 
    }
    // If the water level is between 10% and 40%, ensure the pump is off
    else if (WATER_LEVEL >= 30 ) {
      digitalWrite(RELY_4_PIN, HIGH);  // Turn off water filling pump
    }

  }// If AutoWaterFilling is disabled, ensure the pump is off
  else {
    digitalWrite(RELY_4_PIN, HIGH);  // Make sure the pump is off if AutoWaterFilling is disabled
  }
}

// -- RETURN WIFI SSID -- //
String hundelSSID() {
  return String(wifi_ssid);
};

// -- RETURN WIFI RSSI -- //
String hundelRSSI() {
int rssi = WiFi.RSSI();  // Get the current RSSI value from WiFi
  if (rssi >= -50) {
    return "Excellent Signal";  // RSSI values close to 0 are excellent
  } else if (rssi >= -60) {
    return "Good Signal";  // RSSI between -50 and -60 is considered good
  } else if (rssi >= -70) {
    return "Fair Signal";  // RSSI between -60 and -70 is considered fair
  } else if (rssi >= -80) {
    return "Weak Signal";  // RSSI between -70 and -80 is weak
  } else {
    return "Very Weak Signal";  // RSSI below -80 is considered very weak
  }};

// -- ACTIVATE BUZZER ALARM -- //
void activateBuzzerAlarm() {
    AlarmAllowed = true;
};

// -- DISACTIVATE BUZZER ALARM -- //
void disactivateBuzzerAlarm() {
    AlarmAllowed = false;
};

// -- ACTIVATE BUZZER ALARM -- //
void activateAutoWaterFilling() {
  AutoWaterFillingAllowed = true;
  digitalWrite(RELY_4_PIN, LOW);
};

void shutdownRelays() {
  digitalWrite(RELY_1_PIN, HIGH);
  digitalWrite(RELY_2_PIN, HIGH);
  digitalWrite(RELY_3_PIN, HIGH);
}

void checkRain() {
  int rain_sensor_status = digitalRead(RAIN_SENSOR_PIN);
  
  if (rain_sensor_status == LOW) {  // Rain detected
      isRaining = true;
      shutdownRelays();
  } else {  // No rain detected
    isRaining = false;
  }
}

String RainStatus() {
  if (isRaining == true ) {
    return "Raining";
  } else {
    return "No Rain Detected";
  }
}

void disactivateAutoWaterFilling() {
  AutoWaterFillingAllowed = false;
  digitalWrite(RELY_4_PIN, HIGH);
};


void turnOnZone1() {
digitalWrite(RELY_1_PIN, LOW);
digitalWrite(defaultIrrigtionMotor, LOW);
isZone1Active = true;

};

void turnOffZone1() {
  digitalWrite(RELY_1_PIN, HIGH);
  isZone1Active = false;
  if (errorOnEV1 == true) {
    digitalWrite(RELY_5_PIN, HIGH);
  }    
digitalWrite(defaultIrrigtionMotor, HIGH);

  isZone1Active = false;            // If Zone 1 is not active, no error
};



void turnOnZone2() {
  digitalWrite(RELY_2_PIN, LOW);
  isZone2Active = true;
  digitalWrite(defaultIrrigtionMotor, LOW);

};

void checkArrigationTasks() {
  int motor1Status = digitalRead(18);
  int motor2Status = digitalRead(19);
  if (motor1Status == LOW || motor2Status == LOW) {
    isIrrigationActive == true;
  } else {
    isIrrigationActive == false;
  }
}

void checkSystemErrors() {
  unsigned long currentMillis = millis();  // Get the current time

  if (currentMillis - errorCheckPreviousMillis  >= errorCheckInterval) {

    if (isZone1Active == true) {
      if (flowRate1 < MIN_FLOW_RATE) {
          digitalWrite(RELY_1_PIN, HIGH);   // Turn off the first relay (assuming HIGH turns it off)
          digitalWrite(RELY_5_PIN, LOW);    // Turn on the second relay (assuming LOW turns it on)
          errorOnEV1 = true;                 // Mark an error
      } else {
          errorOnEV1 = false;                // No error, reset the flag
      }
    } else {
      errorOnEV1 = false;                  // If Zone 1 is not active, no error
    }

    if (isIrrigationActive == true)  {
      if (flowRate2  < MIN_FLOW_RATE) {
        errorOnEV2 = true;  
        if (defaultIrrigtionMotor == 18 ) {
          digitalWrite(defaultIrrigtionMotor, HIGH);
          digitalWrite(RELY_7_PIN, LOW);
        } else {
          digitalWrite(defaultIrrigtionMotor, LOW);
          digitalWrite(RELY_7_PIN, HIGH);
        }
      } else {
        errorOnEV2 = false;                 
      }
    }

  }
}


void turnOffZone2() {
  digitalWrite(RELY_2_PIN, HIGH);
  isZone2Active = false; 
digitalWrite(defaultIrrigtionMotor, HIGH);

};


void turnOnZone3() {
  digitalWrite(RELY_3_PIN, LOW);
  isZone3Active = true;
  digitalWrite(defaultIrrigtionMotor, LOW);
};

void turnOffZone3() {
  digitalWrite(RELY_3_PIN, HIGH);
  isZone3Active = false; 
  digitalWrite(defaultIrrigtionMotor, HIGH);  
}; 

void hundelIrrigationMotors() {
  if (isZone1Active == true || isZone2Active == true || isZone3Active == true ) {
    digitalWrite(defaultIrrigtionMotor, LOW);
  } else {
    digitalWrite(defaultIrrigtionMotor, HIGH);
  }
}


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

String hundelDefSelectedMotor() {
  if (defaultIrrigtionMotor == 18) {
    return "Motor 1";
  } else {
    return "Motor 2";
  }
}

String flowRate1Status() {
  return String(flowRate1);
}

String flowRate2Status() {
  return String(flowRate2);
}

// Function to calculate flow rate from sensor 1
float calculateFlowRateSensor1(int pulseCount) {
  // Convert pulses to liters per minute (7.5 pulses per liter for YF-S201 sensor)
  float flowRate = (pulseCount / 7.5);  // Convert pulses to liters
  flowRate *= 60;  // Convert from seconds to minutes
  return flowRate;
}

// Function to calculate flow rate from sensor 2
float calculateFlowRateSensor2(int pulseCount) {
  // Convert pulses to liters per minute (7.5 pulses per liter for YF-S201 sensor)
  float flowRate = (pulseCount / 7.5);  // Convert pulses to liters
  flowRate *= 60;  // Convert from seconds to minutes
  return flowRate;
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
  } else if(var == "RAIN"){
    return RainStatus();
  } else if (var == "FLOW_R_1") {
    return flowRate1Status();
  } else if (var == "FLOW_R_2") {
    return flowRate2Status();
  } else if (var == "EV1_E") {
    return hundelEv1Error();
  } else if (var == "EV2_E") {
    return hundelEv2Error();
  } else if (var == "DEF_ARIG_MOTOR") {
    return hundelDefSelectedMotor();
  }
    return String();
}

void startIrrigation(String zone, int duration) {
  const unsigned long currentMillis = millis();
  
  // Zone 1
  if (zone == "1") {
    if (!isZone1Active) {
      turnOnZone1();
      zone1IrrigationPreviousMillis = currentMillis;
      isZone1Active = true;
    }

    // If the duration has passed, turn off Zone 1
    if (isZone1Active && (currentMillis - zone1IrrigationPreviousMillis >= duration * 1000)) {
      turnOffZone1();
      isZone1Active = false;
    }

  } 
  // Zone 2
  else if (zone == "2") {
    if (!isZone2Active) {
      turnOnZone2(); // Corrected to turn on Zone 2
      zone2IrrigationPreviousMillis = currentMillis;
      isZone2Active = true;
    }

    // If the duration has passed, turn off Zone 2
    if (isZone2Active && (currentMillis - zone2IrrigationPreviousMillis >= duration * 1000)) {
      turnOffZone2();
      isZone2Active = false;
    }

  }
  // Zone 3
  else if (zone == "3") {
    if (!isZone3Active) {
      turnOnZone3(); // Corrected to turn on Zone 3
      zone3IrrigationPreviousMillis = currentMillis;
      isZone3Active = true;
    }

    // If the duration has passed, turn off Zone 3
    if (isZone3Active && (currentMillis - zone3IrrigationPreviousMillis >= duration * 1000)) {
      turnOffZone3();
      isZone3Active = false;
    }

  }
}

void selectMotor2AsDefault() {
  defaultIrrigtionMotor = RELY_7_PIN;
}

void selectMotor1AsDefault() {
  defaultIrrigtionMotor = RELY_6_PIN;
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



// Function to display the active zones or home screen
void lcd_active_zones() {
  lcd.clear();  // Clear the screen

  // Check if any zone is active (i.e., any relay is HIGH)
  if (digitalRead(RELY_1_PIN) == HIGH || digitalRead(RELY_2_PIN) == HIGH || digitalRead(RELY_3_PIN) == HIGH) {
    
    // Check Zone 1 (RELY_1_PIN)
    if (digitalRead(RELY_1_PIN) == HIGH) {
      lcd.setCursor(0, 0);  // Set cursor to the first line, first column
      lcd.print("Z1: ON");
    } else {
      lcd.setCursor(0, 0);  // Set cursor to the first line, first column
      lcd.print("Z1: OFF");
    }

    // Check Zone 2 (RELY_2_PIN)
    if (digitalRead(RELY_2_PIN) == HIGH) {
      lcd.setCursor(0, 1);  // Set cursor to the second line, first column
      lcd.print("Z2: ON");
    } else {
      lcd.setCursor(0, 1);  // Set cursor to the second line, first column
      lcd.print("Z2: OFF");
    }

    // Check Zone 3 (RELY_3_PIN)
    if (digitalRead(RELY_3_PIN) == HIGH) {
      lcd.setCursor(8, 1);  // Set cursor to the second line, 8th column
      lcd.print("Z3: ON");
    } else {
      lcd.setCursor(8, 1);  // Set cursor to the second line, 8th column
      lcd.print("Z3: OFF");
    }

  } else {
    // If no zone is active, show the home screen message
    lcd.setCursor(0, 0);  // Set cursor to the first line, first column
    lcd_info_page();
  }
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
lcd_ProgressBar(5);
lcd_welcome_screen();
  lcd_info_page();

  // ROUTES 
  // RO
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", dropwise_app, processor);
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


  server.on("/flowrate/1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", flowRate1Status().c_str());
  });
  
  server.on("/flowrate/2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", flowRate2Status().c_str());
  });
  server.on("/rain", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", RainStatus().c_str());
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

server.on("/error/ev/1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelEv1Error().c_str());
  });

  server.on("/error/ev/2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hundelEv2Error().c_str());
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
  
   server.on("/defmotor/select/1", HTTP_GET, [](AsyncWebServerRequest *request){
    selectMotor1AsDefault();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

  server.on("/defmotor/select/2", HTTP_GET, [](AsyncWebServerRequest *request){
    selectMotor2AsDefault();  // Execute the function, but don't send any response
    request->send(200, "text/plain", "");  // Send an empty response
  });

   server.on("/defarrigmotor", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/plain", hundelDefSelectedMotor().c_str());
  });
    
server.on("/schedule_arrigation", HTTP_GET, [](AsyncWebServerRequest *request){
   
    // Get parameters from the URL
    String zone = request->getParam("zone")->value();
    String duration = request->getParam("duration")->value();  // Use a different parameter name for duration
    
    // Convert duration to integer (seconds)
    int duration_s = duration.toInt();


    startIrrigation(zone, duration_s);

    request->send_P(200, "text/html", dropwise_app, processor); 
});

  // Start server 
  server.begin();
}


// Function to update DuckDNS with the current public IP
void updateDuckDNS() {

  // check if wifi is connected 
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // url for my duckdns account
    String url = "https://www.duckdns.org/update?domains=" + duckdns_domain + "&token=" + duckdns_token + "&ip=" + wifi_ip;
    
    http.begin(url); 
    int httpCode = http.GET();  // Send GET request

    // Check if the request was sent 
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("DuckDNS update successful: " + payload);
    } else {
      Serial.println("Error updating DuckDNS: " + String(httpCode));
    }

    http.end();  // Close the HTTP connection
  } else {
    Serial.println("WiFi not connected. Cannot update DuckDNS.");
  }
}



void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Every second, calculate the flow rates for both sensors
    previousMillis = currentMillis;

    // Calculate flow rate for sensor 1
    if (isZone1Active == true) {
      flowRate1 = calculateFlowRateSensor1(pulseCount1);
    }

    if (isWaterFillingActive == true) {
    // Calculate flow rate for sensor 2
      flowRate2 = calculateFlowRateSensor2(pulseCount2);
    }
    
    // Reset pulse counts for both sensors for the next measurement
    pulseCount1 = 0;
    pulseCount2 = 0;
    
  }
  
  //hundelIrrigationMotors();
  hundelWaterLevel();
  hundelTemperature();
  hundelWaterFlowing();
  checkRain();
  checkSystemErrors();
  hundelHumidity();
  updateDuckDNS();
  hundelWaterFilling();
   checkArrigationTasks();
  lcd_info_page();  // Update LCD with current info
  delay(1000);
  }


  
