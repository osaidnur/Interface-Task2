// Include the LiquidCrystal library for LCD functions
#include <LiquidCrystal.h>

#define Vertical A0


#define ena 11 // PWM output for motor speed control
#define n1 10  // Motor direction control pin 1
#define n2 9   // Motor direction control pin 2
int mpuAngle= 360;
// Enable pins in the motor drive
#define encoderA 2
#define encoderB 3
int kp = 15;

// Define LCD pin assignments
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 12, d7 = 13;

// Create an LCD object with the specified pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//-------------------------------
#include <Wire.h>
float RateRoll, RatePitch, RateYaw;
float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;
float LoopTimer;
void gyro_signals(void) {
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(); 
  Wire.requestFrom(0x68,6);
  int16_t AccXLSB = Wire.read() << 8 | Wire.read();
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();
  Wire.beginTransmission(0x68);
  Wire.write(0x1B); 
  Wire.write(0x8);
  Wire.endTransmission();                                                   
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t GyroX=Wire.read()<<8 | Wire.read();
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();
  RateRoll=(float)GyroX/65.5;
  RatePitch=(float)GyroY/65.5;
  RateYaw=(float)GyroZ/65.5;
  AccX=(float)AccXLSB/4096;
  AccY=(float)AccYLSB/4096;
  AccZ=(float)AccZLSB/4096;
  AngleRoll=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
  AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
  //AnglePitch = atan2(-AccX, sqrt(AccY * AccY + AccZ * AccZ)) * (180.0 / 3.141592653589793);

}
//-------------------------------

int vert_nutral; 
int pos = 0;     

void readEncoderB() {
  int b = digitalRead(encoderB);
  if (b > 0) {
    pos++;
  } else {     
    pos--;
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize the LCD with 16 columns and 2 rows
  lcd.begin(16, 2);

  pinMode(ena, OUTPUT);
  pinMode(n1, OUTPUT);
  pinMode(n2, OUTPUT);

  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);

  digitalWrite(n1, LOW);
  digitalWrite(n2, LOW);

  attachInterrupt(digitalPinToInterrupt(encoderA), readEncoderB, RISING);
  Wire.setClock(400000);
  Wire.begin();
  Wire.beginTransmission(0x68); 
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
}

void loop() {
  gyro_signals();
 
  // Calculate encoderAngle from encoder position
  Serial.print(" Pitch: ");
 
  Serial.println(ceil(AnglePitch));

mpuAngle = ceil(AnglePitch);

float encoderAngle = ((int)(pos * 1.54)) ;
Serial.println(encoderAngle);

int error = encoderAngle - mpuAngle; 

 if (error >= -15 && error <= 15) {
    digitalWrite(n1, LOW);
    digitalWrite(n2, LOW);  
    // Stop motor once target angle is reached
    Serial.println("Stopped!!!!!!!!!!!!!");
  }
  else if(error > 15){
      digitalWrite(n1, HIGH);
      digitalWrite(n2, LOW);
      analogWrite(ena, kp*abs(error));
  }else if (error < -15) {
      digitalWrite(n1, LOW);
      digitalWrite(n2, HIGH);
      analogWrite(ena, kp*abs(error));
  }

  
  delay(200); 
}