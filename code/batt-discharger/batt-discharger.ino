#define V_ADC_PIN 8
#define HS_FET 9
#define LS_FET 10
#define BUTTON_PIN 3

float volt;
int cellCount;
volatile bool running;
float vdd;

void toggleRun(){
  running = !running;
}

void measureVdd(){
  analogReference(VDD);
  VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
  // there is a settling time between when reference is turned on, and when it becomes valid.
  // since the reference is normally turned on only when it is requested, this virtually guarantees
  // that the first reading will be garbage; subsequent readings taken immediately after will be fine.
  // VREF.CTRLB|=VREF_ADC0REFEN_bm;
  // delay(10);
  uint16_t reading = analogRead(ADC_INTREF);
  reading = analogRead(ADC_INTREF);
  uint32_t intermediate = 1023UL * 1500;
  vdd = intermediate / (1000.0 * reading);
}

void updateVoltage(){
  volt = 11 * analogRead(V_ADC_PIN) * vdd / 1024; // ATTiny1604 has 10 bit ADC
}

void updateCellCount(){
  updateVoltage();
  if (volt > 18 && volt < 26)
    cellCount = 6;
  else if (volt > 12 && volt < 17)
    cellCount = 4;
  else
    cellCount = -1;
}

void setup(){
  // Set debug UART pins to default, initialize to 57600 baud
  Serial.pins(5, 4);
  Serial.begin(57600, (SERIAL_8N1));
  pinMode(HS_FET, OUTPUT);
  pinMode(LS_FET, OUTPUT);
  digitalWrite(HS_FET, LOW);
  digitalWrite(LS_FET, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  measureVdd();
  Serial.print("Initialized, Measured Vdd: ");
  Serial.println(vdd);
  updateCellCount();
  if (cellCount == -1){
    Serial.println("Invalid Cell Count, Aborting...");
    while (1);
  }
  Serial.print("Cell Count: ");
  Serial.println(cellCount);
  attachInterrupt(BUTTON_PIN, toggleRun, FALLING);
}

void loop(){
  updateVoltage();
  if (running){
    if (volt < 3.8*cellCount)
      running = false;
    else{
      digitalWrite(HS_FET, HIGH);
      digitalWrite(LS_FET, HIGH);
      Serial.print("Discharging, Voltage: ");
    }
  }else{
    digitalWrite(HS_FET, LOW);
    digitalWrite(LS_FET, LOW);
    Serial.print("In Standby, Voltage: ");
  }
  Serial.println(volt);
  delay(100);
}
