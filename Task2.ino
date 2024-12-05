// Include the LiquidCrystal library for LCD functions
#include <LiquidCrystal.h>

#define Vertical A0


#define ena 11 // PWM output for motor speed control
#define n1 10  // Motor direction control pin 1
#define n2 9   // Motor direction control pin 2

// Enable pins in the motor drive
#define encoderA 2
#define encoderB 3

// Define LCD pin assignments
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 12, d7 = 13;

// Create an LCD object with the specified pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int vert_nutral; 
int pos = 0;     

// Interrupt service routine for the encoder
void readEncoderB() {
  int b = digitalRead(encoderB);
  if (b > 0) { // Clockwise rotation
    pos++;
  } else {     // Counter-clockwise rotation
    pos--;
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize the LCD with 16 columns and 2 rows
  lcd.begin(16, 2);

  vert_nutral = analogRead(Vertical);

  pinMode(ena, OUTPUT);
  pinMode(n1, OUTPUT);
  pinMode(n2, OUTPUT);

  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);

  digitalWrite(n1, LOW);
  digitalWrite(n2, LOW);

  attachInterrupt(digitalPinToInterrupt(encoderA), readEncoderB, RISING);
}

void loop() {
  int vert = analogRead(Vertical);

  // forward
  if (vert < 400) { 
    digitalWrite(n1, HIGH);
    digitalWrite(n2, LOW);
    analogWrite(ena, map(vert, 0, 400, 255, 0));
  } 
  // backward
  else if (vert > 600) { 
    digitalWrite(n1, LOW);
    digitalWrite(n2, HIGH);
    analogWrite(ena, map(vert, 600, 1024, 0, 255)); 
  } 
  // Stop
  else { 
    digitalWrite(n1, LOW);
    digitalWrite(n2, LOW);
  }

  // Calculate degrees from encoder position
  float degrees = pos * 1.7306; 

  lcd.clear();        
  lcd.setCursor(0, 0); 

  // if (degrees < 0) {
  //   lcd.print("CCW: ");
  //   lcd.print(-degrees*3, 2); 
  // } else {
  //   lcd.print("CW:  ");
  //   lcd.print(degrees*3, 2); 
  // }
  lcd.print(degrees*3,2);

  Serial.println(degrees);

  delay(200); 
}