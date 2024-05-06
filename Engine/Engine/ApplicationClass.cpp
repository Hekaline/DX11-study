////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	// 클래스 변수 초기화
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Direct3D 객체 생성 및 초기화
	m_Direct3D = new D3DClass;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
	}

	// 카메라 객체 생성
	m_Camera = new CameraClass;

	// 카메라 기본 위치 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// 모델 객체 생성 및 초기화
	m_Model = new ModelClass;

	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initializ the model object.", L"Error", MB_OK);
		return false;
	}

	// 컬러 셰이더 객체 생성 및 초기화
	m_ColorShader = new ColorShaderClass;

	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
	// 컬러 셰이더 객체 반환
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// 모델 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 카메라 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

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

// 장면 클리어
// 카메라 객체에 대해 Render 함수를 호출하여 Initialize 함수에서 설정한 카메라 위치를 기반으로 뷰 행렬 생성
// 생성되면 카메라 클래스에서 해당 뷰 행렬의 복사본 가져옴
// D3DClass 객체에서 월드와 프로젝션 행렬의 복사본 가져옴
// 녹색 삼각형 모델 지오메트리를 그래픽 파이프라인에 배치하기 위해 ModelClass::Render 함수 호출
// 이제 정점이 준비된 상태에서 모델 정보와 각 정점을 배치하는 세 개의 행렬을 사용하여 컬러 셰이더를 호출하여 정점을 그림
// 녹색 삼각형이 백버퍼에 그려진다.
// 장면이 완성되고 화면에 표시하기 위해 EndScene을 호출한다.
bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 씬을 시작하기 위해 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 포지션을 바탕으로 뷰 매트릭스를 생성한다.
	m_Camera->Render();

	// 월드, 뷰와 프로젝션 행렬들을 카메라와 D3D 오브젝트들에서 가져온다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델 꼭짓점과 인덱스 버퍼들을 그래픽 파이프라인에 배치하여 도면 작성을 준비
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 컬러 셰이더를 사용하여 모델 렌더링
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix);
	
	if (!result)
	{
		return false;
	}

	// 씬을 그리기 위해 버퍼를 지웁니다
	// 매개변수는 차례대로 RGBA 입니다.
    // m_Direct3D->BeginScene(1.f, 1.f, 0.0f, 1.f);
 
 
    // 버퍼의 내용을 화면에 출력합니다
    m_Direct3D->EndScene();
 
    return true;
}