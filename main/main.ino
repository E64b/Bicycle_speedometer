#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

float SPEED, DIST, TRIP; //Скорость, Дистанция
float w_length = 2.070; //Длина окружности колеса в метрах
bool disp;
float DisplaySpeed;
unsigned long lastturn, time_press;
boolean state, button;

void setup() 
{
    lcd.init();
    lcd.backlight();
    lcd.home();
    lcd.print("Speedometr"); //Тут может быть любая надпись
    DIST=(float)EEPROM.read(0)/10.0; //Загружаем из памяти пробег при включении
    attachInterrupt(0, wheelrotation, RISING); //Подключить прерывание на 2 пин при повышении сигнала
    pinMode(3, OUTPUT);   //3 пин как выход
    digitalWrite(3, HIGH);  //Подать 5 вольт на 3 пин для датчика 
    pinMode(8, INPUT_PULLUP);   //Сюда подключена кнопка  
    TRIP = 0;
    delay(3000);
    lcd.clear();
}

void wheelrotation() 
{
  if (millis()-lastturn > 80) 
  { 
    SPEED=w_length/((float)(millis()-lastturn)/1000)*3.6;  //Расчет скорости, км/ч
    DisplaySpeed = SPEED;
    lastturn=millis();  //Время последнего оборота
    DIST = DIST + w_length / 1000;//Прибавляем длину колеса к дистанции при каждом обороте
    TRIP =  TRIP + w_length/1000;
  }
}
void loop() 
{  
/*Если 2 секунды нет движения, считаем что скорость 0 и сохраняем ODO*/
  if ((millis()-lastturn)>2000) 
  { 
    SPEED=0;
    EEPROM.write(0,(float)DIST*10.0);
  }

/*Если данные изменились, меняем их на дисплее*/
  if (DisplaySpeed != SPEED) 
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Speed"); lcd.print(' '); lcd.print(SPEED); lcd.print(' '); lcd.print("km/h");
      if (disp == false) 
      {
        lcd.setCursor(0, 1);
        lcd.print("ODO"); lcd.print(' '); lcd.print(DIST); lcd.print(' '); lcd.print("km");
      }
      else
      {
      lcd.setCursor(0, 1);
      lcd.print("Trip"); lcd.print(' '); lcd.print(TRIP); lcd.print(' '); lcd.print("km");
      }
  }

/*Обработка кнопки*/
  if (digitalRead(8) == LOW)
  {
    state = 1;
    button = 1;
    time_press = millis();
     while (millis() - time_press <= 500) 
     {       
      if (digitalRead(8) == HIGH) 
      {            
        button=0;
        break;
       }
     }
  }

/*Однократное нажатие меняет вывод ODO и Trip, долгое нажатие в режиме Trip обнуляет его*/
  switch (button) 
  {
    case 0:
        disp =! disp;
        state = 0;
      break;
    case 1:
      if (disp == true) 
      {
        TRIP = 0;
        state = 0;
      }
      break;
  }
}
