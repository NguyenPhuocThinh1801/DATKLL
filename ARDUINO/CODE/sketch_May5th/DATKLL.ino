#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleStream.h>
#include <Wire.h>
#include "DHT.h"
#include "timer.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
WidgetLED led1(V5);
WidgetLED led2(V6);
WidgetLED led3(V7);

#define sensor A1
#define sign A2
#define DS_pin 4
#define STCP_pin 3
#define SHCP_pin 2
#define RED 7
#define YELLOW 6
#define GREEN 5
#define DHTPIN 8 // DHT11 data pin is connected to Arduino pin 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

///////////////////////////////////////
char temperature[] = "Temp = 00.0 C  ";
char humidity[] = "RH   = 00.0 %  ";
int gas_sign, TEM, HUM;;
int dad_value;
///////////////////////////////////////

BlynkTimer timer;
char auth[] = "8K56Y0Egg8iK0x18eELWW_P4iqPwb5Jd";
byte numbers[10] {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110};

// Đọc độ ẩm + nhiệt độ
void DHT_11()
{
  TEM = dht.readTemperature();
  HUM = dht.readHumidity();
  byte RH = HUM;
  byte Temp = TEM;
  if (isnan(RH) || isnan(Temp))
  {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Error");
    return;
  }
  temperature[7] = Temp / 10 + 48;
  temperature[8] = Temp % 10 + 48;
  temperature[11] = 223;
  humidity[7] = RH / 10 + 48;
  humidity[8] = RH % 10 + 48;
}

/// Đèn gas
void Gas_led(int x){
  switch(x)
  {
    case 2:
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, LOW);
      break;
    case 1:
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(GREEN, LOW);
      break;
    case 0:
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, HIGH);
      break;
  }
}

/// Lấy giá trị của gas để tìm tín hiệu đèn
void GAS()
{
  int value = analogRead(sensor);
  value = map(value, 0, 1023, 0, 100);
  if (value >= 60)
    gas_sign = 2;
  else if(value >= 30 && value < 60)
    gas_sign = 1;
  else
    gas_sign = 0;
}

// Đọc độ ẩm đất
void do_am_dat()
{
  int value = analogRead(sign);
  value = map(value, 0, 1023, 0, 100);
  if(value > 70)
  {
    analogWrite(9, 0);
    analogWrite(10, 0);
  }
  else{
    analogWrite(9, 150);
    analogWrite(10, 0);
  }
  dad_value = value;
}

// Hiện thị độ ẩm đất ra đèn led 7 đoạn
void display_digit(int num){
  int a = num/10;
  int b = num%10;
  digitalWrite(STCP_pin,LOW);
  shiftOut(DS_pin, SHCP_pin, LSBFIRST, numbers[a]);
  shiftOut(DS_pin, SHCP_pin, LSBFIRST, numbers[b]);
  digitalWrite(STCP_pin, HIGH);
}

// Display lcd + led + 7-seg
void display_lcd()
{
  lcd.setCursor(0, 0);
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print(humidity);
  Gas_led(gas_sign);
  display_digit(dad_value);
}

// Chạy hết chương trình
void run_now(){
  DHT_11();
  delay(10);
  GAS();
  delay(10);
  do_am_dat();
  delay(10);
  display_lcd();
}

void sendToBlynk()
{
  Blynk.virtualWrite(V1,TEM);
  Blynk.virtualWrite(V2,HUM);
  Blynk.virtualWrite(V3,dad_value);
  if(gas_sign == 2)
  {
    led1.on();
    led2.off();
    led3.off();
  }
  else if(gas_sign == 1)
  {
    led1.off();
    led2.on();
    led3.off();
  }
  else
  {
    led1.off();
    led2.off();
    led3.on();
  }
  setTimer0(5000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Blynk.begin(Serial,auth);
  lcd.init();
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(sensor, INPUT);
  pinMode(sign, INPUT);
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
  dht.begin();
  timer.setInterval(1000L, sendToBlynk);
}

void loop()
{
    run_now();
    Blynk.run();
    timer.run();
    setTimer0(10000);
}
