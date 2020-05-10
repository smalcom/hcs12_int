///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

///TODO: Map priority
/// Registers
/// Exception vector/BDM ROM
/// RAM
/// EEPROM
/// Flash
/// Expansion windows

CCore::CMMC::CMMC(CCore& pCore)
	: CoreRef(pCore)
{
	Reset();
}

void CCore::CMMC::Reset()
{
	///NOTE: Делаем небольшую помощь алгоритму - устанавливаем начальное значение отображения регистров.
	Mapping.Register = 0x0000;
	// Теперь можно провести инициализацию с правильной записью в регистры.
	WriteU8(ERegAddr::INITRM, 0x08 /* RAM11 */ | EINITRM_RAMHAL);
	WriteU8(ERegAddr::INITRG, 0x00);
	WriteU8(ERegAddr::INITEE, static_cast<uint8_t>(EMCU_RegDef::INITEE));
	WriteU8(ERegAddr::MISC, 0x0D);
	CoreRef.mSubReg[CU16(ERegAddr::MTST0)] = 0x00;
	CoreRef.mSubReg[CU16(ERegAddr::MTST1)] = 0x10;
	CoreRef.mSubReg[CU16(ERegAddr::MEMSIZ0)] = CU8(EMCU_RegDef::MEMSIZ0);
	CoreRef.mSubReg[CU16(ERegAddr::MEMSIZ1)] = CU8(EMCU_RegDef::MEMSIZ1);
	WriteU8(ERegAddr::PPAGE, CU8(EMCU_RegDef::PPAGE));
}

uint8_t CCore::CMMC::ReadU8(const uint16_t pAddr) const
{
	// Определяем подблок в соответствии с приоритетом.
	//
	// 1. Регистры:
	if((pAddr >= Mapping.Register) && (pAddr < (Mapping.Register + MCU_RegSize)))
	{
		qDebug("Read: [0x%X] = 0x%X", pAddr, CoreRef.mSubReg[pAddr - Mapping.Register]);
		return CoreRef.mSubReg[pAddr - Mapping.Register];
	}

	/// 2. BDM ПЗУ:
	///TODO(#9): BDM ПЗУ не реализован.
	// 3. ОЗУ
	if((pAddr >= Mapping.RAM) && (pAddr < (Mapping.RAM + MCU_RAMSize)))
	{
		qDebug("Read: [0x%X] = 0x%X", pAddr, CoreRef.mRAM[pAddr - Mapping.RAM]);
		return CoreRef.mRAM[pAddr - Mapping.RAM];
	}

	// 4. ЭСППЗУ
	if((pAddr >= Mapping.EEPROM) && (pAddr < (Mapping.EEPROM + MCU_EEPROMSize)))
	{
		if((CoreRef.mSubReg[CU16(ERegAddr::INITEE)] & EINITEE_EEON) != 0)// Проверяем, что ЭСППЗУ включено.
		{
			qDebug("ReadEE: [0x%X] = 0x%X", pAddr, CoreRef.mEEPROM[pAddr - Mapping.EEPROM]);
			return CoreRef.mEEPROM[pAddr - Mapping.EEPROM];
		}
	}

	// 5. Флэш
	if((CoreRef.mSubReg[CU16(ERegAddr::MISC)] & EMISC_ROMON) != 0)// Проверяем, что флэш-память включена.
	{
		constexpr uint16_t addrPage3E_beg = 0x4000;
		constexpr uint16_t addrPage3E_end = 0x7FFF;
		constexpr uint16_t addrPageVar_beg = 0x8000;
		constexpr uint16_t addrPageVar_end = 0xBFFF;
		constexpr uint16_t addrPage3F_beg = 0xC000;
		constexpr uint16_t addrPage3F_end = 0xFFFF;

		// Закреплённая страница 0x3E.
		if((pAddr >= addrPage3E_beg) && (pAddr <= addrPage3E_end))
		{
			if((CoreRef.mSubReg[CU16(ERegAddr::MISC)] & EMISC_ROMHM) == 0)// Для доступа к странице в нижней половине адресного пространства должен быть сброшен запрещающий бит.
			{
				qDebug("FlashRead3E: 0x%X", Flash_PageAddress(0x3E & PPAGE_Mask) + (pAddr - addrPage3E_beg));
				return CoreRef.mFlash[Flash_PageAddress(0x3E & PPAGE_Mask) + (pAddr - addrPage3E_beg)];
			}
		}

		if((pAddr >= addrPageVar_beg) && (pAddr <= addrPageVar_end))
		{
			qDebug("FlashReadVar: 0x%X", Flash_PageAddress(CoreRef.mSubReg[CU16(ERegAddr::PPAGE)] & PPAGE_Mask) + (pAddr - addrPageVar_beg));
			return CoreRef.mFlash[Flash_PageAddress(CoreRef.mSubReg[CU16(ERegAddr::PPAGE)] & PPAGE_Mask) + (pAddr - addrPageVar_beg)];
		}

		// Закреплённая страница 0x3F.
		if((pAddr >= addrPage3F_beg) && (pAddr <= addrPage3F_end))
		{
			qDebug("FlashRead3F: 0x%X", Flash_PageAddress(0x3F & PPAGE_Mask) + (pAddr - addrPage3F_beg));
			return CoreRef.mFlash[Flash_PageAddress(0x3F & PPAGE_Mask) + (pAddr - addrPage3F_beg)];
		}

	/// 6. Окно расширенной памяти.
	}

	///TODO(#9): Обращение к внешней памяти не реализовано.
	qFatal("MMC::ReadU8. Unknown range for address 0x%X", pAddr);
}

