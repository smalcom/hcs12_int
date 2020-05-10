///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

CCore::CMEBI::CMEBI(CCore& pCore)
	: CoreRef(pCore)
{
	Reset();
}

void CCore::CMEBI::Reset()
{
	///TODO: void CCore::CMEBI::Reset()
	CoreRef.mSubReg[CU16(ESubRegAddr::MEBI_Map1_Begin) + CU8(ERegAddr::IRQCR)] = IRQCR_IRQEN;
}

void CCore::CMEBI::Step()
{
	///TODO: void CCore::CMEBI::Step()
}

void CCore::CMEBI::WriteU8(const uint8_t pAddr, const uint8_t pVal)
{
	///TODO(#9): показывать конфигурацию портов.
	if(pAddr == CU8(ERegAddr::IRQCR))
	{
		CoreRef.mSubReg[CU16(ESubRegAddr::MEBI_Map1_Begin) + CU8(ERegAddr::IRQCR)] = pVal & IRQCR_Mask;
	}
	else
	{
		goto UnknownSpace;
	}

	return;

UnknownSpace:

	qFatal("MEBI::WriteU8. Unknown range for address 0x%X", pAddr);
}
