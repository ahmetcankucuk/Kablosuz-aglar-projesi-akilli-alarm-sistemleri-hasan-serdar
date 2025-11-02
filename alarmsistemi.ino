#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Rıfat";
const char* password = "rifat546";

const char* mqtt_server = "192.168.159.188"; 
const int mqtt_port = 1883;
const char* mqtt_topic = "alarm/hareket"; 

const int HAREKET_SENSOR_PINI = 2; // (GPIO 2, D2)

WiFiClient espClient;
PubSubClient client(espClient);
int sonSensorDurumu = LOW; 

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_AlarmClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(HAREKET_SENSOR_PINI, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int mevcutSensorDurumu = digitalRead(HAREKET_SENSOR_PINI);

  if (mevcutSensorDurumu == HIGH && sonSensorDurumu == LOW) {
    unsigned long currentMillis = millis();
    unsigned long totalSeconds = currentMillis / 1000;
    int seconds = totalSeconds % 60;
    unsigned long totalMinutes = totalSeconds / 60;
    int minutes = totalMinutes % 60;
    unsigned long totalHours = totalMinutes / 60;
    int hours = totalHours % 24; 

    char payload[100];
    sprintf(payload, "hareket algılandı: %02d:%02d:%02d", hours, minutes, seconds);

    Serial.println(payload); 
    client.publish(mqtt_topic, payload);
    
    delay(100); 
  }

  sonSensorDurumu = mevcutSensorDurumu;

  delay(50); 
}