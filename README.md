# E-book

Firmware for a simple e-reader on an **ESP32 board with an e-paper display and touch panel**
from LilyGo (the one supported by the LilyGo-EPD47 library), written in April 2022. It is the software part of the graduation project
[GraduateWork_E-book](https://github.com/beanbo/GraduateWork_E-book) («Электронная книга на базе
ESP32», BSUIR, 2022).

Based on the LilyGo display library: https://github.com/Xinyuan-LilyGO/LilyGo-EPD47

## What it does (from the code)

- On start it shows a logo, mounts **SPIFFS** and lists the files in its root as books.
- Main menu: the books are shown three at a time; touch selects a book or scrolls the list.
- A book is a plain-text file; the text is split into pages and drawn with the Fira Sans font
  (`firasans.h`), with next/previous page navigation by touch.
- The power button (GPIO 39) draws a sleep screen and puts the board into deep sleep; the same
  button wakes it up.
- PSRAM is required: the build stops with *"Please enable PSRAM !!!"* if `BOARD_HAS_PSRAM`
  is not defined.
- `data/` contains four sample books (`testBook*.txt`).

Not in this code: the project paper in
[MyScientificPublications](https://github.com/beanbo/MyScientificPublications) also describes
an SD card and uploading books over Wi-Fi through a Telegram bot. This firmware has neither —
books are read from SPIFFS only.

## Stack

- C++ / Arduino on ESP32 (Arduino core for ESP32 **1.0.6**, according to the include paths in
  `E-book.vcxproj`)
- Libraries: LilyGo-EPD47 (`epd_driver.h`), its touch driver (`touch.h`), Button2, Wire, SPIFFS
- Edited in Visual Studio with the Visual Micro extension (`E-book.sln`, `E-book.vcxproj`);
  `E-book.ino` is a regular Arduino sketch

## Build and flash

1. Install the Arduino IDE and the ESP32 board package (the project was built against 1.0.6).
2. Install the LilyGo-EPD47 library (it also provides the touch driver) and Button2.
3. Board: an ESP32 board with PSRAM enabled (for example *ESP32 Dev Module* with
   *PSRAM: Enabled*).
4. Open `E-book.ino`, build and upload.
5. Upload the books from `data/` to SPIFFS (for example with the *ESP32 Sketch Data Upload*
   tool for the Arduino IDE).

Not verified here: this needs the board, the ESP32 toolchain and network access to install it.
The exact board settings above are a suggestion and are not recorded in the repository.

## Context

Graduation project, Belarusian State University of Informatics and Radioelectronics (BSUIR),
2022. Design documents, schematics and drawings are in
[GraduateWork_E-book](https://github.com/beanbo/GraduateWork_E-book).
