// Include necessary libraries
// Forward declaration
void smoothRotateMotor(int stepPin, int dirPin, bool reverse = false);

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h>
#include <HX711.h>  // Include HX711 library for weight sensor
#include <Keypad.h> // Include keypad library
#include <Adafruit_NeoPixel.h> // Include NeoPixel library


// Pins and configuration
const int DT_PIN = 38;
const int SCK_PIN = 40;

HX711 scale;
long units;

// DHT22 sensor configuration
#define DHTPIN 18          // Pin connected to the DHT22 sensor
#define DHTTYPE DHT22      // DHT22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4

#define PIN            27
#define NUMPIXELS      16
#define SPEEDDELAY     10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int STPPin1 = 3;   
const int DIR_Pin1 = 2; 

const int STPPin2 = 23;  
const int DIR_Pin2 = 25; 

const int STPPin3 = 51;  
const int DIR_Pin3= 53; 
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Ultrasonic Sensor Pins
#define TRIG_PIN1 11
#define ECHO_PIN1 10
#define TRIG_PIN2 9
#define ECHO_PIN2 8


#define MOTOR1_PIN 7
#define MOTOR2_PIN 6
#define MOTOR3_PIN 5
#define MOTOR4_PIN 4

#define MOTOR5_PIN 47
#define MOTOR6_PIN 49

#define MOTOR7_PIN 50
#define MOTOR8_PIN 52

#define MOTOR9_PIN 48
#define MOTOR10_PIN 46

#define MOTOR11_PIN 44
#define MOTO12_PIN 42

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {45, 43, 41, 39}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {37, 35, 33, 31}; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int buzzerPin = 17;     
float getWeightInKg();
void setup() {
  lcd.begin(LCD_COLUMNS, LCD_LINES);
  lcd.backlight();
  lcd.print("Select option:");
  lcd.setCursor(0, 1);
  lcd.print("1. Start Process");
  lcd.setCursor(0, 2);
  lcd.print("2. Help");
  lcd.setCursor(5, 3);
  lcd.print("GROUP 05");

  pinMode(STPPin1, OUTPUT);
  pinMode(DIR_Pin1, OUTPUT);

  pinMode(STPPin2, OUTPUT);
  pinMode(DIR_Pin2, OUTPUT);

  pinMode(MOTOR1_PIN, OUTPUT);
  pinMode(MOTOR2_PIN, OUTPUT);
  pinMode(MOTOR3_PIN, OUTPUT);
  pinMode(MOTOR4_PIN, OUTPUT);

  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  pinMode(buzzerPin, OUTPUT);

  // Initialize NeoPixel
  pixels.begin();

  dht.begin(); // Initialize DHT sensor
  // Initialize the weight sensor
scale.begin(DT_PIN,SCK_PIN);
 
  

}
void loop() {
  char key = keypad.getKey();

  if (key) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Key Pressed: ");
    lcd.print(key);

    switch (key) {
      case '1':
        startMotorOperation();
        break;
      case '2':
        displayHelp();
        break;
      default:
        lcd.setCursor(0, 1);
        lcd.print("Invalid Option");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Select option:");
        lcd.setCursor(0, 1);
        lcd.print("1. Start Process");
        lcd.setCursor(0, 2);
        lcd.print("2. Help");
        break;
    }
  }
}

