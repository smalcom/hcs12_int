///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

class CSPI
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
		SPICR1 = 0x00,
		SPICR2 = 0x01,
		SPIBR  = 0x02,
		SPISR  = 0x03,
		SPIDR  = 0x05
	};

	enum
	{
		SPICR1_CPHA = (1 << 2),

		SPICR2_MODFEN  = (1 << 4),
		SPICR2_BIDIROE = (1 << 3),
		SPICR2_SPISWAI = (1 << 1),
		SPICR2_SPC0    = (1 << 0),
		SPICR2_Mask = SPICR2_MODFEN | SPICR2_BIDIROE| SPICR2_SPISWAI | SPICR2_SPC0,

		SPIBR_SPPR2 = (1 << 6),
		SPIBR_SPPR1 = (1 << 5),
		SPIBR_SPPR0 = (1 << 4),
		SPIBR_SPR2  = (1 << 2),
		SPIBR_SPR1  = (1 << 1),
		SPIBR_SPR0  = (1 << 0),
		SPIBR_Mask  = SPIBR_SPPR2 | SPIBR_SPPR1 | SPIBR_SPPR0 | SPIBR_SPR2 | SPIBR_SPR1 | SPIBR_SPR0,

		SPISR_SPIF  = (1 << 7),
		SPISR_SPTEF = (1 << 5),
		SPISR_MODF  = (1 << 4),
		SPISR_Mask  = SPISR_SPIF | SPISR_SPTEF | SPISR_MODF,
	};

	const uint16_t mBaseAddr;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	CCore& CoreRef;

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CSPI() = delete;
	CSPI(const CSPI&) = delete;
	CSPI(const CSPI&&) = delete;

public:

	explicit CSPI(CCore& pCore, const ESubRegAddr pBaseAddr);

	void Reset();

	void Step();

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint8_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(CU8(pAddr), pVal); }
};
