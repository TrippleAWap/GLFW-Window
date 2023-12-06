#pragma once

class Window
{
	private:
		static const constexpr char* glsl_version = "#version 330";
		const struct flag {
			int GLFW_Flag, State;
		};
	public:
		GLFWwindow* glfwWindow{};
		HWND hwnd{};

		std::map<const char*, bool> states{};
		explicit Window(const int& width, const int& height, const char* title, const std::vector<flag>& flags)
		{
			for (const flag& flag : flags)
				glfwWindowHint(flag.GLFW_Flag, flag.State);
			this->glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
			this->hwnd = glfwGetWin32Window(this->glfwWindow);
			glfwMakeContextCurrent(this->glfwWindow);
			glfwSwapInterval(0);
			ImGui::CreateContext();
			ImGui_ImplGlfw_InitForOpenGL(this->glfwWindow, true);
			ImGui_ImplOpenGL3_Init(this->glsl_version);
		}
		void SetWindowAffinity(const DWORD& affinity)
		{
			SetWindowDisplayAffinity(this->hwnd, affinity);
			glfwShowWindow(this->glfwWindow);
		};
		void SetVisible(const bool& visible)
		{
			if (visible)
				glfwShowWindow(this->glfwWindow);
			else
				glfwHideWindow(this->glfwWindow);
		};
		void NewFrame()
		{	
			glfwPollEvents();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
		void EndFrame()
		{
			ImGui::Render();
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(this->glfwWindow);
		}
		void SetInteractable(const bool& interactable)
		{
			glfwSetWindowAttrib(this->glfwWindow, GLFW_MOUSE_PASSTHROUGH, !interactable);
		}
		~Window()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			glfwDestroyWindow(this->glfwWindow);
			glfwTerminate();
		}
};		