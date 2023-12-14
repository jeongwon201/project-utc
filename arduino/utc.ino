#include <Adafruit_ILI9341.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_GFX.h>

#define PIN_CS D3                   // chip select for the display
#define PIN_DC D4                    // d/c pin for the display
#define PIN_TRIG D0
#define PIN_ECHO D8

// fire up the display using a very fast driver
Adafruit_ILI9341 Display = Adafruit_ILI9341(PIN_CS, PIN_DC);

// create some colors for the keypad buttons
#define C_BLUE Display.color565(0,0,255)
#define C_RED Display.color565(255,0,0)
#define C_GREEN Display.color565(0,255,0)
#define C_WHITE Display.color565(255,255,255)
#define C_BLACK Display.color565(0,0,0)
#define C_LTGREY Display.color565(200,200,200)
#define C_DKGREY Display.color565(80,80,80)
#define C_GREY Display.color565(127,127,127)

unsigned long CurTime;

uint16_t TheColor;
// start with some initial colors
uint16_t MinTemp = 20;
uint16_t MaxTemp = 33;

// variables for interpolated colors
byte red, green, blue;

// variables for row/column interpolation
byte i, j, k, row, col, incr;
float intPoint, val, a, b, c, d, ii;
byte aLow, aHigh;

// size of a display "pixel"
byte BoxWidth = 3;
byte BoxHeight = 3;

uint16_t PixelColor;

int x, y;
char buf[20];

// variable to toggle the display grid
int ShowGrid = -1;
int DefaultTemp = -1;

// array for the 8 x 8 measured pixels
float pixels[64];

// array for the interpolated array
float HDTemp[80][80];

// create the camara object
Adafruit_AMG88xx ThermalSensor;

float forehead_t, nose_t, cheek_t, chin_t, neck_t;
float forehead_s, nose_s, cheek_s, chin_s, neck_s;
float avg_t, avg_wrist;

float duration, distance;

