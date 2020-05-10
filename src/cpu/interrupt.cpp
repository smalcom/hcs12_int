///TODO: unc
#if 0
#include "cpuinterrupt.h"
#include "../n64memory.h"

#include <cexception.h>

extern bool CPUDoInterruptCheckPended;

uint32_t VerticalRefreshTime;

bool CPUIsInterruptSet(enum ECPUInterruptSource pInterruptSource)
{
	switch(pInterruptSource)
	{
		case CIS_Timer:
			return (CPUCOP0RegisterCause & CC0RM_CauseIP7) != 0;
		case CIS_IRQ0:
			return (CPUCOP0RegisterCause & CC0RM_CauseIP2) != 0;
		default:
			Throw("IsInterruptSet. Unknown interrupt source");
	}
	return false;
}

void CPUInterruptSet(enum ECPUInterruptSource pInterruptSource)
{
uint32_t statusim = CPUCOP0RegisterStatus & CC0RM_StatusIM;
bool ie = CPUCOP0RegisterStatus & CC0RM_StatusIE;

	//interrupts disabled?
	if(ie == 0) return;
	switch(pInterruptSource)
	{
		case CIS_Timer:
			CPUCOP0RegisterCause |= CC0RM_CauseIP7;
			if((statusim & CC0RM_CauseIP7) == 0) return;
			CPUDoInterruptCheckPended = true;
			break;
		case CIS_IRQ0:
			CPUCOP0RegisterCause |= CC0RM_CauseIP2;
			if((statusim & CC0RM_CauseIP2) == 0) return;
			CPUDoInterruptCheckPended = true;
			break;
		default:
			Throw("InterruptSet. Unknown interrupt source");
	}
}

void CPUInterruptClear(enum ECPUInterruptSource pInterruptSource)
{
	switch(pInterruptSource)
	{
		case CIS_IRQ0:
			CPUCOP0RegisterCause &= ~CC0RM_CauseIP2;
			break;
		case CIS_Timer:
			CPUCOP0RegisterCause &= ~CC0RM_CauseIP7;
			break;
		default:
			Throw("InterruptClear. Unknown interrupt source");
	}
}

void CPUInterruptCheckTimer(void)
{
	if(CPUCOP0RegisterCount == CPUCOP0RegisterCompare)
	{
		CPUInterruptSet(CIS_Timer);
	}
}

//void MemoryMIInterruptSignalSet(enum EMIInterruptSource pInterruptSource);
void CPUInterruptCheckVI(void)
{
	if(CPUCOP0RegisterCount == VerticalRefreshTime)
	{
		VerticalRefreshTime += T_VerticalRefresh;
		MemoryMIInterruptSignalSet(MIS_VideoInterface);
	}
}

void CPUInterruptCheckPended(void)
{
uint32_t flags = CPUCOP0RegisterCause & CPUCOP0RegisterStatus & CC0RM_StatusIM;
bool ie = CPUCOP0RegisterStatus & CC0RM_StatusIE;

	//interrupts disabled?
	if((flags == 0) || (ie == 0)) return;
	if((flags & (CC0RM_CauseIP2 | CC0RM_CauseIP7)) != 0)
	{
		CPUCOP0ExceptionTry(CEC_Interrupt);
		return;
	}
	Throw("Unknown interrupt pended.");
}
#endif // 0
