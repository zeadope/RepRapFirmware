/****************************************************************************************************

RepRapFirmware - Tool

This class implements a tool in the RepRap machine, usually (though not necessarily) an extruder.

Tools may have zero or more drives associated with them and zero or more heaters.  There are a fixed number
of tools in a given RepRap, with fixed heaters and drives.  All this is specified on reboot, and cannot
be altered dynamically.  This restriction may be lifted in the future.  Tool descriptions are stored in
GCode macros that are loaded on reboot.

-----------------------------------------------------------------------------------------------------

Version 0.1

Created on: Apr 11, 2014

Adrian Bowyer
RepRap Professional Ltd
http://reprappro.com

Licence: GPL

****************************************************************************************************/

#include "RepRapFirmware.h"

Tool::Tool(int toolNumber, long d[], int dCount, long h[], int hCount)
{
	myNumber = toolNumber;
	next = NULL;
	active = false;
	driveCount = dCount;
	heaterCount = hCount;
	heaterFault = false;
	mixing = false;

	if(driveCount > 0)
	{
		if(driveCount > DRIVES - AXES)
		{
			reprap.GetPlatform()->Message(HOST_MESSAGE, "Tool creation: attempt to use more drives than there are in the RepRap...");
			driveCount = 0;
			heaterCount = 0;
			return;
		}
		drives = new int[driveCount];
		mix = new float[driveCount];
		float r = 1.0/(float)driveCount;
		for(int8_t drive = 0; drive < driveCount; drive++)
		{
			drives[drive] = d[drive];
			mix[drive] = r;
		}
	}

	if(heaterCount > 0)
	{
		if(heaterCount > HEATERS)
		{
			reprap.GetPlatform()->Message(HOST_MESSAGE, "Tool creation: attempt to use more heaters than there are in the RepRap...");
			driveCount = 0;
			heaterCount = 0;
			return;
		}
		heaters = new int[heaterCount];
		activeTemperatures = new float[heaterCount];
		standbyTemperatures = new float[heaterCount];
		for(int8_t heater = 0; heater < heaterCount; heater++)
		{
			heaters[heater] = h[heater];
			activeTemperatures[heater] = ABS_ZERO;
			standbyTemperatures[heater] = ABS_ZERO;
		}
	}
}

void Tool::Print(char* reply)
{
	snprintf(reply, STRING_LENGTH, "Tool %d - drives: ", myNumber);
	char comma = ',';
	for(int8_t drive = 0; drive < driveCount; drive++)
	{
		if(drive >= driveCount - 1)
			comma = ';';
		snprintf(scratchString, STRING_LENGTH, "%d%c ", drives[drive], comma);
		strncat(reply, scratchString, STRING_LENGTH);
	}
	strncat(reply, "heaters (active/standby temps): ", STRING_LENGTH);
	comma = ',';
	for(int8_t heater = 0; heater < heaterCount; heater++)
	{
			if(heater >= heaterCount - 1)
				comma = ';';
			snprintf(scratchString, STRING_LENGTH, "%d (%.1f/%.1f)%c ", heaters[heater],
					activeTemperatures[heater], standbyTemperatures[heater], comma);
			strncat(reply, scratchString, STRING_LENGTH);
	}
	strncat(reply, " status: ", STRING_LENGTH);
	if(active)
		strncat(reply, "selected", STRING_LENGTH);
	else
		strncat(reply, "standby", STRING_LENGTH);
}

float Tool::MaxFeedrate()
{
	if(driveCount <= 0)
	{
		reprap.GetPlatform()->Message(HOST_MESSAGE, "Attempt to get maximum feedrate for a tool with no drives.\n");
		return 1.0;
	}
	float result = 0.0;
	float mf;
	for(int8_t d = 0; d < driveCount; d++)
	{
		mf = reprap.GetPlatform()->MaxFeedrate(drives[d] + AXES);
		if(mf > result)
			result = mf;
	}
	return result;
}

