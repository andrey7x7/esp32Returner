#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  // Wait for display
  delay(500);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  delay(1000);
}

void loop() {
    // Пример использования функции с разными углами
    for (int angle = 0; angle < 360; angle += 5) {
        drawCompassArrow(angle);  // Нарисовать стрелку компаса для текущего угла
        delay(100);              // Задержка для отображения стрелки
    }
}

// Функция для отображения стрелки компаса
void drawCompassArrow(int angle) {
    // Очистить дисплей
    display.clearDisplay();

    // Перевести угол в радианы
    float radians = angle * (PI / 180);

    // Определяем координаты центра и конца стрелки
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    int arrowLength = 40; // Длина стержня стрелки
    int backLength = arrowLength/2; // Длина части стержня, уходящей назад
    int tipLength = 6; // Длина наконечника стрелки
    int arrowWidth = 6; // Ширина наконечника стрелки

    // Конец стержня стрелки (продленный назад)
    int startX = centerX - backLength * cos(radians);
    int startY = centerY - backLength * sin(radians);
    int endX = startX + arrowLength * cos(radians);
    int endY = startY + arrowLength * sin(radians);

    // Отрисовать стержень стрелки
    display.drawLine(startX, startY, endX, endY, SSD1306_WHITE);

    // Вычисляем координаты наконечника стрелки (треугольник)
    float arrowAngle1 = radians + (PI / 6); // Угол одного края наконечника
    float arrowAngle2 = radians - (PI / 6); // Угол другого края наконечника

    // Координаты кончиков наконечника стрелки
    int tipX1 = endX - tipLength * cos(arrowAngle1);
    int tipY1 = endY - tipLength * sin(arrowAngle1);
    int tipX2 = endX - tipLength * cos(arrowAngle2);
    int tipY2 = endY - tipLength * sin(arrowAngle2);

    // Отрисовать наконечник стрелки (треугольник)
    display.fillTriangle(endX, endY, tipX1, tipY1, tipX2, tipY2, SSD1306_WHITE);

    // Рисуем круг вокруг стрелки
    display.drawCircle(centerX, centerY, 6+backLength, SSD1306_WHITE);

    // Вывод текста "Azimut" и угла
    display.setCursor(5, 0); // Устанавливаем курсор в верхний левый угол
    display.setTextSize(1);  // Размер текста
    display.setTextColor(SSD1306_WHITE); // Цвет текста
    display.print("Azimut");
    display.setCursor(15, 10);
    display.print(angle);     // Значение угла

    // Вывод текста "Speed" и статического значения 60 км/ч
    display.setCursor(SCREEN_WIDTH - 35, 0); // Устанавливаем курсор в верхний правый угол
    display.print("Speed");
    display.setCursor(SCREEN_WIDTH - 25, 10); 
    display.print("60");

    // Обновить дисплей с новым содержимым
    display.display();
}
