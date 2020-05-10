#pragma once

#include <inttypes.h>

/*************************************/
/************* Константы *************/
/*************************************/

enum class EInterruptVector : uint16_t
{
	Reset = 0xFFFE
};

///TODO: unc
#if 0

#include "cpu.h"
#include "cop0.h"
#include "cop0exception.h"

#include <basetypes.h>

/////////////////////////////////////////////////
//		Enums
/////////////////////////////////////////////////
//! For SetInterrupt
enum ECPUInterruptSource
{
	//IP2
	CIS_IRQ0,
	//IP7
	CIS_Timer
};

enum EMIInterruptSource
{
	MIS_VideoInterface,
	MIS_PeripheralInterface,
	MIS_SerialInterface,
        MIS_AudioInterface
};

extern uint32_t VerticalRefreshTime;

bool CPUIsInterruptSet(enum ECPUInterruptSource pInterruptSource);
void CPUInterruptSet(enum ECPUInterruptSource pInterruptSource);
void CPUInterruptClear(enum ECPUInterruptSource pInterruptSource);

void CPUInterruptCheckTimer(void);
void CPUInterruptCheckVI(void);
void CPUInterruptCheckPended(void);

#endif // 0
