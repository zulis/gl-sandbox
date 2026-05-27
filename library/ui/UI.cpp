#include "UI.h"

#include "system/Subsystem.h"
#include "window/Window.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <cassert>

namespace library
{
	class UI::Impl
	{
	public:
		Impl();
		~Impl();

		void makeCurrent();
		void draw(ImDrawData *draw_data);

		float deltaTime{1.0f / 60.0f};

	private:
		ImGuiContext *context{nullptr};
	};

	UI::Impl::Impl()
	{
		auto &window = subsystem::get<Window>();

		context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);

		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.Fonts->AddFontDefault();

		ImGui::StyleColorsDark();

		ImGuiStyle &style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.PopupRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabRounding = 0.0f;

		assert(ImGui_ImplSDL3_InitForOpenGL(window.getSDLWindow(), window.getGLContext()));
		assert(ImGui_ImplOpenGL3_Init("#version 430 core"));

		window.eventCallback = [](const SDL_Event *event)
		{
			ImGui_ImplSDL3_ProcessEvent(event);
		};
	}

	UI::Impl::~Impl()
	{
		if (subsystem::has<Window>())
		{
			subsystem::get<Window>().eventCallback = nullptr;
		}

		ImGui::SetCurrentContext(context);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext(context);
	}

	void UI::Impl::makeCurrent()
	{
		ImGui::SetCurrentContext(context);
	}

	void UI::Impl::draw(ImDrawData *draw_data)
	{
		ImGui_ImplOpenGL3_RenderDrawData(draw_data);
	}

	UI::UI() : impl{std::make_unique<Impl>()}
	{
	}

	UI::~UI()
	{
	}

	void UI::setEvents(const Events &events)
	{
		impl->makeCurrent();
		if (events.deltaTime > 0.0f)
		{
			impl->deltaTime = events.deltaTime;
		}
	}

	void UI::frameStart()
	{
		impl->makeCurrent();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::GetIO().DeltaTime = impl->deltaTime;
		ImGui::NewFrame();
	}

	void UI::frameEnd()
	{
		impl->makeCurrent();
		ImGui::Render();
		impl->draw(ImGui::GetDrawData());
	}
}