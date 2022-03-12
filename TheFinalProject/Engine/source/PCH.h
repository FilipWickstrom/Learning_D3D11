#pragma once

// Standard
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <chrono>
#include <fcntl.h>
#include <io.h>

// Windows
#include <Windows.h>

// DirectX
#include <d3d11.h>
#include <DirectXMath.h>

// DirectXTK
#include <Keyboard.h>
#include <Mouse.h>

// Paths
const std::string MODELPATH		= "../Assets/Models/";		//remove "../" at start when building zip
const std::string MATERIALPATH	= "../Assets/Materials/";	//remove "../" at start when building zip
const std::string TEXTUREPATH	= "../Assets/Textures/";	//remove "../" at start when building zip
const std::string SHADERPATH	= "Shaders/";
