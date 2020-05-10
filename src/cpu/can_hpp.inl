///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

class CCAN
{
private:
	///TODO(#9): В/В, внешние источники, прерывания.

	/*************************************/
	/************ Типы данных ************/
	/*************************************/

	/*************************************/
	/************* Константы *************/
	/*************************************/

	enum class ERegAddr : uint8_t
	{
		CANCTL0 = 0x00,
		CANCTL1 = 0x01,
		CANBTR0 = 0x02,
		CANBTR1 = 0x03,
		CANRFLG = 0x04,
		CANRIER = 0x05,
		CANTFLG = 0x06,
		CANTIER = 0x07,
		CANTBSEL = 0x0A,
		CANIDAC  = 0x0B,
		CANIDAR0 = 0x10,
		CANIDAR1 = 0x11,
		CANIDAR2 = 0x12,
		CANIDAR3 = 0x13,
		CANIDMR0 = 0x14,
		CANIDMR1 = 0x15,
		CANIDMR2 = 0x16,
		CANIDMR3 = 0x17,
		CANIDAR4 = 0x18,
		CANIDAR5 = 0x19,
		CANIDAR6 = 0x1A,
		CANIDAR7 = 0x1B,
		CANIDMR4 = 0x1C,
		CANIDMR5 = 0x1D,
		CANIDMR6 = 0x1E,
		CANIDMR7 = 0x1F,

		CANTXFG_Beg = 0x30,
		CANTXFG_End = 0x3F,
	};

	enum
	{
		CANCTL0_RXFRM   = (1 << 7),
		CANCTL0_CSWAI   = (1 << 5),
		CANCTL0_TIME    = (1 << 3),
		CANCTL0_WUPE    = (1 << 2),
		CANCTL0_SLPRQ   = (1 << 1),
		CANCTL0_INITRQ  = (1 << 0),
		CANCTL0_MaskWOF = CANCTL0_CSWAI | CANCTL0_TIME | CANCTL0_WUPE | CANCTL0_SLPRQ | CANCTL0_INITRQ,

		CANCTL1_CANE   = (1 << 7),
		CANCTL1_CLKSRC = (1 << 6),
		CANCTL1_LOOPB  = (1 << 5),
		CANCTL1_LISTEN = (1 << 4),
		CANCTL1_WUPM   = (1 << 2),
		CANCTL1_SLPAK  = (1 << 1),
		CANCTL1_INITAK = (1 << 0),
		CANCTL1_Mask   = CANCTL1_CANE | CANCTL1_CLKSRC | CANCTL1_LOOPB | CANCTL1_LISTEN | CANCTL1_WUPM,
		CANCTL1_MaskF  = CANCTL1_SLPAK | CANCTL1_INITAK,

		CANRFLG_RSTAT1 = (1 << 5),
		CANRFLG_RSTAT0 = (1 << 4),
		CANRFLG_TSTAT1 = (1 << 3),
		CANRFLG_TSTAT0 = (1 << 2),
		CANRFLG_MaskF  = CANRFLG_RSTAT1 | CANRFLG_RSTAT0 | CANRFLG_TSTAT1 | CANRFLG_TSTAT0,

		CANTFLG_TXE2 = (1 << 2),
		CANTFLG_TXE1 = (1 << 1),
		CANTFLG_TXE0 = (1 << 0),
		CANTFLG_Mask = CANTFLG_TXE2 | CANTFLG_TXE1 | CANTFLG_TXE0,

		CANTIER_TXEIE2 = (1 << 2),
		CANTIER_TXEIE1 = (1 << 1),
		CANTIER_TXEIE0 = (1 << 0),
		CANTIER_Mask = CANTIER_TXEIE2 | CANTIER_TXEIE1 | CANTIER_TXEIE0,

		CANTBSEL_TXE2 = (1 << 2),
		CANTBSEL_TXE1 = (1 << 1),
		CANTBSEL_TXE0 = (1 << 0),
		CANTBSEL_Mask = CANTBSEL_TXE2 | CANTBSEL_TXE1 | CANTBSEL_TXE0,

		CANIDAC_IDAM1 = (1 << 5),
		CANIDAC_IDAM0 = (1 << 4),
		CANIDAC_Mask  = CANIDAC_IDAM1 | CANIDAC_IDAM0,
	};

	const uint16_t mBaseAddr;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	CCore& CoreRef;

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CCAN() = delete;
	CCAN(const CCAN&) = delete;
	CCAN(const CCAN&&) = delete;

public:

	explicit CCAN(CCore& pCore, const ESubRegAddr pBaseAddr);

	void Reset();

	void Step();

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint8_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(CU8(pAddr), pVal); }
};
