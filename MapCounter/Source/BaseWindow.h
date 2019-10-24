#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <set>
#include <algorithm>
#include <functional>
#include <vector>
#include <sstream>
#include <thread>
#include <atomic>
#include <iomanip>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

class BaseWindow
{
public:
	BaseWindow();
	~BaseWindow();

	// Member functions
	virtual void Update() = 0;

	// Member variables
	const char* m_Name = nullptr;
	bool m_bOpen = false;
};

