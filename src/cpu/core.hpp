#pragma once

#include <stdint.h>

#include <QtCore/QString>

#include <mcu_def.hpp>

#define CU8(pVal)	static_cast<uint8_t>(pVal)
#define CU16(pVal)	static_cast<uint16_t>(pVal)

class CCore
{
private:

	/*************************************/
	/************ Типы данных ************/
	/*************************************/

	typedef void (CCore::*Func_InstOp)(void);

	struct SRegister
	{
		union
		{
			struct
			{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				uint8_t B;
				uint8_t A;
#else
				uint8_t A;
				uint8_t B;
#endif // __BYTE_ORDER__
			} __attribute__((packed));

			uint16_t D;
		} Accumulator;

		struct
		{
			uint16_t X, Y;///< Индексные регистры.
		} Index;

		uint16_t SP;///< Указатель стека.
		uint16_t PC;///< Счётчик программы.

		union
		{
			struct
			{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				bool C : 1;
				bool V : 1;
				bool Z : 1;
				bool N : 1;
				bool I : 1;
				bool H : 1;
				bool X : 1;
				bool S : 1;
#else
				bool S : 1;
				bool X : 1;
				bool H : 1;
				bool I : 1;
				bool N : 1;
				bool Z : 1;
				bool V : 1;
				bool C : 1;
#endif // __BYTE_ORDER__
			} __attribute__((packed));

			uint8_t All;
		} CC;///< Флаги состояния
	};

	/*************************************/
	/************* Константы *************/
	/*************************************/

	/// \brief Страницы кодов инструкций.
	enum
	{
		IPage_1 = 0,
		IPage_2 = 1,
		IPage_Count
	};

