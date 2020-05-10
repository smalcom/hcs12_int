///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

CCore::CECT::CECT(CCore& pCore)
	: CoreRef(pCore)
{
	Reset();
}

void CCore::CECT::Reset()
{
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TIOS)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TCNT_H)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TCNT_L)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TSCR1)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TIE)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TC4_H)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TC4_L)] = 0x00;
}

void CCore::CECT::Step()
{
uint16_t tcnt;

	tcnt = CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TCNT_H)];
	tcnt <<= 8;
	tcnt |= CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TCNT_L)];
	tcnt++;
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TCNT_H)] = (tcnt >> 8);
	CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TCNT_L)] = tcnt;
}

void CCore::CECT::WriteU8(const uint8_t pAddr, const uint8_t pVal)
{
	///TODO(#9): показывать конфигурацию подблока.
	switch(pAddr)
	{
		case CU8(ERegAddr::TIOS): CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TIOS)] = pVal; return;
		case CU8(ERegAddr::TSCR1): CoreRef.mSubReg[CU16(ESubRegAddr::ECT_Begin) + CU8(ERegAddr::TSCR1)] = pVal & TSCR1_Mask; return;

		case CU8(ERegAddr::TIE):
		case CU8(ERegAddr::TC4_H):
		case CU8(ERegAddr::TC4_L):
			CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + pAddr] = pVal;

			return;
		default:
			///TODO: unk qFatal("Unknown ECT register: 0x%X", pAddr);
			qWarning("Unknown ECT register: 0x%X", pAddr);

			break;
	}// switch(pAddr)
}