float Tool::InstantDv()
{
	if(driveCount <= 0)
	{
		reprap.GetPlatform()->Message(HOST_MESSAGE, "Attempt to get InstantDv for a tool with no drives.\n");
		return 1.0;
	}
	float result = FLT_MAX;
	float idv;
	for(int8_t d = 0; d < driveCount; d++)
	{
		idv = reprap.GetPlatform()->InstantDv(drives[d] + AXES);
		if(idv < result)
			result = idv;
	}
	return result;
}

// Add a tool to the end of the linked list.
// (We must already be in it.)

void Tool::AddTool(Tool* tool)
{
	Tool* t = this;
	Tool* last;
	while(t != NULL)
	{
		if(t->Number() == tool->Number())
		{
			reprap.GetPlatform()->Message(HOST_MESSAGE, "Add tool: tool number already in use.\n");
			return;
		}
		last = t;
		t = t->Next();
	}
	tool->next = NULL; // Defensive...
	last->next = tool;
}

// There is a temperature fault on a heater.
// Disable all tools using that heater.
// This function must be called for the first
// entry in the linked list.

void Tool::FlagTemperatureFault(int8_t heater)
{
	Tool* n = this;
	while(n != NULL)
	{
		n->SetTemperatureFault(heater);
		n = n->Next();
	}
}

void Tool::ClearTemperatureFault(int8_t heater)
{
	Tool* n = this;
	while(n != NULL)
	{
		n->ResetTemperatureFault(heater);
		n = n->Next();
	}
}

void Tool::SetTemperatureFault(int8_t dudHeater)
{
	for(int8_t heater = 0; heater < heaterCount; heater++)
		if(dudHeater == heaters[heater])
		{
			heaterFault = true;
			return;
		}
}

void Tool::ResetTemperatureFault(int8_t wasDudHeater)
{
	for(int8_t heater = 0; heater < heaterCount; heater++)
		if(wasDudHeater == heaters[heater])
		{
			heaterFault = false;
			return;
		}
}

bool Tool::AllHeatersAtHighTemperature()
{
	for(int8_t heater = 0; heater < heaterCount; heater++)
		if(reprap.GetHeat()->GetTemperature(heaters[heater]) < HOT_ENOUGH_TO_EXTRUDE)
			return false;
	return true;
}

void Tool::Activate(Tool* currentlyActive)
{
	if(active)
		return;
	if(currentlyActive != NULL && currentlyActive != this)
		currentlyActive->Standby();
	for(int8_t heater = 0; heater < heaterCount; heater++)
	{
		reprap.GetHeat()->SetActiveTemperature(heaters[heater], activeTemperatures[heater]);
		reprap.GetHeat()->SetStandbyTemperature(heaters[heater], standbyTemperatures[heater]);
		reprap.GetHeat()->Activate(heaters[heater]);
	}
	active = true;
}

void Tool::Standby()
{
	if(!active)
		return;
	for(int8_t heater = 0; heater < heaterCount; heater++)
	{
		reprap.GetHeat()->SetStandbyTemperature(heaters[heater], standbyTemperatures[heater]);
		reprap.GetHeat()->Standby(heaters[heater]);
	}
	active = false;
}

void Tool::SetVariables(float* standby, float* active)
{
	for(int8_t heater = 0; heater < heaterCount; heater++)
	{
		activeTemperatures[heater] = active[heater];
		standbyTemperatures[heater] = standby[heater];
		reprap.GetHeat()->SetActiveTemperature(heaters[heater], activeTemperatures[heater]);
		reprap.GetHeat()->SetStandbyTemperature(heaters[heater], standbyTemperatures[heater]);
	}
}

void Tool::GetVariables(float* standby, float* active)
{
	for(int8_t heater = 0; heater < heaterCount; heater++)
	{
		active[heater] = activeTemperatures[heater];
		standby[heater] = standbyTemperatures[heater];
	}
}

bool Tool::ToolCanDrive()
{
	if(heaterFault)
		return false;
	if(reprap.ColdExtrude() || AllHeatersAtHighTemperature())
		return true;
	return false;
}

