
float motor_velocity = 0;
float motor_power = 0;
long last_received = 0;
long last_time = 0;

typedef union {
  char c[8];
  float f[2];
} two_floats;

void process_packet(CanMessage &msg) {
  if (msg.id == 0x501) {
    last_received = millis();
    two_floats data;
    for (int i = 0; i < 8; i++) data.c[i] = msg.data[i];
    motor_velocity = data.f[0];
    motor_power = data.f[1];
  }
}

void setup() {
  Can.attach(&process_packet);
  Can.begin(1000);
  Serial.begin(115200);
  Serial.println("Can interrupt test program begin");
  Serial.println("If 'Last Seen' is 0, that means no motor drive command packets were ever received correctly, or something isn't working");
}

void loop() {
  if (millis() - last_time > 500) {
    last_time = millis();
    Serial.print("Last Seen: ");
    Serial.print(last_received);
    Serial.print(" ms ago\tVelocity: ");
    Serial.print(motor_velocity);
    Serial.print("m/s\tPower: ");
    Serial.print(motor_power);
    Serial.println("%\r\n");
  }
}
