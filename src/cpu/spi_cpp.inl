///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

CCore::CSPI::CSPI(CCore& pCore, const ESubRegAddr pBaseAddr)
	: mBaseAddr(CU16(pBaseAddr)), CoreRef(pCore)
{
	Reset();
}

void CCore::CSPI::Reset()
{
	///TODO: void CCore::CSPI::Reset()
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPICR1)] = SPICR1_CPHA;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPICR2)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPIBR)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPISR)] = SPISR_SPTEF;
}

void CCore::CSPI::Step()
{
	///TODO: void CCore::CSPI::Step()
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPISR)] |= SPISR_SPIF;
}

void CCore::CSPI::WriteU8(const uint8_t pAddr, const uint8_t pVal)
{
	///TODO(#9): показывать конфигурацию портов.
	switch(pAddr)
	{
		case CU8(ERegAddr::SPICR1): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPICR1)] = pVal; break;
		case CU8(ERegAddr::SPICR2): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPICR2)] = pVal & SPICR2_Mask; break;
		case CU8(ERegAddr::SPIBR): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPIBR)] = pVal & SPIBR_Mask; break;
		case CU8(ERegAddr::SPISR): break;
		case CU8(ERegAddr::SPIDR): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::SPIDR)] = pVal; break;
		default:
			goto UnknownSpace;
	}

	return;

UnknownSpace:

	qFatal("CSPI::WriteU8. Unknown range for address 0x%X", pAddr);
}
