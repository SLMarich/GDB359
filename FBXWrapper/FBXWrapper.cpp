// FBXWrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FBXWrapper.h"

namespace FBXWrapper {
	FBXWRAPPER_API double TestFunction::add(double a, double b) {
		return a + b;
	}
}

