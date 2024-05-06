////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// 시스템 객체 생성
	System = new SystemClass;

	// 시스템 객체 초기화 및 실행
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// 시스템 객체 셧다운 및 반환
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}