uint16_t CCore::CMMC::ReadU16(const uint16_t pAddr) const
{
uint16_t tval16;

	tval16 = ReadU8(pAddr);
	tval16 <<= 8;
	tval16 |= ReadU8(pAddr + 1);

	return tval16;
}

void CCore::CMMC::WriteU8(const uint16_t pAddr, const uint8_t pVal)
{
	qDebug("Write: [0x%X] = 0x%X", pAddr, pVal);
	// Определяем подблок в соответствии с приоритетом.
	//
	// 1. Регистры:
	if((pAddr >= Mapping.Register) && (pAddr < (Mapping.Register + MCU_RegSize)))
	{
		uint16_t addr_subreg = pAddr - Mapping.Register;

		// Диапазоны адресов подблоков.
		if(((addr_subreg >= CU16(ESubRegAddr::MEBI_Map1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MEBI_Map1_End))) ||
			((addr_subreg >= CU16(ESubRegAddr::MEBI_Map2_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MEBI_Map2_End))) ||
			((addr_subreg >= CU16(ESubRegAddr::MEBI_Map3_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MEBI_Map3_End))))
		{
			CoreRef.mPIM.WriteU8(addr_subreg - CU16(ESubRegAddr::MEBI_Map1_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map1_End)))
		{
			switch(addr_subreg)
			{
				case CU16(ERegAddr::INITRM):
					CoreRef.mSubReg[CU16(ERegAddr::INITRM)] = pVal & (EINITRM_AddrMask | EINITRM_RAMHAL);
					if(pVal & EINITRM_RAMHAL)
					{// Выравнивание по верхней границе страницы.
						Mapping.RAM = (CU16(pVal & EINITRM_AddrMask) << 8) | 0x3FFF;// Устанавливаем адрес равный концу 16-килобайтного блока.
						Mapping.RAM -= (MCU_RAMSize - 1);// Используя полученный последний адрес вычисляем адрес начала.
					}
					else
					{// Выравнивание по нижней границе страницы.
						Mapping.RAM = CU16(pVal & EINITRM_AddrMask) << 8;
					}

					break;
				case CU16(ERegAddr::INITRG):
					CoreRef.mSubReg[CU16(ERegAddr::INITRG)] = pVal & EINITRG_AddrMask;
					Mapping.Register = CU16(pVal & EINITRG_AddrMask) << 8;

					break;
				case CU16(ERegAddr::INITEE):
					CoreRef.mSubReg[CU16(ERegAddr::INITEE)] = pVal & (EINITEE_AddrMask | EINITEE_EEON);
					Mapping.EEPROM = CU16(pVal & EINITEE_AddrMask) << 8;

					break;

				// Никаких особых действий не делаем: управление тактовыми сигналами не входит в задачи данной программы, а биты доступа к флэш проверяются при каждом обращении
				// к соответствующим областям памяти.
				case CU16(ERegAddr::MISC): CoreRef.mSubReg[CU16(ERegAddr::MISC)] = pVal & EMISC_WrMask; break;
				case CU16(ERegAddr::MTST0): break;
				default:
					qWarning("MMC::WriteU8. Unknown register with address 0x%X in MMC_Map1.", addr_subreg);

					goto UnknownSpace;
			}
		}// if((addr_subreg >= CU16(ERegAddr::MMC_Map1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map1_End)))
		else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map2_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map2_End)))
		{
			switch(addr_subreg)
			{
				case CU16(ERegAddr::MTST1): break;
				default:
					qWarning("MMC::WriteU8. Unknown register with address 0x%X in MMC_Map2.", addr_subreg);

					goto UnknownSpace;
			}
		}// else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map2_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map2_End)))
		else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map3_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map3_End)))
		{
			switch(addr_subreg)
			{
				case CU16(ERegAddr::MEMSIZ0): break;
				case CU16(ERegAddr::MEMSIZ1): break;
				default:
					qWarning("MMC::WriteU8. Unknown register with address 0x%X in MMC_Map3.", addr_subreg);

					goto UnknownSpace;
			}
		}// else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map3_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map3_End)))
		else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map4_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map4_End)))
		{
			switch(addr_subreg)
			{
				case CU16(ERegAddr::PPAGE): CoreRef.mSubReg[CU16(ERegAddr::PPAGE)] = pVal & EPPAGE_AddrMask; break;
				default:
					qWarning("MMC::WriteU8. Unknown register with address 0x%X in MMC_Map4.", addr_subreg);

					goto UnknownSpace;
			}
		}// else if((addr_subreg >= CU16(ESubRegAddr::MMC_Map4_Begin)) && (addr_subreg <= CU16(ESubRegAddr::MMC_Map4_End)))
		else if((addr_subreg >= CU16(ESubRegAddr::CRG_Begin)) && (addr_subreg <= CU16(ESubRegAddr::CRG_End)))
		{
			CoreRef.mCRG.WriteU8(addr_subreg - CU16(ESubRegAddr::CRG_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::ECT_Begin)) && (addr_subreg <= CU16(ESubRegAddr::ECT_End)))
		{
			CoreRef.mECT.WriteU8(addr_subreg - CU16(ESubRegAddr::ECT_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::ATD0_Begin)) && (addr_subreg <= CU16(ESubRegAddr::ATD0_End)))
		{
			CoreRef.mSubReg[addr_subreg] = pVal;///TODO: at now
		}
		else if((addr_subreg >= CU16(ESubRegAddr::PWM_Begin)) && (addr_subreg <= CU16(ESubRegAddr::PWM_End)))
		{
			CoreRef.mSubReg[addr_subreg] = pVal;///TODO: at now
		}
		else if((addr_subreg >= CU16(ESubRegAddr::SCI0_Begin)) && (addr_subreg <= CU16(ESubRegAddr::SCI0_End)))
		{
			CoreRef.mSubReg[addr_subreg] = pVal;///TODO: at now
		}
		else if((addr_subreg >= CU16(ESubRegAddr::SCI1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::SCI1_End)))
		{
			CoreRef.mSubReg[addr_subreg] = pVal;///TODO: at now
		}
		else if((addr_subreg >= CU16(ESubRegAddr::PIM_Begin)) && (addr_subreg <= CU16(ESubRegAddr::PIM_End)))
		{
			CoreRef.mPIM.WriteU8(addr_subreg - CU16(ESubRegAddr::PIM_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::SPI0_Begin)) && (addr_subreg <= CU16(ESubRegAddr::SPI0_End)))
		{
			CoreRef.mSPI0.WriteU8(addr_subreg - CU16(ESubRegAddr::SPI0_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::SPI1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::SPI1_End)))
		{
			CoreRef.mSPI1.WriteU8(addr_subreg - CU16(ESubRegAddr::SPI1_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::SPI2_Begin)) && (addr_subreg <= CU16(ESubRegAddr::SPI2_End)))
		{
			CoreRef.mSPI2.WriteU8(addr_subreg - CU16(ESubRegAddr::SPI2_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::EEPROM_Begin)) && (addr_subreg <= CU16(ESubRegAddr::EEPROM_End)))
		{
			CoreRef.mSubReg[addr_subreg] = pVal;///TODO: at now
		}
		else if((addr_subreg >= CU16(ESubRegAddr::ATD1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::ATD1_End)))
		{
			CoreRef.mSubReg[addr_subreg] = pVal;///TODO: at now
		}
		else if((addr_subreg >= CU16(ESubRegAddr::CAN0_Begin)) && (addr_subreg <= CU16(ESubRegAddr::CAN0_End)))
		{
			CoreRef.mCAN0.WriteU8(addr_subreg - CU16(ESubRegAddr::CAN0_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::CAN1_Begin)) && (addr_subreg <= CU16(ESubRegAddr::CAN1_End)))
		{
			CoreRef.mCAN1.WriteU8(addr_subreg - CU16(ESubRegAddr::CAN1_Begin), pVal);
		}
		else if((addr_subreg >= CU16(ESubRegAddr::CAN4_Begin)) && (addr_subreg <= CU16(ESubRegAddr::CAN4_End)))
		{
			CoreRef.mCAN4.WriteU8(addr_subreg - CU16(ESubRegAddr::CAN4_Begin), pVal);
		}
		else
		{
			qWarning("MMC::WriteU8. Unknown register with address 0x%X.", addr_subreg);

			goto UnknownSpace;
		}

		return;// Запись выполнена успешно.
	}// if((pAddr >= Mapping.Register) && (pAddr < (Mapping.Register + MCU_RegSize)))

///TODO: Необходимые действия при записи.
#if 0
	/// 2. BDM ПЗУ:
	///TODO(#9): BDM ПЗУ не реализован.
#endif // 0
	/// 3. ОЗУ
	if((pAddr >= Mapping.RAM) && (pAddr < (Mapping.RAM + MCU_RAMSize)))
	{
		CoreRef.mRAM[pAddr - Mapping.RAM] = pVal;

		return;// Запись выполнена успешно.
	}

#if 0
	/// 4. ЭСППЗУ
	if((pAddr >= Mapping.EEPROM) && (pAddr < (Mapping.EEPROM + MCU_EEPROMSize)))
	{
		CoreRef.mEEPROM[pAddr - Mapping.EEPROM] = pVal;
	}
#endif // 0

UnknownSpace:

	/// 6. Окно расширенной памяти.
	///TODO(#9): Обращение к внешней памяти не реализовано.
	qFatal("MMC::WriteU8. Unknown range for address 0x%X", pAddr);
}

void CCore::CMMC::WriteU16(const uint16_t pAddr, const uint16_t pVal)
{
	WriteU8(pAddr, pVal >> 8);
	WriteU8(pAddr + 1, pVal);
}
