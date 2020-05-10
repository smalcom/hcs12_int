#include "core.hpp"

#include <QtCore/QFile>

#include "interrupt.hpp"
#include <mcu.hpp>

/*************************************/
/************** Подблоки *************/
/*************************************/

#include "mmc_cpp.inl"
#include "crg_cpp.inl"
#include "can_cpp.inl"
#include "ect_cpp.inl"
#include "mebi_cpp.inl"
#include "pim_cpp.inl"
#include "spi_cpp.inl"

/*************************************/
/************** Функции **************/
/*************************************/

CCore::CCore()
	: mMMC(*this), mCAN0(*this, ESubRegAddr::CAN0_Begin), mCAN1(*this, ESubRegAddr::CAN1_Begin), mCAN4(*this, ESubRegAddr::CAN4_Begin), mCRG(*this), mECT(*this), mMEBI(*this),
		mPIM(*this), mSPI0(*this, ESubRegAddr::SPI0_Begin), mSPI1(*this, ESubRegAddr::SPI1_Begin), mSPI2(*this, ESubRegAddr::SPI2_Begin)
{
	// Устанавливаем исходные обработчики инструкций.
	for(size_t ii = 0; ii < 256; ii++)
	{
		mInst_Op[IPage_1][ii] = &CCore::Inst_OpP1_Invalid;
		mInst_Op[IPage_2][ii] = &CCore::Inst_OpP2_Invalid;
	}

	/********** 0x00 **********/
	mInst_Op[IPage_1][IP1_INY]      = &CCore::Inst_OpP1_INY;
	mInst_Op[IPage_1][IP1_DEY]      = &CCore::Inst_OpP1_DEY;
	mInst_Op[IPage_1][IP1_LoopSet]  = &CCore::Inst_OpP1_LoopSet;
	mInst_Op[IPage_1][IP1_JMP_05]   = &CCore::Inst_OpP1_JMP_05;
	mInst_Op[IPage_1][IP1_JMP_06]   = &CCore::Inst_OpP1_JMP_06;
	mInst_Op[IPage_1][IP1_BSR]      = &CCore::Inst_OpP1_BSR;
	mInst_Op[IPage_1][IP1_INX]      = &CCore::Inst_OpP1_INX;
	mInst_Op[IPage_1][IP1_DEX]      = &CCore::Inst_OpP1_DEX;
	mInst_Op[IPage_1][IP1_RTC]      = &CCore::Inst_OpP1_RTC;
	mInst_Op[IPage_1][IP1_BSET_0C]  = &CCore::Inst_OpP1_BSET_0C;
	mInst_Op[IPage_1][IP1_BCLR_0D]  = &CCore::Inst_OpP1_BCLR_0D;
	mInst_Op[IPage_1][IP1_BRSET_0E] = &CCore::Inst_OpP1_BRSET_0E;
	mInst_Op[IPage_1][IP1_BRCLR_0F] = &CCore::Inst_OpP1_BRCLR_0F;
	/********** 0x10 **********/
	mInst_Op[IPage_1][IP1_ANDCC]    = &CCore::Inst_OpP1_ANDCC;
	mInst_Op[IPage_1][IP1_EDIV]     = &CCore::Inst_OpP1_EDIV;
	mInst_Op[IPage_1][IP1_MUL]      = &CCore::Inst_OpP1_MUL;
	mInst_Op[IPage_1][IP1_EMUL]     = &CCore::Inst_OpP1_EMUL;
	mInst_Op[IPage_1][IP1_ORCC]     = &CCore::Inst_OpP1_ORCC;
	mInst_Op[IPage_1][IP1_JSR_15]   = &CCore::Inst_OpP1_JSR_15;
	mInst_Op[IPage_1][IP1_JSR_16]   = &CCore::Inst_OpP1_JSR_16;
	mInst_Op[IPage_1][IP1_JSR_17]   = &CCore::Inst_OpP1_JSR_17;
	mInst_Op[IPage_1][IP1_InstP2]   = &CCore::Inst_OpP1_InstP2;
	mInst_Op[IPage_1][IP1_LEAY]     = &CCore::Inst_OpP1_LEAY;
	mInst_Op[IPage_1][IP1_LEAX]     = &CCore::Inst_OpP1_LEAX;
	mInst_Op[IPage_1][IP1_LEAS]     = &CCore::Inst_OpP1_LEAS;
	mInst_Op[IPage_1][IP1_BSET_1C]  = &CCore::Inst_OpP1_BSET_1C;
	mInst_Op[IPage_1][IP1_BCLR_1D]  = &CCore::Inst_OpP1_BCLR_1D;
	mInst_Op[IPage_1][IP1_BRSET_1E] = &CCore::Inst_OpP1_BRSET_1E;
	mInst_Op[IPage_1][IP1_BRCLR_1F] = &CCore::Inst_OpP1_BRCLR_1F;
	/********** 0x20 **********/
	mInst_Op[IPage_1][IP1_BRA]      = &CCore::Inst_OpP1_BRA;
	mInst_Op[IPage_1][IP1_BRN]      = &CCore::Inst_OpP1_BRN;
	mInst_Op[IPage_1][IP1_BHI]      = &CCore::Inst_OpP1_BHI;
	mInst_Op[IPage_1][IP1_BLS]      = &CCore::Inst_OpP1_BLS;
	mInst_Op[IPage_1][IP1_BCC]      = &CCore::Inst_OpP1_BCC;
	mInst_Op[IPage_1][IP1_BCS]      = &CCore::Inst_OpP1_BCS;
	mInst_Op[IPage_1][IP1_BNE]      = &CCore::Inst_OpP1_BNE;
	mInst_Op[IPage_1][IP1_BEQ]      = &CCore::Inst_OpP1_BEQ;
	mInst_Op[IPage_1][IP1_BVC]      = &CCore::Inst_OpP1_BVC;
	mInst_Op[IPage_1][IP1_BVS]      = &CCore::Inst_OpP1_BVS;
	mInst_Op[IPage_1][IP1_BPL]      = &CCore::Inst_OpP1_BPL;
	mInst_Op[IPage_1][IP1_BMI]      = &CCore::Inst_OpP1_BMI;
	mInst_Op[IPage_1][IP1_BGE]      = &CCore::Inst_OpP1_BGE;
	mInst_Op[IPage_1][IP1_BLT]      = &CCore::Inst_OpP1_BLT;
	mInst_Op[IPage_1][IP1_BGT]      = &CCore::Inst_OpP1_BGT;
	mInst_Op[IPage_1][IP1_BLE]      = &CCore::Inst_OpP1_BLE;
	/********** 0x30 **********/
	mInst_Op[IPage_1][IP1_PULX]     = &CCore::Inst_OpP1_PULX;
	mInst_Op[IPage_1][IP1_PULY]     = &CCore::Inst_OpP1_PULY;
	mInst_Op[IPage_1][IP1_PULA]     = &CCore::Inst_OpP1_PULA;
	mInst_Op[IPage_1][IP1_PULB]     = &CCore::Inst_OpP1_PULB;
	mInst_Op[IPage_1][IP1_PSHX]     = &CCore::Inst_OpP1_PSHX;
	mInst_Op[IPage_1][IP1_PSHY]     = &CCore::Inst_OpP1_PSHY;
	mInst_Op[IPage_1][IP1_PSHA]     = &CCore::Inst_OpP1_PSHA;
	mInst_Op[IPage_1][IP1_PSHB]     = &CCore::Inst_OpP1_PSHB;
	mInst_Op[IPage_1][IP1_PULC]     = &CCore::Inst_OpP1_PULC;
	mInst_Op[IPage_1][IP1_PSHC]     = &CCore::Inst_OpP1_PSHC;
	mInst_Op[IPage_1][IP1_PULD]     = &CCore::Inst_OpP1_PULD;
	mInst_Op[IPage_1][IP1_PSHD]     = &CCore::Inst_OpP1_PSHD;
	mInst_Op[IPage_1][IP1_RTS]      = &CCore::Inst_OpP1_RTS;
	/********** 0x40 **********/
	mInst_Op[IPage_1][IP1_NEGA]     = &CCore::Inst_OpP1_NEGA;
	mInst_Op[IPage_1][IP1_COMA]     = &CCore::Inst_OpP1_COMA;
	mInst_Op[IPage_1][IP1_INCA]     = &CCore::Inst_OpP1_INCA;
	mInst_Op[IPage_1][IP1_DECA]     = &CCore::Inst_OpP1_DECA;
	mInst_Op[IPage_1][IP1_LSRA]     = &CCore::Inst_OpP1_LSRA;
	mInst_Op[IPage_1][IP1_ROLA]     = &CCore::Inst_OpP1_ROLA;
	mInst_Op[IPage_1][IP1_RORA]     = &CCore::Inst_OpP1_RORA;
	mInst_Op[IPage_1][IP1_ASRA]     = &CCore::Inst_OpP1_ASRA;
	mInst_Op[IPage_1][IP1_ASLA]     = &CCore::Inst_OpP1_ASLA;
	mInst_Op[IPage_1][IP1_LSRD]     = &CCore::Inst_OpP1_LSRD;
	mInst_Op[IPage_1][IP1_CALL_4A]  = &CCore::Inst_OpP1_CALL_4A;
	mInst_Op[IPage_1][IP1_CALL_4B]  = &CCore::Inst_OpP1_CALL_4B;
	mInst_Op[IPage_1][IP1_BSET_4C]  = &CCore::Inst_OpP1_BSET_4C;
	mInst_Op[IPage_1][IP1_BCLR_4D]  = &CCore::Inst_OpP1_BCLR_4D;
	mInst_Op[IPage_1][IP1_BRSET_4E] = &CCore::Inst_OpP1_BRSET_4E;
	mInst_Op[IPage_1][IP1_BRCLR_4F] = &CCore::Inst_OpP1_BRCLR_4F;
	/********** 0x50 **********/
	mInst_Op[IPage_1][IP1_NEGB]     = &CCore::Inst_OpP1_NEGB;
	mInst_Op[IPage_1][IP1_COMB]     = &CCore::Inst_OpP1_COMB;
	mInst_Op[IPage_1][IP1_INCB]     = &CCore::Inst_OpP1_INCB;
	mInst_Op[IPage_1][IP1_DECB]     = &CCore::Inst_OpP1_DECB;
	mInst_Op[IPage_1][IP1_LSRB]     = &CCore::Inst_OpP1_LSRB;
	mInst_Op[IPage_1][IP1_ROLB]     = &CCore::Inst_OpP1_ROLB;
	mInst_Op[IPage_1][IP1_RORB]     = &CCore::Inst_OpP1_RORB;
	mInst_Op[IPage_1][IP1_ASRB]     = &CCore::Inst_OpP1_ASRB;
	mInst_Op[IPage_1][IP1_ASLB]     = &CCore::Inst_OpP1_ASLB;
	mInst_Op[IPage_1][IP1_ASLD]     = &CCore::Inst_OpP1_ASLD;
	mInst_Op[IPage_1][IP1_STAA_5A]  = &CCore::Inst_OpP1_STAA_5A;
	mInst_Op[IPage_1][IP1_STAB_5B]  = &CCore::Inst_OpP1_STAB_5B;
	mInst_Op[IPage_1][IP1_STD_5C]   = &CCore::Inst_OpP1_STD_5C;
	mInst_Op[IPage_1][IP1_STY_5D]   = &CCore::Inst_OpP1_STY_5D;
	mInst_Op[IPage_1][IP1_STS_5F]   = &CCore::Inst_OpP1_STS_5F;
	/********** 0x60 **********/
	mInst_Op[IPage_1][IP1_NEG_60]   = &CCore::Inst_OpP1_NEG_60;
	mInst_Op[IPage_1][IP1_COM_61]   = &CCore::Inst_OpP1_COM_61;
	mInst_Op[IPage_1][IP1_INC_62]   = &CCore::Inst_OpP1_INC_62;
	mInst_Op[IPage_1][IP1_DEC_63]   = &CCore::Inst_OpP1_DEC_63;
	mInst_Op[IPage_1][IP1_LSR_64]   = &CCore::Inst_OpP1_LSR_64;
	mInst_Op[IPage_1][IP1_ROL_65]   = &CCore::Inst_OpP1_ROL_65;
	mInst_Op[IPage_1][IP1_ROR_66]   = &CCore::Inst_OpP1_ROR_66;
	mInst_Op[IPage_1][IP1_ASR_67]   = &CCore::Inst_OpP1_ASR_67;
	mInst_Op[IPage_1][IP1_ASL_68]   = &CCore::Inst_OpP1_ASL_68;
	mInst_Op[IPage_1][IP1_CLR_69]   = &CCore::Inst_OpP1_CLR_69;
	mInst_Op[IPage_1][IP1_STAA_6A]  = &CCore::Inst_OpP1_STAA_6A;
	mInst_Op[IPage_1][IP1_STAB_6B]  = &CCore::Inst_OpP1_STAB_6B;
	mInst_Op[IPage_1][IP1_STD_6C]   = &CCore::Inst_OpP1_STD_6C;
	mInst_Op[IPage_1][IP1_STY_6D]   = &CCore::Inst_OpP1_STY_6D;
	mInst_Op[IPage_1][IP1_STX_6E]   = &CCore::Inst_OpP1_STX_6E;
	mInst_Op[IPage_1][IP1_STS_6F]   = &CCore::Inst_OpP1_STS_6F;
	/********** 0x70 **********/
	mInst_Op[IPage_1][IP1_NEG_70]   = &CCore::Inst_OpP1_NEG_70;
	mInst_Op[IPage_1][IP1_COM_71]   = &CCore::Inst_OpP1_COM_71;
	mInst_Op[IPage_1][IP1_INC_72]   = &CCore::Inst_OpP1_INC_72;
	mInst_Op[IPage_1][IP1_DEC_73]   = &CCore::Inst_OpP1_DEC_73;
	mInst_Op[IPage_1][IP1_LSR_74]   = &CCore::Inst_OpP1_LSR_74;
	mInst_Op[IPage_1][IP1_ROL_75]   = &CCore::Inst_OpP1_ROL_75;
	mInst_Op[IPage_1][IP1_ROR_76]   = &CCore::Inst_OpP1_ROR_76;
	mInst_Op[IPage_1][IP1_ASR_77]   = &CCore::Inst_OpP1_ASR_77;
	mInst_Op[IPage_1][IP1_ASL_78]   = &CCore::Inst_OpP1_ASL_78;
	mInst_Op[IPage_1][IP1_CLR_79]   = &CCore::Inst_OpP1_CLR_79;
	mInst_Op[IPage_1][IP1_STAA_7A]  = &CCore::Inst_OpP1_STAA_7A;
	mInst_Op[IPage_1][IP1_STAB_7B]  = &CCore::Inst_OpP1_STAB_7B;
	mInst_Op[IPage_1][IP1_STD_7C]   = &CCore::Inst_OpP1_STD_7C;
	mInst_Op[IPage_1][IP1_STY_7D]   = &CCore::Inst_OpP1_STY_7D;
	mInst_Op[IPage_1][IP1_STX_7E]   = &CCore::Inst_OpP1_STX_7E;
	mInst_Op[IPage_1][IP1_STS_7F]   = &CCore::Inst_OpP1_STS_7F;
	/********** 0x80 **********/
	mInst_Op[IPage_1][IP1_SUBA_80]  = &CCore::Inst_OpP1_SUBA_80;
	mInst_Op[IPage_1][IP1_CMPA_81]  = &CCore::Inst_OpP1_CMPA_81;
	mInst_Op[IPage_1][IP1_SBCA_82]  = &CCore::Inst_OpP1_SBCA_82;
	mInst_Op[IPage_1][IP1_SUBD_83]  = &CCore::Inst_OpP1_SUBD_83;
	mInst_Op[IPage_1][IP1_ANDA_84]  = &CCore::Inst_OpP1_ANDA_84;
	mInst_Op[IPage_1][IP1_LDAA_86]  = &CCore::Inst_OpP1_LDAA_86;
	mInst_Op[IPage_1][IP1_CLRA]     = &CCore::Inst_OpP1_CLRA;
	mInst_Op[IPage_1][IP1_EORA_88]  = &CCore::Inst_OpP1_EORA_88;
	mInst_Op[IPage_1][IP1_ADCA_89]  = &CCore::Inst_OpP1_ADCA_89;
	mInst_Op[IPage_1][IP1_ORAA_8A]  = &CCore::Inst_OpP1_ORAA_8A;
	mInst_Op[IPage_1][IP1_ADDA_8B]  = &CCore::Inst_OpP1_ADDA_8B;
	mInst_Op[IPage_1][IP1_CPD_8C]   = &CCore::Inst_OpP1_CPD_8C;
	mInst_Op[IPage_1][IP1_CPY_8D]   = &CCore::Inst_OpP1_CPY_8D;
	mInst_Op[IPage_1][IP1_CPX_8E]   = &CCore::Inst_OpP1_CPX_8E;
	mInst_Op[IPage_1][IP1_CPS_8F]   = &CCore::Inst_OpP1_CPS_8F;
	/********** 0x90 **********/
	mInst_Op[IPage_1][IP1_SUBA_90]  = &CCore::Inst_OpP1_SUBA_90;
	mInst_Op[IPage_1][IP1_CMPA_91]  = &CCore::Inst_OpP1_CMPA_91;
	mInst_Op[IPage_1][IP1_SBCA_92]  = &CCore::Inst_OpP1_SBCA_92;
	mInst_Op[IPage_1][IP1_SUBD_93]  = &CCore::Inst_OpP1_SUBD_93;
	mInst_Op[IPage_1][IP1_ANDA_94]  = &CCore::Inst_OpP1_ANDA_94;
	mInst_Op[IPage_1][IP1_LDAA_96]  = &CCore::Inst_OpP1_LDAA_96;
	mInst_Op[IPage_1][IP1_TSTA]     = &CCore::Inst_OpP1_TSTA;
	mInst_Op[IPage_1][IP1_EORA_98]  = &CCore::Inst_OpP1_EORA_98;
	mInst_Op[IPage_1][IP1_ADCA_99]  = &CCore::Inst_OpP1_ADCA_99;
	mInst_Op[IPage_1][IP1_ORAA_9A]  = &CCore::Inst_OpP1_ORAA_9A;
	mInst_Op[IPage_1][IP1_ADDA_9B]  = &CCore::Inst_OpP1_ADDA_9B;
	mInst_Op[IPage_1][IP1_CPD_9C]   = &CCore::Inst_OpP1_CPD_9C;
	mInst_Op[IPage_1][IP1_CPY_9D]   = &CCore::Inst_OpP1_CPY_9D;
	mInst_Op[IPage_1][IP1_CPX_9E]   = &CCore::Inst_OpP1_CPX_9E;
	mInst_Op[IPage_1][IP1_CPS_9F]   = &CCore::Inst_OpP1_CPS_9F;
	/********** 0xA0 **********/
	mInst_Op[IPage_1][IP1_SUBA_A0]  = &CCore::Inst_OpP1_SUBA_A0;
	mInst_Op[IPage_1][IP1_CMPA_A1]  = &CCore::Inst_OpP1_CMPA_A1;
	mInst_Op[IPage_1][IP1_SBCA_A2]  = &CCore::Inst_OpP1_SBCA_A2;
	mInst_Op[IPage_1][IP1_SUBD_A3]  = &CCore::Inst_OpP1_SUBD_A3;
	mInst_Op[IPage_1][IP1_ANDA_A4]  = &CCore::Inst_OpP1_ANDA_A4;
	mInst_Op[IPage_1][IP1_LDAA_A6]  = &CCore::Inst_OpP1_LDAA_A6;
	mInst_Op[IPage_1][IP1_NOP]      = &CCore::Inst_OpP1_NOP;
	mInst_Op[IPage_1][IP1_EORA_A8]  = &CCore::Inst_OpP1_EORA_A8;
	mInst_Op[IPage_1][IP1_ADCA_A9]  = &CCore::Inst_OpP1_ADCA_A9;
	mInst_Op[IPage_1][IP1_ORAA_AA]  = &CCore::Inst_OpP1_ORAA_AA;
	mInst_Op[IPage_1][IP1_ADDA_AB]  = &CCore::Inst_OpP1_ADDA_AB;
	mInst_Op[IPage_1][IP1_CPD_AC]   = &CCore::Inst_OpP1_CPD_AC;
	mInst_Op[IPage_1][IP1_CPY_AD]   = &CCore::Inst_OpP1_CPY_AD;
	mInst_Op[IPage_1][IP1_CPX_AE]   = &CCore::Inst_OpP1_CPX_AE;
	mInst_Op[IPage_1][IP1_CPS_AF]   = &CCore::Inst_OpP1_CPS_AF;
	/********** 0xB0 **********/
	mInst_Op[IPage_1][IP1_SUBA_B0]  = &CCore::Inst_OpP1_SUBA_B0;
	mInst_Op[IPage_1][IP1_CMPA_B1]  = &CCore::Inst_OpP1_CMPA_B1;
	mInst_Op[IPage_1][IP1_SBCA_B2]  = &CCore::Inst_OpP1_SBCA_B2;
	mInst_Op[IPage_1][IP1_SUBD_B3]  = &CCore::Inst_OpP1_SUBD_B3;
	mInst_Op[IPage_1][IP1_ANDA_B4]  = &CCore::Inst_OpP1_ANDA_B4;
	mInst_Op[IPage_1][IP1_LDAA_B6]  = &CCore::Inst_OpP1_LDAA_B6;
	mInst_Op[IPage_1][IP1_TFREXG]   = &CCore::Inst_OpP1_TFREXG;
	mInst_Op[IPage_1][IP1_EORA_B8]  = &CCore::Inst_OpP1_EORA_B8;
	mInst_Op[IPage_1][IP1_ADCA_B9]  = &CCore::Inst_OpP1_ADCA_B9;
	mInst_Op[IPage_1][IP1_ORAA_BA]  = &CCore::Inst_OpP1_ORAA_BA;
	mInst_Op[IPage_1][IP1_ADDA_BB]  = &CCore::Inst_OpP1_ADDA_BB;
	mInst_Op[IPage_1][IP1_CPD_BC]   = &CCore::Inst_OpP1_CPD_BC;
	mInst_Op[IPage_1][IP1_CPY_BD]   = &CCore::Inst_OpP1_CPY_BD;
	mInst_Op[IPage_1][IP1_CPX_BE]   = &CCore::Inst_OpP1_CPX_BE;
	mInst_Op[IPage_1][IP1_CPS_BF]   = &CCore::Inst_OpP1_CPS_BF;
	/********** 0xC0 **********/
	mInst_Op[IPage_1][IP1_SUBB_C0]  = &CCore::Inst_OpP1_SUBB_C0;
	mInst_Op[IPage_1][IP1_CMPB_C1]  = &CCore::Inst_OpP1_CMPB_C1;
	mInst_Op[IPage_1][IP1_SBCB_C2]  = &CCore::Inst_OpP1_SBCB_C2;
	mInst_Op[IPage_1][IP1_ADDD_C3]  = &CCore::Inst_OpP1_ADDD_C3;
	mInst_Op[IPage_1][IP1_ANDB_C4]  = &CCore::Inst_OpP1_ANDB_C4;
	mInst_Op[IPage_1][IP1_LDAB_C6]  = &CCore::Inst_OpP1_LDAB_C6;
	mInst_Op[IPage_1][IP1_CLRB]     = &CCore::Inst_OpP1_CLRB;
	mInst_Op[IPage_1][IP1_EORB_C8]  = &CCore::Inst_OpP1_EORB_C8;
	mInst_Op[IPage_1][IP1_ADCB_C9]  = &CCore::Inst_OpP1_ADCB_C9;
	mInst_Op[IPage_1][IP1_ORAB_CA]  = &CCore::Inst_OpP1_ORAB_CA;
	mInst_Op[IPage_1][IP1_ADDB_CB]  = &CCore::Inst_OpP1_ADDB_CB;
	mInst_Op[IPage_1][IP1_LDD_CC]   = &CCore::Inst_OpP1_LDD_CC;
	mInst_Op[IPage_1][IP1_LDY_CD]   = &CCore::Inst_OpP1_LDY_CD;
	mInst_Op[IPage_1][IP1_LDX_CE]   = &CCore::Inst_OpP1_LDX_CE;
	mInst_Op[IPage_1][IP1_LDS_CF]   = &CCore::Inst_OpP1_LDS_CF;
	/********** 0xD0 **********/
	mInst_Op[IPage_1][IP1_SUBB_D0]  = &CCore::Inst_OpP1_SUBB_D0;
	mInst_Op[IPage_1][IP1_CMPB_D1]  = &CCore::Inst_OpP1_CMPB_D1;
	mInst_Op[IPage_1][IP1_SBCB_D2]  = &CCore::Inst_OpP1_SBCB_D2;
	mInst_Op[IPage_1][IP1_ADDD_D3]  = &CCore::Inst_OpP1_ADDD_D3;
	mInst_Op[IPage_1][IP1_ANDB_D4]  = &CCore::Inst_OpP1_ANDB_D4;
	mInst_Op[IPage_1][IP1_LDAB_D6]  = &CCore::Inst_OpP1_LDAB_D6;
	mInst_Op[IPage_1][IP1_TSTB]     = &CCore::Inst_OpP1_TSTB;
	mInst_Op[IPage_1][IP1_EORB_D8]  = &CCore::Inst_OpP1_EORB_D8;
	mInst_Op[IPage_1][IP1_ADCB_D9]  = &CCore::Inst_OpP1_ADCB_D9;
	mInst_Op[IPage_1][IP1_ORAB_DA]  = &CCore::Inst_OpP1_ORAB_DA;
	mInst_Op[IPage_1][IP1_ADDB_DB]  = &CCore::Inst_OpP1_ADDB_DB;
	mInst_Op[IPage_1][IP1_LDD_DC]   = &CCore::Inst_OpP1_LDD_DC;
	mInst_Op[IPage_1][IP1_LDY_DD]   = &CCore::Inst_OpP1_LDY_DD;
	mInst_Op[IPage_1][IP1_LDX_DE]   = &CCore::Inst_OpP1_LDX_DE;
	mInst_Op[IPage_1][IP1_LDS_DF]   = &CCore::Inst_OpP1_LDS_DF;
	/********** 0xE0 **********/
	mInst_Op[IPage_1][IP1_SUBB_E0]  = &CCore::Inst_OpP1_SUBB_E0;
	mInst_Op[IPage_1][IP1_CMPB_E1]  = &CCore::Inst_OpP1_CMPB_E1;
	mInst_Op[IPage_1][IP1_SBCB_E2]  = &CCore::Inst_OpP1_SBCB_E2;
	mInst_Op[IPage_1][IP1_ADDD_E3]  = &CCore::Inst_OpP1_ADDD_E3;
	mInst_Op[IPage_1][IP1_ANDB_E4]  = &CCore::Inst_OpP1_ANDB_E4;
	mInst_Op[IPage_1][IP1_LDAB_E6]  = &CCore::Inst_OpP1_LDAB_E6;
	mInst_Op[IPage_1][IP1_TST_E7]   = &CCore::Inst_OpP1_TST_E7;
	mInst_Op[IPage_1][IP1_EORB_E8]  = &CCore::Inst_OpP1_EORB_E8;
	mInst_Op[IPage_1][IP1_ADCB_E9]  = &CCore::Inst_OpP1_ADCB_E9;
	mInst_Op[IPage_1][IP1_ORAB_EA]  = &CCore::Inst_OpP1_ORAB_EA;
	mInst_Op[IPage_1][IP1_ADDB_EB]  = &CCore::Inst_OpP1_ADDB_EB;
	mInst_Op[IPage_1][IP1_LDD_EC]   = &CCore::Inst_OpP1_LDD_EC;
	mInst_Op[IPage_1][IP1_LDY_ED]   = &CCore::Inst_OpP1_LDY_ED;
	mInst_Op[IPage_1][IP1_LDX_EE]   = &CCore::Inst_OpP1_LDX_EE;
	mInst_Op[IPage_1][IP1_LDS_EF]   = &CCore::Inst_OpP1_LDS_EF;
	/********** 0xF0 **********/
	mInst_Op[IPage_1][IP1_SUBB_F0]  = &CCore::Inst_OpP1_SUBB_F0;
	mInst_Op[IPage_1][IP1_CMPB_F1]  = &CCore::Inst_OpP1_CMPB_F1;
	mInst_Op[IPage_1][IP1_SBCB_F2]  = &CCore::Inst_OpP1_SBCB_F2;
	mInst_Op[IPage_1][IP1_ADDD_F3]  = &CCore::Inst_OpP1_ADDD_F3;
	mInst_Op[IPage_1][IP1_ANDB_F4]  = &CCore::Inst_OpP1_ANDB_F4;
	mInst_Op[IPage_1][IP1_LDAB_F6]  = &CCore::Inst_OpP1_LDAB_F6;
	mInst_Op[IPage_1][IP1_TST_F7]   = &CCore::Inst_OpP1_TST_F7;
	mInst_Op[IPage_1][IP1_EORB_F8]  = &CCore::Inst_OpP1_EORB_F8;
	mInst_Op[IPage_1][IP1_ADCB_F9]  = &CCore::Inst_OpP1_ADCB_F9;
	mInst_Op[IPage_1][IP1_ORAB_FA]  = &CCore::Inst_OpP1_ORAB_FA;
	mInst_Op[IPage_1][IP1_ADDB_FB]  = &CCore::Inst_OpP1_ADDB_FB;
	mInst_Op[IPage_1][IP1_LDD_FC]   = &CCore::Inst_OpP1_LDD_FC;
	mInst_Op[IPage_1][IP1_LDY_FD]   = &CCore::Inst_OpP1_LDY_FD;
	mInst_Op[IPage_1][IP1_LDX_FE]   = &CCore::Inst_OpP1_LDX_FE;
	mInst_Op[IPage_1][IP1_LDS_FF]   = &CCore::Inst_OpP1_LDS_FF;

	/********* 0x1800 *********/
	mInst_Op[IPage_2][IP2_MOVW_00]  = &CCore::Inst_OpP2_MOVW_00;
	mInst_Op[IPage_2][IP2_MOVW_01]  = &CCore::Inst_OpP2_MOVW_01;
	mInst_Op[IPage_2][IP2_MOVW_02]  = &CCore::Inst_OpP2_MOVW_02;
	mInst_Op[IPage_2][IP2_MOVW_03]  = &CCore::Inst_OpP2_MOVW_03;
	mInst_Op[IPage_2][IP2_MOVW_04]  = &CCore::Inst_OpP2_MOVW_04;
	mInst_Op[IPage_2][IP2_MOVW_05]  = &CCore::Inst_OpP2_MOVW_05;
	mInst_Op[IPage_2][IP2_MOVB_08]  = &CCore::Inst_OpP2_MOVB_08;
	mInst_Op[IPage_2][IP2_MOVB_09]  = &CCore::Inst_OpP2_MOVB_09;
	mInst_Op[IPage_2][IP2_MOVB_0A]  = &CCore::Inst_OpP2_MOVB_0A;
	mInst_Op[IPage_2][IP2_MOVB_0B]  = &CCore::Inst_OpP2_MOVB_0B;
	mInst_Op[IPage_2][IP2_MOVB_0C]  = &CCore::Inst_OpP2_MOVB_0C;
	mInst_Op[IPage_2][IP2_MOVB_0D]  = &CCore::Inst_OpP2_MOVB_0D;
	mInst_Op[IPage_2][IP2_TAB]      = &CCore::Inst_OpP2_TAB;
	mInst_Op[IPage_2][IP2_TBA]      = &CCore::Inst_OpP2_TBA;

	/********* 0x1810 *********/
	mInst_Op[IPage_2][IP2_IDIV]     = &CCore::Inst_OpP2_IDIV;
	mInst_Op[IPage_2][IP2_FDIV]     = &CCore::Inst_OpP2_FDIV;
	mInst_Op[IPage_2][IP2_EMULS]    = &CCore::Inst_OpP2_EMULS;
	mInst_Op[IPage_2][IP2_EDIVS]    = &CCore::Inst_OpP2_EDIVS;
	mInst_Op[IPage_2][IP2_IDIVS]    = &CCore::Inst_OpP2_IDIVS;
	mInst_Op[IPage_2][IP2_CBA]      = &CCore::Inst_OpP2_CBA;

	/********* 0x1820 *********/
	mInst_Op[IPage_2][IP2_LBRA]     = &CCore::Inst_OpP2_LBRA;
	mInst_Op[IPage_2][IP2_LBRN]     = &CCore::Inst_OpP2_LBRN;
	mInst_Op[IPage_2][IP2_LBHI]     = &CCore::Inst_OpP2_LBHI;
	mInst_Op[IPage_2][IP2_LBLS]     = &CCore::Inst_OpP2_LBLS;
	mInst_Op[IPage_2][IP2_LBCC]     = &CCore::Inst_OpP2_LBCC;
	mInst_Op[IPage_2][IP2_LBCS]     = &CCore::Inst_OpP2_LBCS;
	mInst_Op[IPage_2][IP2_LBNE]     = &CCore::Inst_OpP2_LBNE;
	mInst_Op[IPage_2][IP2_LBEQ]     = &CCore::Inst_OpP2_LBEQ;
	mInst_Op[IPage_2][IP2_LBVC]     = &CCore::Inst_OpP2_LBVC;
	mInst_Op[IPage_2][IP2_LBVS]     = &CCore::Inst_OpP2_LBVS;
	mInst_Op[IPage_2][IP2_LBPL]     = &CCore::Inst_OpP2_LBPL;
	mInst_Op[IPage_2][IP2_LBMI]     = &CCore::Inst_OpP2_LBMI;
	mInst_Op[IPage_2][IP2_LBGE]     = &CCore::Inst_OpP2_LBGE;
	mInst_Op[IPage_2][IP2_LBLT]     = &CCore::Inst_OpP2_LBLT;
	mInst_Op[IPage_2][IP2_LBGT]     = &CCore::Inst_OpP2_LBGT;
	mInst_Op[IPage_2][IP2_LBLE]     = &CCore::Inst_OpP2_LBLE;

	// Исходные значения ячеек флэш-памяти.
	memset(mFlash, 0xFF, MCU_FlashSize);
}

