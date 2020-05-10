///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

CCore::CPIM::CPIM(CCore& pCore)
	: CoreRef(pCore)
{
	Reset();
}

void CCore::CPIM::Reset()
{
	///TODO: void CCore::CPIM::Reset()
}

void CCore::CPIM::Step()
{
	///TODO: void CCore::CPIM::Step()
}

void CCore::CPIM::WriteU8(const uint8_t pAddr, const uint8_t pVal)
{
	///TODO(#9): показывать конфигурацию портов.
	// Port T
	if(((pAddr >= CU8(ERegAddr::PTT)) && (pAddr <= CU8(ERegAddr::PPST))) || ((pAddr >= CU8(ERegAddr::PTS)) && (pAddr <= CU8(ERegAddr::WOMS))) ||
		((pAddr >= CU8(ERegAddr::PTM)) && (pAddr <= CU8(ERegAddr::MODRR))) || ((pAddr >= CU8(ERegAddr::PTP)) && (pAddr <= CU8(ERegAddr::PIFP))) ||
		((pAddr >= CU8(ERegAddr::PTJ)) && (pAddr <= CU8(ERegAddr::PIFJ))) || ((pAddr >= CU8(ERegAddr::PTH)) && (pAddr <= CU8(ERegAddr::PIFH))))
	{
		CoreRef.mSubReg[CU16(ESubRegAddr::PIM_Begin) + pAddr] = pVal;
	}
	else
	{
		goto UnknownSpace;
	}

	return;

UnknownSpace:

	qFatal("PIM::WriteU8. Unknown range for address 0x%X", pAddr);
}
