#pragma once
#include "BaseWindow.h"

class DateSelector : public BaseWindow
{
public:
	DateSelector();
	~DateSelector();

	// Member Functions
	void Update();

	// Member Variables
	static bool m_bDateActive;
	static const char* m_kcStartDate;
	static const char* m_kcEndDate;
	static ImGuiTextFilter m_StartDateFilter;
	static ImGuiTextFilter m_EndDateFilter;
};

