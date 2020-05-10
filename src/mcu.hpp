#pragma once

#include <QtCore/QString>

#include "cpu/core.hpp"

/*************************************/
/************ Типы данных ************/
/*************************************/

class CMCU
{
private:

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	CCore Core;

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CMCU(const CMCU&) = delete;
	CMCU(const CMCU&&) = delete;

public:

	CMCU();
	~CMCU();

	/// \brief Загрузка содержимого флэш-памяти.
	bool LoadFirmware(QString& pFWFilename);

	/// \brief Загрузка содержимого ЭСППЗУ.
	bool LoadEEPROM(QString& pEEFilename);

	/// \brief Общий сброс.
	void Reset();

	/// \brief Сделать шаг выполнения программы.
	void Step();
};
