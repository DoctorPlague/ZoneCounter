#include "DateSelector.h"

bool DateSelector::m_bDateActive = false;
const char* DateSelector::m_kcStartDate = "2019/10/14";
const char* DateSelector::m_kcEndDate = "2019/10/24";
ImGuiTextFilter DateSelector::m_StartDateFilter;
ImGuiTextFilter DateSelector::m_EndDateFilter;

DateSelector::DateSelector()
{
	m_Name = "Date Selector";
}

DateSelector::~DateSelector()
{

}

void DateSelector::Update()
{
	// Run window if open
	if (m_bOpen)
	{
		// Determine window flags
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize;

		// Begin Window
		ImGui::SetNextWindowPos(ImVec2(545, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 260), ImGuiCond_Once);
		if (!ImGui::Begin(m_Name, &m_bOpen, window_flags))
		{
			ImGui::End();
			return;
		}

		// Checkbox
		ImGui::Checkbox("Enabled", &m_bDateActive);

		if (m_bDateActive)
		{
			// Header
			ImGui::NewLine();
			ImGui::Text("Start Date:");
			
			// Filter
			m_StartDateFilter.Draw("Ex: 2019/10/14", 80.0f);
			m_kcStartDate = m_StartDateFilter.InputBuf;

			// Header
			ImGui::NewLine();
			ImGui::Text("End Date:");

			// Filter
			m_EndDateFilter.Draw("Ex: 2019/11/14", 80.0f);
			m_kcEndDate = m_EndDateFilter.InputBuf;
		}		

		// Cleanup
		ImGui::End();
	}
}
