#pragma once
#include <string>
#include <vector>

#include <shared/postMaster/Observer.h>
#include "Window.h"

class World;
class Texture;

namespace MENU
{
	class ConsoleWindow : public WindowBase, public FE::Observer
	{
	public:
		ConsoleWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		~ConsoleWindow();

		void Show(const UpdateContext& aContext) override;

	private:
		//// Portable helpers
		//static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
		//static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
		//static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }
		static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
		
		void ClearLog();
		void AddLog(const char* fmt, ...) IM_FMTARGS(2);
		
		void RecieveMessage(const FE::Message& aMessage) override;

		char                  InputBuf[256];
		ImVector<char*>       Items;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImGuiTextFilter       Filter;
		bool                  AutoScroll;
		bool                  ScrollToBottom;

		ImVec4 Red = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
		ImVec4 Yellow = ImVec4(1.0f, 0.4f, 0.f, 1.0f);
		ImVec4 Blue = ImVec4(0.f, 0.2f, 0.8f, 1.0f);
	};
}