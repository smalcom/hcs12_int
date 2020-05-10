///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

CCore::CCRG::CCRG(CCore& pCore)
	: CoreRef(pCore)
{
	Reset();
}

void CCore::CCRG::Reset()
{
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::SYNR)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::REFDV)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CTFLG)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CRGFLG)] = ECRGFLG_PORF;///NOTE: это не включение питания, но другого способа пока не нет.
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CRGINT)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CLKSEL)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::PLLCTL)] = EPLLCTL_CME | EPLLCTL_PLLON | EPLLCTL_AUTO | EPLLCTL_ACQ | EPLLCTL_SCME;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::RTICTL)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::COPCTL)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::FORBYP)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CTCTL)] = 0x00;
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::ARMCOP)] = 0x00;
}

void CCore::CCRG::Step()
{
	///TODO: void CCore::CCRG::Step()
	CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CRGFLG)] |= ECRGFLG_LOCK;
}

void CCore::CCRG::WriteU8(const uint8_t pAddr, const uint8_t pVal)
{
	///TODO(#9): показывать конфигурацию подблока.
	switch(pAddr)
	{
		case CU8(ERegAddr::SYNR): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::SYNR)] = pVal & ESYNR_Mask; return;
		case CU8(ERegAddr::REFDV): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::REFDV)] = pVal & EREFDV_Mask; return;
		case CU8(ERegAddr::CTFLG): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CTFLG)] = 0x00; return;
		case CU8(ERegAddr::CRGFLG):
			{
				uint8_t& reg = CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CRGFLG)];

				if(pVal & ECRGFLG_RTIF) reg &= ~ECRGFLG_RTIF;
				if(pVal & ECRGFLG_PORF) reg &= ~ECRGFLG_PORF;
				if(pVal & ECRGFLG_LVRF) reg &= ~ECRGFLG_LVRF;
				if(pVal & ECRGFLG_LOCKIF) reg &= ~ECRGFLG_LOCKIF;
				if(pVal & ECRGFLG_SCMIF) reg &= ~ECRGFLG_SCMIF;

			}

			return;
		case CU8(ERegAddr::CRGINT): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::CRGINT)] = pVal & (ECRGINT_RTIE | ECRGINT_LOCKIE | ECRGINT_SCMIE); return;
		case CU8(ERegAddr::PLLCTL): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::PLLCTL)] = pVal & EPLLCTL_Mask; return;
		case CU8(ERegAddr::RTICTL): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::RTICTL)] = pVal & ERTICTL_Mask; return;
		case CU8(ERegAddr::COPCTL): CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + CU8(ERegAddr::COPCTL)] = pVal & ECOPCTL_Mask; return;
		default:
			CoreRef.mSubReg[CU16(ESubRegAddr::CRG_Begin) + pAddr] = pVal;

			return;
	}// switch(pAddr)
}
