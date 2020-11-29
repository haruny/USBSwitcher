#include <Arduino.h>
#include <EspMQTTClient.h> // https://github.com/plapointe6/EspMQTTClient


//based on Wemos D1 Mini
// see https://workspace.circuitmaker.com/Projects/Details/HarunY/USB-Switcher for the circuit

EspMQTTClient client(
  "<SSID>",       // ssid
  "<WIFI_PASSWORD>",   // ssid password
  "<MQTT-SERVER_IP>",  // MQTT Broker server ip
  "<MQTT_User>",   // MQTTUsername Can be omitted if not needed
  "<MQTT_PASS>",   // MQTTPassword Can be omitted if not needed
  "d1minisw",     // Client name that uniquely identify your device
  <MQTT_PORT>              // The MQTT port, default to 1883. this line can be omitted
);

int USB1_PIN = D6; 
int USB2_PIN = D7; 
int SW_OUT = D5;

void setup(){
  pinMode(SW_OUT,OUTPUT);
  pinMode(USB1_PIN,INPUT_PULLUP);
  pinMode(USB2_PIN,INPUT_PULLUP);
  client.enableLastWillMessage("d1/usbswitcher/switch/status","off");
  client.enableHTTPWebUpdater("admin", "apass", "/up");
}
void flipUSBOutput(){
    digitalWrite(SW_OUT, HIGH);
    client.executeDelayed( 500, []() {
        digitalWrite(SW_OUT, LOW);
        getUSBStatus();
    });
}
void getUSBStatus(){
  client.publish("d1/usbswitcher/usb1/status",(digitalRead(USB1_PIN)? "on" : "off"));
  client.publish("d1/usbswitcher/usb2/status",(digitalRead(USB2_PIN)? "on" : "off"));
  client.publish("d1/usbswitcher/switch/status", digitalRead(USB1_PIN)? "usb1":"usb2");
 }
void mqttMSG(String topic, String  payload){
    if (payload=="switch"){
      flipUSBOutput();
    }

    if (payload=="usb1" && digitalRead(USB1_PIN)==0){
      client.publish("d1/usbswitcher/switch/status", "usb1");
      flipUSBOutput();
    }
    
    if (payload=="usb2" && digitalRead(USB2_PIN)==0){
      client.publish("d1/usbswitcher/switch/status", "usb2");
      flipUSBOutput();
    }
    if (payload=="restart"){
      ESP.restart();
    }
}

void onConnectionEstablished(){
    client.subscribe("d1/usbswitcher/switch/cmd", mqttMSG);
    client.publish("d1/usbswitcher/switch/status", digitalRead(USB1_PIN)? "usb1":"usb2");
    getUSBStatus();
}

void loop(){
  client.loop();
}
