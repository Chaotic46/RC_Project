#include <Wire.h>

#define MPU_SLAVE_ADDR 0x68
#define MPU_PWR_MGMT_1 0x6B
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define GYRO_SENSITIVITY 131    // LSB/degree/s - see section 4.19 of MPU-6050 register manual
#define ACCEL_SENSITIVITY 16384 // LSB/g - see section 4.17 of MPU-6050 register manual

int32_t numMeasurements;

float accelX;
float accelY;
float accelZ;

float gyroX;
float gyroY;
float gyroZ;

int8_t timerIntCount;

void setup() {
  // Setup I2C and serial
  Serial.begin(9600);
  Wire.begin();

  // Wake I2C up
  Wire.beginTransmission(MPU_SLAVE_ADDR);
  Wire.write(MPU_PWR_MGMT_1);
  Wire.write(0b00000000);
  Wire.endTransmission();

  // Gyroscope configuration
  Wire.beginTransmission(MPU_SLAVE_ADDR);
  Wire.write(GYRO_CONFIG);
  Wire.write(0b00000000);
  Wire.endTransmission();

  // Acceleration configuration
  Wire.beginTransmission(MPU_SLAVE_ADDR);
  Wire.write(ACCEL_CONFIG);
  Wire.write(0b00000000);
  Wire.endTransmission();

  Serial.println("Program ready for execution, type any character to begin");
  while(Serial.available() && Serial.read());
  while(!Serial.available());
  while(Serial.available() && Serial.read());
  
  // Initialize global variables
  accelX = 0;
  accelY = 0;
  accelZ = 0;
  gyroX = 0;
  gyroY = 0;
  gyroZ = 0;
  timerIntCount = 0;

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
  OCR1A = ( 16000000 / (1024 * 1) ) - 1;
  TCNT1 = 0;
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  timerIntCount++;
  if (timerIntCount == 10) {
    Serial.println("Offset calculation sequence completed.");
    Serial.print("accelX offset: ");
    Serial.print(accelX / ((float) numMeasurements), 4);
    Serial.print(" ");
    Serial.print("accelY offset: ");
    Serial.print(accelY / ((float) numMeasurements), 4);
    Serial.print(" ");
    Serial.print("accelZ offset: ");
    Serial.print(accelZ / ((float) numMeasurements), 4);
    Serial.print(" ");
    Serial.print("gyroX offset: ");
    Serial.print(gyroX / (float) numMeasurements, 4);
    Serial.print(" ");
    Serial.print("gyroY offset: ");
    Serial.print(gyroY / (float) numMeasurements, 4);
    Serial.print(" ");
    Serial.print("gyroZ offset: ");
    Serial.print(gyroZ / (float) numMeasurements, 4);
    Serial.println();
    //while(1);
    cli();
  }
}

void recordAccelRegisters() {
  Wire.beginTransmission(MPU_SLAVE_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU_SLAVE_ADDR, 6);  
  while(Wire.available() < 6);
  accelX += (( Wire.read() << 8 ) | Wire.read()) / ACCEL_SENSITIVITY;
  accelY += (( Wire.read() << 8 ) | Wire.read()) / ACCEL_SENSITIVITY;
  accelZ += (( Wire.read() << 8 ) | Wire.read()) / ACCEL_SENSITIVITY;
  /*Serial.print("accelX: ");
  Serial.print(accelX, 4);
  Serial.print(" ");
  Serial.print("accelY: ");
  Serial.print(accelY, 4);
  Serial.print(" ");
  Serial.print("accelZ: ");
  Serial.print(accelZ, 4);
  Serial.print(" ");*/
}

void recordGyroRegisters() {
  Wire.beginTransmission(MPU_SLAVE_ADDR);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(MPU_SLAVE_ADDR, 6);  
  while(Wire.available() < 6);
  gyroX += (( Wire.read() << 8 ) | Wire.read()) / GYRO_SENSITIVITY;
  gyroY += (( Wire.read() << 8 ) | Wire.read()) / GYRO_SENSITIVITY;
  gyroZ += (( Wire.read() << 8 ) | Wire.read()) / GYRO_SENSITIVITY;
  /*Serial.print("gyroX: ");
  Serial.print(gyroX, 3);
  Serial.print(" ");
  Serial.print("gyroY: ");
  Serial.print(gyroY, 3);
  Serial.print(" ");
  Serial.print("gyroZ: ");
  Serial.print(gyroZ, 3);
  Serial.print(" ");
  Serial.println();*/
}

void loop() {
  // put your main code here, to run repeatedly:
  numMeasurements++;
  recordAccelRegisters();
  recordGyroRegisters();
}
