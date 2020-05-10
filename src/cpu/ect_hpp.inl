///NOTE: Файл используется как включение. Не использовать как самостоятельную транслируюмую единицу.

class CECT
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
		TIOS   = 0x00,
		TCNT_H = 0x04,
		TCNT_L = 0x05,
		TSCR1  = 0x06,
		TIE    = 0x0C,
		TC4_H  = 0x18,
		TC4_L  = 0x19,
	};

	enum
	{
		TSCR1_Mask = 0xF0
	};

	CCore& CoreRef;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CECT() = delete;
	CECT(const CECT&) = delete;
	CECT(const CECT&&) = delete;

public:

	explicit CECT(CCore& pCore);

	void Reset();

	void Step();

	/// \brief Запись данных по указанному адресу.
	/// \param [in] pAddr - адрес.
	/// \param [in] pVal - записываемое значение.
	void WriteU8(const uint8_t pAddr, const uint8_t pVal);

	/// \overload void WriteU8(const uint16_t pAddr, const uint8_t pVal)
	inline void WriteU8(const ERegAddr pAddr, const uint8_t pVal) { WriteU8(CU8(pAddr), pVal); }
};