void startMotorOperation() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting Process...");

  bool overflowDetected = false;

  while (true) {
    float distance1 = measureDistance(TRIG_PIN1, ECHO_PIN1);

    while (distance1 > 3) {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Motor 1: ON ");
      lcd.setCursor(0, 2);
      lcd.print("Filling Grain");
      digitalWrite(MOTOR1_PIN, HIGH);  
      delay(100);
      digitalWrite(MOTOR1_PIN, LOW); 
      distance1 = measureDistance(TRIG_PIN1, ECHO_PIN1);
      lcd.setCursor(0, 3);
      lcd.print(distance1);
      delay(100);
    } 

    digitalWrite(MOTOR2_PIN, LOW);  // Stop Motor 1 when distance1 <= 3 cm
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Motor 1: OFF");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("Overflow Detected");
    delay(2000);
    
    lcd.clear();
    lcd.setCursor(0, 2);
    lcd.print("Open Valve?");
    lcd.setCursor(0, 3);
    lcd.print("1. Yes  2. No");

    while (true) {
      char valveKey = keypad.getKey();

      if (valveKey == '1') {
        float distance2 = measureDistance(TRIG_PIN2, ECHO_PIN2);

        if (distance2 < 3) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Grain Detected");
          lcd.setCursor(0, 1);
          lcd.print("Start M2 Motor?");
          lcd.setCursor(0, 2);
          lcd.print("1. Yes  2. No");

          while (true) {
            char confirmKey = keypad.getKey();

            if (confirmKey == '1') {
              // Motor 2 Operation when distance2 < 3
              lcd.clear();
              lcd.print("Valve opening");
              smoothRotateMotor(STPPin1, DIR_Pin1, false);

              // Continuously operate Motor 2 while monitoring distance2
              while (distance2 < 3) {
                lcd.setCursor(0, 1);
                lcd.print("Motor 2: ON ");
                lcd.setCursor(0, 2);
                lcd.print("Grain Moving");

                digitalWrite(MOTOR4_PIN, HIGH);
                delay(10);
                digitalWrite(MOTOR4_PIN, LOW);
                distance2 = measureDistance(TRIG_PIN2, ECHO_PIN2);
                lcd.setCursor(0, 3);
                lcd.print(distance2);
              }

              // Stop Motor 2 and close the valve
              lcd.clear();
              lcd.setCursor(0, 1);
              lcd.print("Valve closing");
              smoothRotateMotor(STPPin1, DIR_Pin1, true); // Reverse rotation
              digitalWrite(MOTOR4_PIN, LOW);
              lcd.setCursor(0, 2);
              lcd.print("Grains moved");
              lcd.setCursor(0, 3);
              lcd.print("To Steam");

              checkTemperature();
              break;  // Exit the loop after handling Motor 2
            } else if (confirmKey == '2') {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Rechecking Grain");
              delay(1000);
              lcd.clear();
              overflowDetected = false; // Continue to process after rechecking
              break;
            }
          }
        }
      } else if (valveKey == '2') {
        return;  // Exit the function if "No" is selected
      }
    }

    if (overflowDetected) {
      while (measureDistance(TRIG_PIN2, ECHO_PIN2) <= 3) {
        lcd.setCursor(0, 1);
        lcd.print("Motor 3: ON ");
        digitalWrite(MOTOR3_PIN, HIGH);
        delay(100);
        digitalWrite(MOTOR3_PIN, LOW);
        delay(100);
      }

      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Motor 3: OFF");
      digitalWrite(MOTOR3_PIN, LOW);
      overflowDetected = false;  // Reset overflow flag
    }

    delay(500);  // Small delay to avoid excessive updates
  }
}


float measureDistance(int trigPin, int echoPin) {
  // Send a us pulse to trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the pulse width of the echo
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm
  float distance = (duration / 2.0) * 0.0344;
  return distance;
}
void checkTemperature() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Temp:");

  // Wait for temperature input from the user (assuming it's entered via keypad)
  String enteredTempStr = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        enteredTempStr += key;
        lcd.setCursor(12, 0);
        lcd.print(enteredTempStr);
      } else if (key == '#') {
        break; // Assuming '#' is used to confirm the entered temperature
      } else if (key == '*') {
        enteredTempStr = ""; // Clear the input if '*' is pressed
        lcd.setCursor(12, 0);
        lcd.print("    "); // Clear display area
        lcd.setCursor(12, 0);
      }
    }
  }

  float enteredTemp = enteredTempStr.toFloat();
  float sensorTemp = dht.readTemperature();

  if (enteredTemp > sensorTemp) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp OK, Steaming");
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");
    delay(5000); // Wait for 5 seconds

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Start Drying?");
    lcd.setCursor(0, 1);
    lcd.print("1. Yes  2. No");

    while (true) {
      char confirmKey = keypad.getKey();
      if (confirmKey) {
        if (confirmKey == '1') {
          float t = dht.readTemperature();
          if (t > 40) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("High Temp!");
            lcd.setCursor(0, 1);
            lcd.print("Cooling Grains...");
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enter Fan Speed:");
            delay(1000);

            // Wait for fan speed input from the user (assuming it's entered via keypad)
            String inputString = "";  // To store the input string
            while (true) {
              char key = keypad.getKey();
              if (key) {
                if (key >= '0' && key <= '9') {
                  inputString += key;
                  lcd.setCursor(12, 1);
                  lcd.print(inputString);
                } else if (key == '#') { // Assume '#' is used to submit the number
                  int speed = inputString.toInt();  // Convert string to integer
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Fan Speed:");
                  lcd.setCursor(0, 1);
                  lcd.print(speed);
                  
                  // Function call to control fan speed
                  fanSpeed(STPPin2, DIR_Pin2, false, speed);
                  updateRGBCircle(10000); // Call to update RGB circle (assumed function)
                  break;
                } else if (key == '*') { // Clear input
                  inputString = "";
                  lcd.setCursor(12, 1);
                  lcd.print("    "); // Clear display area
                  lcd.setCursor(12, 1);
                } else {
                  lcd.setCursor(0, 1);
                  lcd.print("Invalid input");
                }
              }
            }
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Drying Complete");
             fillStorage();
            break;
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Temp Normal");
            lcd.setCursor(0, 1);
            lcd.print("Drying Complete");
           break;
            
          }
        } else if (confirmKey == '2') {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Process Stopped");
          break;
        }
      }
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Higher Temp!");
    tone(buzzerPin, 262, 500); // Buzzer sound
    delay(500); // Buzzer on duration
    noTone(buzzerPin); // Stop buzzer
    delay(500); // Buzzer off duration
    tone(buzzerPin, 262, 500); // Buzzer sound
    delay(500); // Buzzer off duration
    tone(buzzerPin, 262, 500); // Buzzer sound
    delay(500); // Buzzer off duration
    checkTemperature(); // Re-check temperature
  }
  
  delay(2000); // Additional delay to avoid rapid changes
}