	/// \brief Коды инструкций.
	enum
	{
		IP1_INY      = 0x02,
		IP1_DEY      = 0x03,
		IP1_LoopSet  = 0x04,
		IP1_JMP_05   = 0x05,
		IP1_JMP_06   = 0x06,
		IP1_BSR      = 0x07,
		IP1_INX      = 0x08,
		IP1_DEX      = 0x09,
		IP1_RTC      = 0x0A,
		IP1_BSET_0C  = 0x0C,
		IP1_BCLR_0D  = 0x0D,
		IP1_BRSET_0E = 0x0E,
		IP1_BRCLR_0F = 0x0F,
		IP1_ANDCC    = 0x10,
		IP1_EDIV     = 0x11,
		IP1_MUL      = 0x12,
		IP1_EMUL     = 0x13,
		IP1_ORCC     = 0x14,
		IP1_JSR_15   = 0x15,
		IP1_JSR_16   = 0x16,
		IP1_JSR_17   = 0x17,
		IP1_InstP2   = 0x18,
			IP2_MOVW_00 = 0x00,
			IP2_MOVW_01 = 0x01,
			IP2_MOVW_02 = 0x02,
			IP2_MOVW_03 = 0x03,
			IP2_MOVW_04 = 0x04,
			IP2_MOVW_05 = 0x05,
			IP2_MOVB_08 = 0x08,
			IP2_MOVB_09 = 0x09,
			IP2_MOVB_0A = 0x0A,
			IP2_MOVB_0B = 0x0B,
			IP2_MOVB_0C = 0x0C,
			IP2_MOVB_0D = 0x0D,
			IP2_TAB     = 0x0E,
			IP2_TBA     = 0x0F,
			IP2_IDIV    = 0x10,
			IP2_FDIV    = 0x11,
			IP2_EMULS   = 0x13,
			IP2_EDIVS   = 0x14,
			IP2_IDIVS   = 0x15,
			IP2_CBA     = 0x17,
			IP2_LBRA    = 0x20,
			IP2_LBRN    = 0x21,
			IP2_LBHI    = 0x22,
			IP2_LBLS    = 0x23,
			IP2_LBCC    = 0x24,
			IP2_LBCS    = 0x25,
			IP2_LBNE    = 0x26,
			IP2_LBEQ    = 0x27,
			IP2_LBVC    = 0x28,
			IP2_LBVS    = 0x29,
			IP2_LBPL    = 0x2A,
			IP2_LBMI    = 0x2B,
			IP2_LBGE    = 0x2C,
			IP2_LBLT    = 0x2D,
			IP2_LBGT    = 0x2E,
			IP2_LBLE    = 0x2F,
		IP1_LEAY     = 0x19,
		IP1_LEAX     = 0x1A,
		IP1_LEAS     = 0x1B,
		IP1_BSET_1C  = 0x1C,
		IP1_BCLR_1D  = 0x1D,
		IP1_BRSET_1E = 0x1E,
		IP1_BRCLR_1F = 0x1F,
		IP1_BRA      = 0x20,
		IP1_BRN      = 0x21,
		IP1_BHI      = 0x22,
		IP1_BLS      = 0x23,
		IP1_BCC      = 0x24,
		IP1_BCS      = 0x25,
		IP1_BNE      = 0x26,
		IP1_BEQ      = 0x27,
		IP1_BVC      = 0x28,
		IP1_BVS      = 0x29,
		IP1_BPL      = 0x2A,
		IP1_BMI      = 0x2B,
		IP1_BGE      = 0x2C,
		IP1_BLT      = 0x2D,
		IP1_BGT      = 0x2E,
		IP1_BLE      = 0x2F,
		IP1_PULX     = 0x30,
		IP1_PULY     = 0x31,
		IP1_PULA     = 0x32,
		IP1_PULB     = 0x33,
		IP1_PSHX     = 0x34,
		IP1_PSHY     = 0x35,
		IP1_PSHA     = 0x36,
		IP1_PSHB     = 0x37,
		IP1_PULC     = 0x38,
		IP1_PSHC     = 0x39,
		IP1_PULD     = 0x3A,
		IP1_PSHD     = 0x3B,
		IP1_RTS      = 0x3D,
		IP1_NEGA     = 0x40,
		IP1_COMA     = 0x41,
		IP1_INCA     = 0x42,
		IP1_DECA     = 0x43,
		IP1_LSRA     = 0x44,
		IP1_ROLA     = 0x45,
		IP1_RORA     = 0x46,
		IP1_ASRA     = 0x47,
		IP1_ASLA     = 0x48,
		IP1_LSRD     = 0x49,
		IP1_CALL_4A  = 0x4A,
		IP1_CALL_4B  = 0x4B,
		IP1_BSET_4C  = 0x4C,
		IP1_BCLR_4D  = 0x4D,
		IP1_BRSET_4E = 0x4E,
		IP1_BRCLR_4F = 0x4F,
		IP1_NEGB     = 0x50,
		IP1_COMB     = 0x51,
		IP1_INCB     = 0x52,
		IP1_DECB     = 0x53,
		IP1_LSRB     = 0x54,
		IP1_ROLB     = 0x55,
		IP1_RORB     = 0x56,
		IP1_ASRB     = 0x57,
		IP1_ASLB     = 0x58,
		IP1_ASLD     = 0x59,
		IP1_STAA_5A  = 0x5A,
		IP1_STAB_5B  = 0x5B,
		IP1_STD_5C   = 0x5C,
		IP1_STY_5D   = 0x5D,
		IP1_STX_5E   = 0x5E,
		IP1_STS_5F   = 0x5F,
		IP1_NEG_60   = 0x60,
		IP1_COM_61   = 0x61,
		IP1_INC_62   = 0x62,
		IP1_DEC_63   = 0x63,
		IP1_LSR_64   = 0x64,
		IP1_ROL_65   = 0x65,
		IP1_ROR_66   = 0x66,
		IP1_ASR_67   = 0x67,
		IP1_ASL_68   = 0x68,
		IP1_CLR_69   = 0x69,
		IP1_STAA_6A  = 0x6A,
		IP1_STAB_6B  = 0x6B,
		IP1_STD_6C   = 0x6C,
		IP1_STY_6D   = 0x6D,
		IP1_STX_6E   = 0x6E,
		IP1_STS_6F   = 0x6F,
		IP1_NEG_70   = 0x70,
		IP1_COM_71   = 0x71,
		IP1_INC_72   = 0x72,
		IP1_DEC_73   = 0x73,
		IP1_LSR_74   = 0x74,
		IP1_ROL_75   = 0x75,
		IP1_ROR_76   = 0x76,
		IP1_ASR_77   = 0x77,
		IP1_ASL_78   = 0x78,
		IP1_CLR_79   = 0x79,
		IP1_STAA_7A  = 0x7A,
		IP1_STAB_7B  = 0x7B,
		IP1_STD_7C   = 0x7C,
		IP1_STY_7D   = 0x7D,
		IP1_STX_7E   = 0x7E,
		IP1_STS_7F   = 0x7F,
		IP1_SUBA_80  = 0x80,
		IP1_CMPA_81  = 0x81,
		IP1_SBCA_82  = 0x82,
		IP1_SUBD_83  = 0x83,
		IP1_ANDA_84  = 0x84,
		IP1_LDAA_86  = 0x86,
		IP1_CLRA     = 0x87,
		IP1_EORA_88  = 0x88,
		IP1_ADCA_89  = 0x89,
		IP1_ORAA_8A  = 0x8A,
		IP1_ADDA_8B  = 0x8B,
		IP1_CPD_8C   = 0x8C,
		IP1_CPY_8D   = 0x8D,
		IP1_CPX_8E   = 0x8E,
		IP1_CPS_8F   = 0x8F,
		IP1_SUBA_90  = 0x90,
		IP1_CMPA_91  = 0x91,
		IP1_SBCA_92  = 0x92,
		IP1_SUBD_93  = 0x93,
		IP1_ANDA_94  = 0x94,
		IP1_LDAA_96  = 0x96,
		IP1_TSTA     = 0x97,
		IP1_EORA_98  = 0x98,
		IP1_ADCA_99  = 0x99,
		IP1_ORAA_9A  = 0x9A,
		IP1_ADDA_9B  = 0x9B,
		IP1_CPD_9C   = 0x9C,
		IP1_CPY_9D   = 0x9D,
		IP1_CPX_9E   = 0x9E,
		IP1_CPS_9F   = 0x9F,
		IP1_SUBA_A0  = 0xA0,
		IP1_CMPA_A1  = 0xA1,
		IP1_SBCA_A2  = 0xA2,
		IP1_SUBD_A3  = 0xA3,
		IP1_ANDA_A4  = 0xA4,
		IP1_LDAA_A6  = 0xA6,
		IP1_NOP      = 0xA7,
		IP1_EORA_A8  = 0xA8,
		IP1_ADCA_A9  = 0xA9,
		IP1_ORAA_AA  = 0xAA,
		IP1_ADDA_AB  = 0xAB,
		IP1_CPD_AC   = 0xAC,
		IP1_CPY_AD   = 0xAD,
		IP1_CPX_AE   = 0xAE,
		IP1_CPS_AF   = 0xAF,
		IP1_SUBA_B0  = 0xB0,
		IP1_CMPA_B1  = 0xB1,
		IP1_SBCA_B2  = 0xB2,
		IP1_SUBD_B3  = 0xB3,
		IP1_ANDA_B4  = 0xB4,
		IP1_LDAA_B6  = 0xB6,
		IP1_TFREXG   = 0xB7,
		IP1_EORA_B8  = 0xB8,
		IP1_ADCA_B9  = 0xB9,
		IP1_ORAA_BA  = 0xBA,
		IP1_ADDA_BB  = 0xBB,
		IP1_CPD_BC   = 0xBC,
		IP1_CPY_BD   = 0xBD,
		IP1_CPX_BE   = 0xBE,
		IP1_CPS_BF   = 0xBF,
		IP1_SUBB_C0  = 0xC0,
		IP1_CMPB_C1  = 0xC1,
		IP1_SBCB_C2  = 0xC2,
		IP1_ADDD_C3  = 0xC3,
		IP1_ANDB_C4  = 0xC4,
		IP1_LDAB_C6  = 0xC6,
		IP1_CLRB     = 0xC7,
		IP1_EORB_C8  = 0xC8,
		IP1_ADCB_C9  = 0xC9,
		IP1_ORAB_CA  = 0xCA,
		IP1_ADDB_CB  = 0xCB,
		IP1_LDD_CC   = 0xCC,
		IP1_LDY_CD   = 0xCD,
		IP1_LDX_CE   = 0xCE,
		IP1_LDS_CF   = 0xCF,
		IP1_SUBB_D0  = 0xD0,
		IP1_CMPB_D1  = 0xD1,
		IP1_SBCB_D2  = 0xD2,
		IP1_ADDD_D3  = 0xD3,
		IP1_ANDB_D4  = 0xD4,
		IP1_LDAB_D6  = 0xD6,
		IP1_TSTB     = 0xD7,
		IP1_EORB_D8  = 0xD8,
		IP1_ADCB_D9  = 0xD9,
		IP1_ORAB_DA  = 0xDA,
		IP1_ADDB_DB  = 0xDB,
		IP1_LDD_DC   = 0xDC,
		IP1_LDY_DD   = 0xDD,
		IP1_LDX_DE   = 0xDE,
		IP1_LDS_DF   = 0xDF,
		IP1_SUBB_E0  = 0xE0,
		IP1_CMPB_E1  = 0xE1,
		IP1_SBCB_E2  = 0xE2,
		IP1_ADDD_E3  = 0xE3,
		IP1_ANDB_E4  = 0xE4,
		IP1_LDAB_E6  = 0xE6,
		IP1_TST_E7   = 0xE7,
		IP1_EORB_E8  = 0xE8,
		IP1_ADCB_E9  = 0xE9,
		IP1_ORAB_EA  = 0xEA,
		IP1_ADDB_EB  = 0xEB,
		IP1_LDD_EC   = 0xEC,
		IP1_LDY_ED   = 0xED,
		IP1_LDX_EE   = 0xEE,
		IP1_LDS_EF   = 0xEF,
		IP1_SUBB_F0  = 0xF0,
		IP1_CMPB_F1  = 0xF1,
		IP1_SBCB_F2  = 0xF2,
		IP1_ADDD_F3  = 0xF3,
		IP1_ANDB_F4  = 0xF4,
		IP1_LDAB_F6  = 0xF6,
		IP1_TST_F7   = 0xF7,
		IP1_EORB_F8  = 0xF8,
		IP1_ADCB_F9  = 0xF9,
		IP1_ORAB_FA  = 0xFA,
		IP1_ADDB_FB  = 0xFB,
		IP1_LDD_FC   = 0xFC,
		IP1_LDY_FD   = 0xFD,
		IP1_LDX_FE   = 0xFE,
		IP1_LDS_FF   = 0xFF,
	};

