#include "UI.h"
#include "graphics/GL.h"
#include "graphics/Shader.h"
#include "graphics/Geometry.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace library
{
	class UI::Impl
	{
	public:
		Impl();
		virtual ~Impl();

		void draw(ImDrawData* draw_data);

	private:
		ImGuiContext * mImgui;
		GLuint  g_FontTexture = 0;
		Shader mShader;
		Geometry mGeometry;
	};

	UI::Impl::Impl()
	{
		mImgui = ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();

		// Create font
		unsigned char *pixels;
		int width, height;
		io.Fonts->AddFontDefault();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		// Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &g_FontTexture);
		glBindTexture(GL_TEXTURE_2D, g_FontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

		// Restore state
		glBindTexture(GL_TEXTURE_2D, last_texture);

		mShader.fromString(R"(
			[Vertex]
			#version 430
			layout (location = 0) in vec3 VertexPosition;
			layout (location = 2) in vec2 VertexTexCoord;
			//in vec4 Color;
			out vec2 TexCoord;
			out vec4 vColor;

			uniform mat4 projection;

			void main()
			{
				TexCoord = VertexTexCoord;
				vColor = vec4(1, 1, 1, 0); //Color;
				//gl_Position = projection * vec4(VertexPosition.xy, 0, 1);
				gl_Position = vec4(VertexPosition.xy, 0, 1);
			}

			[Fragment]
			#version 430
			in vec2 TexCoord;
			in vec4 vColor;
			out vec4 FragColor;

			uniform sampler2D colorMap;

			void main()
			{
				//FragColor = vColor * texture(colorMap, TexCoord.st);
				FragColor = texture(colorMap, TexCoord);
			}
		)");

		std::vector<vec2> vertices = {
			vec2(-1.0f, -1.0f),
			vec2(1.0f, -1.0f),
			vec2(-1.0f, 1.0f),
			vec2(1.0f, 1.0f)
		};

		std::vector<unsigned int> indices = { 0, 1, 2, 1, 3, 2 };

		std::vector<vec2> texCoords = {
			vec2(0.0f, 1.0f),
			vec2(1.0f, 1.0f),
			vec2(0.0f, 0.0f),
			vec2(1.0f, 0.0f)
		};

		mGeometry.setVertices(vertices);
		mGeometry.setIndices(indices);
		mGeometry.setTexCoords(texCoords);
	}

	UI::Impl::~Impl()
	{
		if (g_FontTexture)
		{
			glDeleteTextures(1, &g_FontTexture);
			ImGui::GetIO().Fonts->TexID = 0;
			g_FontTexture = 0;
		}

		ImGui::DestroyContext();
	}

	void UI::Impl::draw(ImDrawData* draw_data)
	{
		ImGuiIO& io = ImGui::GetIO();

		/*const float ortho_projection[4][4] =
		{
			{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
			{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f,                  0.0f,                    -1.0f, 0.0f },
			{ -1.0f,                 1.0f,                     0.0f, 1.0f },
		};*/

		mat4 orthoProjection(0.0f);
		orthoProjection[0][0] = 2.0f / io.DisplaySize.x;
		orthoProjection[0][1] = 2.0f / -io.DisplaySize.y;
		orthoProjection[2][2] = -1.0f;
		orthoProjection[3][0] = -1.0f;
		orthoProjection[3][1] = 1.0f;
		orthoProjection[3][3] = 1.0f;

		/*
		vec4 orthoProjection(0.0f);
		orthoProjection[0] = 2.0f / io.DisplaySize.x;
		orthoProjection[5] = 2.0f / -io.DisplaySize.y;
		orthoProjection[10] = -1.0f;
		orthoProjection[12] = -1.0f;
		orthoProjection[13] = 1.0f;
		orthoProjection[15] = 1.0f;
		*/

		mShader.bind();
		//mShader.setUniform("projection", orthoProjection);
		mGeometry.draw();
	}

	//=========================================================================

	UI::UI() : impl{ std::make_unique<Impl>() }
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.PopupRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabRounding = 0.0f;
	}

	UI::~UI()
	{
		//imguiShutdown();
	}

	void UI::setEvents(const Events &events)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)events.windowSize.x, (float)events.windowSize.y);
		io.DeltaTime = events.deltaTime;
		io.KeyShift = events.isKeyShiftDown;
		io.KeyCtrl = events.isKeyCtrlDown;
		io.KeyAlt = events.isKeyAltDown;
		io.KeySuper = events.isKeySuperDown;
		io.MousePos = ImVec2((float)events.mousePosition.x, (float)events.mousePosition.y);
		io.MouseWheel = (float)events.mouseWheel;
		io.MouseDown[0] = events.isMouseButtonLeftDown;
		io.MouseDown[1] = events.isMouseButtonRightDown;
		io.MouseDown[2] = events.isMouseButtonMiddleDown;
		memcpy(io.KeysDown, events.keysDown, 512 * sizeof(bool));
		io.AddInputCharactersUTF8(events.textInput);
	}

	void UI::frameStart()
	{
		ImGui::NewFrame();
	}

	void UI::frameEnd()
	{
		ImGui::Render();
		impl->draw(ImGui::GetDrawData());
	}
}