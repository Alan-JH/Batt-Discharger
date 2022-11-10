#define V_ADC_PIN 8
#define HS_FET 9
#define LS_FET 10
#define BUTTON_PIN 3

void setup(){
  // Set debug UART pins to default, initialize to 57600 baud
  Serial.pins(5, 4);
  Serial.begin(57600, (SERIAL_8N1));
}

void loop(){
  float volt = 11 * analogRead(V_ADC_PIN) * 3.3 / 1024; // ATTiny1604 has 10 bit ADC
  Serial.println(volt);
  delay(100);
}