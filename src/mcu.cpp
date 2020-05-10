#include "mcu.hpp"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include <string.h>

CMCU::CMCU()
{
	Reset();
}

CMCU::~CMCU()
{
}

bool CMCU::LoadFirmware(QString& pFWFilename)
{
QFile fw_file(pFWFilename);
bool rv = false;

	qInfo() << "Load FW from file: " << pFWFilename;

	if(fw_file.open(QIODevice::ReadOnly))
	{
		QByteArray ba;

		ba = fw_file.readAll();
		fw_file.close();

		if(static_cast<size_t>(ba.size()) > MCU_FlashSize)
		{
			qCritical() << "Firmware is bigger than flash.";
		}
		else if(ba.size() == 0)
		{
			qCritical() << "File is empty.";
		}
		else
		{
			// Копируем содержимое файла во флэш.
			Core.Flash_WriteArr(0x00000000, reinterpret_cast<const uint8_t*>(static_cast<const char*>(ba)), static_cast<uint32_t>(ba.size()));
			rv = true;
		}
	}// if(fw_file.open(QIODevice::ReadOnly))

	return rv;
}

bool CMCU::LoadEEPROM(QString& pEEFilename)
{
QFile ee_file(pEEFilename);
bool rv = false;

	qInfo() << "Load EEPROM from file: " << pEEFilename;

	if(ee_file.open(QIODevice::ReadOnly))
	{
		QByteArray ba;

		ba = ee_file.readAll();
		ee_file.close();

		if(static_cast<size_t>(ba.size()) > MCU_EEPROMSize)
		{
			qCritical() << "File is bigger than EEPROM.";
		}
		else if(ba.size() == 0)
		{
			qCritical() << "File is empty.";
		}
		else
		{
			// Копируем содержимое файла в ЭСППЗУ.
			Core.EEPROM_WriteArr(0x0000, reinterpret_cast<const uint8_t*>(static_cast<const char*>(ba)), static_cast<uint32_t>(ba.size()));
			rv = true;
		}
	}// if(ee_file.open(QIODevice::ReadOnly))

	return rv;
}

void CMCU::Reset()
{
	///TODO: void Reset()
	Core.Reset();
}

void CMCU::Step()
{
	///TODO: void Step()
	Core.Step();
}
