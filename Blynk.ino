#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
Servo myservo;

/* WIFI */
char ssid[] = "";
char pass[] = "";

/* PINS */

#define TRIG D3
#define ECHO D4
#define SOIL A0
#define RAIN D0
#define SMOKE D8
#define PUMP D5
#define DHTPIN D6

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,16,2);

BlynkTimer timer;

int plantHeight;
int soilValue;
int rainValue;
int smokeValue;
float temperature;
float humidity;

/*SMOKE*/
int getsmoke()
{
  int smoke = digitalRead(SMOKE);
  
  return smoke;

}


/* ULTRASONIC */

int getHeight()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  int distance = duration * 0.034 / 2;

  int height = 17;

  return height;
}

void sendData()
{

  plantHeight = getHeight();
  soilValue = analogRead(SOIL);
  rainValue = digitalRead(RAIN);
  smokeValue = getsmoke();

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  /* SERIAL MONITOR */

  Serial.println("------ SENSOR DATA ------");

  Serial.print("Height: ");
  Serial.println(plantHeight);

  Serial.print("Soil: ");
  Serial.println(soilValue);

  Serial.print("Rain: ");
  Serial.println(rainValue);

  Serial.print("Smoke: ");
  Serial.println(smokeValue);

  Serial.print("Temp: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.println("-------------------------");

  /* LCD DISPLAY */

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("H:");
  lcd.print(plantHeight);
  lcd.print(" S:");
  lcd.print(soilValue);

  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print(" H:");
  lcd.print(humidity);

  /* PUMP CONTROL */

  if(soilValue < 700)
  {
    digitalWrite(PUMP, LOW);
  }
  else
  {
    digitalWrite(PUMP, HIGH);
  }
/*
  SERVO
  myservo.write(180);
  delay(2000);

  myservo.write(0);
  delay(60000);
*/
  /* BLYNK */

  Blynk.virtualWrite(V0, plantHeight);
  Blynk.virtualWrite(V8, soilValue);
  Blynk.virtualWrite(V2, rainValue);
  Blynk.virtualWrite(V5, smokeValue);
  Blynk.virtualWrite(V9, temperature);
  Blynk.virtualWrite(V10, humidity);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RAIN, INPUT);
  pinMode(SMOKE, INPUT);
  pinMode(PUMP, OUTPUT);  
  pinMode(DHTPIN, INPUT);

  dht.begin();

  lcd.init();
  lcd.backlight();

  timer.setInterval(2000L, sendData);
}

void loop()
{
  Blynk.run();
  timer.run();
}
