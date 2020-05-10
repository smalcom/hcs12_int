///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

///TODO(#9): Режимы отображения памяти "EXPANDED" и "SPECIAL SINGLE CHIP" не реализованы. Реализован только "NORMAL SINGLE CHIP"
class CMMC
{
private:

	/*************************************/
	/************ Типы данных ************/
	/*************************************/

	/// \brief Информация об отображении участков памяти в адресное пространство. Должна заполняться при записи в регистры.
	struct SMapping
	{
		uint16_t Register;
		uint16_t RAM;
		uint16_t EEPROM;
		uint16_t Flash;
	};

	/*************************************/
	/************* Константы *************/
	/*************************************/

public:

	enum class ERegAddr : uint8_t
	{
		INITRM  = 0x10,
		INITRG  = 0x11,
		INITEE  = 0x12,
		MISC    = 0x13,
		MTST0   = 0x14,
		MTST1   = 0x17,
		MEMSIZ0 = 0x1C,
		MEMSIZ1 = 0x1D,
		PPAGE   = 0x30
	};

private:

	enum
	{
		EINITRM_AddrMask = 0xF8,
		EINITRM_RAMHAL   = (1 << 0),
		EINITRG_AddrMask = 0x78,
		EINITEE_AddrMask = 0xF8,
		EINITEE_EEON     = (1 << 0),
		EMISC_WrMask     = 0x0F,
		EMISC_ROMON      = (1 << 0),
		EMISC_ROMHM      = (1 << 1),
		EPPAGE_AddrMask  = 0x3F
	};

	CCore& CoreRef;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	SMapping Mapping;

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CMMC() = delete;
	CMMC(const CMMC&) = delete;
	CMMC(const CMMC&&) = delete;

	/// \param [in] pPageIndex - индекс страницы во флеш-памяти.
	/// \return Адрес указанной страницы во флеш-памяти.
	inline uint32_t Flash_PageAddress(const uint8_t pPageIndex) const { return static_cast<uint32_t>(pPageIndex) * Flash_PageSize; }

public:

	explicit CMMC(CCore& pCore);

	void Reset();

	/// \brief Чтение данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \return прочитанное значение.
	uint8_t ReadU8(const uint16_t pAddr) const;

	/// \brief Чтение регистров MMC.
	inline uint8_t ReadU8(const ERegAddr pAddr) const { return ReadU8(Mapping.Register + CU16(pAddr)); }

	/// \overload uint8_t ReadU8(const uint16_t pAddr)
	uint16_t ReadU16(const uint16_t pAddr) const;

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint16_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(Mapping.Register + CU16(pAddr), pVal); }

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	void WriteU16(const uint16_t pAddr, const uint16_t pVal);
};