void CCore::Flash_WriteArr(const uint32_t pAddr, const uint8_t* pSrc, const uint32_t pSize)
{
	if(pSize > 0)
	{
		memcpy(&mFlash[pAddr], pSrc, pSize);
		qDebug("Filled flash[0x%X:0x%X]", pAddr, pAddr + pSize - 1);
	}
	else
	{
		qWarning("Trying to fill flash from empty source.");
	}
}

void CCore::EEPROM_WriteArr(const uint16_t pAddr, const uint8_t* pSrc, const uint16_t pSize)
{
	if(pSize > 0)
	{
		memcpy(&mEEPROM[pAddr], pSrc, pSize);
		qDebug("Filled EEPROM[0x%X:0x%X]", pAddr, pAddr + pSize - 1);
	}
	else
	{
		qWarning("Trying to fill EEPROM from empty source.");
	}
}

bool CCore::DumpRAM(const QString& pFileName)
{
QFile ram_file(pFileName);
bool rv = false;

	if(ram_file.open(QIODevice::WriteOnly))
	{
		if(ram_file.write((const char*)mRAM, MCU_RAMSize) == MCU_RAMSize) rv = true;

		ram_file.close();
	}

	return rv;
}

void CCore::Inst_Fetch()
{
	qDebug("Start. \tPC = 0x%X, \tSP = 0x%X, \tD = 0x%X, \tX = 0x%X, \tY = 0x%X", mRegister.PC, mRegister.SP, mRegister.Accumulator.D, mRegister.Index.X, mRegister.Index.Y);
	{///TODO: dbg
//		static uint8_t step = 0;

		if((mRegister.PC == 0x97D0) && ((mSubReg[0x0030] & PPAGE_Mask) == 0x05))
		{
			mRAM[0x1A51] |= 0x80;
		}

		if((mRegister.PC == 0x831A) && ((mSubReg[0x0030] & PPAGE_Mask) == 0x00))///TODO: dbg
		{
			qDebug("0x831A");
			mRAM[0x0663] = 0x80;
			mRAM[0x1625] = true;
			mRAM[0x2266] = true;
#if 0
			mRegister.PC = 0xB0B3;
			mSubReg[0x0030] = 0x01;
			if(step == 0)
			{
				/*mRAM[0x0196] = 0x17;
				mRAM[0x1FF2 + 1] = 0x23;
				mRAM[0x1FF2 + 2] = 0x67;
				mRAM[0x2288] = 3;*/
				mRAM[0x027D] = 0x05;
				step++;
			}
			else if(step == 1)
			{
				mRAM[0x1D51] &= ~0x20;
			}
#endif // 0
		}

		if(mRegister.PC == 0xA872)
		{
			qDebug("0xA872");
		}
	}///TODO: dbgend

	mInst_Cur = mMMC.ReadU8(mRegister.PC);
	mInst_PC = mRegister.PC;
	mRegister.PC++;
}

