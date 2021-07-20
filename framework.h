#pragma once

#define WIN32_LEAN_AND_MEAN             // Escludere gli elementi usati raramente dalle intestazioni di Windows
// File di intestazione di Windows
#include <windows.h>
#include <filesystem>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include "detours.h"
#include "xorstr.hpp"
#pragma comment(lib, "detours.lib")