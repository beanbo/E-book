#pragma once
#include "MainView.h"
#include "FileHelper.h"
#include <Arduino.h>
#include <touch.h>
#include "DrawHelper.h"
#include "Button2.h"
#include "Defines.h"
#include <Wire.h>

Button2 btnPower(BUTTON_POWER);
TouchClass touch;
MainView mainView;

void setup()
{
    Serial.begin(115200);
	
    InitializeTouch();
    InitializeScreen();
    fileHelper.InitializeFileSystem();

    btnPower.setPressedHandler(buttonPressed);

    drawStartImage();	

    mainView.Show();
}

void loop()
{	
    btnPower.loop();
	
    if (digitalRead(TOUCH_INT) && touch.scanPoint())
    {
        uint16_t x, y;
        touch.getPoint(x, y, 0);
        y = EPD_HEIGHT - y;

        mainView.HitTest(x, y);
    }
}

void buttonPressed(Button2& button)
{
    Serial.println("Button attached to pin " + String(button.getAttachPin()));

    if (button.getAttachPin() == BUTTON_POWER)
    {
        //fileHelper.TryChangeFileSystem();
		touch.wakeup();

        drawSleepImage();
        BookSleep();
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
    //esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_gpio_wakeup();
	
    esp_light_sleep_start();
}