void fillStorage() {
  for (int i = 1; i <= 100; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Filling Storage");

    // Display the count (from 1 to 100)
    lcd.setCursor(0, 1);
    lcd.print("Packets: ");
    lcd.print(i);

    // Motor rotation logic for one full rotation
    digitalWrite(MOTOR5_PIN, HIGH); // Motor ON
    delay(5); // Adjust this delay according to the time required for one full rotation
    digitalWrite(MOTOR5_PIN, LOW);  // Motor OFF
       
   
  }
   delay(100); // Short delay before the next count update

  // After the loop, display the final message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("100 Packets");
  lcd.setCursor(0, 1);
  lcd.print("KG Finished");

  digitalWrite(MOTOR5_PIN, LOW); 
  fillPackets();
}
void checkWeight() {
  float currentWeight = getWeightInKg();
  
  

  while (currentWeight <= 63.08) {
    
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Weight: ");
  lcd.print(currentWeight, 2); // Display weight with 2 decimal places
  lcd.print(" kg");
      lcd.setCursor(0, 3);
    lcd.print("Filling packets...");
    openValve(STPPin3, DIR_Pin3, false);
     fillPackets();
    currentWeight = getWeightInKg();
    delay(500); // Add delay to avoid rapid updates
  }

  lcd.clear();
  lcd.setCursor(0,0);
   lcd.print(currentWeight);
  lcd.setCursor(0, 1);
  lcd.print("Packet Filled");
  lcd.setCursor(0, 2);
  lcd.print("Motor6: ON");
  for (int i = 0; i < 300; i++) {
    digitalWrite(MOTOR9_PIN, HIGH);
    delay(5);
    digitalWrite(MOTOR9_PIN, LOW);
    delay(5);
    lcd.setCursor(0, 3);
    lcd.print("Storing....");
    digitalWrite(MOTOR11_PIN, HIGH);
    delay(5);
    digitalWrite(MOTOR11_PIN, LOW);
    delay(5);
    
  }
   
  fillPackets();

}


 

void displayHelp() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Help Menu:");
  lcd.setCursor(0, 1);
  lcd.print("1. Start - Run process");
  lcd.setCursor(0, 2);
  lcd.print("2. Help - Show this");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select option:");
  lcd.setCursor(0, 1);
  lcd.print("1. Start Process");
  lcd.setCursor(0, 2);
  lcd.print("2. Help");
}

void smoothRotateMotor(int stepPin, int dirPin, bool reverse) {
  digitalWrite(dirPin, reverse ? LOW : HIGH);
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
}

float getWeightInKg() {
  // Assuming you have already initialized the scale in the setup() function
  // Call tare() in the setup() to reset the scale to 0 before weighing
  
  // Get the weight in grams
  float weight = scale.get_units(); // Read 5 times for more accurate measurement
  
  // Convert grams to kilograms
  float weightInKg = weight*0.0751;



  return weightInKg;
}


void openValve(int stepPin, int dirPin, bool reverse){

    digitalWrite(dirPin, reverse ? LOW : HIGH);
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(6000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(6000);
}
}
void fillPackets() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moving Empty Packets");
  int i = 0;
  while (i < 100) {
    digitalWrite(MOTOR7_PIN, HIGH); // Motor ON
    delay(100); // Adjust this delay according to the time required for one full rotation
    digitalWrite(MOTOR7_PIN, LOW);
    i++;
  }
 

   checkWeight();
  



}

void fanSpeed(int stepPin, int dirPin, bool reverse, int speed) {
  digitalWrite(dirPin, reverse ? LOW : HIGH);
  
  int delayTime = map(speed, 0, 100, 2000, 200); // Map speed (0-100) to delay (2000µs to 200µs)
  
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayTime);
  }
}

void updateRGBCircle(int position) {
uint16_t led, pos;

  for(pos = 256; pos > 0; pos--) {
    for(led = 0; led < NUMPIXELS; led++) {
      int brightness = Wheel(((led * 768 / NUMPIXELS) + pos) & 255);
      pixels.setPixelColor(led, 0.5 * brightness, 0, brightness);
    }
    pixels.show();
    delay(SPEEDDELAY);
    
}
}
int Wheel(byte WheelPos) {
  int brightness;
 
  if(WheelPos < 85) {
   brightness = 0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   brightness = WheelPos * 3;
  } else {
   WheelPos -= 170;
   brightness = 255 - WheelPos * 3;
  }
  return brightness;
}
