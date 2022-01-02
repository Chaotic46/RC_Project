int32_t leftMotorTicks;
int32_t rightMotorTicks;

void setup() {
  leftMotorTicks = 0;
  rightMotorTicks = 0;
  
  DDRD = 0;
  DDRD |= (1 << DDD7) | (1 << DDD6) | (1 << DDD2); // enable pins 6 and 7 as outputs
  PORTD = 0;
  PORTD |= (1 << PORTD5); // enable pull-up resistor on pin 5

  PCICR = 0;
  PCICR |= (1 << PCIE2);
  PCMSK2 = 0;
  PCMSK2 |= (1 << PCINT21);
  Serial.begin(9600);
}

ISR(PCINT2_vect) {
  Serial.print("Here\n");
  leftMotorTicks += 1;
}

void loop() {
  PORTD = 0b10000100; //pin 7 goes high, pin 6 goes low, pin 2 goes high
  delay(1000);
  PORTD = 0b01000000; //pin 7 goes low, pin 6 goes high, pin 2 goes low
  delay(1000);
}
