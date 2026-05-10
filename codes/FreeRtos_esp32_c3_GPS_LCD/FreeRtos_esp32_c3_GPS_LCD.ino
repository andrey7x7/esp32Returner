#include <TinyGPSPlus.h>
#include <iarduino_GPS_NMEA.h>  
#include "GyverButton.h"
#include "WiFi.h"
#include "BluetoothSerial.h"
#include <oledDisplay.h>

OledDisplay oledDisp;

float HOME_LAT = 54.973861, HOME_LON = 73.376773;

#define POW_PIN 0
#define BTN_PIN 10
GButton butt1(BTN_PIN);  

// Функция для перевода градусов в радианы
#define DEG_TO_RAD(degrees) ((degrees * M_PI) / 180.0)

#define LED_BUILTIN 8 // Only one LED onboard

#define TIME_ZONE 6
iarduino_GPS_NMEA gps;

void TaskBtn(void *pvParameters);
void TaskLCD(void *pvParameters);
void TaskGPS(void *pvParameters);

String sats="0/0";
String dateTimeGPS="0 / 0";
double latitude;
double longitude;
uint16_t altitude;
uint16_t speed;
uint16_t course;
float new_x2;
float new_y2;

uint8_t menu=0;

void setup() {
    WiFi.mode(WIFI_OFF);
    btStop();
    // инициализация COM1
    Serial1.begin(9600, SERIAL_8N1, 20, 21); 
 
    // Настройте две задачи для независимого выполнения.
    uint32_t btn_delay = 1;  // Delay between changing state on LED pin
    uint32_t lcd_delay = 1000;  // Delay between changing state on LED pin
    uint32_t gps_delay = 10;  // Delay between changing state on LED pin

    xTaskCreate(
      TaskBtn, "TaskBtn"  // A name just for humans
      ,
      1024  // Размер стека можно проверить, вызвав функцию `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`.
      ,
      (void *)&btn_delay  // Параметр задачи, который может изменять поведение задачи. Он должен передаваться в виде указателя на тип void.
      ,
      2     // Priority
      ,
      NULL  // С помощью обработчика задач можно управлять этой задачей.
    );

    xTaskCreate(
      TaskGPS, "TaskGPS"  // A name just for humans
      ,
      2048  // Размер стека можно проверить, вызвав функцию `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`.
      ,
      (void *)&gps_delay  // Параметр задачи, который может изменять поведение задачи. Он должен передаваться в виде указателя на тип void.
      ,
      2     // Priority
      ,
      NULL  // С помощью обработчика задач можно управлять этой задачей.
    );

    xTaskCreate(
      TaskLCD, "TaskLCD"  // A name just for humans
      ,
      2048  // Размер стека можно проверить, вызвав функцию `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`.
      ,
      (void *)&lcd_delay  // Параметр задачи, который может изменять поведение задачи. Он должен передаваться в виде указателя на тип void.
      ,
      2     // Priority
      ,
      NULL  // Здесь не используется дескриптор задачи — просто передайте NULL.
    );
}

// the loop function runs over and over again forever
void loop() {

}

void TaskBtn(void *pvParameters) {  
/*
    Кнопка
*/
  pinMode(LED_BUILTIN, OUTPUT);
  uint32_t btn_delay = *((uint32_t *)pvParameters);

  for (;;) {

    butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться

    if (butt1.isSingle()) menu++;       // проверка на один клик
    if (butt1.isDouble()) menu--;       // проверка на двойной клик
    if (menu>1) menu=0;
    delay(btn_delay);
  }
}

void TaskGPS(void *pvParameters) {
/*
    GPS
*/
  uint32_t blink_delay = *((uint32_t *)pvParameters);
  //  Инициируем расшифровку строк NMEA указав объект используемой шины UART (вместо программной шины, можно указывать аппаратные: Serial, Serial1, Serial2, Serial3).
  gps.begin(Serial1);
  gps.timeZone(TIME_ZONE); 

  for (;;) {
    gps.read(); 
    sats = String(gps.satellites[GPS_ACTIVE]) + "/" + String(gps.satellites[GPS_VISIBLE]);

    dateTimeGPS = getDateTime(gps.Hours, gps.minutes, gps.seconds, gps.day, gps.month, gps.year);

    latitude = gps.latitude;
    longitude = gps.longitude;
    altitude = gps.altitude;

    speed = gps.speed;
    course = gps.course;
    
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(blink_delay);
  }
}

void TaskLCD(void *pvParameters) {
/*
    LCD
*/
  uint32_t blink_delay = *((uint32_t *)pvParameters);
  // инициализация LCD
  oledDisp.setup();

  for (;;) {
    if(menu==0){
      oledDisp.menu1(sats, dateTimeGPS, latitude, longitude, altitude, speed, course, getDistToPoint());
    }
    if(menu==1){
      calcNewXY(courseToHomeCorrect());
      oledDisp.menu2(course, speed, getDistToPoint(), new_x2, new_y2);
    }
    delay(blink_delay);
  }
}

void calcNewXY(int az){
  float xc = 64.0f;   // Координата X центра вращения
  float yc = 32.0f;   // Координата Y центра вращения
  float x1 = 64.0f;   // Исходная координата X
  float y1 = 10.0f;    // Исходная координата Y
  float angle_deg = az; // Угол поворота в градусах

  // Перевод угла в радианы
  float theta_rad = DEG_TO_RAD(angle_deg);

  // Вычисляем новые координаты
  new_x2 = xc + (x1 - xc)*cos(theta_rad) - (y1 - yc)*sin(theta_rad);
  new_y2 = yc + (x1 - xc)*sin(theta_rad) + (y1 - yc)*cos(theta_rad);
}

float courseToHomeCorrect(){
    float courseToHome =
    TinyGPSPlus::courseTo(
      latitude,
      longitude,
      HOME_LAT, 
      HOME_LON);
    float curse = course;
  return courseToHome - curse;
}

String getDistToPoint(){
  double dist = TinyGPSPlus::distanceBetween(latitude, longitude, HOME_LAT, HOME_LON);
  if(dist<1000)
    return String(dist)+" м";
  if(dist>1000 && dist<8900000)
    return String(dist)+" км";
  return "---";
}

String getDateTime(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month, uint8_t year){
  char buffer[20];
  sprintf(buffer, "%02d:%02d:%02d / %02d.%02d.%02d", hours, minutes, seconds, day, month, year); // Результат: "Temp: 25 C"
  return String(buffer);
}
