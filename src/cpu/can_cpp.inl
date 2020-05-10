///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

CCore::CCAN::CCAN(CCore& pCore, const ESubRegAddr pBaseAddr)
	: mBaseAddr(CU16(pBaseAddr)), CoreRef(pCore)
{
	Reset();
}

void CCore::CCAN::Reset()
{
	///TODO: void CCore::CCAN::Reset()
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANCTL0)] = CANCTL0_INITRQ;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANCTL1)] = CANCTL1_LISTEN | CANCTL1_INITAK;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANBTR0)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANBTR1)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANRFLG)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANRIER)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANTFLG)] = CANTFLG_Mask;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANTIER)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAC)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAR0)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAR1)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAR2)] = 0;
	CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAR3)] = 0;
}

void CCore::CCAN::Step()
{
	///TODO: void CCore::CCAN::Step()
}

void CCore::CCAN::WriteU8(const uint8_t pAddr, const uint8_t pVal)
{
uint8_t tval8;

	///TODO(#9): показывать конфигурацию портов.
	if((pAddr >= CU8(ERegAddr::CANIDAR0)) && (pAddr <= CU8(ERegAddr::CANIDMR7)))
	{
		CoreRef.mSubReg[mBaseAddr + pAddr] = pVal;

		return;
	}

	if((pAddr >= CU8(ERegAddr::CANTXFG_Beg)) && (pAddr <= CU8(ERegAddr::CANTXFG_End)))
	{
		CoreRef.mSubReg[mBaseAddr + pAddr] = pVal;

		return;
	}

	switch(pAddr)
	{
		case CU8(ERegAddr::CANCTL0):
			tval8 = CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANCTL0)];
			tval8 = (tval8 & ~CANCTL0_MaskWOF) | (pVal & CANCTL0_MaskWOF);
			if(pVal & CANCTL0_RXFRM) tval8 &= ~CANCTL0_RXFRM;

			CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANCTL0)] = tval8;

			break;
		case CU8(ERegAddr::CANCTL1):
			tval8 = (pVal & CANCTL0_MaskWOF) | (CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANCTL1)] & CANCTL1_MaskF);
			CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANCTL1)] = tval8;

			break;
		case CU8(ERegAddr::CANBTR0): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANBTR0)] = pVal; break;
		case CU8(ERegAddr::CANBTR1): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANBTR1)] = pVal; break;
		case CU8(ERegAddr::CANRFLG): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANRFLG)] &= ~(pVal & ~CANRFLG_MaskF); break;
		case CU8(ERegAddr::CANRIER): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANRIER)] = pVal; break;
		case CU8(ERegAddr::CANTFLG): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANTFLG)] &= ~(pVal & CANTFLG_Mask); break;
		case CU8(ERegAddr::CANTIER): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANTIER)] = pVal & CANTIER_Mask; break;
		case CU8(ERegAddr::CANTBSEL): CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANTBSEL)] = pVal & CANTBSEL_Mask; break;
		case CU8(ERegAddr::CANIDAC):
			tval8 = (pVal & CANIDAC_Mask) | (CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAC)] & ~CANIDAC_Mask);
			CoreRef.mSubReg[mBaseAddr + CU8(ERegAddr::CANIDAC)] = tval8;

			break;
		default:
			goto UnknownSpace;
	}

	return;

UnknownSpace:

	qFatal("CAN::WriteU8. Unknown range for address 0x%X", pAddr);
}
