///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

class CCRG
{
private:
	///TODO(#9): отслеживание режимов.

	/*************************************/
	/************ Типы данных ************/
	/*************************************/

	/*************************************/
	/************* Константы *************/
	/*************************************/

	enum class ERegAddr : uint8_t
	{
		SYNR   = 0x00,
		REFDV  = 0x01,
		CTFLG  = 0x02,
		CRGFLG = 0x03,
		CRGINT = 0x04,
		CLKSEL = 0x05,
		PLLCTL = 0x06,
		RTICTL = 0x07,
		COPCTL = 0x08,
		FORBYP = 0x09,
		CTCTL  = 0x0A,
		ARMCOP = 0x0B
	};

	enum
	{
		ESYNR_Mask     = 0x3F,
		EREFDV_Mask    = 0x0F,
		ECRGFLG_RTIF   = (1 << 7),
		ECRGFLG_PORF   = (1 << 6),
		ECRGFLG_LVRF   = (1 << 5),
		ECRGFLG_LOCKIF = (1 << 4),
		ECRGFLG_LOCK   = (1 << 3),
		ECRGFLG_SCMIF  = (1 << 1),

		ECRGINT_RTIE   = (1 << 7),
		ECRGINT_LOCKIE = (1 << 4),
		ECRGINT_SCMIE  = (1 << 1),

		EPLLCTL_CME   = (1 << 7),
		EPLLCTL_PLLON = (1 << 6),
		EPLLCTL_AUTO  = (1 << 5),
		EPLLCTL_ACQ   = (1 << 4),
		EPLLCTL_PRE   = (1 << 2),
		EPLLCTL_PCE   = (1 << 1),
		EPLLCTL_SCME  = (1 << 0),
		EPLLCTL_Mask  = EPLLCTL_CME | EPLLCTL_PLLON | EPLLCTL_AUTO | EPLLCTL_ACQ | EPLLCTL_PRE | EPLLCTL_PCE | EPLLCTL_SCME,

		ERTICTL_Mask  = 0x7F,
		ECOPCTL_Mask  = 0xC7,
	};

	CCore& CoreRef;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CCRG() = delete;
	CCRG(const CCRG&) = delete;
	CCRG(const CCRG&&) = delete;

public:

	explicit CCRG(CCore& pCore);

	void Reset();

	void Step();

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint8_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(CU8(pAddr), pVal); }
};
