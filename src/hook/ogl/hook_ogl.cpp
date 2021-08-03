#include "hook/ogl/hook_ogl.h"
#include "hook/gpc_hook_utils.h"

namespace gpc {
    bool HookOGLEntry(const char* dllName) {
        HMODULE module = LoadLibrary(dllName);
        if (module) {
            void* pFuncAddress = GetProcAddress(module, "wglSwapBuffers");
        }
        return true;
    }
}
