#include "ZoneCounter.h"

#include "DateSelector.h"

// Initialize static member variables
ImGuiTextBuffer ZoneCounter::m_TextBuffer;
ImGuiTextFilter ZoneCounter::m_Filter;
std::thread ZoneCounter::m_ProcessThread;
std::atomic<bool> ZoneCounter::m_bThreadActive = false;
bool ZoneCounter::m_bDataCollected = false;
bool ZoneCounter::m_bShowDateFiltering = false;
std::unique_ptr<DateSelector> ZoneCounter::m_DateSelector = std::make_unique<DateSelector>();
std::multimap<std::string, std::string> ZoneCounter::m_ZoneData; // First = ZoneName, Second = Date
std::map<std::string, int> ZoneCounter::m_FilteredZoneData;

ZoneCounter::ZoneCounter()
{
	m_Name = "Zone Counter";	
}

ZoneCounter::~ZoneCounter()
{

}

void ZoneCounter::Update()
{
	// Run date selector window
	m_DateSelector->Update();	

	// Run window if open
	if (m_bOpen)
	{
		// Determine window flags
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize;

		// Begin Window
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
		if (!ImGui::Begin(m_Name, &m_bOpen, window_flags))
		{
			ImGui::End();
			return;
		}

		// Buttons
		if (ImGui::Button("Load Zone Info"))
		{			
			// Begin LoadingZoneInfo on seperate thread
			m_ProcessThread = std::thread(&ZoneCounter::LoadZoneInfo, this);

			// Set flags
			m_bThreadActive = true;
			m_bDataCollected = false;			
		}

		// make a window that allows the user to use sliders to select start and end dates
		ImGui::SameLine();
		if (ImGui::Button("Date Filtering"))
		{
			// Toggle Show Window
			m_DateSelector->m_bOpen = !m_DateSelector->m_bOpen;
		}

		// BeginChild - Text Log
		ImGui::BeginChild("Log");

		// Draw Filter		
		m_Filter.Draw();

		// Only use m_TextBuffer if our Process Thread is inactive
		if (m_bThreadActive == false)
		{
			// Output ZoneInfo
			std::string bufferContents = m_TextBuffer.c_str();
			std::string currentLine;
			for (int i = 0; i < bufferContents.size(); i++)
			{
				// If newline char, we've finished storing a line
				if (bufferContents[i] == '\n')
				{
					// Check if line passes filter
					if (m_Filter.PassFilter(currentLine.c_str()))
					{
						ImGui::Text(currentLine.c_str());
					}

					// Clear line
					currentLine.clear();
				}
				else
				{
					// add to current line
					currentLine.push_back(bufferContents[i]);
				}
			}

			// Join Threads if possible
			if (m_bDataCollected && m_ProcessThread.joinable())
			{
				m_ProcessThread.join();
			}
		}
		else
		{
			// Display text to indicate loading
			ImGui::Text("Loading data...");
		}

		// Cleanup
		ImGui::EndChild();
		ImGui::End();
	}	
}

void ZoneCounter::LoadZoneInfo()
{
	// Only load the file if we haven't already
	if (m_ZoneData.size() < 1)
	{
		// Collect all zone entry data 
		std::string line;
		std::ifstream myfile("C:\\Program Files (x86)\\.My Games\\Grinding Gear Games\\Path of Exile\\logs\\Client.txt");
		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				std::size_t returnValue = line.find("entered");
				if (returnValue != std::string::npos)
				{
					std::istringstream dateSS(line.substr(0, 10));
					std::string ZoneName = ExtractZoneName(line);
					std::string ZoneDate = dateSS.str();
					m_ZoneData.insert(std::pair<std::string, std::string>(ZoneName, ZoneDate));
				}
			}
			myfile.close();
		}
	}
		

	// Parse zone info and count up how many times each zone appears
	DateFilterZoneInfo();

	// Sort the zone info by frequency
	SortZoneInfo();
}

void ZoneCounter::DateFilterZoneInfo() 
{	
	// clear the filtered map
	m_FilteredZoneData.clear();

	// Parse dates to seconds so we can compare dates
	std::time_t startTime;
	std::time_t endTime;
	ParseDateToSeconds(m_DateSelector->m_kcStartDate, &startTime);
	ParseDateToSeconds(m_DateSelector->m_kcEndDate, &endTime);	

	std::time_t tempTime;
	for (auto& itr : m_ZoneData)
	{		
		// Get the date of the itr zone and parse it to seconds
		std::string firstString = itr.first;
		std::string secondString = itr.second;		
		std::string tempSubstring = secondString.substr(0, 10);
		ParseDateToSeconds(tempSubstring.c_str(), &tempTime);

		bool bDateActive = m_DateSelector->m_bDateActive;
		if (bDateActive)
		{
			// Check if the date is within our start/end bounds
			if (tempTime >= startTime && tempTime <= endTime)
			{				
				AddFilteredZone(firstString);
			}
		}
		else
		{
			AddFilteredZone(firstString);
		}		
	}	
}

void ZoneCounter::AddFilteredZone(std::string& tempString)
{
	// Increment the zones counter if zone already registered, otherwise insert pair
	std::map<std::string, int>::iterator FilteredItr;
	FilteredItr = m_FilteredZoneData.find(tempString);
	if (FilteredItr != m_FilteredZoneData.end())
	{
		FilteredItr->second++;
	}
	else
	{
		m_FilteredZoneData.insert(std::pair<const char*, int>(tempString.c_str(), 1));
	}
}

void ZoneCounter::SortZoneInfo()
{
	// Sorting
	// Declaring the type of Predicate that accepts 2 pairs and return a bool
	typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;

	// Defining a lambda function to compare two pairs. It will compare two pairs using second field
	Comparator compFunctor =
		[](std::pair<std::string, int> elem1, std::pair<std::string, int> elem2)
	{
		return elem1.second > elem2.second;
	};

	// Declaring a set that will store the pairs using above comparision logic
	std::set<std::pair<std::string, int>, Comparator> setOfZones(m_FilteredZoneData.begin(), m_FilteredZoneData.end(), compFunctor);

	// Clear the text buffer
	m_TextBuffer.clear();

	// Iterate over a set using range base for loop
	// It will display the items in sorted order of values
	for (std::pair<std::string, int> element : setOfZones)
	{
		std::string tempString;
		std::stringstream ss;
		ss << element.first;
		ss << " :: ";
		ss << element.second;

		tempString = ss.str();
		tempString.push_back('\n');
		m_TextBuffer.append(tempString.c_str());
	}

	// Set flags
	m_bThreadActive = false;
	m_bDataCollected = true;
}

void ZoneCounter::ParseDateToSeconds(const char* _kcDate, std::time_t* _time)
{
	// Parse and calculate date in seconds
	const char* kcDate = _kcDate;
	std::istringstream ss(kcDate);
	static std::tm dateTMB;
	ss >> std::get_time(&dateTMB, "%Y/%m/%d");
	time_t tempTime = std::mktime(&dateTMB);
	*_time = tempTime;
}

std::string ZoneCounter::ExtractZoneName(std::string _fullString)
{
	auto index = _fullString.find("entered");

	std::string ZoneName;
	for (int i = index + 8; i < _fullString.length() - 1; i++)
	{
		ZoneName.push_back(_fullString[i]);
	}
	return ZoneName;
}
