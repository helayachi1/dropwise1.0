// DROPWISE APP | Made with love by Elayachi Hamrit 2025.

// LIBS 
#include <Wire.h>
#include <WiFi.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "ESPAsyncWebServer.h"
#include <TaskScheduler.h>

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

// Enable buzzer of it's true
bool AlarmAllowed = true;

// Enable auto water filling of it's true!
bool AutoWaterFillingAllowed = true;

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
                /* For webkit browsers like Chrome, Safari */
                ::-webkit-scrollbar {
                    height: 2px; /* Set the height of the horizontal scrollbar */
                }

                ::-webkit-scrollbar-track {
                    background: #f1f1f1; /* Background color of the track (area the thumb moves along) */
                }

                ::-webkit-scrollbar-thumb {
                    background: #888; /* Color of the draggable thumb */
                    border-radius: 2px; /* Round the corners of the thumb */
                }

                ::-webkit-scrollbar-thumb:hover {
                    background: #555; /* Change the thumb color when hovering */
                }

                /* Apply the custom scrollbar to an element with class .scroll-container */
.scroll-container {
    overflow-x: scroll; /* Enable horizontal scrolling */
    width: 100%; /* Set the width to allow scrolling */
    height: 100px; /* Set a fixed height */
}

/* Now apply the scrollbar styles */
.scroll-container::-webkit-scrollbar {
    height: 2px; /* Custom height for horizontal scrollbar */
}

.scroll-container::-webkit-scrollbar-track {
    background: #f1f1f1;
}

.scroll-container::-webkit-scrollbar-thumb {
    background: none;
    border-radius: 10px;
}

.scroll-container::-webkit-scrollbar-thumb:hover {
    background: #555;
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

 <div class="env_infos scroll-container">
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
                <p id="wifi_rssi">{{W_RSSI}}</p><span> db/ms</span>
                <span class="wifi_signal"></span>


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

        </div>


        <div class="header schuduling_header">
            <h2>Irrigation Scheduling</h2>
        </div>


           <form class="scheduler_wrapper" method="GET" action="/schedule_arrigation">
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
            
            // Relay Status 
            let zone1RelayStatus = document.getElementById('rely1Status');
            let zone2RelayStatus = document.getElementById('rely2Status');
            let zone3RelayStatus = document.getElementById('rely3Status');
            let autoWaterFillingStatus = document.getElementById('rely4Status');
            let schedulerButton = document.getElementById('scheduler_button');

            schedulerButton.addEventListener('click', (event) => {

                let rssiValue = document.getElementById('wifi_rssi').innerText.trim();
                let wifiSignalStatus = document.querySelector('.wifi_signal').innerText;
                             
                if (rssiValue === "-30") {
                    wifiSignalStatus.style.color = 'green';  // Excellent signal
                    wifiSignalStatus.innerText = 'Excellent Signal';
                } else if (rssiValue === "-40") {
                    wifiSignalStatus.style.color = 'green';  // Excellent signal
                    wifiSignalStatus.innerText = 'Excellent Signal';
                } else if (rssiValue === "-50") {
                    wifiSignalStatus.style.color = 'green';  // Excellent signal
                    wifiSignalStatus.innerText = 'Excellent Signal';
                } else if (rssiValue === "-60") {
                    wifiSignalStatus.style.color = 'yellow';  // Good signal
                    wifiSignalStatus.innerText = 'Good Signal';
                } else if (rssiValue === "-70") {
                    wifiSignalStatus.style.color = 'orange';  // Fair signal
                    wifiSignalStatus.innerText = 'Fair Signal';
                } else if (rssiValue === "-80") {
                    wifiSignalStatus.style.color = 'red';  // Weak signal
                    wifiSignalStatus.innerText = 'Weak Signal';
                } else if (rssiValue === "-90") {
                    wifiSignalStatus.style.color = 'darkred';  // Very weak signal
                    wifiSignalStatus.innerText = 'Very Weak Signal';
                } else {
                    wifiSignalStatus.style.color = 'gray';  // No signal
                    wifiSignalStatus.innerText = 'No Signal';
                }

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
                    zone1RelayStatus.style.color = "#61e350";
                    zone1RelayController.style.backgroundColor = "red";
                    zone1RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/1/off", true);
                   zone1RelayController.setAttribute('data-state', 'off');
                   zone1RelayStatus.style.color = "red";

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
                    zone2RelayStatus.style.color = "#61e350";
                    zone2RelayController.style.backgroundColor = "red";
                    zone2RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/2/off", true);
                   zone2RelayController.setAttribute('data-state', 'off');
                   zone2RelayStatus.style.color = "red";

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
                    zone3RelayStatus.style.color = "#61e350";
                    zone3RelayController.style.backgroundColor = "red";
                    zone3RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/3/off", true);
                   zone3RelayController.setAttribute('data-state', 'off');
                   zone3RelayStatus.style.color = "red";

                   zone3RelayController.style.backgroundColor = "#61e350";
                   zone3RelayController.innerText = "Start";
                }

                xhttp.send();
            });

            zone3RelayController.addEventListener('click', () => {

                let xhttp = new XMLHttpRequest();
                
                let zone3RelayStatusContent = zone3RelayStatus.innerText.trim();

                if (zone3RelayStatusContent == "Inactive") {
                    xhttp.open("GET", "zone/3/on", true);
                    zone3RelayController.setAttribute('data-state', 'on');
                    zone3RelayStatus.style.color = "#61e350";
                    zone3RelayController.style.backgroundColor = "red";
                    zone3RelayController.innerText = "Stop";

                } else {
                   xhttp.open("GET", "zone/3/off", true);
                   zone3RelayController.setAttribute('data-state', 'off');
                   zone3RelayStatus.style.color = "red";

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
  // Only proceed if AutoWaterFilling is allowed
  if (AutoWaterFillingAllowed) {
    // If the water level is very low (below 10%)
    if (WATER_LEVEL < 10) {
      if (AlarmAllowed) {
        beep_buzzer(3, 500);  // Trigger buzzer if water level is low
      }
      digitalWrite(RELY_4_PIN, LOW);  // Turn on water filling pump
    }
    // If the water level is between 10% and 40%, ensure the pump is off
    else if (WATER_LEVEL >= 10 && WATER_LEVEL <= 40) {
      digitalWrite(RELY_4_PIN, HIGH);  // Turn off water filling pump
    }
    // If the water level is significantly high (above 40%), turn off the pump
    else if (WATER_LEVEL > 40) {
      digitalWrite(RELY_4_PIN, HIGH);  // Turn off water filling pump
    }
  }
  // If AutoWaterFilling is disabled, ensure the pump is off
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
  return String(wifi_rssi);
};

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
  lcd_welcome_screen();
  lcd_info_page();

  // ROUTES 
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
  

  
  server.on("/schedule_arrigation", HTTP_GET, [](AsyncWebServerRequest *request){
    // Get 'zone' and 'duration' parameters from the GET request
    String zone = request->getParam("zone")->value(); 
    int duration = request->getParam("duration")->value().toInt(); // Convert duration to an integer
      if (zone == "1") {
          turnOnZone1();
          delay(duration);
          turnOffZone1();
        } else if (zone == "2") {
          turnOnZone2();
          delay(duration);
          turnOffZone2();
        } else if (zone == "3") {
          turnOnZone3();
          delay(duration);
          turnOffZone3();
        } 
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