	/// \brief Относительные адреса подблоков внутри области регистров.
	enum class ESubRegAddr : uint16_t
	{
		MEBI_Map1_Begin = 0x0000,
		MEBI_Map1_End   = 0x000F,
		MMC_Map1_Begin  = 0x0010,
		MMC_Map1_End    = 0x0014,
		INT_Map1_Begin  = 0x0015,
		INT_Map1_End    = 0x0016,
		MMC_Map2_Begin  = 0x0017,
		MMC_Map2_End    = 0x0017,
		MiscPer_Begin   = 0x0018,
		MiscPer_End     = 0x001B,
		MMC_Map3_Begin  = 0x001C,
		MMC_Map3_End    = 0x001D,
		MEBI_Map2_Begin = 0x001E,
		MEBI_Map2_End   = 0x001E,
		INT_Map2_Begin  = 0x001F,
		INT_Map2_End    = 0x001F,
		BKP_Begin       = 0x0028,
		BKP_End         = 0x002F,
		MMC_Map4_Begin  = 0x0030,
		MMC_Map4_End    = 0x0031,
		MEBI_Map3_Begin = 0x0032,
		MEBI_Map3_End   = 0x0033,
		CRG_Begin       = 0x0034,
		CRG_End         = 0x003F,
		ECT_Begin       = 0x0040,
		ECT_End         = 0x007F,
		ATD0_Begin      = 0x0080,
		ATD0_End        = 0x009F,
		PWM_Begin       = 0x00A0,
		PWM_End         = 0x00C7,
		SCI0_Begin      = 0x00C8,
		SCI0_End        = 0x00CF,
		SCI1_Begin      = 0x00D0,
		SCI1_End        = 0x00D7,
		SPI0_Begin      = 0x00D8,
		SPI0_End        = 0x00DF,
		SPI1_Begin      = 0x00F0,
		SPI1_End        = 0x00F7,
		SPI2_Begin      = 0x00F8,
		SPI2_End        = 0x00FF,

		EEPROM_Begin    = 0x0110,
		EEPROM_End      = 0x011F,
		ATD1_Begin      = 0x0120,
		ATD1_End        = 0x013F,

		CAN0_Begin      = 0x140,
		CAN0_End        = 0x17F,
		CAN1_Begin      = 0x180,
		CAN1_End        = 0x1BF,
		CAN4_Begin      = 0x280,
		CAN4_End        = 0x2BF,


		PIM_Begin       = 0x0240,
		PIM_End         = 0x027F,
	};

	static constexpr uint16_t Flash_PageSize = 16 * 1024;// Размер страницы флеш-памяти.
	static constexpr uint8_t PPAGE_Mask = MCU_FlashSize / Flash_PageSize - 1;

	/*************************************/
	/************** Подблоки *************/
	/*************************************/

	uint8_t mRAM[MCU_RAMSize];
	uint8_t mEEPROM[MCU_EEPROMSize];
	uint8_t mFlash[MCU_FlashSize];
	uint8_t mSubReg[MCU_RegSize];