void CCore::Inst_Execute()
{
	(this->*mInst_Op[IPage_1][mInst_Cur])();// Здесь всегда первая страница. Вторая страница используется в функциях, вызванных из первой страницы.
}

void CCore::Inst_OpP1_InstP2()
{
	Inst_Fetch();
	mInst_PC--;// Return value to begin of instruction chain.
	(this->*mInst_Op[IPage_2][mInst_Cur])();
}

void CCore::Inst_OpP1_Invalid()
{
	DumpRAM("ram.bin");
	qFatal("Instruction = p1:%X, PC = 0x%X. NI", mInst_Cur, mRegister.PC - 1);// "-1" - потому что PC был увеличен на 1 после чтения инструкции.
}

void CCore::Inst_OpP2_Invalid()
{
	DumpRAM("ram.bin");
	qFatal("Instruction = p2:%X, PC = 0x%X. NI", mInst_Cur, mRegister.PC - 1);// "-1" - потому что PC был увеличен на 1 после чтения инструкции.
}

void CCore::Reset()
{
	// Регистры
	mRegister.Accumulator.D = 0x0000;
	mRegister.Index.X = 0x0000;
	mRegister.Index.Y = 0x0000;
	mRegister.CC.All = 0x00;
	mRegister.SP = MCU_RAMSize - 1;
	//
	// Подблоки
	//
	mMMC.Reset();
	mCRG.Reset();
	mECT.Reset();
	mMEBI.Reset();
	mPIM.Reset();
	mSPI0.Reset();
	mSPI1.Reset();
	mSPI2.Reset();

	// Загрузка вектора на обработчик сброса. Мы это можем сделать только после "сброса" MMC, чтобы верно работало отображение памяти.
	mRegister.PC = mMMC.ReadU16(static_cast<uint16_t>(EInterruptVector::Reset));
}

