#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t groundControllerMAC[] = {
  0x02, 0x00, 0x00, 0x00, 0x00, 0x01
};

uint8_t flightControllerMAC[] = {
  0x02, 0x00, 0x00, 0x00, 0x00, 0x02
};

struct TelemetryPacket {
  int droppedPackets; // Running total, prior 5 seconds
  int warningStatus; /*
    0: Nominal
    1: Brownout
    2: High temperature - Controller
    3: High temperature - Motor
    4: Other/Unknown
  */
  float temperature;
  float roll;
  float pitch;
  float totalAccel;
  float thrustPercent;
  float aileron;
  float elevator;
  float rudder;
};

struct ControlPacket {
  float power; // 0 to 1
  float roll; // -1 to 1
  float pitch; // -1 to 1
  float yaw; // -1 to 1
};

TelemetryPacket telemetry = {};

long lastPacket = millis();

void onNewPacket(const esp_now_recv_info_t *info, const unsigned char *data, int len) {

  if (len != sizeof(telemetry)) {
    Serial.println("Damaged Packet Received");
  }

  long timeSinceLastPacket = millis() - lastPacket;
  lastPacket = millis();

  memcpy(&telemetry, data, sizeof(data));

  Serial.printf(
    "Telemetry: %i,%i,%f,%f,%f,%f,%f,%f,%f,%f,%f",
    telemetry.droppedPackets,
    telemetry.warningStatus,
    telemetry.temperature,
    telemetry.roll,
    telemetry.pitch,
    telemetry.totalAccel,
    telemetry.thrustPercent,
    telemetry.aileron,
    telemetry.elevator,
    telemetry.rudder,
    timeSinceLastPacket
  );

}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  delay(4000); // Await before sending anything over serial

  if (esp_now_init() != ESP_OK) {
    Serial.println("Network initialization failed - client side");
    return;
  }
  Serial.println("Network intialized - client side");

  esp_wifi_set_mac(WIFI_IF_STA, &groundControllerMAC[0]);
  delay(100);
  Serial.print("Ground Controller MAC Address: ");
  Serial.println(WiFi.macAddress());

  esp_now_register_recv_cb(onNewPacket);

  Serial.println("Ground controller telemetry receiver setup");
}

void loop() {
  
}


/*
Compilation error: invalid conversion from 
'void (*)(const esp_now_recv_info_t*, const char*, int)' 
'esp_now_recv_cb_t' 

{aka 'void (*)(const esp_now_recv_info*, const char*, int)'}
{aka 'void (*)(const esp_now_recv_info*, const unsigned char*, int)'}
*/