	#include "mmc_hpp.inl"
	#include "crg_hpp.inl"
	#include "can_hpp.inl"
	#include "ect_hpp.inl"
	#include "mebi_hpp.inl"
	#include "pim_hpp.inl"
	#include "spi_hpp.inl"

	CMMC mMMC;
	CCAN mCAN0, mCAN1, mCAN4;
	CCRG mCRG;
	CECT mECT;
	CECT mMEBI;
	CPIM mPIM;
	CSPI mSPI0, mSPI1, mSPI2;

	/*************************************/
	/************* Переменные ************/
	/*************************************/

	SRegister mRegister;
	Func_InstOp mInst_Op[IPage_Count][256];
	uint8_t mInst_Cur;
	uint16_t mInst_PC;///< Адрес исполняемой инструкции.

	/*************************************/
	/************** Функции **************/
	/*************************************/

	CCore(const CCore&) = delete;
	CCore(const CCore&&) = delete;

	/// \brief Сохранить ОЗУ в файл.
	bool DumpRAM(const QString& pFileName);

	void Inst_Fetch();
	void Inst_Execute();
	void Inst_OpP1_Invalid();
	void Inst_OpP2_Invalid();

	inline void UpdateConditionFlags_Z_u8(const uint8_t pValue) { mRegister.CC.Z = (pValue == 0); }
	inline void UpdateConditionFlags_Z_u16(const uint16_t pValue) { mRegister.CC.Z = (pValue == 0); }

	inline void UpdateConditionFlags_NZ_u8(const uint8_t pValue)
	{
		mRegister.CC.N = ((pValue & (1 << 7)) != 0);
		mRegister.CC.Z = (pValue == 0);
	}

	inline void UpdateConditionFlags_NZ_u16(const uint16_t pValue)
	{
		mRegister.CC.N = ((pValue & (1 << 15)) != 0);
		mRegister.CC.Z = (pValue == 0);
	}

	inline void UpdateConditionFlags_NZ_u32(const uint32_t pValue)
	{
		mRegister.CC.N = ((pValue & (1 << 31)) != 0);
		mRegister.CC.Z = (pValue == 0);
	}

	inline void UpdateConditionFlags_NZV0(const uint8_t pValue)
	{
		UpdateConditionFlags_NZ_u8(pValue);
		mRegister.CC.V = false;
	}

	inline void UpdateConditionFlags_NZV0C0(const uint8_t pValue)
	{
		UpdateConditionFlags_NZV0(pValue);
		mRegister.CC.C = false;
	}

	inline void UpdateConditionFlags_NZV0(const uint16_t pValue)
	{
		mRegister.CC.N = ((pValue & 0x8000) != 0);
		mRegister.CC.Z = (pValue == 0);
		mRegister.CC.V = false;
	}

	inline void UpdateConditionFlags_Add_NZV_u8(const uint8_t pArg1, const uint8_t pArg2, const uint8_t pSumm)
	{
		UpdateConditionFlags_NZ_u8(pSumm);
		mRegister.CC.V = ( ( ((pArg1 & pArg2 & ~pSumm) | (~pArg1 & ~pArg2 & pSumm)) & (1 << 7) ) != 0);
	}

	inline void UpdateConditionFlags_Add_HNZVC_u8(const uint8_t pArg1, const uint8_t pArg2, const uint8_t pSumm)
	{
		UpdateConditionFlags_Add_NZV_u8(pArg1, pArg2, pSumm);
		mRegister.CC.H = ( ( ((pArg1 & pArg2) | (pArg2 & ~pSumm) | (~pSumm & pArg1)) & (1 << 3) ) != 0 );
		mRegister.CC.C = ( ( ((pArg1 & pArg2) | (pArg2 & ~pSumm) | (~pSumm & pArg1)) & (1 << 7) ) != 0);
	}

	inline void UpdateConditionFlags_Add_NZVC_u16(const uint16_t pArg1, const uint16_t pArg2, const uint16_t pSumm)
	{
		UpdateConditionFlags_NZ_u16(pSumm);
		mRegister.CC.V = ( ( ((pArg1 & pArg2 & ~pSumm) | (~pArg1 & ~pArg2 & pSumm)) & (1 << 15) ) != 0);
		mRegister.CC.C = ( ( ((pArg1 & pArg2) | (pArg2 & ~pSumm) | (~pSumm & pArg1)) & (1 << 15) ) != 0);
	}

	inline void UpdateConditionFlags_Diff_NZV_u8(const uint8_t pValueFrom, const uint8_t pValueWhat, const uint8_t pDiff)
	{
		UpdateConditionFlags_NZ_u8(pDiff);
		mRegister.CC.V = (( ((pValueFrom & ~pValueWhat & ~pDiff) | (~pValueFrom & pValueWhat & pDiff)) & (1 << 7) ) != 0);
	}

	inline void UpdateConditionFlags_Diff_NZVC_u8(const uint8_t pValueFrom, const uint8_t pValueWhat, const uint8_t pResult)
	{
		UpdateConditionFlags_Diff_NZV_u8(pValueFrom, pValueWhat, pResult);
		mRegister.CC.C = (( ((~pValueFrom & pValueWhat) | (pValueWhat & pResult) | (pResult & ~pValueFrom)) & (1 << 7) ) != 0);
	}

	inline void UpdateConditionFlags_Diff_NZVC_u16(const uint16_t pValueFrom, const uint16_t pValueWhat, const uint16_t pResult)
	{
		UpdateConditionFlags_NZ_u16(pResult);
		mRegister.CC.V = (( ((pValueFrom & ~pValueWhat & ~pResult) | (~pValueFrom & pValueWhat & pResult)) & (1 << 15) ) != 0);
		mRegister.CC.C = (( ((~pValueFrom & pValueWhat) | (pValueWhat & pResult) | (pResult & ~pValueFrom)) & (1 << 15) ) != 0);
	}