void CCore::Step()
{
	mCRG.Step();// Обновление состояния ГФАПЧ.
	mECT.Step();
	mMEBI.Step();
	mPIM.Step();// Обновление состояния портов.
	mSPI0.Step();
	mSPI1.Step();
	mSPI2.Step();
	///TODO: Проверка запросов прерываний.
	Inst_Fetch();// Выборка инструкции.
	Inst_Execute();// Исполнение инструкции.
}

uint16_t CCore::DataByMode_PostByte(uint16_t* pIndirectAddr, bool* pIndirect)
{
uint8_t postbyte;
int16_t off16;

auto Offset_i5toi8 = [&]() -> int8_t { return ((int8_t)(postbyte << 3) >> 3); };
auto Register16 = [&](const uint8_t pShiftedPB) -> uint16_t&
	{
		switch(pShiftedPB & 0x03)
		{
			case 0: return mRegister.Index.X;
			case 1: return mRegister.Index.Y;
			case 2: return mRegister.SP;
			case 3: default:
				return mRegister.PC;
		}
	};

auto RegisterA = [&](const uint8_t pShiftedPB) -> uint16_t
	{
		switch(pShiftedPB & 0x03)
		{
			case 0: return CU16(mRegister.Accumulator.A);
			case 1: return CU16(mRegister.Accumulator.B);
			case 2: default: return mRegister.Accumulator.D;
		}
	};

	postbyte = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((postbyte & 0b0010'0000) == 0)
	{// 5-bit constant offset
		return Register16(postbyte >> 6) + Offset_i5toi8();
	}
	else if((postbyte & 0b1110'0100) == 0b1110'0000)
	{
		if((postbyte & 0b0000'0011) == 0b0000'0011)
		{// 16-bit offset indexed-indirect
			off16 = mMMC.ReadU16(mRegister.PC), mRegister.PC += 2;
			off16 += Register16(postbyte >> 3);
			if(pIndirectAddr != nullptr) *pIndirectAddr = off16;
			if(pIndirect != nullptr) *pIndirect = true;

			return mMMC.ReadU16(off16);
		}
		else
		{// Constant offset
			if((postbyte & 0b0000'0010) != 0)
			{// 16-bit
				off16 = mMMC.ReadU16(mRegister.PC), mRegister.PC += 2;

				return Register16(postbyte >> 3) + off16;
			}
			else
			{// 9-bit with sign on LSB of postbyte
				off16 = CU16(mMMC.ReadU8(mRegister.PC)), mRegister.PC++;
				if((postbyte & 0b0000'0001) != 0) off16 |= 0xFF00;

				return Register16(postbyte >> 3) + off16;
			}
		}// if((postbyte & 0b0000'0011) == 0b0000'0011) else
	}
	else if((postbyte & 0b1110'0100) == 0b1110'0100)
	{
		if((postbyte & 0b1110'0111) == 0b1110'0111)
		{// Accumulator D offset indexed-indirect
			off16 = Register16(postbyte >> 3) + mRegister.Accumulator.D;
			if(pIndirectAddr != nullptr) *pIndirectAddr = off16;
			if(pIndirect != nullptr) *pIndirect = true;

			return mMMC.ReadU16(off16);
		}
		else
		{// Accumulator offset
			return Register16(postbyte >> 3) + RegisterA(postbyte);
		}
	}
	else if((postbyte & 0b0010'0000) == 0b0010'0000)
	{// Auto predecrement, preincrement, postdecrement or postincrement
		bool pre = ((postbyte & 0b0001'0000) == 0);
		uint16_t addr;
		uint16_t& reg = Register16(postbyte >> 6);

		if(postbyte & 0b0000'1000)
			off16 = CU16(postbyte & 0x0F) | 0xFFF0;// negative offset
		else
			off16 = (postbyte & 0x0F) + 1;// positive offset

		if(pre)
		{
			reg += off16;
			addr = reg;
		}
		else
		{
			addr = reg;
			reg += off16;
		}

		return addr;
	}
	else
	{
		qFatal("Unknown postbyte value: 0x%X.", postbyte);
	}
}

/************************* Обработчики инструкций ************************/

/********** 0x00 **********/

void CCore::Inst_OpP1_INY()
{
	mRegister.Index.Y++;
	UpdateConditionFlags_Z_u16(mRegister.Index.Y);
}

void CCore::Inst_OpP1_DEY()
{
	mRegister.Index.Y--;
	UpdateConditionFlags_Z_u16(mRegister.Index.Y);
}

void CCore::Inst_OpP1_LoopSet()
{
int16_t rel16;
uint8_t lb;
bool jump;

auto Register_Get = [&]() -> uint16_t
	{
		switch(lb & 0b0000'0111)
		{
			case 0b0000'0000: return CU16(mRegister.Accumulator.A);
			case 0b0000'0001: return CU16(mRegister.Accumulator.B);
			case 0b0000'0100: return mRegister.Accumulator.D;
			case 0b0000'0101: return mRegister.Index.X;
			case 0b0000'0110: return mRegister.Index.Y;
			case 0b0000'0111: return mRegister.SP;
			default: qFatal("LoopSet: unknown register index: 0x%X", lb & 0b0000'0011);
		}
	};

auto Register_Set = [&](const uint16_t pValue) -> void
	{
		switch(lb & 0b0000'0111)
		{
			case 0b0000'0000: mRegister.Accumulator.A = pValue; break;
			case 0b0000'0001: mRegister.Accumulator.B = pValue; break;
			case 0b0000'0100: mRegister.Accumulator.D = pValue; break;
			case 0b0000'0101: mRegister.Index.X = pValue; break;
			case 0b0000'0110: mRegister.Index.Y = pValue; break;
			case 0b0000'0111: mRegister.SP = pValue; break;
			default: qFatal("LoopSet: unknown register index: 0x%X", lb & 0b0000'0011);
		}
	};

	lb = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel16 = CU16(mMMC.ReadU8(mRegister.PC)), mRegister.PC++;
	// Apply 9 bit.
	if(lb & (1 << 4)) rel16 |= 0xFF00;

	// Choose operator.
	switch(lb & 0b1100'0000)
	{
		case 0b0000'0000:// DBEQ, DBNE
			{
				Register_Set(Register_Get() - 1);
				// Choose operator variant.
				if(lb & (1 << 5))
					jump = (Register_Get() != 0);// DBNE
				else
					jump = (Register_Get() == 0);// DBEQ
			}

			break;
		case 0b0100'0000:// TBEQ, TBNE
			// Choose operator variant.
			if(lb & (1 << 5))
				jump = (Register_Get() != 0);// TBNE
			else
				jump = (Register_Get() == 0);// TBEQ

			break;
		case 0b1000'0000:// IBEQ, IBNE
			{
				Register_Set(Register_Get() + 1);
				// Choose operator variant.
				if(lb & (1 << 5))
					jump = (Register_Get() != 0);// IBNE
				else
					jump = (Register_Get() == 0);// IBEQ
			}

			break;
		default:
			qFatal("LoopSet: unknown operator index: 0x%X", lb & 0b1100'0000);
	}

	if(jump)
		mRegister.PC += rel16;
}

void CCore::Inst_OpP1_JMP_05() { mRegister.PC = DataByMode_PostByte(); }
void CCore::Inst_OpP1_JMP_06() { mRegister.PC = DataByMode_EXT(); }

void CCore::Inst_OpP1_BSR()
{
int8_t rel8;

	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	Stack_Push(mRegister.PC);
	mRegister.PC += rel8;
}

void CCore::Inst_OpP1_INX()
{
	mRegister.Index.X++;
	UpdateConditionFlags_Z_u16(mRegister.Index.X);
}

void CCore::Inst_OpP1_DEX()
{
	mRegister.Index.X--;
	UpdateConditionFlags_Z_u16(mRegister.Index.X);
}

void CCore::Inst_OpP1_RTC()
{
uint8_t tval8;

	Stack_Pop(tval8);
	mMMC.WriteU8(CMMC::ERegAddr::PPAGE, tval8);
	Stack_Pop(mRegister.PC);
}

void CCore::Inst_OpP1_BSET_0C()
{
uint16_t tgtaddr;
uint8_t val_mem, val_mask;

	tgtaddr = DataByMode_PostByte();
	val_mask = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// Маска хранится в самом конце инструкции.
	val_mem = mMMC.ReadU8(tgtaddr);
	val_mem |= val_mask;
	mMMC.WriteU8(tgtaddr, val_mem);
	UpdateConditionFlags_NZV0(val_mem);
}

void CCore::Inst_OpP1_BCLR_0D()
{
uint16_t tgtaddr;
uint8_t val_mem, val_mask;

	tgtaddr = DataByMode_PostByte();
	val_mask = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// Маска хранится в самом конце инструкции.
	val_mem = mMMC.ReadU8(tgtaddr);
	val_mem &= ~val_mask;
	mMMC.WriteU8(tgtaddr, val_mem);
	UpdateConditionFlags_NZV0(val_mem);
}

void CCore::Inst_OpP1_BRSET_0E()
{
uint16_t memaddr;
uint8_t mem8, msk8;
int8_t rel8;

	memaddr = DataByMode_PostByte();
	mem8 = mMMC.ReadU8(memaddr);
	msk8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((~mem8 & msk8) == 0) mRegister.PC += rel8;

}

void CCore::Inst_OpP1_BRCLR_0F()
{
uint16_t memaddr;
uint8_t mem8, msk8;
int8_t rel8;

	memaddr = DataByMode_PostByte();
	mem8 = mMMC.ReadU8(memaddr);
	msk8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((mem8 & msk8) == 0) mRegister.PC += rel8;

}

/********** 0x10 **********/

void CCore::Inst_OpP1_ANDCC() { mRegister.CC.All &= DataByMode_IMM_8i(); }

void CCore::Inst_OpP1_EDIV()
{
uint32_t arg1;
uint32_t res;

	if(mRegister.Index.X != 0)
	{
		arg1 = ((uint32_t)mRegister.Index.Y << 16) | mRegister.Accumulator.D;
		res = arg1 / mRegister.Index.X;
		mRegister.Accumulator.D = arg1 % mRegister.Index.X;
		mRegister.Index.Y = res;
		UpdateConditionFlags_NZ_u32(res);
		mRegister.CC.V = (res > 0x0000FFFF);
		mRegister.CC.C = false;
	}
	else
	{
		mRegister.CC.C = true;
	}
}

void CCore::Inst_OpP1_MUL()
{
uint16_t res;

	res = mRegister.Accumulator.A;
	res *= mRegister.Accumulator.B;
	mRegister.Accumulator.A = res >> 8;
	mRegister.Accumulator.B = res;
	mRegister.CC.C = ((res & (1 << 7)) != 0);
}

void CCore::Inst_OpP1_EMUL()
{
uint32_t res;

	res = mRegister.Accumulator.D;
	res *= mRegister.Index.Y;
	mRegister.Index.Y = res >> 16;
	mRegister.Accumulator.D = res;
	UpdateConditionFlags_NZ_u32(res);
	mRegister.CC.C = ((res & (1 << 15)) != 0);
}

void CCore::Inst_OpP1_ORCC() { mRegister.CC.All |= DataByMode_IMM_8i(); }

void CCore::Inst_OpP1_JSR_15()
{
uint16_t tgtaddr;

	tgtaddr = DataByMode_PostByte();
	Stack_Push(mRegister.PC);
	mRegister.PC = tgtaddr;
}

void CCore::Inst_OpP1_JSR_16()
{
uint16_t tgtaddr;

	tgtaddr = DataByMode_EXT();
	Stack_Push(mRegister.PC);
	mRegister.PC = tgtaddr;
}

void CCore::Inst_OpP1_JSR_17()
{
uint16_t tgtaddr;

	tgtaddr = DataByMode_DIR();
	Stack_Push(mRegister.PC);
	mRegister.PC = tgtaddr;
}

void CCore::Inst_OpP1_LEAY() { mRegister.Index.Y = DataByMode_PostByte(); }
void CCore::Inst_OpP1_LEAX() { mRegister.Index.X = DataByMode_PostByte(); }
void CCore::Inst_OpP1_LEAS() { mRegister.SP = DataByMode_PostByte(); }

void CCore::Inst_OpP1_BSET_1C()
{
uint16_t tgtaddr;
uint8_t val_mem, val_mask;

	tgtaddr = DataByMode_EXT();
	val_mask = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// Маска хранится в самом конце инструкции.
	val_mem = mMMC.ReadU8(tgtaddr);
	val_mem |= val_mask;
	mMMC.WriteU8(tgtaddr, val_mem);
	UpdateConditionFlags_NZV0(val_mem);
}

void CCore::Inst_OpP1_BCLR_1D()
{
uint16_t tgtaddr;
uint8_t val_mem, val_mask;

	tgtaddr = DataByMode_EXT();
	val_mask = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// Маска хранится в самом конце инструкции.
	val_mem = mMMC.ReadU8(tgtaddr);
	val_mem &= ~val_mask;
	mMMC.WriteU8(tgtaddr, val_mem);
	UpdateConditionFlags_NZV0(val_mem);
}

void CCore::Inst_OpP1_BRSET_1E()
{
uint16_t memaddr;
uint8_t mem8, msk8;
int8_t rel8;

	memaddr = DataByMode_EXT();
	mem8 = mMMC.ReadU8(memaddr);
	msk8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((~mem8 & msk8) == 0) mRegister.PC += rel8;

}

void CCore::Inst_OpP1_BRCLR_1F()
{
uint16_t memaddr;
uint8_t mem8, msk8;
int8_t rel8;

	memaddr = DataByMode_EXT();
	mem8 = mMMC.ReadU8(memaddr);
	msk8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((mem8 & msk8) == 0) mRegister.PC += rel8;

}

/********** 0x20 **********/

#define Inst_OpP1_Branch(pCondition)	\
	int8_t rel8; \
	 \
	rel8 = DataByMode_IMM_8i(); \
	if(pCondition) mRegister.PC += rel8; \
	 \
	do {} while(false)

void CCore::Inst_OpP1_BRA() { Inst_OpP1_Branch(true); }
void CCore::Inst_OpP1_BRN() { Inst_OpP1_Branch(false); }
void CCore::Inst_OpP1_BHI() { Inst_OpP1_Branch(!(mRegister.CC.C || mRegister.CC.Z)); }
void CCore::Inst_OpP1_BLS() { Inst_OpP1_Branch(mRegister.CC.C || mRegister.CC.Z); }
void CCore::Inst_OpP1_BCC() { Inst_OpP1_Branch(!mRegister.CC.C); }
void CCore::Inst_OpP1_BCS() { Inst_OpP1_Branch(mRegister.CC.C); }
void CCore::Inst_OpP1_BNE() { Inst_OpP1_Branch(!mRegister.CC.Z); }
void CCore::Inst_OpP1_BEQ() { Inst_OpP1_Branch(mRegister.CC.Z); }
void CCore::Inst_OpP1_BVC() { Inst_OpP1_Branch(!mRegister.CC.V); }
void CCore::Inst_OpP1_BVS() { Inst_OpP1_Branch(mRegister.CC.V); }
void CCore::Inst_OpP1_BPL() { Inst_OpP1_Branch(!mRegister.CC.N); }
void CCore::Inst_OpP1_BMI() { Inst_OpP1_Branch(mRegister.CC.N); }
void CCore::Inst_OpP1_BGE() { Inst_OpP1_Branch(!(mRegister.CC.N ^ mRegister.CC.V)); }
void CCore::Inst_OpP1_BLT() { Inst_OpP1_Branch(mRegister.CC.N ^ mRegister.CC.V); }
void CCore::Inst_OpP1_BGT() { Inst_OpP1_Branch(!(mRegister.CC.Z || (mRegister.CC.N ^ mRegister.CC.V))); }
void CCore::Inst_OpP1_BLE() { Inst_OpP1_Branch(mRegister.CC.Z || (mRegister.CC.N ^ mRegister.CC.V)); }

/********** 0x30 **********/

void CCore::Inst_OpP1_RTS() { Stack_Pop(mRegister.PC); }
void CCore::Inst_OpP1_PULX() { Stack_Pop(mRegister.Index.X); }
void CCore::Inst_OpP1_PULY() { Stack_Pop(mRegister.Index.Y); }
void CCore::Inst_OpP1_PULA() { Stack_Pop(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_PULB() { Stack_Pop(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_PSHX() { Stack_Push(mRegister.Index.X); }
void CCore::Inst_OpP1_PSHY() { Stack_Push(mRegister.Index.Y); }
void CCore::Inst_OpP1_PSHA() { Stack_Push(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_PSHB() { Stack_Push(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_PULC() { Stack_Pop(mRegister.CC.All); }
void CCore::Inst_OpP1_PSHC() { Stack_Push(mRegister.CC.All); }
void CCore::Inst_OpP1_PULD() { Stack_Pop(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_PSHD() { Stack_Push(mRegister.Accumulator.D); }

/********** 0x40 **********/

#define Inst_OpP1_NEG_reg8(pRegister) \
	uint8_t tval8; \
	 \
	tval8 = pRegister; \
	pRegister = -tval8; \
	UpdateConditionFlags_NZ_u8(pRegister); \
	mRegister.CC.V = (tval8 == 0x80); \
	mRegister.CC.C = (tval8 != 0); \
	do {} while(false)

#define Inst_OpP1_COM_val8(pValue) \
	pValue = 0xFF - pValue; \
	UpdateConditionFlags_NZ_u8(pValue); \
	mRegister.CC.V = false; \
	mRegister.CC.C = true; \
	do {} while(false)

#define Inst_OpP1_COM_mem(pFunc_Data) \
	uint16_t tgtaddr = pFunc_Data(); \
	uint8_t tval8; \
	 \
	tval8 = mMMC.ReadU8(tgtaddr); \
	Inst_OpP1_COM_val8(tval8); \
	mMMC.WriteU8(tgtaddr, tval8); \
	do {} while(false)

#define Inst_OpP1_ASR_reg(pRegister)	\
	mRegister.CC.C = (pRegister & 0x01); \
	pRegister = static_cast<int8_t>(pRegister) >> 1; \
	UpdateConditionFlags_NZ_u8(pRegister); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_ASL_reg8(pRegister)	\
	mRegister.CC.C = (pRegister & 0x80); \
	pRegister = static_cast<int8_t>(pRegister) << 1; \
	UpdateConditionFlags_NZ_u8(pRegister); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_ASL_reg16(pRegister)	\
	mRegister.CC.C = (pRegister & 0x8000); \
	pRegister = static_cast<int8_t>(pRegister) << 1; \
	UpdateConditionFlags_NZ_u16(pRegister); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_DEC_reg8(pRegister)	\
	uint8_t arg1, diff; \
	 \
	arg1 = pRegister; \
	diff = arg1 - 1; \
	pRegister = diff; \
	UpdateConditionFlags_Diff_NZV_u8(arg1, 1, diff); \
	do {} while(false)

#define Inst_OpP1_INC_reg8(pRegister)	\
	uint8_t arg1, summ; \
	 \
	arg1 = pRegister; \
	summ = arg1 + 1; \
	pRegister = summ; \
	UpdateConditionFlags_Add_NZV_u8(arg1, 1, summ); \
	do {} while(false)

#define Inst_OpP1_LSR_val8(pValue)	\
	mRegister.CC.C = (pValue & 0x01); \
	pValue >>= 1; \
	UpdateConditionFlags_NZ_u8(pValue); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_LSR_val16(pValue)	\
	mRegister.CC.C = (pValue & 0x01); \
	pValue >>= 1; \
	UpdateConditionFlags_NZ_u16(pValue); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_LSR_mem(pFunc_Data)	\
	uint16_t tgtaddr = pFunc_Data(); \
	uint8_t val8 = mMMC.ReadU8(tgtaddr); \
	 \
	Inst_OpP1_LSR_val8(val8); \
	mMMC.WriteU8(tgtaddr, val8); \
	do {} while(false)

void CCore::Inst_OpP1_NEGA() { Inst_OpP1_NEG_reg8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_COMA() { Inst_OpP1_COM_val8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_INCA() { Inst_OpP1_INC_reg8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_DECA() { Inst_OpP1_DEC_reg8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_LSRA() { Inst_OpP1_LSR_val8(mRegister.Accumulator.A); }

#define Inst_OpP1_ROL_val8(pVal8)	\
	uint8_t res = (pVal8 << 1); \
	if(mRegister.CC.C) res |= (1 << 0); \
	 \
	mRegister.CC.C = ((pVal8 & (1 << 7)) != 0); \
	pVal8 = res; \
	UpdateConditionFlags_NZ_u8(pVal8); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_ROR_val8(pVal8)	\
	uint8_t res = (pVal8 >> 1); \
	if(mRegister.CC.C) res |= (1 << 7); \
	 \
	mRegister.CC.C = ((pVal8 & (1 << 0)) != 0); \
	pVal8 = res; \
	UpdateConditionFlags_NZ_u8(pVal8); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

void CCore::Inst_OpP1_ROLA() { Inst_OpP1_ROL_val8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_RORA() { Inst_OpP1_ROR_val8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ASRA() { Inst_OpP1_ASR_reg(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ASLA() { Inst_OpP1_ASL_reg8(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_LSRD() { Inst_OpP1_LSR_val16(mRegister.Accumulator.D); }

void CCore::Inst_OpP1_CALL_4A()
{
uint16_t tgtaddr;
uint8_t tgtpage;

	tgtaddr = DataByMode_EXT();
	tgtpage = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	Stack_Push(mRegister.PC);
	Stack_Push(mMMC.ReadU8(CMMC::ERegAddr::PPAGE));
	mMMC.WriteU8(CMMC::ERegAddr::PPAGE, tgtpage);
	mRegister.PC = tgtaddr;
}

void CCore::Inst_OpP1_CALL_4B()
{
uint16_t tgtaddr, indaddr;
uint8_t tgtpage;
bool indirect = false;

	tgtaddr = DataByMode_PostByte(&indaddr, &indirect);
	if(indirect)
		tgtpage = mMMC.ReadU8(indaddr + 2);// "+ 2" - пропускаем прочитанный адрес.
	else
		tgtpage = mMMC.ReadU8(mRegister.PC), mRegister.PC++;

	Stack_Push(mRegister.PC);
	Stack_Push(mMMC.ReadU8(CMMC::ERegAddr::PPAGE));
	mMMC.WriteU8(CMMC::ERegAddr::PPAGE, tgtpage);
	mRegister.PC = tgtaddr;
}

void CCore::Inst_OpP1_BSET_4C()
{
uint16_t tgtaddr;
uint8_t val_mem, val_mask;

	tgtaddr = DataByMode_DIR();
	val_mask = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// Маска хранится в самом конце инструкции.
	val_mem = mMMC.ReadU8(tgtaddr);
	val_mem |= val_mask;
	mMMC.WriteU8(tgtaddr, val_mem);
	UpdateConditionFlags_NZV0(val_mem);
}

///TODO: вынести общее для BCLR, BSET, BRCLR, BRSET.
void CCore::Inst_OpP1_BCLR_4D()
{
uint16_t tgtaddr;
uint8_t val_mem, val_mask;

	tgtaddr = DataByMode_DIR();
	val_mask = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// Маска хранится в самом конце инструкции.
	val_mem = mMMC.ReadU8(tgtaddr);
	val_mem &= ~val_mask;
	mMMC.WriteU8(tgtaddr, val_mem);
	UpdateConditionFlags_NZV0(val_mem);
}

void CCore::Inst_OpP1_BRSET_4E()
{
uint16_t memaddr;
uint8_t mem8, msk8;
int8_t rel8;

	memaddr = DataByMode_DIR();
	mem8 = mMMC.ReadU8(memaddr);
	msk8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((~mem8 & msk8) == 0) mRegister.PC += rel8;

}

void CCore::Inst_OpP1_BRCLR_4F()
{
uint16_t memaddr;
uint8_t mem8, msk8;
int8_t rel8;

	memaddr = DataByMode_DIR();
	mem8 = mMMC.ReadU8(memaddr);
	msk8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	rel8 = mMMC.ReadU8(mRegister.PC), mRegister.PC++;
	if((mem8 & msk8) == 0) mRegister.PC += rel8;

}

/********** 0x50 **********/

void CCore::Inst_OpP1_NEGB() { Inst_OpP1_NEG_reg8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_COMB() { Inst_OpP1_COM_val8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_INCB() { Inst_OpP1_INC_reg8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_DECB() { Inst_OpP1_DEC_reg8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_LSRB() { Inst_OpP1_LSR_val8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ROLB() { Inst_OpP1_ROL_val8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_RORB() { Inst_OpP1_ROR_val8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ASRB() { Inst_OpP1_ASR_reg(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ASLB() { Inst_OpP1_ASL_reg8(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ASLD() { Inst_OpP1_ASL_reg16(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_STAA_5A() { RegOp_Store_DIR(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_STAB_5B() { RegOp_Store_DIR(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_STD_5C() { RegOp_Store_DIR(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_STY_5D() { RegOp_Store_DIR(mRegister.Index.Y); }
void CCore::Inst_OpP1_STX_5E() { RegOp_Store_DIR(mRegister.Index.X); }
void CCore::Inst_OpP1_STS_5F() { RegOp_Store_DIR(mRegister.SP); }

/********** 0x60 **********/

#define Inst_OpP1_NEG_mem(pFunc_Data) \
	uint16_t tgtaddr; \
	uint8_t arg, res; \
	 \
	tgtaddr = pFunc_Data(); \
	arg = mMMC.ReadU8(tgtaddr); \
	res = -arg; \
	mMMC.WriteU8(tgtaddr, res); \
	UpdateConditionFlags_NZ_u8(res); \
	mRegister.CC.V = (arg == 0x80); \
	mRegister.CC.C = (arg != 0); \
	do {} while(false)

#define Inst_OpP1_INC(pFunc_Data)	\
	uint16_t tgtaddr; \
	uint8_t arg1, summ; \
	 \
	tgtaddr = pFunc_Data(); \
	arg1 = mMMC.ReadU8(tgtaddr); \
	summ = arg1 + 1; \
	mMMC.WriteU8(tgtaddr, summ); \
	UpdateConditionFlags_Add_NZV_u8(arg1, 1, summ); \
	do {} while(false)

#define Inst_OpP1_DEC(pFunc_Data)	\
	uint16_t tgtaddr; \
	uint8_t arg1, diff; \
	 \
	tgtaddr = pFunc_Data(); \
	arg1 = mMMC.ReadU8(tgtaddr); \
	diff = arg1 - 1; \
	mMMC.WriteU8(tgtaddr, diff); \
	UpdateConditionFlags_Diff_NZV_u8(arg1, 1, diff); \
	do {} while(false)

#define Inst_OpP1_ASL(pFunc_Data)	\
	uint16_t tgtaddr; \
	uint8_t arg; \
	 \
	tgtaddr = pFunc_Data(); \
	arg = mMMC.ReadU8(tgtaddr); \
	mRegister.CC.C = (arg & 0x80); \
	arg = static_cast<int8_t>(arg) << 1; \
	UpdateConditionFlags_NZ_u8(arg); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

#define Inst_OpP1_ASR(pFunc_Data)	\
	uint16_t tgtaddr; \
	uint8_t arg; \
	 \
	tgtaddr = pFunc_Data(); \
	arg = mMMC.ReadU8(tgtaddr); \
	mRegister.CC.C = (arg & 0x01); \
	arg = static_cast<int8_t>(arg) >> 1; \
	UpdateConditionFlags_NZ_u8(arg); \
	mRegister.CC.V = mRegister.CC.N ^ mRegister.CC.C; \
	do {} while(false)

void CCore::Inst_OpP1_NEG_60() { Inst_OpP1_NEG_mem(DataByMode_PostByte); }
void CCore::Inst_OpP1_COM_61() { Inst_OpP1_COM_mem(DataByMode_PostByte); }
void CCore::Inst_OpP1_INC_62() { Inst_OpP1_INC(DataByMode_PostByte); }
void CCore::Inst_OpP1_DEC_63() { Inst_OpP1_DEC(DataByMode_PostByte); }
void CCore::Inst_OpP1_LSR_64() { Inst_OpP1_LSR_mem(DataByMode_PostByte); }

#define Inst_OpP1_ROL_mem8(pFunc_Data)	\
	uint16_t tgtaddr = pFunc_Data(); \
	uint8_t tval8 = mMMC.ReadU8(tgtaddr); \
	 \
	Inst_OpP1_ROL_val8(tval8); \
	mMMC.WriteU8(tgtaddr, tval8); \
	do {} while(false)

#define Inst_OpP1_ROR_mem8(pFunc_Data)	\
	uint16_t tgtaddr = pFunc_Data(); \
	uint8_t tval8 = mMMC.ReadU8(tgtaddr); \
	 \
	Inst_OpP1_ROR_val8(tval8); \
	mMMC.WriteU8(tgtaddr, tval8); \
	do {} while(false)

void CCore::Inst_OpP1_ROL_65() { Inst_OpP1_ROL_mem8(DataByMode_PostByte); }
void CCore::Inst_OpP1_ROR_66() { Inst_OpP1_ROR_mem8(DataByMode_PostByte); }
void CCore::Inst_OpP1_ASR_67() { Inst_OpP1_ASR(DataByMode_PostByte); }
void CCore::Inst_OpP1_ASL_68() { Inst_OpP1_ASL(DataByMode_PostByte); }
void CCore::Inst_OpP1_CLR_69()
{
uint16_t tgtaddr;

	tgtaddr = DataByMode_PostByte();
	mMMC.WriteU8(tgtaddr, 0);
	UpdateConditionFlags_ForClear();
}

void CCore::Inst_OpP1_STAA_6A() { RegOp_Store_pb(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_STAB_6B() { RegOp_Store_pb(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_STD_6C() { RegOp_Store_pb(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_STY_6D() { RegOp_Store_pb(mRegister.Index.Y); }
void CCore::Inst_OpP1_STX_6E() { RegOp_Store_pb(mRegister.Index.X); }
void CCore::Inst_OpP1_STS_6F() { RegOp_Store_pb(mRegister.SP); }

/********** 0x70 **********/

void CCore::Inst_OpP1_NEG_70() { Inst_OpP1_NEG_mem(DataByMode_EXT); }
void CCore::Inst_OpP1_COM_71() { Inst_OpP1_COM_mem(DataByMode_EXT); }
void CCore::Inst_OpP1_INC_72() { Inst_OpP1_INC(DataByMode_EXT); }
void CCore::Inst_OpP1_DEC_73() { Inst_OpP1_DEC(DataByMode_EXT); }
void CCore::Inst_OpP1_LSR_74() { Inst_OpP1_LSR_mem(DataByMode_EXT); }
void CCore::Inst_OpP1_ROL_75() { Inst_OpP1_ROL_mem8(DataByMode_EXT); }
void CCore::Inst_OpP1_ROR_76() { Inst_OpP1_ROR_mem8(DataByMode_EXT); }
void CCore::Inst_OpP1_ASR_77() { Inst_OpP1_ASR(DataByMode_EXT); }
void CCore::Inst_OpP1_ASL_78() { Inst_OpP1_ASL(DataByMode_EXT); }

void CCore::Inst_OpP1_CLR_79()
{
uint16_t tgtaddr;

	tgtaddr = DataByMode_EXT();
	mMMC.WriteU8(tgtaddr, 0);
	UpdateConditionFlags_ForClear();
}

void CCore::Inst_OpP1_STAA_7A() { RegOp_Store_EXT(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_STAB_7B() { RegOp_Store_EXT(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_STD_7C() { RegOp_Store_EXT(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_STY_7D() { RegOp_Store_EXT(mRegister.Index.Y); }
void CCore::Inst_OpP1_STX_7E() { RegOp_Store_EXT(mRegister.Index.X); }
void CCore::Inst_OpP1_STS_7F() { RegOp_Store_EXT(mRegister.SP); }

/********** 0x80 **********/

void CCore::Inst_OpP1_SUBA_80() { RegOp_SubwoC_u8(mRegister.Accumulator.A, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_CMPA_81() { RegOp_Compare_IMM_8i(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_SBCA_82() { RegOp_SubC_u8(mRegister.Accumulator.A, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_ANDA_84() { RegOp_AND_IMM_8i(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_SUBD_83() { RegOp_SubwoC_u16(mRegister.Accumulator.D, DataByMode_IMM_16i()); }
void CCore::Inst_OpP1_LDAA_86() { RegOp_Load_IMM_8i(mRegister.Accumulator.A); }

void CCore::Inst_OpP1_CLRA()
{
	mRegister.Accumulator.A = 0;// Режим адресации - IMM.
	UpdateConditionFlags_ForClear();
}

void CCore::Inst_OpP1_EORA_88() { RegOp_EOR_IMM_8i(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADCA_89() { RegOp_AddC_u8(mRegister.Accumulator.A, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_ORAA_8A() { RegOp_OR_IMM_8i(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADDA_8B() { RegOp_AddwoC_u8(mRegister.Accumulator.A, DataByMode_IMM_8i()); }

void CCore::Inst_OpP1_CPD_8C() { RegOp_Compare_IMM_16i(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_CPY_8D() { RegOp_Compare_IMM_16i(mRegister.Index.Y); }
void CCore::Inst_OpP1_CPX_8E() { RegOp_Compare_IMM_16i(mRegister.Index.X); }
void CCore::Inst_OpP1_CPS_8F() { RegOp_Compare_IMM_16i(mRegister.SP); }

/********** 0x90 **********/

void CCore::Inst_OpP1_SUBA_90() { RegOp_SubwoC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_CMPA_91() { RegOp_Compare_DIR(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_SBCA_92() { RegOp_SubC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_SUBD_93() { RegOp_SubwoC_u16(mRegister.Accumulator.D, mMMC.ReadU16(DataByMode_DIR())); }
void CCore::Inst_OpP1_ANDA_94() { RegOp_AND_DIR(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_LDAA_96() { RegOp_Load_DIR(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_TSTA() { UpdateConditionFlags_NZV0C0(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_EORA_98() { RegOp_EOR_DIR(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADCA_99() { RegOp_AddC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_ORAA_9A() { RegOp_OR_DIR(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADDA_9B() { RegOp_AddwoC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_CPD_9C() { RegOp_Compare_DIR(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_CPY_9D() { RegOp_Compare_DIR(mRegister.Index.Y); }
void CCore::Inst_OpP1_CPX_9E() { RegOp_Compare_DIR(mRegister.Index.X); }
void CCore::Inst_OpP1_CPS_9F() { RegOp_Compare_DIR(mRegister.SP); }

/********** 0xA0 **********/

void CCore::Inst_OpP1_SUBA_A0() { RegOp_SubwoC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_CMPA_A1() { RegOp_Compare_pb(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_SBCA_A2() { RegOp_SubC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_SUBD_A3() { RegOp_SubwoC_u16(mRegister.Accumulator.D, mMMC.ReadU16(DataByMode_PostByte())); }
void CCore::Inst_OpP1_ANDA_A4() { RegOp_AND_pb(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_LDAA_A6() { RegOp_Load_pb(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_NOP() { }
void CCore::Inst_OpP1_EORA_A8() { RegOp_EOR_pb(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADCA_A9() { RegOp_AddC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_ORAA_AA() { RegOp_OR_pb(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADDA_AB() { RegOp_AddwoC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_CPD_AC() { RegOp_Compare_pb(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_CPY_AD() { RegOp_Compare_pb(mRegister.Index.Y); }
void CCore::Inst_OpP1_CPX_AE() { RegOp_Compare_pb(mRegister.Index.X); }
void CCore::Inst_OpP1_CPS_AF() { RegOp_Compare_pb(mRegister.SP); }

/********** 0xB0 **********/

void CCore::Inst_OpP1_SUBA_B0() { RegOp_SubwoC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_CMPA_B1() { RegOp_Compare_EXT(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_SBCA_B2() { RegOp_SubC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_SUBD_B3() { RegOp_SubwoC_u16(mRegister.Accumulator.D, mMMC.ReadU16(DataByMode_EXT())); }
void CCore::Inst_OpP1_ANDA_B4() { RegOp_AND_EXT(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_LDAA_B6() { RegOp_Load_EXT(mRegister.Accumulator.A); }

void CCore::Inst_OpP1_TFREXG()
{
auto exch = [&](uint8_t& pArg1, uint8_t& pArg2) -> void { uint8_t tval8 = pArg2; pArg2 = pArg1; pArg1 = tval8; };
auto exch0816 = [&](uint8_t& pArg1, uint16_t& pArg2) -> void { uint16_t tval16 = (uint16_t)pArg1; pArg1 = pArg2; pArg2 = tval16; };
auto exchF816 = [&](uint8_t& pArg1, uint16_t& pArg2) -> void { uint16_t tval16 = 0xFF00 | pArg1; pArg1 = pArg2; pArg2 = tval16; };
auto exch16 = [&](uint16_t& pArg1, uint16_t& pArg2) -> void { uint16_t tval16 = pArg1; pArg1 = pArg2; pArg2 = tval16; };

uint8_t objcode;

	objcode = mMMC.ReadU8(mRegister.PC), mRegister.PC++;// По объектному коду определяем тип операции.
	switch(objcode)
	{
		/**** TFR ****/
		// A
		case 0x00: return;// A => A
		case 0x01: mRegister.Accumulator.B = mRegister.Accumulator.A; return;
		case 0x02: mRegister.CC.All = mRegister.Accumulator.A; return;
		case 0x04: mRegister.Accumulator.D = (int16_t)(int8_t)mRegister.Accumulator.A; break;
		case 0x05: mRegister.Index.X = (int16_t)(int8_t)mRegister.Accumulator.A; break;
		case 0x06: mRegister.Index.Y = (int16_t)(int8_t)mRegister.Accumulator.A; break;
		case 0x07: mRegister.SP = (int16_t)(int8_t)mRegister.Accumulator.A; break;
		// B
		case 0x10: mRegister.Accumulator.A = mRegister.Accumulator.B; return;
		case 0x11: return;// B => B
		case 0x12: mRegister.CC.All = mRegister.Accumulator.B; return;
		case 0x14: mRegister.Accumulator.D = (int16_t)(int8_t)mRegister.Accumulator.B; break;
		case 0x15: mRegister.Index.X = (int16_t)(int8_t)mRegister.Accumulator.B; break;
		case 0x16: mRegister.Index.Y = (int16_t)(int8_t)mRegister.Accumulator.B; break;
		case 0x17: mRegister.SP = (int16_t)(int8_t)mRegister.Accumulator.B; break;
		// CCR
		case 0x20: mRegister.Accumulator.A = mRegister.CC.All; return;
		case 0x21: mRegister.Accumulator.B = mRegister.CC.All; return;
		case 0x22: return;// CCR => CCR
		case 0x24: mRegister.Accumulator.D = (int16_t)(int8_t)mRegister.CC.All; return;
		case 0x25: mRegister.Index.X = (int16_t)(int8_t)mRegister.CC.All; return;
		case 0x26: mRegister.Index.Y = (int16_t)(int8_t)mRegister.CC.All; return;
		case 0x27: mRegister.SP = (int16_t)(int8_t)mRegister.CC.All; return;
		// D
		case 0x45: mRegister.Index.X = mRegister.Accumulator.D; return;
		case 0x46: mRegister.Index.Y = mRegister.Accumulator.D; return;
		// X
		case 0x54: mRegister.Accumulator.D = mRegister.Index.X; return;
		case 0x56: mRegister.Index.Y = mRegister.Index.X; return;
		case 0x57: mRegister.SP = mRegister.Index.X; return;
		// Y
		case 0x60: mRegister.Accumulator.A = mRegister.Index.Y; return;
		case 0x61: mRegister.Accumulator.B = mRegister.Index.Y; return;
		case 0x64: mRegister.Accumulator.D = mRegister.Index.Y; return;
		case 0x65: mRegister.Index.X = mRegister.Index.Y; return;
		// SP
		case 0x70: mRegister.Accumulator.A = mRegister.SP; return;
		case 0x71: mRegister.Accumulator.B = mRegister.SP; return;
		case 0x72: mRegister.CC.All = mRegister.SP; return;
		case 0x74: mRegister.Accumulator.D = mRegister.SP; return;
		case 0x75: mRegister.Index.X = mRegister.SP; return;
		case 0x76: mRegister.Index.Y = mRegister.SP; return;
		case 0x77: return;// SP => SP

		/**** EXG ****/
		case 0x80: return;// A <=> A
		case 0x81: exch(mRegister.Accumulator.B, mRegister.Accumulator.A); return;
		case 0x82: exch(mRegister.CC.All, mRegister.Accumulator.A); return;
		case 0x84: mRegister.Accumulator.D = (uint16_t)mRegister.Accumulator.A; break;
		case 0x85: exch0816(mRegister.Accumulator.A, mRegister.Index.X); break;
		case 0x86: exch0816(mRegister.Accumulator.A, mRegister.Index.Y); break;
		case 0x87: exch0816(mRegister.Accumulator.A, mRegister.SP); break;
		// B
		case 0x90: exch(mRegister.Accumulator.A, mRegister.Accumulator.B); return;
		case 0x91: return;// B <=> B
		case 0x92: exch(mRegister.CC.All, mRegister.Accumulator.B); return;
		case 0x94: mRegister.Accumulator.D = (uint16_t)mRegister.Accumulator.B; break;
		case 0x95: exch0816(mRegister.Accumulator.B, mRegister.Index.X); break;
		case 0x96: exch0816(mRegister.Accumulator.B, mRegister.Index.Y); break;
		case 0x97: exch0816(mRegister.Accumulator.B, mRegister.SP); break;
		// CCR
		case 0xA0: exch(mRegister.Accumulator.A, mRegister.CC.All); return;
		case 0xA1: exch(mRegister.Accumulator.B, mRegister.CC.All); return;
		case 0xA2: return;// CCR <=> CCR
		case 0xA4: exch0816(mRegister.CC.All, mRegister.Accumulator.D); return;
		case 0xA5: exch0816(mRegister.CC.All, mRegister.Index.X); break;
		case 0xA6: exch0816(mRegister.CC.All, mRegister.Index.Y); break;
		case 0xA7: exch0816(mRegister.CC.All, mRegister.SP); break;
		// D
		case 0xC0: exch(mRegister.Accumulator.A, mRegister.Accumulator.B); return;
		case 0xC1: mRegister.Accumulator.A = 0xFF; return;
		case 0xC2: exchF816(mRegister.CC.All, mRegister.Accumulator.D); return;
		case 0xC4: return;// D <=> D
		case 0xC5: exch16(mRegister.Index.X, mRegister.Accumulator.D); return;
		case 0xC6: exch16(mRegister.Index.Y, mRegister.Accumulator.D); return;
		case 0xC7: exch16(mRegister.SP, mRegister.Accumulator.D); return;
		// X
		case 0xD0: exch0816(mRegister.Accumulator.A, mRegister.Index.X); return;
		case 0xD1: exchF816(mRegister.Accumulator.B, mRegister.Index.X); return;
		case 0xD2: exchF816(mRegister.CC.All, mRegister.Index.X); return;
		case 0xD4: exch16(mRegister.Accumulator.D, mRegister.Index.X); return;
		case 0xD5: return;// X <=> X
		case 0xD6: exch16(mRegister.Index.Y, mRegister.Index.X); return;
		case 0xD7: exch16(mRegister.SP, mRegister.Index.X); return;
		// Y
		case 0xE0: exch0816(mRegister.Accumulator.A, mRegister.Index.Y); return;
		case 0xE1: exchF816(mRegister.Accumulator.B, mRegister.Index.Y); return;
		case 0xE2: exchF816(mRegister.CC.All, mRegister.Index.Y); return;
		case 0xE4: exch16(mRegister.Accumulator.D, mRegister.Index.Y); return;
		case 0xE5: exch16(mRegister.Index.X, mRegister.Index.Y); return;
		case 0xE6: return;// Y <=> Y
		case 0xE7: exch16(mRegister.SP, mRegister.Index.Y); return;
		default:
			qFatal("TFR: unknown object code 0x%X", objcode);
	}
}

void CCore::Inst_OpP1_EORA_B8() { RegOp_EOR_EXT(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADCA_B9() { RegOp_AddC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_ORAA_BA() { RegOp_OR_EXT(mRegister.Accumulator.A); }
void CCore::Inst_OpP1_ADDA_BB() { RegOp_AddwoC_u8(mRegister.Accumulator.A, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_CPD_BC() { RegOp_Compare_EXT(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_CPY_BD() { RegOp_Compare_EXT(mRegister.Index.Y); }
void CCore::Inst_OpP1_CPX_BE() { RegOp_Compare_EXT(mRegister.Index.X); }
void CCore::Inst_OpP1_CPS_BF() { RegOp_Compare_EXT(mRegister.SP); }

/********** 0xC0 **********/

void CCore::Inst_OpP1_SUBB_C0() { RegOp_SubwoC_u8(mRegister.Accumulator.B, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_CMPB_C1() { RegOp_Compare_IMM_8i(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_SBCB_C2() { RegOp_SubC_u8(mRegister.Accumulator.B, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_ADDD_C3() { RegOp_AddwoC_u16(mRegister.Accumulator.D, DataByMode_IMM_16i()); }
void CCore::Inst_OpP1_ANDB_C4() { RegOp_AND_IMM_8i(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_LDAB_C6() { RegOp_Load_IMM_8i(mRegister.Accumulator.B); }

void CCore::Inst_OpP1_CLRB()
{
	mRegister.Accumulator.B = 0;// Режим адресации - IMM.
	UpdateConditionFlags_ForClear();
}

void CCore::Inst_OpP1_EORB_C8() { RegOp_EOR_IMM_8i(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADCB_C9() { RegOp_AddC_u8(mRegister.Accumulator.B, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_ORAB_CA() { RegOp_OR_IMM_8i(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADDB_CB() { RegOp_AddwoC_u8(mRegister.Accumulator.B, DataByMode_IMM_8i()); }
void CCore::Inst_OpP1_LDD_CC() { RegOp_Load_IMM_16i(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_LDY_CD() { RegOp_Load_IMM_16i(mRegister.Index.Y); }
void CCore::Inst_OpP1_LDX_CE() { RegOp_Load_IMM_16i(mRegister.Index.X); }
void CCore::Inst_OpP1_LDS_CF() { RegOp_Load_IMM_16i(mRegister.SP); }

/********** 0xD0 **********/

void CCore::Inst_OpP1_SUBB_D0() { RegOp_SubwoC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_CMPB_D1() { RegOp_Compare_DIR(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_SBCB_D2() { RegOp_SubC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_ADDD_D3() { RegOp_AddwoC_u16(mRegister.Accumulator.D, mMMC.ReadU16(DataByMode_DIR())); }
void CCore::Inst_OpP1_ANDB_D4() { RegOp_AND_DIR(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_LDAB_D6() { RegOp_Load_DIR(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_TSTB() { UpdateConditionFlags_NZV0C0(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_EORB_D8() { RegOp_EOR_DIR(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADCB_D9() { RegOp_AddC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_ORAB_DA() { RegOp_OR_DIR(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADDB_DB() { RegOp_AddwoC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_DIR())); }
void CCore::Inst_OpP1_LDD_DC() { RegOp_Load_DIR(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_LDY_DD() { RegOp_Load_DIR(mRegister.Index.Y); }
void CCore::Inst_OpP1_LDX_DE() { RegOp_Load_DIR(mRegister.Index.X); }
void CCore::Inst_OpP1_LDS_DF() { RegOp_Load_DIR(mRegister.SP); }

/********** 0xE0 **********/

void CCore::Inst_OpP1_SUBB_E0() { RegOp_SubwoC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_CMPB_E1() { RegOp_Compare_pb(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_SBCB_E2() { RegOp_SubC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_ADDD_E3() { RegOp_AddwoC_u16(mRegister.Accumulator.D, mMMC.ReadU16(DataByMode_PostByte())); }
void CCore::Inst_OpP1_ANDB_E4() { RegOp_AND_pb(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_LDAB_E6() { RegOp_Load_pb(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_TST_E7() { UpdateConditionFlags_NZV0C0(mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_EORB_E8() { RegOp_EOR_pb(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADCB_E9() { RegOp_AddC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_ORAB_EA() { RegOp_OR_pb(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADDB_EB() { RegOp_AddwoC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_PostByte())); }
void CCore::Inst_OpP1_LDD_EC() { RegOp_Load_pb(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_LDY_ED() { RegOp_Load_pb(mRegister.Index.Y); }
void CCore::Inst_OpP1_LDX_EE() { RegOp_Load_pb(mRegister.Index.X); }
void CCore::Inst_OpP1_LDS_EF() { RegOp_Load_pb(mRegister.SP); }

/********** 0xF0 **********/

void CCore::Inst_OpP1_SUBB_F0() { RegOp_SubwoC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_CMPB_F1() { RegOp_Compare_EXT(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_SBCB_F2() { RegOp_SubC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_ADDD_F3() { RegOp_AddwoC_u16(mRegister.Accumulator.D, mMMC.ReadU16(DataByMode_EXT())); }
void CCore::Inst_OpP1_ANDB_F4() { RegOp_AND_EXT(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_LDAB_F6() { RegOp_Load_EXT(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_TST_F7() { UpdateConditionFlags_NZV0C0(mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_EORB_F8() { RegOp_EOR_EXT(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADCB_F9() { RegOp_AddC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_ORAB_FA() { RegOp_OR_EXT(mRegister.Accumulator.B); }
void CCore::Inst_OpP1_ADDB_FB() { RegOp_AddwoC_u8(mRegister.Accumulator.B, mMMC.ReadU8(DataByMode_EXT())); }
void CCore::Inst_OpP1_LDD_FC() { RegOp_Load_EXT(mRegister.Accumulator.D); }
void CCore::Inst_OpP1_LDY_FD() { RegOp_Load_EXT(mRegister.Index.Y); }
void CCore::Inst_OpP1_LDX_FE() { RegOp_Load_EXT(mRegister.Index.X); }
void CCore::Inst_OpP1_LDS_FF() { RegOp_Load_EXT(mRegister.SP); }

/********* 0x1800 *********/

void CCore::Inst_OpP2_MOVW_00()
{
uint16_t tgtaddr;

	// No inlines because need strong order of reading data.
	tgtaddr = DataByMode_PostByte();
	mMMC.WriteU16(tgtaddr, DataByMode_IMM_16i());
}

void CCore::Inst_OpP2_MOVW_01()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	tgtaddr = DataByMode_PostByte();
	srcaddr = DataByMode_EXT();
	mMMC.WriteU16(tgtaddr, mMMC.ReadU16(srcaddr));
}

void CCore::Inst_OpP2_MOVW_02()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	srcaddr = DataByMode_PostByte();
	tgtaddr = DataByMode_PostByte();
	mMMC.WriteU16(tgtaddr, mMMC.ReadU16(srcaddr));
}

void CCore::Inst_OpP2_MOVW_03()
{
uint16_t val16;
uint16_t tgtaddr;

	// No inlines because need strong order of reading data.
	val16 = DataByMode_IMM_16i();
	tgtaddr = DataByMode_EXT();
	mMMC.WriteU16(tgtaddr, val16);
}

void CCore::Inst_OpP2_MOVW_04()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	srcaddr = DataByMode_EXT();
	tgtaddr = DataByMode_EXT();
	mMMC.WriteU16(tgtaddr, mMMC.ReadU16(srcaddr));
}

void CCore::Inst_OpP2_MOVW_05()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	srcaddr = DataByMode_PostByte();
	tgtaddr = DataByMode_EXT();
	mMMC.WriteU16(tgtaddr, mMMC.ReadU16(srcaddr));
}

void CCore::Inst_OpP2_MOVB_08()
{
uint16_t tgtaddr;

	// No inlines because need strong order of reading data.
	tgtaddr = DataByMode_PostByte();
	mMMC.WriteU8(tgtaddr, DataByMode_IMM_8i());
}

void CCore::Inst_OpP2_MOVB_09()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	tgtaddr = DataByMode_PostByte();
	srcaddr = DataByMode_EXT();
	mMMC.WriteU8(tgtaddr, mMMC.ReadU8(srcaddr));
}

void CCore::Inst_OpP2_MOVB_0A()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	srcaddr = DataByMode_PostByte();
	tgtaddr = DataByMode_PostByte();
	mMMC.WriteU8(tgtaddr, mMMC.ReadU8(srcaddr));
}

void CCore::Inst_OpP2_MOVB_0B()
{
uint8_t val8;
uint16_t tgtaddr;

	// No inlines because need strong order of reading data.
	val8 = DataByMode_IMM_8i();
	tgtaddr = DataByMode_EXT();
	mMMC.WriteU8(tgtaddr, val8);
}

void CCore::Inst_OpP2_MOVB_0C()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	srcaddr = DataByMode_EXT();
	tgtaddr = DataByMode_EXT();
	mMMC.WriteU8(tgtaddr, mMMC.ReadU8(srcaddr));
}

void CCore::Inst_OpP2_MOVB_0D()
{
uint16_t srcaddr, tgtaddr;

	// No inlines because need strong order of reading data.
	srcaddr = DataByMode_PostByte();
	tgtaddr = DataByMode_EXT();
	mMMC.WriteU8(tgtaddr, mMMC.ReadU8(srcaddr));
}

void CCore::Inst_OpP2_TAB()
{
	mRegister.Accumulator.B = mRegister.Accumulator.A;
	UpdateConditionFlags_NZV0(mRegister.Accumulator.A);
}

void CCore::Inst_OpP2_TBA()
{
	mRegister.Accumulator.A = mRegister.Accumulator.B;
	UpdateConditionFlags_NZV0(mRegister.Accumulator.B);
}

/********* 0x1810 *********/

void CCore::Inst_OpP2_IDIV()
{
uint16_t quot;

	if(mRegister.Index.X != 0)
	{
		quot = mRegister.Accumulator.D / mRegister.Index.X;
		mRegister.Accumulator.D %= mRegister.Index.X;// Остаток.
		mRegister.Index.X = quot;
		UpdateConditionFlags_Z_u16(quot);
		mRegister.CC.V = false;
		mRegister.CC.C = false;
	}
	else
	{
		mRegister.Index.X = 0xFFFF;
		mRegister.CC.C = true;
	}
}

void CCore::Inst_OpP2_FDIV()
{
uint32_t numerator;

	if(mRegister.Index.X != 0)
	{
		mRegister.CC.V = (mRegister.Index.X <= mRegister.Accumulator.D);
		numerator = mRegister.Accumulator.D;
		numerator <<= 16;
		mRegister.Index.X = numerator / mRegister.Index.X;
		mRegister.Accumulator.D = numerator % mRegister.Index.X;// Остаток.
		UpdateConditionFlags_Z_u16(mRegister.Index.X);
		mRegister.CC.C = false;
	}
	else
	{
		mRegister.Index.X = 0xFFFF;
		mRegister.CC.C = true;
	}
}

void CCore::Inst_OpP2_EMULS()
{
int32_t res;

	res = (int16_t)mRegister.Accumulator.D;
	res *= (int16_t)mRegister.Index.Y;
	mRegister.Index.Y = res >> 16;
	mRegister.Accumulator.D = res;
	UpdateConditionFlags_NZ_u32(res);
	mRegister.CC.C = ((res & (1 << 15)) != 0);
}

void CCore::Inst_OpP2_EDIVS()
{
uint32_t arg1;
uint32_t res;

	if(mRegister.Index.X != 0)
	{
		arg1 = ((uint32_t)mRegister.Index.Y << 16) | mRegister.Accumulator.D;
		res = (int32_t)arg1 / (int16_t)mRegister.Index.X;
		mRegister.Accumulator.D = (int32_t)arg1 % (int16_t)mRegister.Index.X;
		mRegister.Index.Y = res;
		UpdateConditionFlags_NZ_u32(res);
		mRegister.CC.V = (res > 0x0000FFFF);
		mRegister.CC.C = false;
	}
	else
	{
		mRegister.CC.C = true;
	}
}

void CCore::Inst_OpP2_IDIVS()
{
uint16_t quot;

	if(mRegister.Index.X != 0)
	{
		mRegister.CC.V = (mRegister.Index.X = 0xFFFF) && (mRegister.Accumulator.D == 0x8000);
		quot = (int16_t)mRegister.Accumulator.D / (int16_t)mRegister.Index.X;
		mRegister.Accumulator.D = (int16_t)mRegister.Accumulator.D % (int16_t)mRegister.Index.X;// Остаток.
		mRegister.Index.X = quot;
		UpdateConditionFlags_NZ_u16(quot);
		mRegister.CC.C = false;
	}
	else
	{
		mRegister.Index.X = 0xFFFF;
		mRegister.CC.C = true;
	}
}

void CCore::Inst_OpP2_CBA() { RegOp_Compare_u8(mRegister.Accumulator.A, mRegister.Accumulator.B); }

/********* 0x1820 *********/

#define Inst_OpP2_LBranch(pCondition)	\
	int16_t rel16; \
	 \
	rel16 = DataByMode_IMM_16i(); \
	if(pCondition) mRegister.PC += rel16; \
	 \
	do {} while(false)

void CCore::Inst_OpP2_LBRA() { Inst_OpP2_LBranch(true); }
void CCore::Inst_OpP2_LBRN() { Inst_OpP2_LBranch(false); }
void CCore::Inst_OpP2_LBHI() { Inst_OpP2_LBranch(!(mRegister.CC.C || mRegister.CC.Z)); }
void CCore::Inst_OpP2_LBLS() { Inst_OpP2_LBranch(mRegister.CC.C || mRegister.CC.Z); }
void CCore::Inst_OpP2_LBCC() { Inst_OpP2_LBranch(!mRegister.CC.C); }
void CCore::Inst_OpP2_LBCS() { Inst_OpP2_LBranch(mRegister.CC.C); }
void CCore::Inst_OpP2_LBNE() { Inst_OpP2_LBranch(!mRegister.CC.Z); }
void CCore::Inst_OpP2_LBEQ() { Inst_OpP2_LBranch(mRegister.CC.Z); }
void CCore::Inst_OpP2_LBVC() { Inst_OpP2_LBranch(!mRegister.CC.V); }
void CCore::Inst_OpP2_LBVS() { Inst_OpP2_LBranch(mRegister.CC.V); }
void CCore::Inst_OpP2_LBPL() { Inst_OpP2_LBranch(!mRegister.CC.N); }
void CCore::Inst_OpP2_LBMI() { Inst_OpP2_LBranch(mRegister.CC.N); }
void CCore::Inst_OpP2_LBGE() { Inst_OpP2_LBranch(!(mRegister.CC.N ^ mRegister.CC.V)); }
void CCore::Inst_OpP2_LBLT() { Inst_OpP2_LBranch(mRegister.CC.N ^ mRegister.CC.V); }
void CCore::Inst_OpP2_LBGT() { Inst_OpP2_LBranch(!(mRegister.CC.Z || (mRegister.CC.N ^ mRegister.CC.V))); }
void CCore::Inst_OpP2_LBLE() { Inst_OpP2_LBranch(mRegister.CC.Z || (mRegister.CC.N ^ mRegister.CC.V)); }
