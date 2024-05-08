////////////////////////////////////
// Filename: CameraClass.cpp
////////////////////////////////////
#include "CameraClass.h"
#include "stdafx.h"

// 변수 초기화
CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

// SetPosition과 SetRotation 함수는 위치와 회전을 설정하는 데 사용된다.
void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}

// GetPosition과 GetRotation은 카메라의 위치와 회전을 반환한다.
XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

// 이 함수는 뷰 행렬을 빌드하고 갱신하기 위해 카메라의 위치와 회전을 사용한다.
// 먼저 위쪽, 위치, 회전 등에 대한 변수를 설정한다. 
// 그런 다음 장면의 원점에서 카메라의 x, y, z 회전을 기반으로 카메라를 먼저 회전한다.
// 제대로 회전되면 카메라를 3D 공간의 위치로 이동한다.
// 위치, LookAt 및 위쪽의 올바른 값을 사용하면 XMMATRIX LookAtLH 함수를 사용하여
// 현재 카메라 회전 및 위치를 나타내는 뷰 행렬을 만들 수 있다.
void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// 위를 가리키는 벡터를 설정한다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR 구조체로 로드한다.
	upVector = XMLoadFloat3(&up);

	// 월드 내 카메라의 위치를 설정한다.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// XMVECTOR 구조체로 로드한다.
	positionVector = XMLoadFloat3(&position);

	// 카메라가 어디를 보고 있는지를 기본값으로 설정한다.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR 구조체로 로드한다.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Yaw(y축), Pitch(X축), Roll(Z축) 회전을 라디안으로 설정한다.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// 회전 행렬을 yaw, pitch, roll 값으로부터 만든다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 회전 행렬에 의해 lookAt 및 up 벡터를 변환하여
	// 뷰가 원점에서 올바르게 회전되도록 함
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전된 카메라 포지션을 뷰어의 위치로 가져온다
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 세 개의 갱신된 벡터로부터 뷰 행렬을 만든다.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

// Render 함수를 호출하여 뷰 행렬을 만든 후 이 함수를 사용하여
// 갱신된 뷰 행렬을 호출 함수에 제공할 수 있다.
// 뷰 행렬을 HLSL 버텍스 셰이더에 사용되는 세 가지 주요 행렬 중 하나이다.
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}