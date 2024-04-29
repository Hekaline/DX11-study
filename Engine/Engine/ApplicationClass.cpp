////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
	m_Direct3D = 0;
}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Direct3D 오브젝트 생성 및 초기화
	m_Direct3D = new D3DClass;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
	}

	return true;
}

void ApplicationClass::Shutdown()
{
	// Direct3D 객체 반환
    if(m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }
}

bool ApplicationClass::Frame()
{
	// 그래픽 랜더링 처리
    return Render();
}

bool ApplicationClass::Render()
{
	// 씬을 그리기 위해 버퍼를 지웁니다
	// 매개변수는 차례대로 RGBA 입니다.
    m_Direct3D->BeginScene(1.f, 1.f, 0.0f, 1.f);
 
 
    // 버퍼의 내용을 화면에 출력합니다
    m_Direct3D->EndScene();
 
    return true;
}