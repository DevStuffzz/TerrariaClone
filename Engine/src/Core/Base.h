#pragma once

// ENGINE_BUILD_DLL: compiling the Engine DLL (exports symbols).
// ENGINE_STATIC:    compiling engine sources directly into another binary (tests).
// Otherwise:        a client of the DLL (imports symbols).
#if defined(ENGINE_STATIC) || !defined(_WIN32)
    #define ENGINE_API
#elif defined(ENGINE_BUILD_DLL)
    #define ENGINE_API __declspec(dllexport)
#else
    #define ENGINE_API __declspec(dllimport)
#endif
