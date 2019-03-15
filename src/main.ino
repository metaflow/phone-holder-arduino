#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  120 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  530 // this is the 'maximum' pulse length count (out of 4096)


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    while (!Serial) {
    }
     Wire.begin();

  lcd.begin(16, 2); // initialize the lcd

  lcd.setBacklight(1);
  lcd.clear();
  Serial.println("AT");
  lcd_line(String("AT"));

  pwm.begin();
  pwm.setPWMFreq(60);
  set_x_rotation(90);
  set_y_rotation(10);
}

int line = 0;

void lcd_line(const String& s) {
  lcd.setCursor(0, line);
  lcd.print((s + "               ").c_str());
  line = (line + 1) % 2;
}

String bt_buffer;

void bt_loop() {
  while (Serial.available()){
    bt_buffer += char(Serial.read());
  }  
  if (bt_buffer.endsWith(String("\r\n"))) {
    lcd_line(bt_buffer.substring(0, bt_buffer.length()-2));
    Serial.print(bt_buffer);
    bt_buffer = "";
  }
}

double deg = 0;
void servo_loop() {
  double d = deg;
  if (d > 180) d = 360 - d;
  set_x_rotation(d - 90);
  set_y_rotation(d);
  deg += 0.5;
  if (deg > 360) deg = 0;
}

void set_x_rotation(double angle) {
  angle = constrain(angle, -90, 90);
  lcd_line(String(angle));
  angle += 90;
  double w = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(0, 0, int(w));
}

void set_y_rotation(double angle) {
  angle = constrain(angle, 10, 60);
  lcd_line(String(angle));
  angle = 180 - angle;
  double w = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(1, 0, int(w));

}

void loop() {
  bt_loop();
  servo_loop();
  delay(20);
}
