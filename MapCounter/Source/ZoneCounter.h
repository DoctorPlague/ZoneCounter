#pragma once

#include "BaseWindow.h"

class ZoneCounter :	public BaseWindow
{
public:
	ZoneCounter();
	~ZoneCounter();

	// Member Functions
	void Update();
	void LoadZoneInfo();
	void DateFilterZoneInfo();
	void AddFilteredZone(std::string& tempString);
	void SortZoneInfo();
	void ParseDateToSeconds(const char* _kcDate, std::time_t* _time);
	std::string ExtractZoneName(std::string _fullString);

	// Member Variables
	static ImGuiTextBuffer m_TextBuffer;
	static ImGuiTextFilter m_Filter;
	static std::thread m_ProcessThread;
	static std::atomic<bool> m_bThreadActive;
	static bool m_bDataCollected;
	static bool m_bShowDateFiltering;
	static std::unique_ptr<class DateSelector> m_DateSelector;
	static std::multimap<std::string, std::string> m_ZoneData; // First = ZoneName, Second = Date
	static std::map<std::string, int> m_FilteredZoneData;
};