	inline void UpdateConditionFlags_ForClear()
	{
		mRegister.CC.N = false;
		mRegister.CC.Z = true;
		mRegister.CC.V = false;
		mRegister.CC.C = false;
	}

	/// \return Данные в режиме адресации - IMM.
	inline uint8_t DataByMode_IMM_8i()
	{
		uint8_t tval8 = mMMC.ReadU8(mRegister.PC);

		mRegister.PC++;

		return tval8;
	}

	/// \return Данные в режиме адресации - IMM.
	inline uint16_t DataByMode_IMM_16i() { return DataByMode_EXT(); }

	/// \return Данные в режиме адресации - DIR.
	inline uint16_t DataByMode_DIR()
	{
		uint16_t tval16 = CU16(mMMC.ReadU8(mRegister.PC));

		mRegister.PC++;

		return tval16;
	}

	/// \return Данные в режиме адресации - EXT.
	inline uint16_t DataByMode_EXT()
	{
		uint16_t tval16 = mMMC.ReadU16(mRegister.PC);

		mRegister.PC += 2;

		return tval16;
	}

	/// \param [in] pIndirect - если используется, то предварительно должно быть записано значение false.
	uint16_t DataByMode_PostByte(uint16_t* pIndirectAddr = nullptr, bool* pIndirect = nullptr);

	inline void Stack_Push(const uint8_t pValue) { mRegister.SP--; mMMC.WriteU8(mRegister.SP, pValue); }
	inline void Stack_Push(const uint16_t pValue) { mRegister.SP -= 2; mMMC.WriteU16(mRegister.SP, pValue); }
	inline void Stack_Pop(uint8_t& pValue) { pValue = mMMC.ReadU8(mRegister.SP); mRegister.SP++; }
	inline void Stack_Pop(uint16_t& pValue) { pValue = mMMC.ReadU16(mRegister.SP); mRegister.SP += 2; }

