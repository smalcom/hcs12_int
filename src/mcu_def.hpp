#pragma once

/*************************************/
/************* Константы *************/
/*************************************/

enum class EMCU_AddrDef : uint16_t
{
	Reg_Begin    = 0x0000,
	Reg_End      = 0x03FF,
	EEPROM_Begin = 0x0000,
	EEPROM_End   = 0x0FFF,// Размер ЭСППЗУ: 4 кБ.
	RAM_Begin    = 0x1000,
	RAM_End      = 0x3FFF,// Размер ОЗУ: 12 кБ.
};

/// \brief Исходные значения регистров, которые определяются для модели ИМС.
enum class EMCU_RegDef : uint8_t
{
	INITEE = 0x01,
	PPAGE  = 0x00,
	MEMSIZ0 = 0x25,
	MEMSIZ1 = 0x81,
};

constexpr uint32_t MCU_FlashSize = 256 * 1024;
constexpr uint32_t MCU_RegSize = static_cast<uint16_t>(EMCU_AddrDef::Reg_End) - static_cast<uint16_t>(EMCU_AddrDef::Reg_Begin) + 1;
constexpr uint32_t MCU_EEPROMSize = static_cast<uint16_t>(EMCU_AddrDef::EEPROM_End) - static_cast<uint16_t>(EMCU_AddrDef::EEPROM_Begin) + 1;
constexpr uint32_t MCU_RAMSize = static_cast<uint16_t>(EMCU_AddrDef::RAM_End) - static_cast<uint16_t>(EMCU_AddrDef::RAM_Begin) + 1;
