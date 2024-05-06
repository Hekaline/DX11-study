////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SystemClass.h"

// 이들을 null로 만들지 않으면 Shutdown 함수가 이들을 정리할 수도 있다.
SystemClass::SystemClass()
{
	m_Input = 0;
	m_Application = 0;
}

// ExitThread()와 같은 특정 Windows 함수는 클래스 소멸자를 호출하지 않아
// 메모리 누수가 발생하는 것으로 알려져 있습니다.
SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// 함수에 변수들을 보내기 전 스크린 넓이 높이 초기화
	screenWidth = 0;
	screenHeight = 0;

	// Windows API 초기화
	InitializeWindows(screenWidth, screenHeight);

	// 인풋 객체 생성 및 초기화
	// 유저로부터 키보드 입력을 받는 데 사용될 변수
	m_Input = new InputClass;
	m_Input->Initialize();

	// 어플리케이션 클래스 객체 생성 및 초기화
	// 이 어플리케이션에 관한 모든 그래픽을 렌더링하는 것을 조작한다.
	m_Application = new ApplicationClass;
	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);

	if (!result)
	{
		return false;
	}
	return true;
}


// 청소를 함. 어플리케이션과 입력 객체에 관련한 모든 것을 닫음
// 창을 닫고 그와 관련한 핸들들을 청소한다.
void SystemClass::Shutdown()
{
	// 어플리케이션 클래스 객체 삭제 (Release)
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// 입력 객체 삭제
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// 창을 닫음
	ShutdownWindows();

	return;
}

// Run 기능은 애플리케이션이 종료하기로 결정할 때까지 애플리케이션이 반복되고 
// 모든 애플리케이션 처리를 수행하는 곳입니다. 
// 애플리케이션 처리는 각 루프라고 불리는 Frame 함수에서 수행됩니다.
// 
// 애플리케이션의 나머지 부분도 이를 염두에 두고 작성해야 하므로 이는 이해해야 할 중요한 개념입니다.
// 의사 코드는 다음과 같습니다.
// 
// 끝나지 않았을 동안
//    윈도우 시스템 메시지 확인
//    시스템 메시지 실행
//    어플리케이션 무한루프 실행
//	  유저가 프레임 프로세싱 중에 나가기를 원하는지 확인

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// 메시지 구조체 초기화
	ZeroMemory(&msg, sizeof(msg));

	// 창이나 유저로부터 나가라는 메시지가 있을 때까지 루프
	done = false;
	while (!done)
	{
		// 윈도우 메시지 핸들
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우 시그널이 앱을 끝내라고 할 시 끝냄
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 아니면 프레임 프로세싱
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}


// 유저가 나가고 싶은지 아닌지 확인하고
// 나가지 않고 싶어하면 프레임 프로세싱 진행 (그 프레임을 위한 그래픽 렌더링)
bool SystemClass::Frame()
{
	bool result;

	// 유저가 esc 버튼을 누르고 나가고 싶은지 확인
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// 애플리케이션 클래스 객체를 위한 프레임 프로세싱
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

// MessageHandler 함수는 Windows 시스템 메시지를 전달하는 곳입니다. 
// 이런 방식으로 우리는 관심 있는 특정 정보를 들을 수 있습니다.
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 키가 눌렸는지 확인
		case WM_KEYDOWN:
		{
			// 키가 눌렸으면 input 객체로 보내야 상태를 기록할 수 있음
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// 키가 떼졌는지 확인
		case WM_KEYUP:
		{
			// 키가 떼졌으면 마찬가지로 input 객체로 보내야 상태를 기록할 수 있음
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// 다른 메시지는 애플리케이션에서 사용하지 않으므로 기본 메시지 핸들러로 전송됩니다.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

// InitializeWindows 함수는 렌더링에 사용할 창을 빌드하기 위한 코드를 넣는 곳입니다.
// 이는 호출 함수에 screenWidth 및 screenHeight를 다시 반환하므로 애플리케이션 전체에서 이를 사용할 수 있습니다.

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE	dmScreenSettings;
	int posX, posY;

	// 이 개체에 대한 외부 포인터 가져오기
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스 가져오기
	m_hinstance = GetModuleHandle(NULL);

	// 어플리케이션에 이름 부여
	m_applicationName = L"Engine";

	// 윈도우 클래스 기본 설정으로 셋업
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스 등록
	RegisterClassEx(&wc);

	// 클라이언트의 데스크탑 스크린 해상도 결정
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 창 모드인지 풀스크린인지 확인 후 스크린 설정
	if (FULL_SCREEN)
	{
		// 풀스크린이라면 32비트와 데스크탑 사이즈의 최대 크기로 스크린 크기를 바꿈.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 디스플레이 설정 변경
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 창의 위치를 왼쪽 위로 설정
		posX = posY = 0;
	}
	else
	{
		// 풀스크린 아니라면 800x600으로 설정
		screenWidth = 800;
		screenHeight = 600;

		// 창 정가운데로 옮김
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 스크린 설정을 가지고 창을 만든 후 이에 대한 핸들 가져오기
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 창을 가져온 후 메인 포커스로 설정
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서 숨기기
	ShowCursor(false);

	return;
}

// 말 그대로 창을 닫음.
void SystemClass::ShutdownWindows()
{
	// 마우스 커서 보이기
	ShowCursor(true);

	// 전체 화면을 나가면 디스플레이 설정 고치기
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창 지우기
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 어플리케이션 인스턴스 삭제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 이 클래스의 포인터 반환
	ApplicationHandle = NULL;
	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 창이 파괴되고 있는지 확인
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 창이 닫히고 있는지 확인
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 다른 메시지는 이 클래스의 메시지 핸들러에 전달
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}