	inline void RegOp_Load_IMM_8i(uint8_t& pRegister) { pRegister = DataByMode_IMM_8i(); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_IMM_16i(uint16_t& pRegister) { pRegister = DataByMode_IMM_16i(); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_DIR(uint8_t& pRegister) { pRegister = mMMC.ReadU8(DataByMode_DIR()); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_DIR(uint16_t& pRegister) { pRegister = mMMC.ReadU16(DataByMode_DIR()); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_EXT(uint8_t& pRegister) { pRegister = mMMC.ReadU8(DataByMode_EXT()); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_EXT(uint16_t& pRegister) { pRegister = mMMC.ReadU16(DataByMode_EXT()); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_pb(uint8_t& pRegister) { pRegister = mMMC.ReadU8(DataByMode_PostByte()); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Load_pb(uint16_t& pRegister) { pRegister = mMMC.ReadU16(DataByMode_PostByte()); UpdateConditionFlags_NZV0(pRegister); }

	inline void RegOp_Store_DIR(const uint8_t& pRegister) { mMMC.WriteU8(DataByMode_DIR(), pRegister); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Store_DIR(const uint16_t& pRegister) { mMMC.WriteU16(DataByMode_DIR(), pRegister); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Store_EXT(const uint8_t& pRegister) { mMMC.WriteU8(DataByMode_EXT(), pRegister); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Store_EXT(const uint16_t& pRegister) { mMMC.WriteU16(DataByMode_EXT(), pRegister); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Store_pb(const uint8_t& pRegister) { mMMC.WriteU8(DataByMode_PostByte(), pRegister); UpdateConditionFlags_NZV0(pRegister); }
	inline void RegOp_Store_pb(const uint16_t& pRegister) { mMMC.WriteU16(DataByMode_PostByte(), pRegister); UpdateConditionFlags_NZV0(pRegister); }

	inline void RegOp_Compare_u8(const uint8_t& pRegister, const uint8_t pSubtrahend) { UpdateConditionFlags_Diff_NZVC_u8(pRegister, pSubtrahend, pRegister - pSubtrahend); }
	inline void RegOp_Compare_u16(const uint16_t& pRegister, const uint16_t pSubtrahend) { UpdateConditionFlags_Diff_NZVC_u16(pRegister, pSubtrahend, pRegister - pSubtrahend); }

	///TODO: RegOp_Compare_ux
	inline void RegOp_Compare_IMM_8i(const uint8_t& pRegister) { uint8_t subt = DataByMode_IMM_8i(); UpdateConditionFlags_Diff_NZVC_u8(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_IMM_16i(const uint16_t& pRegister) { uint16_t subt = DataByMode_IMM_16i(); UpdateConditionFlags_Diff_NZVC_u16(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_DIR(const uint8_t& pRegister) { uint8_t subt = mMMC.ReadU8(DataByMode_DIR()); UpdateConditionFlags_Diff_NZVC_u8(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_DIR(const uint16_t& pRegister) { uint16_t subt = mMMC.ReadU16(DataByMode_DIR()); UpdateConditionFlags_Diff_NZVC_u16(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_EXT(const uint8_t& pRegister) { uint8_t subt = mMMC.ReadU8(DataByMode_EXT()); UpdateConditionFlags_Diff_NZVC_u8(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_EXT(const uint16_t& pRegister) { uint16_t subt = mMMC.ReadU16(DataByMode_EXT()); UpdateConditionFlags_Diff_NZVC_u16(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_pb(const uint8_t& pRegister) { uint8_t subt = mMMC.ReadU8(DataByMode_PostByte()); UpdateConditionFlags_Diff_NZVC_u8(pRegister, subt, pRegister - subt); }
	inline void RegOp_Compare_pb(const uint16_t& pRegister) { uint16_t subt = mMMC.ReadU16(DataByMode_PostByte()); UpdateConditionFlags_Diff_NZVC_u16(pRegister, subt, pRegister - subt); }

	inline void RegOp_AddwoC_u8(uint8_t& pRegister, const uint8_t pArg)
	{
		uint8_t arg1 = pRegister;

		pRegister += pArg;
		UpdateConditionFlags_Add_HNZVC_u8(arg1, pArg, pRegister);
	}

	inline void RegOp_AddwoC_u16(uint16_t& pRegister, const uint16_t pArg)
	{
		uint16_t arg1 = pRegister;

		pRegister += pArg;
		UpdateConditionFlags_Add_NZVC_u16(arg1, pArg, pRegister);
	}

	inline void RegOp_AddC_u8(uint8_t& pRegister, const uint8_t pArg)
	{
		uint8_t arg1 = pRegister;

		pRegister += pArg;
		if(mRegister.CC.C) pRegister++;

		UpdateConditionFlags_Add_HNZVC_u8(arg1, pArg, pRegister);
	}

	inline void RegOp_SubwoC_u8(uint8_t& pRegister, const uint8_t pSubtrahend)
	{
		uint8_t res = pRegister - pSubtrahend;

		UpdateConditionFlags_Diff_NZVC_u8(pRegister, pSubtrahend, res);
		pRegister = res;
	}

	inline void RegOp_SubwoC_u16(uint16_t& pRegister, const uint16_t pSubtrahend)
	{
		uint16_t res = pRegister - pSubtrahend;

		UpdateConditionFlags_Diff_NZVC_u16(pRegister, pSubtrahend, res);
		pRegister = res;
	}

	inline void RegOp_SubC_u8(uint8_t& pRegister, const uint8_t pSubtrahend)
	{
		uint8_t res = pRegister - pSubtrahend - 1;

		UpdateConditionFlags_Diff_NZVC_u8(pRegister, pSubtrahend, res);
		pRegister = res;
	}

	inline void RegOp_AND_IMM_8i(uint8_t& pRegister)
	{
		pRegister &= DataByMode_IMM_8i();
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_AND_DIR(uint8_t& pRegister)
	{
		pRegister &= mMMC.ReadU8(DataByMode_DIR());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_AND_EXT(uint8_t& pRegister)
	{
		pRegister &= mMMC.ReadU8(DataByMode_EXT());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_AND_pb(uint8_t& pRegister)
	{
		pRegister &= mMMC.ReadU8(DataByMode_PostByte());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_OR_IMM_8i(uint8_t& pRegister)
	{
		pRegister |= DataByMode_IMM_8i();
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_OR_DIR(uint8_t& pRegister)
	{
		pRegister |= mMMC.ReadU8(DataByMode_DIR());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_OR_EXT(uint8_t& pRegister)
	{
		pRegister |= mMMC.ReadU8(DataByMode_EXT());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_OR_pb(uint8_t& pRegister)
	{
		pRegister |= mMMC.ReadU8(DataByMode_PostByte());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_EOR_IMM_8i(uint8_t& pRegister)
	{
		pRegister ^= DataByMode_IMM_8i();
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_EOR_DIR(uint8_t& pRegister)
	{
		pRegister ^= mMMC.ReadU8(DataByMode_DIR());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_EOR_EXT(uint8_t& pRegister)
	{
		pRegister ^= mMMC.ReadU8(DataByMode_EXT());
		UpdateConditionFlags_NZV0(pRegister);
	}

	inline void RegOp_EOR_pb(uint8_t& pRegister)
	{
		pRegister ^= mMMC.ReadU8(DataByMode_PostByte());
		UpdateConditionFlags_NZV0(pRegister);
	}

	/************************* Обработчики инструкций ************************/

	/********** 0x00 **********/
	void Inst_OpP1_INY();
	void Inst_OpP1_DEY();
	void Inst_OpP1_LoopSet();
	void Inst_OpP1_JMP_05();
	void Inst_OpP1_JMP_06();
	void Inst_OpP1_BSR();
	void Inst_OpP1_INX();
	void Inst_OpP1_DEX();
	void Inst_OpP1_RTC();
	void Inst_OpP1_BSET_0C();
	void Inst_OpP1_BCLR_0D();
	void Inst_OpP1_BRSET_0E();
	void Inst_OpP1_BRCLR_0F();
	/********** 0x10 **********/
	void Inst_OpP1_ANDCC();
	void Inst_OpP1_EDIV();
	void Inst_OpP1_MUL();
	void Inst_OpP1_EMUL();
	void Inst_OpP1_ORCC();
	void Inst_OpP1_JSR_15();
	void Inst_OpP1_JSR_16();
	void Inst_OpP1_JSR_17();
	void Inst_OpP1_InstP2();
	void Inst_OpP1_LEAY();
	void Inst_OpP1_LEAX();
	void Inst_OpP1_LEAS();
	void Inst_OpP1_BSET_1C();
	void Inst_OpP1_BCLR_1D();
	void Inst_OpP1_BRSET_1E();
	void Inst_OpP1_BRCLR_1F();
	/********** 0x20 **********/
	void Inst_OpP1_BRA();
	void Inst_OpP1_BRN();
	void Inst_OpP1_BHI();
	void Inst_OpP1_BLS();
	void Inst_OpP1_BCC();
	void Inst_OpP1_BCS();
	void Inst_OpP1_BNE();
	void Inst_OpP1_BEQ();
	void Inst_OpP1_BVC();
	void Inst_OpP1_BVS();
	void Inst_OpP1_BPL();
	void Inst_OpP1_BMI();
	void Inst_OpP1_BGE();
	void Inst_OpP1_BLT();
	void Inst_OpP1_BGT();
	void Inst_OpP1_BLE();
	/********** 0x30 **********/
	void Inst_OpP1_PULX();
	void Inst_OpP1_PULY();
	void Inst_OpP1_PULA();
	void Inst_OpP1_PULB();
	void Inst_OpP1_PSHX();
	void Inst_OpP1_PSHY();
	void Inst_OpP1_PSHA();
	void Inst_OpP1_PSHB();
	void Inst_OpP1_PULC();
	void Inst_OpP1_PSHC();
	void Inst_OpP1_PULD();
	void Inst_OpP1_PSHD();
	void Inst_OpP1_RTS();
	/********** 0x40 **********/
	void Inst_OpP1_NEGA();
	void Inst_OpP1_COMA();
	void Inst_OpP1_INCA();
	void Inst_OpP1_DECA();
	void Inst_OpP1_LSRA();
	void Inst_OpP1_ROLA();
	void Inst_OpP1_RORA();
	void Inst_OpP1_ASRA();
	void Inst_OpP1_ASLA();
	void Inst_OpP1_LSRD();
	void Inst_OpP1_CALL_4A();
	void Inst_OpP1_CALL_4B();
	void Inst_OpP1_BSET_4C();
	void Inst_OpP1_BCLR_4D();
	void Inst_OpP1_BRSET_4E();
	void Inst_OpP1_BRCLR_4F();
	/********** 0x50 **********/
	void Inst_OpP1_NEGB();
	void Inst_OpP1_COMB();
	void Inst_OpP1_INCB();
	void Inst_OpP1_DECB();
	void Inst_OpP1_LSRB();
	void Inst_OpP1_ROLB();
	void Inst_OpP1_RORB();
	void Inst_OpP1_ASRB();
	void Inst_OpP1_ASLB();
	void Inst_OpP1_ASLD();
	void Inst_OpP1_STAA_5A();
	void Inst_OpP1_STAB_5B();
	void Inst_OpP1_STD_5C();
	void Inst_OpP1_STY_5D();
	void Inst_OpP1_STX_5E();
	void Inst_OpP1_STS_5F();
	/********** 0x60 **********/
	void Inst_OpP1_NEG_60();
	void Inst_OpP1_COM_61();
	void Inst_OpP1_INC_62();
	void Inst_OpP1_DEC_63();
	void Inst_OpP1_LSR_64();
	void Inst_OpP1_ROL_65();
	void Inst_OpP1_ROR_66();
	void Inst_OpP1_ASR_67();
	void Inst_OpP1_ASL_68();
	void Inst_OpP1_CLR_69();
	void Inst_OpP1_STAA_6A();
	void Inst_OpP1_STAB_6B();
	void Inst_OpP1_STD_6C();
	void Inst_OpP1_STY_6D();
	void Inst_OpP1_STX_6E();
	void Inst_OpP1_STS_6F();
	/********** 0x70 **********/
	void Inst_OpP1_NEG_70();
	void Inst_OpP1_COM_71();
	void Inst_OpP1_INC_72();
	void Inst_OpP1_DEC_73();
	void Inst_OpP1_LSR_74();
	void Inst_OpP1_ROL_75();
	void Inst_OpP1_ROR_76();
	void Inst_OpP1_ASR_77();
	void Inst_OpP1_ASL_78();
	void Inst_OpP1_CLR_79();
	void Inst_OpP1_STAA_7A();
	void Inst_OpP1_STAB_7B();
	void Inst_OpP1_STD_7C();
	void Inst_OpP1_STY_7D();
	void Inst_OpP1_STX_7E();
	void Inst_OpP1_STS_7F();
	/********** 0x80 **********/
	void Inst_OpP1_SUBA_80();
	void Inst_OpP1_CMPA_81();
	void Inst_OpP1_SBCA_82();
	void Inst_OpP1_SUBD_83();
	void Inst_OpP1_ANDA_84();
	void Inst_OpP1_LDAA_86();
	void Inst_OpP1_CLRA();
	void Inst_OpP1_EORA_88();
	void Inst_OpP1_ADCA_89();
	void Inst_OpP1_ORAA_8A();
	void Inst_OpP1_ADDA_8B();
	void Inst_OpP1_CPD_8C();
	void Inst_OpP1_CPY_8D();
	void Inst_OpP1_CPX_8E();
	void Inst_OpP1_CPS_8F();
	/********** 0x90 **********/
	void Inst_OpP1_SUBA_90();
	void Inst_OpP1_CMPA_91();
	void Inst_OpP1_SBCA_92();
	void Inst_OpP1_SUBD_93();
	void Inst_OpP1_ANDA_94();
	void Inst_OpP1_LDAA_96();
	void Inst_OpP1_TSTA();
	void Inst_OpP1_EORA_98();
	void Inst_OpP1_ADCA_99();
	void Inst_OpP1_ORAA_9A();
	void Inst_OpP1_ADDA_9B();
	void Inst_OpP1_CPD_9C();
	void Inst_OpP1_CPY_9D();
	void Inst_OpP1_CPX_9E();
	void Inst_OpP1_CPS_9F();
	/********** 0xA0 **********/
	void Inst_OpP1_SUBA_A0();
	void Inst_OpP1_CMPA_A1();
	void Inst_OpP1_SBCA_A2();
	void Inst_OpP1_SUBD_A3();
	void Inst_OpP1_ANDA_A4();
	void Inst_OpP1_LDAA_A6();
	void Inst_OpP1_NOP();
	void Inst_OpP1_EORA_A8();
	void Inst_OpP1_ADCA_A9();
	void Inst_OpP1_ORAA_AA();
	void Inst_OpP1_ADDA_AB();
	void Inst_OpP1_CPD_AC();
	void Inst_OpP1_CPY_AD();
	void Inst_OpP1_CPX_AE();
	void Inst_OpP1_CPS_AF();
	/********** 0xB0 **********/
	void Inst_OpP1_SUBA_B0();
	void Inst_OpP1_CMPA_B1();
	void Inst_OpP1_SBCA_B2();
	void Inst_OpP1_SUBD_B3();
	void Inst_OpP1_ANDA_B4();
	void Inst_OpP1_LDAA_B6();
	void Inst_OpP1_TFREXG();
	void Inst_OpP1_EORA_B8();
	void Inst_OpP1_ADCA_B9();
	void Inst_OpP1_ORAA_BA();
	void Inst_OpP1_ADDA_BB();
	void Inst_OpP1_CPD_BC();
	void Inst_OpP1_CPY_BD();
	void Inst_OpP1_CPX_BE();
	void Inst_OpP1_CPS_BF();
	/********** 0xC0 **********/
	void Inst_OpP1_SUBB_C0();
	void Inst_OpP1_CMPB_C1();
	void Inst_OpP1_SBCB_C2();
	void Inst_OpP1_ADDD_C3();
	void Inst_OpP1_ANDB_C4();
	void Inst_OpP1_LDAB_C6();
	void Inst_OpP1_CLRB();
	void Inst_OpP1_EORB_C8();
	void Inst_OpP1_ADCB_C9();
	void Inst_OpP1_ORAB_CA();
	void Inst_OpP1_ADDB_CB();
	void Inst_OpP1_LDD_CC();
	void Inst_OpP1_LDY_CD();
	void Inst_OpP1_LDX_CE();
	void Inst_OpP1_LDS_CF();
	/********** 0xD0 **********/
	void Inst_OpP1_SUBB_D0();
	void Inst_OpP1_CMPB_D1();
	void Inst_OpP1_SBCB_D2();
	void Inst_OpP1_ADDD_D3();
	void Inst_OpP1_ANDB_D4();
	void Inst_OpP1_LDAB_D6();
	void Inst_OpP1_TSTB();
	void Inst_OpP1_EORB_D8();
	void Inst_OpP1_ADCB_D9();
	void Inst_OpP1_ORAB_DA();
	void Inst_OpP1_ADDB_DB();
	void Inst_OpP1_LDD_DC();
	void Inst_OpP1_LDY_DD();
	void Inst_OpP1_LDX_DE();
	void Inst_OpP1_LDS_DF();
	/********** 0xE0 **********/
	void Inst_OpP1_SUBB_E0();
	void Inst_OpP1_CMPB_E1();
	void Inst_OpP1_SBCB_E2();
	void Inst_OpP1_ADDD_E3();
	void Inst_OpP1_ANDB_E4();
	void Inst_OpP1_LDAB_E6();
	void Inst_OpP1_TST_E7();
	void Inst_OpP1_EORB_E8();
	void Inst_OpP1_ADCB_E9();
	void Inst_OpP1_ORAB_EA();
	void Inst_OpP1_ADDB_EB();
	void Inst_OpP1_LDD_EC();
	void Inst_OpP1_LDY_ED();
	void Inst_OpP1_LDX_EE();
	void Inst_OpP1_LDS_EF();
	/********** 0xF0 **********/
	void Inst_OpP1_SUBB_F0();
	void Inst_OpP1_CMPB_F1();
	void Inst_OpP1_SBCB_F2();
	void Inst_OpP1_ADDD_F3();
	void Inst_OpP1_ANDB_F4();
	void Inst_OpP1_LDAB_F6();
	void Inst_OpP1_TST_F7();
	void Inst_OpP1_EORB_F8();
	void Inst_OpP1_ADCB_F9();
	void Inst_OpP1_ORAB_FA();
	void Inst_OpP1_ADDB_FB();
	void Inst_OpP1_LDD_FC();
	void Inst_OpP1_LDY_FD();
	void Inst_OpP1_LDX_FE();
	void Inst_OpP1_LDS_FF();

	/********* 0x1800 *********/
	void Inst_OpP2_MOVW_00();
	void Inst_OpP2_MOVW_01();
	void Inst_OpP2_MOVW_02();
	void Inst_OpP2_MOVW_03();
	void Inst_OpP2_MOVW_04();
	void Inst_OpP2_MOVW_05();
	void Inst_OpP2_MOVB_08();
	void Inst_OpP2_MOVB_09();
	void Inst_OpP2_MOVB_0A();
	void Inst_OpP2_MOVB_0B();
	void Inst_OpP2_MOVB_0C();
	void Inst_OpP2_MOVB_0D();
	void Inst_OpP2_TAB();
	void Inst_OpP2_TBA();

	/********* 0x1810 *********/
	void Inst_OpP2_IDIV();
	void Inst_OpP2_FDIV();
	void Inst_OpP2_EMULS();
	void Inst_OpP2_EDIVS();
	void Inst_OpP2_IDIVS();
	void Inst_OpP2_CBA();

	/********* 0x1820 *********/
	void Inst_OpP2_LBRA();
	void Inst_OpP2_LBRN();
	void Inst_OpP2_LBHI();
	void Inst_OpP2_LBLS();
	void Inst_OpP2_LBCC();
	void Inst_OpP2_LBCS();
	void Inst_OpP2_LBNE();
	void Inst_OpP2_LBEQ();
	void Inst_OpP2_LBVC();
	void Inst_OpP2_LBVS();
	void Inst_OpP2_LBPL();
	void Inst_OpP2_LBMI();
	void Inst_OpP2_LBGE();
	void Inst_OpP2_LBLT();
	void Inst_OpP2_LBGT();
	void Inst_OpP2_LBLE();

public:

	CCore();

	/// \brief Запись данных во флэш-память.
	/// \param [in] pAddr - адрес во флэш-памяти откуда начинается запись данных.
	/// \param [in] pSrc - указатель на источник данных.
	/// \param [in] pSize - размер записываемой порции данных.
	void Flash_WriteArr(const uint32_t pAddr, const uint8_t* pSrc, const uint32_t pSize);

	/// \brief Запись данных в ЭСППЗУ.
	/// \param [in] pAddr - адрес в ЭСППЗУ откуда начинается запись данных.
	/// \param [in] pSrc - указатель на источник данных.
	/// \param [in] pSize - размер записываемой порции данных.
	void EEPROM_WriteArr(const uint16_t pAddr, const uint8_t* pSrc, const uint16_t pSize);

	void Reset();
	void Step();
};
