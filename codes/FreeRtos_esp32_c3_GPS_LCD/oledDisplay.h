#ifndef OLEDDISPLAY_H  // Альтернатива #pragma once для совместимости
#define OLEDDISPLAY_H

#include <Arduino.h> // Необходимо для использования функций типа digitalWrite, pinMode
#include <GyverOLED.h>
#include <Wire.h>

#define I2C_SDA 4
#define I2C_SCL 3

GyverOLED<SSH1106_128x64> display;

class OledDisplay {

  public:
    OledDisplay(){};

    void setup(){
        Wire.begin(I2C_SDA, I2C_SCL);
        display.init();  
    };

    void menu1(String sats, String dateTimeGPS, double latitude, double longitude, uint16_t altitude, uint16_t speed, uint16_t course, String distToPoint){
        display.clear();   // очистить дисплей (или буфер)
        display.setScale(1);

        display.setCursor(1, 0);
        display.print("Sat: ");
        display.print(sats);
    
        display.setCursor(1, 1);
        display.print(dateTimeGPS);

        display.setCursor(1, 2);
        display.print("Ш: ");
        display.print(latitude,4);
        display.setCursor(1, 3);
        display.print("Д: ");
        display.print(longitude,4);
        display.setCursor(1, 4);
        display.print("В: ");
        display.print(altitude,1);
        display.print(" м");

        display.setCursor(1, 5);
        display.print("Скорость: ");
        display.print(speed);
        display.print(" км/ч");
        display.setCursor(1, 6);
        display.print("Курс: ");
        display.print(course);

        display.setCursor(1, 7);
        display.print("Расст: ");
        display.print(distToPoint);
        display.update();
    }

    void menu2(uint16_t course, uint16_t speed, String distToPoint, float new_x, float new_y){
        display.clear();
        display.setScale(1);

        display.setCursor(0, 0);
        display.print("Azimut");
        display.setCursor(95, 0); 
        display.print("Speed");
        display.setCursor(5, 1); 
        display.print(course, 0);
        display.setCursor(103, 1); 
        display.print(speed, 0);
        display.setCursor(100, 2); 
        display.print("км/ч");

        display.setCursor(0, 6);
        display.print("Dist");
        display.setCursor(0, 7);
        display.print(distToPoint);

        display.dot(64, 32);
        display.circle(64, 32, 30, OLED_STROKE);
        display.line(64, 32, new_x, new_y);

        display.update();
    }
};

#endif