int state = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  Display.begin();
  Display.fillScreen(C_BLACK);
  Display.setRotation(3);

  bool status = ThermalSensor.begin();
  delay(100);

  if (!status) {
    while (1) {
      Display.setCursor(20, 150);
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("Sensor: FAIL");
      delay(500);
      Display.setCursor(20, 150);
      Display.setTextColor(C_BLACK, C_BLACK);
      Display.print("Sensor: FAIL");
      delay(500);
    }
  }
  else {
    Display.setCursor(20, 150);
    Display.setTextColor(C_GREEN, C_BLACK);
    Display.print("Sensor: FOUND");
  }

  ThermalSensor.readPixels(pixels);
  if (pixels[0] < 0) {
    while (1) {
      Serial.println(pixels[0]);
      Display.setCursor(20, 180);
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("Readings: FAIL");
      delay(500);
      Display.setCursor(20, 180);
      Display.setTextColor(C_BLACK, C_BLACK);
      Display.print("Readings: FAIL");
      delay(500);
    }
  }
  else {
    Display.setCursor(20, 180);
    Display.setTextColor(C_GREEN, C_BLACK);
    Display.print("Readings: OK");
    delay(2000);
  }

  Display.fillScreen(C_BLACK);
  Display.fillRect(10, 10, 220, 220, C_WHITE);
  Display.setRotation(2);
}
void loop() {
  int st = distanceMethod();
  ThermalSensor.readPixels(pixels);

  forehead_t = ( pixels[10] + pixels[11] + pixels[12] + pixels[13] + pixels[14] + pixels[15] +
               pixels[18] + pixels[19] + pixels[20] + pixels[21] + pixels[22] + pixels[23] + (7.5 * 12)) / 12;
  cheek_t = ( pixels[26] + pixels[27] + pixels[30] + pixels[31] + pixels[34] + pixels[35] + 
              pixels[38] + pixels[39] + (4.5 * 8)) / 8;
  nose_t = ( pixels[28] + pixels[29] + pixels[36] + pixels[37] + (4.8 * 4)) / 4;
  chin_t = ( pixels[42] + pixels[43] + pixels[44] + pixels[45] + pixels[46] + pixels[47] +
           pixels[17] + pixels[18] + pixels[19] + pixels[20] + pixels[21] + pixels[22] + (4.5 * 12)) / 12;
  neck_t = ( pixels[58] + pixels[59] + pixels[60] + pixels[61] + pixels[62] + pixels[63] + (10 * 6)) / 6;

  forehead_s = forehead_t * 0.4;
  cheek_s = cheek_t * 0.05;
  nose_s = nose_t * 0.05;
  chin_s = chin_t * 0.2;
  neck_s = neck_t * 0.3;
  
  avg_t = forehead_s + cheek_s + nose_s + chin_s + neck_s;
  avg_wrist = ( pixels[11] + pixels[12] + pixels[13] + pixels[14] +
                pixels[19] + pixels[20] + pixels[21] + pixels[22] +
                pixels[27] + pixels[28] + pixels[29] + pixels[30] +
                pixels[35] + pixels[36] + pixels[37] + pixels[38] +
                pixels[43] + pixels[44] + pixels[45] + pixels[46] +
                pixels[51] + pixels[52] + pixels[53] + pixels[54] +
                pixels[59] + pixels[60] + pixels[61] + pixels[62] + (15 * 28)) / 28;

  Serial.print("TEMP : ");
  Serial.print(forehead_t); Serial.print(", ");
  Serial.print(cheek_t); Serial.print(", ");
  Serial.print(nose_t); Serial.print(", ");
  Serial.print(chin_t); Serial.print(", ");
  Serial.println(neck_t);

  Serial.print("AVG : ");
  Serial.print(forehead_s); Serial.print(", ");
  Serial.print(cheek_s); Serial.print(", ");
  Serial.print(nose_s); Serial.print(", ");
  Serial.print(chin_s); Serial.print(", ");
  Serial.println(neck_s);
  
  if(forehead_t < 38 && cheek_t < 38 && nose_t < 38 && chin_t < 38) {
    if(avg_t < 37.5) {
      Serial.println("통과");
    } else {
      Serial.println("발열 의심");
    }
  } else {
    Serial.println("고열");
  }
  
  for (row = 0; row < 8; row ++) {
    for (col = 0; col < 70; col ++) {
      aLow =  col / 10 + (row * 8);
      aHigh = (col / 10) + 1 + (row * 8);
      intPoint =   (( pixels[aHigh] - pixels[aLow] ) / 10.0 );
      incr = col % 10;
      val = (intPoint * incr ) +  pixels[aLow];
      HDTemp[ (7 - row) * 10][col] = val;
    }
  }
  for (col = 0; col < 70; col ++) {
    for (row = 0; row < 70; row ++) {
      aLow =  (row / 10 ) * 10;
      aHigh = aLow + 10;
      intPoint =   (( HDTemp[aHigh][col] - HDTemp[aLow][col] ) / 10.0 );
      incr = row % 10;
      val = (intPoint * incr ) +  HDTemp[aLow][col];
      HDTemp[ row ][col] = val;
    }
  }
  for (row = 0; row < 70; row ++) {
    if (ShowGrid < 0) {
      BoxWidth = 3;
    }
    else {
      if ((row % 10 == 9) ) {
        BoxWidth = 2;
      }
      else {
        BoxWidth = 3;
      }
    }
    for (col = 0; col < 70; col++) {
      if (ShowGrid < 0) {
        BoxHeight = 3;
      }
      else {
        if ( (col % 10 == 9)) {
          BoxHeight = 2;
        }
        else {
          BoxHeight = 3;
        }
      }
      val = HDTemp[ row ][col];
      red = constrain(255.0 / (c - b) * val - ((b * 255.0) / (c - b)), 0, 255);

      if ((val > MinTemp) & (val < a)) {
        green = constrain(255.0 / (a - MinTemp) * val - (255.0 * MinTemp) / (a - MinTemp), 0, 255);
      }
      else if ((val >= a) & (val <= c)) {
        green = 255;
      }
      else if (val > c) {
        green = constrain(255.0 / (c - d) * val - (d * 255.0) / (c - d), 0, 255);
      }
      else if ((val > d) | (val < a)) {
        green = 0;
      }

      if (val <= b) {
        blue = constrain(255.0 / (a - b) * val - (255.0 * b) / (a - b), 0, 255);
      }
      else if ((val > b) & (val <= d)) {
        blue = 0;
      }
      else if (val > d) {
        blue = constrain(240.0 / (MaxTemp - d) * val - (d * 240.0) / (MaxTemp - d), 0, 240);
      }
  
      TheColor = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
      Display.fillRect((row * 3) + 15, (col * 3) + 15, BoxWidth, BoxHeight, TheColor);
    }
  }
  SetTempScale();

  if(avg_t >= 37.5 && st == 1) state=1;
  
  if(state == 0) {
    Display.drawRoundRect(60, 20, 120, 180, 100, ILI9341_WHITE);
  } else {
    Display.drawRect(65, 10, 110, 220, ILI9341_WHITE);
    if(st == 4) {
      state = 0;
    }
  }
  resultText(st, avg_t, avg_wrist);
}

