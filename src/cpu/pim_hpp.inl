///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

class CPIM
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
		// Port T
		PTT  = 0x00,
		PTIT = 0x01,
		DDRT = 0x02,
		RDRT = 0x03,
		PERT = 0x04,
		PPST = 0x05,
		// Port S
		PTS  = 0x08,
		PTIS = 0x09,
		DDRS = 0x0A,
		RDRS = 0x0B,
		PERS = 0x0C,
		PPSS = 0x0D,
		WOMS = 0x0E,
		// Port M
		PTM  = 0x10,
		PTIM = 0x11,
		DDRM = 0x12,
		RDRM = 0x13,
		PERM = 0x14,
		PPSM = 0x15,
		WOMM = 0x16,
		MODRR = 0x17,
		// Port P
		PTP  = 0x18,
		PTIP = 0x19,
		DDRP = 0x1A,
		RDRP = 0x1B,
		PERP = 0x1C,
		PPSP = 0x1D,
		PIEP = 0x1E,
		PIFP = 0x1F,
		// Port H
		PTH  = 0x20,
		PTIH = 0x21,
		DDRH = 0x22,
		RDRH = 0x23,
		PERH = 0x24,
		PPSH = 0x25,
		PIEH = 0x26,
		PIFH = 0x27,
		// Port J
		PTJ  = 0x28,
		PTIJ = 0x29,
		DDRJ = 0x2A,
		RDRJ = 0x2B,
		PERJ = 0x2C,
		PPSJ = 0x2D,
		PIEJ = 0x2E,
		PIFJ = 0x2F,
	};

	enum
	{
	};

	CCore& CoreRef;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CPIM() = delete;
	CPIM(const CPIM&) = delete;
	CPIM(const CPIM&&) = delete;

public:

	explicit CPIM(CCore& pCore);

	void Reset();

	void Step();

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint8_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(CU8(pAddr), pVal); }
};
