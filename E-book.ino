#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM !!!"
#endif

#pragma once
#include "Page.h"
#include <Arduino.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "logo.h"
#include "firasans.h"
#include <Wire.h>
#include <touch.h>
#include "lilygo.h"
#include "DrawHelper.h"

#define TOUCH_INT   13
TouchClass touch;
PageManager pageManager;

String AllText = "One thing was certain, that the white kitten had had nothing to do with it:—it was the black kitten’s fault entirely. "\
"For the white kitten had been having its face washed by the old cat for the last quarter of an hour(and bearing it pretty well, considering); so you see that it couldn’t have had any hand in the mischief."\
"The way Dinah washed her children’s faces was this: first she held the poor thing down by its ear with one paw, and then with the other paw she rubbed its face all over, the wrong way, beginning at the nose: and just now, as I said, she was hard at work on the white kitten, which was lying quite still and trying to purr—no doubt feeling that it was all meant for its good."\
"But the black kitten had been finished with earlier in the afternoon, and so, while Alice was sitting curled up in a corner of the great arm-chair, half talking to herself and half asleep, the kitten had been having a grand game of romps with the ball of worsted Alice had been trying to wind up, " \
"and had been rolling it up and down till it had all come undone again; and there it was, spread over the hearth-rug, all knots and tangles, with the kitten running after its own tail in the middle."\
"“Oh, you wicked little thing!” cried Alice, catching up the kitten, and giving it a little kiss to make it understand that it was in disgrace. “Really, Dinah ought to have taught you better manners! You ought, Dinah, you know you ought!” she added, "\
"looking reproachfully at the old cat, and speaking in as cross a voice as she could manage—and then she scrambled back into the arm-chair, taking the kitten and the worsted with her, and began winding up the ball again. "\
"But she didn’t get on very fast, as she was talking all the time, sometimes to the kitten, and sometimes to herself."\
"Kitty sat very demurely on her knee, pretending to watch the progress of the winding, and now and then putting out one paw and gently touching the ball, as if it would be glad to help, if it might.";

void setup()
{
    Serial.begin(115200);
    epd_init();
    pinMode(TOUCH_INT, INPUT_PULLUP);
    Wire.begin(15, 14);

    if (!touch.begin()) 
    {
        Serial.println("start touchscreen failed");
        while (1);
    }

    Serial.println("Started Touchscreen poll...");

    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer) 
    {
        Serial.println("alloc memory failed !!!");
        while (1);
    }

    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    setFont(FiraSans);
    pageManager.SetBookText(AllText);
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

void BookSlepp()
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