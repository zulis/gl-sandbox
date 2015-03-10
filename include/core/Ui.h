#pragma once

#include "core/Window.h"
#include "../external/src/imgui/imgui.h"

namespace ui = ImGui;

namespace ImGuiWrapper
{
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

int mShader, mVertex, mFragment;
int texture_location, proj_mtx_location;
unsigned int mVbo, mVao;
size_t mVboMaxSize = 20000;

//=========================================================================
void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
	if(cmd_lists_count == 0)
		return;

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup orthographic projection matrix
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	const float ortho_projection[4][4] =
	{
		{ 2.0f / width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / -height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
	};
	glUseProgram(mShader);
	glUniform1i(texture_location, 0);
	glUniformMatrix4fv(proj_mtx_location, 1, GL_FALSE, &ortho_projection[0][0]);

	// Grow our buffer according to what we need
	size_t total_vtx_count = 0;

	for(int n = 0; n < cmd_lists_count; n++)
		total_vtx_count += cmd_lists[n]->vtx_buffer.size();

	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	size_t neededBufferSize = total_vtx_count * sizeof(ImDrawVert);

	if(neededBufferSize > mVboMaxSize)
	{
		mVboMaxSize = neededBufferSize + 5000;  // Grow buffer
		glBufferData(GL_ARRAY_BUFFER, mVboMaxSize, NULL, GL_STREAM_DRAW);
	}

	// Copy and convert all vertices into a single contiguous buffer
	unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	if(!buffer_data)
		return;

	for(int n = 0; n < cmd_lists_count; n++)
	{
		const ImDrawList* cmd_list = cmd_lists[n];
		memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
		buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(mVao);

	int cmd_offset = 0;

	for(int n = 0; n < cmd_lists_count; n++)
	{
		const ImDrawList* cmd_list = cmd_lists[n];
		int vtx_offset = cmd_offset;
		const ImDrawCmd* pcmd_end = cmd_list->commands.end();

		for(const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
		{
			glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
			glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
			glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
			vtx_offset += pcmd->vtx_count;
		}

		cmd_offset = vtx_offset;
	}

	// Restore modified state
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_SCISSOR_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//=========================================================================
void LoadFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	//ImFont* my_font1 = io.Fonts->AddFontDefault();
	//ImFont* my_font2 = io.Fonts->AddFontFromFileTTF("extra_fonts/Karla-Regular.ttf", 15.0f);
	//ImFont* my_font3 = io.Fonts->AddFontFromFileTTF("extra_fonts/ProggyClean.ttf", 13.0f); my_font3->DisplayOffset.y += 1;
	//ImFont* my_font4 = io.Fonts->AddFontFromFileTTF("extra_fonts/ProggyTiny.ttf", 10.0f); my_font4->DisplayOffset.y += 1;
	//ImFont* my_font5 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 20.0f, io.Fonts->GetGlyphRangesJapanese());

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void*)(intptr_t)tex_id;
}

//=========================================================================
void Initialize()
{
	const GLchar* vertexShader =
	    "#version 330\n"
	    "uniform mat4 ProjMtx;\n"
	    "in vec2 Position;\n"
	    "in vec2 UV;\n"
	    "in vec4 Color;\n"
	    "out vec2 Frag_UV;\n"
	    "out vec4 Frag_Color;\n"
	    "void main()\n"
	    "{\n"
	    "	Frag_UV = UV;\n"
	    "	Frag_Color = Color;\n"
	    "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
	    "}\n";

	const GLchar* fragmentShader =
	    "#version 330\n"
	    "uniform sampler2D Texture;\n"
	    "in vec2 Frag_UV;\n"
	    "in vec4 Frag_Color;\n"
	    "out vec4 Out_Color;\n"
	    "void main()\n"
	    "{\n"
	    "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
	    "}\n";

	mShader = glCreateProgram();
	mVertex = glCreateShader(GL_VERTEX_SHADER);
	mFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mVertex, 1, &vertexShader, 0);
	glShaderSource(mFragment, 1, &fragmentShader, 0);
	glCompileShader(mVertex);
	glCompileShader(mFragment);
	glAttachShader(mShader, mVertex);
	glAttachShader(mShader, mFragment);
	glLinkProgram(mShader);

	texture_location = glGetUniformLocation(mShader, "Texture");
	proj_mtx_location = glGetUniformLocation(mShader, "ProjMtx");
	int position_location = glGetAttribLocation(mShader, "Position");
	int uv_location = glGetAttribLocation(mShader, "UV");
	int colour_location = glGetAttribLocation(mShader, "Color");

	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboMaxSize, NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(uv_location);
	glEnableVertexAttribArray(colour_location);

	glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(colour_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.0f / 60.0f;                                  // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.RenderDrawListsFn = ImGuiWrapper::ImImpl_RenderDrawLists;
	//io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
	//io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;

	ImGuiWrapper::LoadFontsTexture();

	ImGuiStyle mStyle;

	/*mStyle.WindowPadding = ImVec2(10, 10);
	mStyle.WindowMinSize = ImVec2(160, 80);
	mStyle.FramePadding = ImVec2(4, 4);
	mStyle.ItemSpacing = ImVec2(8, 4);
	mStyle.ItemInnerSpacing = ImVec2(6, 4);
	mStyle.WindowFillAlphaDefault = 1.0f;
	mStyle.WindowRounding = 2.0f;
	mStyle.FrameRounding = 2.0f;
	mStyle.TreeNodeSpacing = 6;
	mStyle.ColumnsMinSpacing = 50;
	mStyle.ScrollbarWidth = 12;*/

// 	mStyle.Colors[ImGuiCol_Text] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
// 	mStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
// 	mStyle.Colors[ImGuiCol_Border] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
// 	mStyle.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.38f);
// 	mStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
// 	mStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
// 	mStyle.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ComboBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
// 	mStyle.Colors[ImGuiCol_CheckHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
// 	mStyle.Colors[ImGuiCol_CheckActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
// 	mStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.22f, 0.22f, 0.22f, 0.50f);
// 	mStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
// 	mStyle.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
 	mStyle.Colors[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
 	mStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
 	mStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
// 	mStyle.Colors[ImGuiCol_Header] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
// 	mStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
// 	mStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
// 	mStyle.Colors[ImGuiCol_Column] = ImVec4(0.04f, 0.04f, 0.04f, 0.22f);
// 	mStyle.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ColumnActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
// 	mStyle.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
// 	mStyle.Colors[ImGuiCol_CloseButton] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
// 	mStyle.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
// 	mStyle.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
// 	mStyle.Colors[ImGuiCol_PlotLines] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
// 	mStyle.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
// 	mStyle.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
// 	mStyle.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
// 	mStyle.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
// 	mStyle.Colors[ImGuiCol_TooltipBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);

	ImGui::GetStyle() = mStyle;
}

//=========================================================================
void Update(const Window& window)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup resolution (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(window.getGLFWWindow(), &w, &h);
	glfwGetFramebufferSize(window.getGLFWWindow(), &display_w, &display_h);
	io.DisplaySize = ImVec2((float)display_w, (float)display_h);                                   // Display size, in pixels. For clamping windows positions.

	// Setup time step
	static double time = 0.0f;
	const double current_time = glfwGetTime();
	io.DeltaTime = (float)(current_time - time);
	time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	double mouse_x, mouse_y;
	glfwGetCursorPos(window.getGLFWWindow(), &mouse_x, &mouse_y);
	mouse_x *= (float)display_w / w;                                                               // Convert mouse coordinates to pixels
	mouse_y *= (float)display_h / h;
	io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);                                          // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	io.MouseDown[0] = window.getInput().isMouseDown(MouseButton::Left);
	io.MouseDown[1] = window.getInput().isMouseDown(MouseButton::Right);
	//io.MouseDown[0] = mousePressed[0] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	//io.MouseDown[1] = mousePressed[1] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != 0;

	// Start the frame
	ImGui::NewFrame();
}

//=========================================================================
void Draw()
{
	ImGui::Render();
}

//=========================================================================
void Shutdown()
{
	// Cleanup
	if(mVao) glDeleteVertexArrays(1, &mVao);

	if(mVbo) glDeleteBuffers(1, &mVbo);

	glDetachShader(mShader, mVertex);
	glDetachShader(mShader, mFragment);
	glDeleteShader(mVertex);
	glDeleteShader(mFragment);
	glDeleteProgram(mShader);

	ImGui::Shutdown();
}
}