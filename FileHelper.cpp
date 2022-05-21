#include "FileHelper.h"
#include <SD.h>
#include <SPI.h>

#define SD_MISO             12
#define SD_MOSI             13
#define SD_SCLK             21
#define SD_CS               22

FileHelper fileHelper;

FileHelper::FileHelper()
{
    m_pCurrentFS = &SPIFFS;
}

void FileHelper::SetCurrentFileSystem(int fs)
{
    if (fs == PSRAM && m_pCurrentFS != &SPIFFS)
    {
        Serial.println("File system changed to PSRAM");
		m_pCurrentFS = &SPIFFS;
    }
    else if (fs == SD_CARD && m_pCurrentFS != &SD)
    {
        Serial.println("File system changed to SD card");
		m_pCurrentFS = &SD;
    }
}

fs::FS* FileHelper::GetCurrentFileSystem()
{
    return m_pCurrentFS;
}

void FileHelper::InitializeFileSystem()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        while (1);
    }
	else
	{
		Serial.println("SPIFFS Mount Success");
	}
}

void FileHelper::ListDir(const char* dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = m_pCurrentFS->open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
                ListDir(file.name(), levels - 1);
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }

        file = root.openNextFile();
    }
}

void FileHelper::ReadFile(const char* path, String& fileText)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = m_pCurrentFS->open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");

    fileText = "";
	
    while (file.available())
        fileText += (char)file.read();

	Serial.println(fileText);
    file.close();
    Serial.println("- file closed");
}

void FileHelper::WriteFile(const char* path, const char* message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = m_pCurrentFS->open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }

    if (file.print(message))
        Serial.println("- file written");
    else
        Serial.println("- write failed");

    file.close();
}

void FileHelper::AppendFile(const char* path, const char* message)
{
    Serial.printf("Appending to file: %s\r\n", path);

    File file = m_pCurrentFS->open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("- failed to open file for appending");
        return;
    }

    if (file.print(message))
        Serial.println("- message appended");
    else
        Serial.println("- append failed");

    file.close();
}

void FileHelper::RenameFile(const char* path1, const char* path2)
{
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);

    if (m_pCurrentFS->rename(path1, path2))
        Serial.println("- file renamed");
    else
        Serial.println("- rename failed");
}

void FileHelper::DeleteFile(const char* path)
{
    Serial.printf("Deleting file: %s\r\n", path);

    if (m_pCurrentFS->remove(path))
        Serial.println("- file deleted");
    else
        Serial.println("- delete failed");
}

void FileHelper::TestFileIO(const char* path)
{
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;

    File file = m_pCurrentFS->open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    uint32_t start = millis();

    Serial.print("- writing");

    for (i = 0; i < 2048; i++)
    {
        if ((i & 0x001F) == 0x001F)
            Serial.print(".");

        file.write(buf, 512);
    }

    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);

    file.close();

    file = m_pCurrentFS->open(path);
    start = millis();
    end = start;
    i = 0;

    if (file && !file.isDirectory())
    {
        len = file.size();
        size_t flen = len;
        start = millis();

        Serial.print("- reading");

        while (len)
        {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F) {
                Serial.print(".");
            }
            len -= toRead;
        }

        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);

        file.close();
    }
    else
        Serial.println("- failed to open file for reading");
}

bool FileHelper::IsSDCardConnected()
{
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS))
    {
        Serial.println("SD card reader module not found");
        return false;
    }
	
    uint8_t cardType = SD.cardType();
	
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
    }
    else
    {
        Serial.print("SD Card Type: ");
        if (cardType == CARD_MMC) {
            Serial.println("MMC");
        }
        else if (cardType == CARD_SD) {
            Serial.println("SDSC");
        }
        else if (cardType == CARD_SDHC) {
            Serial.println("SDHC");
        }
        else 
        {
            Serial.println("UNKNOWN");
        }
    }		
	
    if (cardType == CARD_NONE || cardType >= CARD_UNKNOWN)
        return false;
	
    return true;
}

void FileHelper::TryChangeFileSystem()
{
    SetCurrentFileSystem(IsSDCardConnected() ? SD_CARD : PSRAM);
   /* SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    bool rlst = SD.begin(SD_CS);
    if (!rlst) {
        Serial.println("SD init failed");
    }
    else {
        Serial.println("SD init Suc");
    }*/
    //InitializeTouch();
}
