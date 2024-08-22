
#pragma once

#include "../WorkStep.h"
#include "../../GlobalVariables.h"

class EnvironmentalCalibWork : public CWorkStep
{
public:
	EnvironmentalCalibWork();
	virtual ~EnvironmentalCalibWork();

	virtual	BOOL		InitWork();
	virtual VOID		DoWork();

protected:
	VOID				SetWorkNext();
};
