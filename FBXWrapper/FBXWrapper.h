#pragma once

#ifdef FBXWRAPPER_EXPORTS  
#define FBXWRAPPER_API __declspec(dllexport)   
#else  
#define FBXWRAPPER_API __declspec(dllimport)   
#endif  

#include "stdafx.h"

namespace FBXWrapper {
	class TestFunction {
	public:
		static FBXWRAPPER_API double add(double a, double b);
	};
}