#define GLEW_STATIC
#define GLFW_EXPOSE_NATIVE_WIN32
// ----------------- IMPORTANT -----------------
#include <GL/glew.h>

#include <GLFW/glfw3.h> 
#include <GLFW/glfw3native.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
// ----------------- DEFAULT HEADERS -----------------
#include <iostream>
#include <Windows.h>
#include <map>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <functional>
// ---------------- OTHER CLASSES ----------------
#include "SDK/window.h"

// ----------------- DEFINES -----------------
static void SetGLFWWindowAffinity(GLFWwindow* win, DWORD affinity)
{
	SetWindowDisplayAffinity(glfwGetWin32Window(win), affinity);
	glfwShowWindow(win);
}

static const int glfwListenForKey()
{
	while (true) {
		for (int i = 0x07; i < 0xFE; i++)
		{
			if (GetAsyncKeyState(i) & 1)
				return i;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return -1;
}

static const char* getVKString(int VK) 
{
	UINT scanCode = MapVirtualKey(VK, MAPVK_VK_TO_VSC);
	char keyName[10];
	int result = 0;
	switch (VK) {
	case VK_INSERT:
	case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		scanCode |= KF_EXTENDED;
	default:
		result = GetKeyNameTextA(scanCode << 16, keyName, 10);
	}
	if (result == 0) {
		return nullptr;
	}
	return keyName;
}


static ImVec2 CalcTextSize(const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	va_end(args);
	return ImGui::CalcTextSize(buf);
}

static char* fstring(const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	va_end(args);
	return buf;
}



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int toggleKey{ VK_INSERT };
	if (!glfwInit())
		return 1;
	const GLFWvidmode* metrics = glfwGetVideoMode(glfwGetPrimaryMonitor());
	Window* window = new Window(metrics->width, metrics->height + 1, " ", { // +1 adds an inperfection which is needed ( if perfect size will be black )
		{ GLFW_FOCUSED, GLFW_TRUE },
		{ GLFW_DECORATED, GLFW_FALSE },
		{ GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE },
		{ GLFW_FLOATING, GLFW_TRUE },
		{ GLFW_SCALE_TO_MONITOR, GLFW_TRUE }
	});
	// hide taskbar icon
	SetWindowLong(glfwGetWin32Window(window->glfwWindow), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	glfwShowWindow(window->glfwWindow);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();
	bool running = true;
	bool render = true;
	window->states["interactable"] = true;
	window->states["streamproof"] = false;
	std::map<const char*, int> lastStates{};
	ImFont* font1 = io.Fonts->AddFontFromFileTTF("bold_font.ttf", 30);
	ImFont* font2 = io.Fonts->AddFontDefault();
	float sliderValue{ 3.0f };
	while (!glfwWindowShouldClose(window->glfwWindow) && running) {
		window->NewFrame();
		if (window->states["streamproof"] != lastStates["streamproof"]) {
			lastStates["streamproof"] = window->states["streamproof"];
			window->SetWindowAffinity(window->states["streamproof"] ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
		}
		ImDrawList* drawList = ImGui::GetForegroundDrawList();

		if (GetAsyncKeyState(toggleKey) & 1) {
			render = !render;
			window->states["interactable"] = !window->states["interactable"];
			window->SetInteractable(window->states["interactable"]);
		}
		if (GetAsyncKeyState(VK_DELETE) & 1)
			running = false;
		if (render) {
			ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings);
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::SetWindowSize(ImVec2(metrics->width, metrics->height));

			ImGui::BeginTabBar("##tabbar", ImGuiTabBarFlags_NoTooltip);
			if (ImGui::BeginTabItem("Combat")) {
				ImGui::SetCursorPos(ImVec2(10, 50));
				ImGui::Checkbox("Aimbot", &window->states["aimbot"]);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Visuals")) {
				ImGui::SetCursorPos(ImVec2(10, 50));
				if (ImGui::Checkbox("Streamproof", &window->states["streamproof"]))
				{
					window->SetWindowAffinity(window->states["streamproof"] ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Misc")) {
				ImGui::SetCursorPos(ImVec2(10, 50));
				if (ImGui::Button("x", ImVec2(50, 50))) {
					window->SetInteractable(false);
					toggleKey = glfwListenForKey();
					window->SetInteractable(true);
				}
				ImGui::SetCursorPos(ImVec2(50, 50));
				ImGui::Text("Toggle Key: %s", getVKString(toggleKey));
				ImGui::EndTabItem();
			}

			ImGui::End();
		}

		const char* text = fstring("FPS: %.2f Time Per Frame: %.1fms", io.Framerate, io.DeltaTime * 1000);
		drawList->AddRectFilled(ImVec2(metrics->width - CalcTextSize(text).x - 5, 0), ImVec2(metrics->width, CalcTextSize(text).y), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.7f)));
		drawList->AddText(ImVec2(metrics->width - CalcTextSize(text).x, 0), ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.7f, 1.0f)), text);

		window->EndFrame();
	}
	delete window;
	return 0;
}