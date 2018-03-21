#pragma once

#include "math/Math.h"
#include <memory>

namespace library
{
	class UI
	{
	public:
		UI();
		virtual ~UI();

		struct Events
		{
			ivec2 windowSize;
			float deltaTime;
			bool isKeyShiftDown;
			bool isKeyCtrlDown;
			bool isKeyAltDown;
			bool isKeySuperDown;
			ivec2 mousePosition;
			int mouseWheel;
			bool isMouseButtonLeftDown;
			bool isMouseButtonRightDown;
			bool isMouseButtonMiddleDown;
			const bool *keysDown;
			const char *textInput;
		};

		void setEvents(const Events &events);
		void frameStart();
		void frameEnd();

	private:
		class Impl;
		std::unique_ptr<Impl> impl;
	};
}