#pragma once
#include "FileHelper.h"
#include "Page.h"
#include <Arduino.h>
#include <touch.h>
#include "DrawHelper.h"
#include "Button2.h"
#include "FileHelper.h"
#include "Defines.h"
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "logo.h"
#include "firasans.h"
#include <Wire.h>
#include "lilygo.h"

Button2 btnPower(BUTTON_POWER);
TouchClass touch;
PageManager pageManager;

void setup()
{
    Serial.begin(115200);
    
    InitializeTouch();
    InitializeScreen();
    InitializeFileSystem();

    pageManager.LoadBook("/testBook.txt");
}

void loop()
{
    if (digitalRead(TOUCH_INT) && touch.scanPoint())
    {
        uint16_t x, y;
        touch.getPoint(x, y, 0);

        if (x < EPD_WIDTH / 2) 
            pageManager.GoToPreviousPage();
        else
            pageManager.GoToNextPage();
    }
}

void InitializeTouch()
{
    pinMode(TOUCH_INT, INPUT_PULLUP);
    Wire.begin(15, 14);

    if (!touch.begin())
    {
        Serial.println("start touchscreen failed");
        while (1);
    }

    Serial.println("Started Touchscreen poll...");
}

void BookSleep()
{
    touch.sleep();
    delay(5);

    pinMode(14, INPUT);
    pinMode(15, INPUT);

    epd_poweroff_all();

    // Set to wake up by GPIO39
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ALL_LOW);

    esp_deep_sleep_start();
}