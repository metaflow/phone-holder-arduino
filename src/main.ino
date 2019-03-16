#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  120 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  530 // this is the 'maximum' pulse length count (out of 4096)


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const String rotate_x_command = "rx";
const String rotate_y_command = "ry";
int rotation_x_target = 0;
int rotation_x = 0;
int rotation_y_target = 0;
int rotation_y = 0;

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
  rotation_y_target = 40;
  rotation_y = 0;
  rotation_x_target = 0;
  rotation_x = 1;
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
    bt_buffer = bt_buffer.substring(0, bt_buffer.length()-2);
    lcd_line(bt_buffer);
    process_command(bt_buffer);
    Serial.print(bt_buffer + "\r\n");
    bt_buffer = "";
  }
}

void process_command(String c) {
  if (c.startsWith(rotate_x_command)) {
    rotation_x_target = c.substring(rotate_x_command.length() + 1).toInt();
  }
  if (c.startsWith(rotate_y_command)) {
    rotation_y_target = c.substring(rotate_y_command.length() + 1).toInt();
  }
}

double deg = 0;


void servo_loop() {
  if (rotation_x != rotation_x_target) {
    int step = 1;
    if (rotation_x_target < rotation_x) step *= -1;
    set_x_rotation(rotation_x + step);
  }
  if (rotation_y != rotation_y_target) {
    int step = 1;
    if (rotation_y_target < rotation_y) step *= -1;
    set_y_rotation(rotation_y + step);
  }
}

void set_x_rotation(int x) {
  rotation_x = constrain(x, -90, 90);
  // lcd_line(String(angle));
  int angle = rotation_x + 90;
  int w = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(0, 0, w);
}

void set_y_rotation(double y) {
  rotation_y = constrain(y, 10, 60);
  lcd_line(String(rotation_y));
  int angle = 180 - rotation_y;
  int w = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  lcd_line(String(w));
  pwm.setPWM(1, 0, w);
}

void loop() {
  bt_loop();
  servo_loop();
  delay(20);
}
