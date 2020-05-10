#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include "mcu.hpp"

int main(int argc, char *argv[])
{
QCoreApplication a(argc, argv);
CMCU mcu;

	qInfo() << "hcs12int started";
	// Проверка аргументов.
	if(argc < 3)
	{
		qCritical() << "Не задана микропрограмма и содержимое ЭСППЗУ.";
		qInfo() << "Использование: " << argv[0] << " микропрограмма ЭСППЗУ";

		return 1;
	}

	{
		QString fw_fn(argv[1]);
		QString ee_fn(argv[2]);

		if(!mcu.LoadFirmware(fw_fn))
		{
			qCritical() << "Can not load firmware.";

			return 2;
		}

		if(!mcu.LoadEEPROM(ee_fn))
		{
			qCritical() << "Can not load EEPROM.";

			return 3;
		}
	}

	mcu.Reset();
	while(true)
	{
		mcu.Step();
	}

	return a.exec();
}

///TODO: Загрузка микропрограммы.
///TODO: Общий сброс.
///TODO: Инициализация gdbserver.
///TODO: Цикл выполнения.
