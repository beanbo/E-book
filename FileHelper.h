#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM !!!"
#endif

#pragma once
#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"

class FileHelper
{
public:
	FileHelper();
	
	enum
	{
		PSRAM = 0,
		SD_CARD = 1
	};

	void SetCurrentFileSystem(int fs);
	fs::FS* GetCurrentFileSystem();
	
	void InitializeFileSystem();

	void ListDir(const char* dirname, uint8_t levels);

	void ReadFile(const char* path, String& fileText);
	void WriteFile(const char* path, const char* message);
	void AppendFile(const char* path, const char* message);
	void RenameFile(const char* path1, const char* path2);
	void DeleteFile(const char* path);

	void TestFileIO(const char* path);

	bool IsSDCardConnected();
	
	void TryChangeFileSystem();

protected:
	fs::FS* m_pCurrentFS;
};

extern FileHelper fileHelper;
