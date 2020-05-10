///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

class CMEBI
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
		IRQCR = 0x1E
	};

	enum
	{
		IRQCR_IRQEN = (1 << 6),
		IRQCR_Mask  = 0xC0
	};

	CCore& CoreRef;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CMEBI() = delete;
	CMEBI(const CMEBI&) = delete;
	CMEBI(const CMEBI&&) = delete;

public:

	explicit CMEBI(CCore& pCore);

	void Reset();

	void Step();

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint8_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(CU8(pAddr), pVal); }
};