void SetTempScale() {
  if (DefaultTemp < 0) {
    MinTemp = 20;
    MaxTemp = 33;
    Getabcd();
  }
  else {

    val = 0.0;
    for (i = 0; i < 64; i++) {
      val = val + pixels[i];
    }
    val = val / 64.0;

    MaxTemp = val + 2.0;
    MinTemp = val - 2.0;
    Getabcd();
  }
}

void Getabcd() {
  a = MinTemp + (MaxTemp - MinTemp) * 0.2121;
  b = MinTemp + (MaxTemp - MinTemp) * 0.3182;
  c = MinTemp + (MaxTemp - MinTemp) * 0.4242;
  d = MinTemp + (MaxTemp - MinTemp) * 0.8182;

}

unsigned long resultText(int st, float temp, float temp_wrist) {
  unsigned long start = micros();
  Display.fillRect(0, 250, 240, 200, C_BLACK);
  if(state == 0) {
    if(st == 1) {
      Display.setCursor(50, 250);
      if(temp < 37.5) {
        Display.setTextColor(C_GREEN);
        Display.setTextSize(3);
        Display.println(temp);
        Display.println("   PASS");
        delay(1000);
      } else {
        Display.setTextColor(ILI9341_RED);
        Display.setTextSize(3);
        Display.println(temp);
        Display.println("   NON-PASS");
        delay(1000);
      }
    } else if(st == 2) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_RED);
      Display.setTextSize(2);
      Display.println("Go back a little");
      Display.setCursor(10, 280);
      Display.println("more");
    } else if(st == 3) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_RED);
      Display.setTextSize(2);
      Display.println("Come a little more");
      Display.setCursor(10, 280);
      Display.println("forward");
    } else if(st == 4) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_WHITE);
      Display.setTextSize(2);
      Display.println("Not Detected");
    }
  } else {
    if(st == 1) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_WHITE);
      Display.setTextSize(2);
      Display.println("Fit your wrist");
      Display.setCursor(10, 270);
      Display.println("to the guidelines");
      Display.setCursor(10, 290);
      if(temp < 37.5) {
        Display.setTextColor(C_GREEN);
        Display.setTextSize(2);
        Display.print(temp);
        Display.setCursor(100, 290);
        Display.println("PASS");
        delay(1000);
      } else {
        Display.setTextColor(C_RED);
        Display.setTextSize(2);
        Display.print(temp);
        Display.setCursor(100, 290);
        Display.println("NON-PASS");
        delay(1000);
      }
    } else if(st == 2) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_RED);
      Display.setTextSize(2);
      Display.println("Go back a little");
      Display.setCursor(10, 280);
      Display.println("more");
    } else if(st == 3) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_RED);
      Display.setTextSize(2);
      Display.println("Come a little more");
      Display.setCursor(10, 280);
      Display.println("forward");
    } else if(st == 4) {
      Display.setCursor(10, 250);
      Display.setTextColor(C_WHITE);
      Display.setTextSize(2);
      Display.println("Not Detected");
    }
  }
  
  return micros() - start;
}

int distanceMethod() {
  digitalWrite(PIN_TRIG,LOW);
  delayMicroseconds(3);
  digitalWrite(PIN_TRIG,HIGH);
  delayMicroseconds(12);
  duration = pulseIn(PIN_ECHO, HIGH);
  distance = (duration/2)/29.1;
  Serial.print("distance : ");
  Serial.print(distance);
  Serial.println("cm");
  if(distance <= 17 && distance >= 16){
    return 1;
  } else if(distance < 15) {
    return 2;
  } else if(distance > 17 && distance < 100) {
    return 3;
  } else {
    return 4;
  }
}
