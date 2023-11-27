#include "PYName.h"


PY_BOOL PYName_Initialize(PY_NAME* pName, PY_HAN* pHan)
{
	pName->pHan = pHan;

	return PY_TRUE;
}

PY_VOID PYName_Terminate(PY_NAME* pName)
{

}
