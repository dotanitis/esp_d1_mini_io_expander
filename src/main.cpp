
#include <PCF8575.h>

// HDMI switch pin mapping
struct HdmiSwitch {
  uint8_t inputPins[3];
  uint8_t outputPin;
};

HdmiSwitch hdmiSwitches[4] = {
  { {P0, P1, P2}, P3 },    // Device 1
  { {P4, P5, P6}, P7 },    // Device 2
  { {P8, P9, P10}, P11 },  // Device 3
  { {P12, P13, P14}, P15 } // Device 4
};


#include <Arduino.h>
#include <PCF8575.h>
#include <i2cdetect.h>
#include <Wire.h>
#include <WiFiManager.h>

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* mqtt_server = "192.168.3.50";
const uint16_t mqtt_port = 1883;

const uint32_t MQTT_RETRY_MS = 5000;
const uint32_t HEAP_REPORT_MS = 60000;
const uint32_t SCHEDULED_RESTART_MS = 7UL * 24 * 3600 * 1000;
const uint32_t MIN_FREE_HEAP = 6000;
const uint32_t MIN_FREE_BLOCK = 3000;
uint32_t lastMqttAttempt = 0;
uint32_t lastHeapReport = 0;

void mqttReconnect() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastMqttAttempt < MQTT_RETRY_MS) return;
  lastMqttAttempt = millis();
  if (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("esp_d1_mini_io_expander")) {
      Serial.println("connected");
      // Subscribe to remote click topics every reconnect
      for (int i = 1; i <= 4; i++) {
        char topic[48];
        snprintf(topic, sizeof(topic), "home/office/hdmi-kvm/device%d/click", i);
        mqttClient.subscribe(topic);
        Serial.print("Subscribed to topic: ");
        Serial.println(topic);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void reportHealth() {
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t maxBlock = ESP.getMaxFreeBlockSize();
  uint8_t frag = ESP.getHeapFragmentation();
  Serial.printf("uptime=%lus heap=%u maxblock=%u frag=%u%% rssi=%d\n",
                millis() / 1000, freeHeap, maxBlock, frag, WiFi.RSSI());
  if (mqttClient.connected()) {
    char payload[128];
    snprintf(payload, sizeof(payload),
      "{\"uptime\":%lu,\"heap\":%u,\"maxBlock\":%u,\"frag\":%u,\"rssi\":%d}",
      millis() / 1000, freeHeap, maxBlock, frag, WiFi.RSSI());
    mqttClient.publish("home/office/hdmi-kvm/health", payload);
  }
  if (freeHeap < MIN_FREE_HEAP || maxBlock < MIN_FREE_BLOCK) {
    Serial.println("Low heap, restarting");
    delay(100);
    ESP.restart();
  }
}

// put function declarations here:
int i = 0;
void read_all_pin_values();

//PCF8575 PCF(0x20);
PCF8575 pcf8575(0x20);


void publish_hdmi_switch_status() {
  PCF8575::DigitalInput di = pcf8575.digitalReadAll();
  // Device 1: P0,P1,P2 (inputs), P3 (output)
  char topic1[] = "home/office/hdmi-kvm/device1/status";
  char payload1[128];
  snprintf(payload1, sizeof(payload1),
    "{\"device\":1,\"inputs\":[%d,%d,%d],\"output\":%d}",
    di.p0, di.p1, di.p2, di.p3);
  mqttClient.publish(topic1, payload1);

  // Device 2: P4,P5,P6 (inputs), P7 (output)
  char topic2[] = "home/office/hdmi-kvm/device2/status";
  char payload2[128];
  snprintf(payload2, sizeof(payload2),
    "{\"device\":2,\"inputs\":[%d,%d,%d],\"output\":%d}",
    di.p4, di.p5, di.p6, di.p7);
  mqttClient.publish(topic2, payload2);

  // Device 3: P8,P9,P10 (inputs), P11 (output)
  char topic3[] = "home/office/hdmi-kvm/device3/status";
  char payload3[128];
  snprintf(payload3, sizeof(payload3),
    "{\"device\":3,\"inputs\":[%d,%d,%d],\"output\":%d}",
    di.p8, di.p9, di.p10, di.p11);
  mqttClient.publish(topic3, payload3);

  // Device 4: P12,P13,P14 (inputs), P15 (output)
  char topic4[] = "home/office/hdmi-kvm/device4/status";
  char payload4[128];
  snprintf(payload4, sizeof(payload4),
    "{\"device\":4,\"inputs\":[%d,%d,%d],\"output\":%d}",
    di.p12, di.p13, di.p14, di.p15);
  mqttClient.publish(topic4, payload4);
}

void handleRemoteClick(int device) {
  Serial.print("handleRemoteClick called for device: ");
  Serial.println(device);
  if (device < 1 || device > 4) {
    Serial.println("Invalid device number!");
    return;
  }
  uint8_t outPin = hdmiSwitches[device-1].outputPin;
  Serial.print("Simulating button press on pin: ");
  Serial.println(outPin);
  pcf8575.digitalWrite(outPin, HIGH);
  delay(200);
  pcf8575.digitalWrite(outPin, LOW);
  delay(200);
  publish_hdmi_switch_status();
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  Serial.print("mqttCallback called. Topic: ");
  Serial.println(topic);
  Serial.print("Payload length: ");
  Serial.println(length);
  if (strstr(topic, "home/office/hdmi-kvm/device") && strstr(topic, "/click")) {
    int device = 0;
    int parsed = sscanf(topic, "home/office/hdmi-kvm/device%d/click", &device);
    Serial.print("Parsed device: ");
    Serial.println(device);
    Serial.print("sscanf return value: ");
    Serial.println(parsed);
    handleRemoteClick(device);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");
  Serial.println("-------");
  Serial.println(ESP.getResetInfo());
  Serial.println("Initializing WiFiManager...");

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP



  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - wipe credentials for testing
  //wm.resetSettings();
  //set custom ip for portal
  //WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  //WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  //wm.addParameter(&custom_mqtt_server);
  //wm.addParameter(&custom_mqtt_port);
  //wm.setTimeout(120); //seconds
  //connect to wifi - it will either connect or start an access point with the specified name
  if (!wm.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }
  
  // setup pinmods
  // Port 1
  pcf8575.pinMode(P0, INPUT);
  pcf8575.pinMode(P1, INPUT);
  pcf8575.pinMode(P2, INPUT);
  pcf8575.pinMode(P3, OUTPUT);
  // Port 2
  pcf8575.pinMode(P4, INPUT);
  pcf8575.pinMode(P5, INPUT);
  pcf8575.pinMode(P6, INPUT);
  pcf8575.pinMode(P7, OUTPUT);
  // Port 3
  pcf8575.pinMode(P8, INPUT);
  pcf8575.pinMode(P9, INPUT);
  pcf8575.pinMode(P10, INPUT);
  pcf8575.pinMode(P11, OUTPUT);
  // Port 4
  pcf8575.pinMode(P12, INPUT);
  pcf8575.pinMode(P13, INPUT);
  pcf8575.pinMode(P14, INPUT);
  pcf8575.pinMode(P15, OUTPUT);

  pcf8575.begin();

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  mqttReconnect();

  //Serial.swap();
  
  //Wire.begin();
  // Serial.println("i2cdetect example\n");
  // Serial.print("Scanning address range 0x03-0x77\n\n");


  // PCF.begin();

  // Serial.println(__FILE__);
  // Serial.print("PCF8575_LIB_VERSION:\t");
  // Serial.println(PCF8575_LIB_VERSION);

  // if (!PCF.begin())
  // {
  //   Serial.println("could not initialize...");
  // }
  // if (!PCF.isConnected())
  // {
  //   Serial.println("=> not connected");
  // }
  // else
  // {
  //   Serial.println("=> connected!!");
  //   PCF.selectNone();
  //   //PCF.select(7);
  // }


  
}

char c;
int selector = 0;

void loop() {

  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();

  if (millis() - lastHeapReport >= HEAP_REPORT_MS) {
    lastHeapReport = millis();
    reportHealth();
  }
  // Stopgap against slow leaks
  if (millis() > SCHEDULED_RESTART_MS) {
    Serial.println("Scheduled restart");
    delay(100);
    ESP.restart();
  }
  yield();

  if (Serial.available() > 0){
    c = Serial.read();

    if (c == '1'){
      pcf8575.digitalWrite(P3,HIGH);
      delay(200);
      pcf8575.digitalWrite(P3,LOW);
      delay(200);
    }
    if (c == '2'){
      pcf8575.digitalWrite(P7,HIGH);
      delay(200);
      pcf8575.digitalWrite(P7,LOW);
      delay(200);
    }
    if (c == '3'){
      pcf8575.digitalWrite(P11,HIGH);
      delay(200);
      pcf8575.digitalWrite(P11,LOW);
      delay(200);
    }
    if (c == '4'){
      pcf8575.digitalWrite(P15,HIGH);
      delay(200);
      pcf8575.digitalWrite(P15,LOW);
      delay(200);
    }
    if (c == '9'){
      // PCF.toggleMask(0x0070);
      read_all_pin_values();
    }
    //Serial.print("Char = ");
    //Serial.println(c, DEC);
  }
  //delay(1000);
}



void read_all_pin_values(){
  PCF8575::DigitalInput di = pcf8575.digitalReadAll();
  Serial.print(di.p15);
  Serial.print(" - ");
  Serial.print(di.p14);
  Serial.print(" - ");
  Serial.print(di.p13);
  Serial.print(" - ");
  Serial.print(di.p12);
  Serial.print(" - ");
  Serial.print(di.p11);
  Serial.print(" - ");
  Serial.print(di.p10);
  Serial.print(" - ");
  Serial.print(di.p9);
  Serial.print(" - ");
  Serial.print(di.p8);
  Serial.print(" - ");
  Serial.print(di.p7);
  Serial.print(" - ");
  Serial.print(di.p6);
  Serial.print(" - ");
  Serial.print(di.p5);
  Serial.print(" - ");
  Serial.print(di.p4);
  Serial.print(" - ");
  Serial.print(di.p3);
  Serial.print(" - ");
  Serial.print(di.p2);
  Serial.print(" - ");
  Serial.print(di.p1);
  Serial.print(" - ");
  Serial.println(di.p0);

  // Publish status to MQTT
  char payload[128];
  snprintf(payload, sizeof(payload),
    "{\"location\":\"home\",\"room\":\"office\",\"id\":\"hdmi-kvm\",\"ports\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]}",
    di.p0, di.p1, di.p2, di.p3, di.p4, di.p5, di.p6, di.p7, di.p8, di.p9, di.p10, di.p11, di.p12, di.p13, di.p14, di.p15);
  mqttClient.publish("home/office/hdmi-kvm/status", payload);

  // Also publish per-device status
  publish_hdmi_switch_status();
}


