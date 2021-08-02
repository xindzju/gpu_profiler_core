#include "hook/ogl/hook_ogl.h"
#include "hook/gpc_hook_utils.h"

namespace gpc {
    bool HookOGLEntry(const char* dllName) {
        using wglSwapBuffers_t = void(*)(_In_ HDC hDc);
        extern "C" wglSwapBuffers_t wglSwapBuffers_return = nullptr;
        extern "C" void wglSwapBuffersTrampoline();

        // Get a handle to the opengl.dll
        HMODULE hOpengl32 = GetModuleHandle(dllName);

        if (hOpengl32 != nullptr) {
            // Get the address of wglSwapBuffers
            DWORD64 wglSwapBuffersHookAddr = (DWORD64)GetProcAddress(hOpengl32, "wglSwapBuffers");
            // Insert a hook to our trampoline at the start of wglSwapBuffers, returns the address to return to
            wglSwapBuffers_return = (glSwapBuffers_t)mem::hookFunction(wglSwapBuffersHookAddr, (DWORD64)wglSwapBuffersTrampoline, 5);
        }
    }

    HWND hGameWindow;
    WNDPROC hGameWindowProc;
    bool menuShown = true;

    LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        // Toggle the overlay using the delete key
        if (uMsg == WM_KEYDOWN && wParam == VK_DELETE) {
            menuShown = !menuShown;
            return false;
        }

        // If the overlay is shown, direct input to the overlay only
        if (menuShown) {
            CallWindowProc(ImGui_ImplWin32_WndProcHandler, hWnd, uMsg, wParam, lParam);
            return true;
        }

        // Otherwise call the game's wndProc function
        return CallWindowProc(hGameWindowProc, hWnd, uMsg, wParam, lParam);
    }

    void glSwapBuffers_hook(HDC hDc)
    {
        // Initialize glew and imgui but only once
        static bool imGuiInitialized = false;
        if (!imGuiInitialized) {
            imGuiInitialized = true;

            // Get the game's window from it's handle
            hGameWindow = WindowFromDC(hDc);

            // Overwrite the game's wndProc function
            hGameWindowProc = (WNDPROC)SetWindowLongPtr(hGameWindow,
                GWLP_WNDPROC, (LONG_PTR)windowProc_hook);

            // Init glew, create imgui context, init imgui
            glewInit();
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(hGameWindow);
            ImGui_ImplOpenGL3_Init();
            ImGui::StyleColorsDark();
            ImGui::GetStyle().AntiAliasedFill = false;
            ImGui::GetStyle().AntiAliasedLines = false;
            ImGui::CaptureMouseFromApp();
            ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
        }

        // If the menu is shown, start a new frame and draw the demo window
        if (menuShown) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();

            // Draw the overlay
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    }
}
