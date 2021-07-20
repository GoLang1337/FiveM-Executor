#include "pch.h"
#include "ShellCode.h"

std::ofstream stream;
std::string exploitablePath, scriptPath, scriptContent;

typedef __int64(__fastcall* t_LoadSystemFile)(__int64 m_state, char* resourceFile, __int64 a3);
t_LoadSystemFile LoadSystemFile = (t_LoadSystemFile)((uintptr_t)GetModuleHandle(xorstr_(L"citizen-scripting-lua.dll")) + 0x27470);

__int64 __fastcall hkLoadSystemFile(__int64 m_state, char* resourceFile, __int64 a3) {
    
    std::string resourceName(resourceFile); // casting to string because char* will not be used as string in the statement

    if (resourceName == exploitablePath) {
        char* path = _strdup(scriptPath.c_str()); // convert string to char*
        return LoadSystemFile(m_state, path, a3); // if exploitable return exploit
    }
    else
        return LoadSystemFile(m_state, resourceFile, a3); // if not exploitable return normal
}

DWORD WINAPI MainThread()
{
    enum pathsEnum { // all types of default paths
        nativesLoader,
        deferred,
        graph,
        scheduler
    };

    std::string paths[] = { // all exploitable paths for lua execution (script runtime are too)
        "citizen:/scripting/lua/natives_loader.lua",
        "citizen:/scripting/lua/deferred.lua",
        "citizen:/scripting/lua/graph.lua",
        "citizen:/scripting/lua/scheduler.lua"
        //"rcon_client.lua"
        //"cl_chat.lua"
        //"client.lua"
    };

    exploitablePath = paths[pathsEnum::graph];
    scriptPath = "C:\\PerfLogs\\client.lua";

    if(std::filesystem::exists(scriptPath))
        std::filesystem::remove(scriptPath.c_str()); // remove the file of scriptPath if already exists

    stream.open(scriptPath.c_str());
    if (exploitablePath == paths[pathsEnum::nativesLoader]) { // better to find a "switch" string method
        std::string content(reinterpret_cast<char*>(ShellCode::nativesLoader), sizeof(ShellCode::nativesLoader));
        stream << content << std::endl << std::endl;
    }
    else if (exploitablePath == paths[pathsEnum::deferred]) {
        std::string content(reinterpret_cast<char*>(ShellCode::deferred), sizeof(ShellCode::deferred));
        stream << content << std::endl << std::endl;
    }
    else if (exploitablePath == paths[pathsEnum::graph]) {
        std::string content(reinterpret_cast<char*>(ShellCode::graph), sizeof(ShellCode::graph));
        stream << content << std::endl << std::endl;
    }
    else if (exploitablePath == paths[pathsEnum::scheduler]) {
        std::string content(reinterpret_cast<char*>(ShellCode::scheduler), sizeof(ShellCode::scheduler));
        stream << content << std::endl << std::endl;
    }

    stream << "if GetCurrentResourceName() == 'rconlog' then \n GiveWeaponToPed(PlayerPedId(), 0x1B06D571, 1000, false, true) \n end"; // statement for executing in a custom resource
    stream.close(); // close buffer

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(LPVOID&)LoadSystemFile, &hkLoadSystemFile); // hooking LSF
    DetourTransactionCommit();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::thread(MainThread).detach(); // create thread and detach it
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
