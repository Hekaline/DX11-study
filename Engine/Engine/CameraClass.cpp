////////////////////////////////////
// Filename: CameraClass.cpp
////////////////////////////////////
#include "CameraClass.h"
#include "stdafx.h"

// ���� �ʱ�ȭ
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

// SetPosition�� SetRotation �Լ��� ��ġ�� ȸ���� �����ϴ� �� ���ȴ�.
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

// GetPosition�� GetRotation�� ī�޶��� ��ġ�� ȸ���� ��ȯ�Ѵ�.
XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

// �� �Լ��� �� ����� �����ϰ� �����ϱ� ���� ī�޶��� ��ġ�� ȸ���� ����Ѵ�.
// ���� ����, ��ġ, ȸ�� � ���� ������ �����Ѵ�. 
// �׷� ���� ����� �������� ī�޶��� x, y, z ȸ���� ������� ī�޶� ���� ȸ���Ѵ�.
// ����� ȸ���Ǹ� ī�޶� 3D ������ ��ġ�� �̵��Ѵ�.
// ��ġ, LookAt �� ������ �ùٸ� ���� ����ϸ� XMMATRIX LookAtLH �Լ��� ����Ͽ�
// ���� ī�޶� ȸ�� �� ��ġ�� ��Ÿ���� �� ����� ���� �� �ִ�.
void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// ���� ����Ű�� ���͸� �����Ѵ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	upVector = XMLoadFloat3(&up);

	// ���� �� ī�޶��� ��ġ�� �����Ѵ�.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	positionVector = XMLoadFloat3(&position);

	// ī�޶� ��� ���� �ִ����� �⺻������ �����Ѵ�.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Yaw(y��), Pitch(X��), Roll(Z��) ȸ���� �������� �����Ѵ�.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// ȸ�� ����� yaw, pitch, roll �����κ��� �����.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// ȸ�� ��Ŀ� ���� lookAt �� up ���͸� ��ȯ�Ͽ�
	// �䰡 �������� �ùٸ��� ȸ���ǵ��� ��
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// ȸ���� ī�޶� �������� ����� ��ġ�� �����´�
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// �� ���� ���ŵ� ���ͷκ��� �� ����� �����.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

// Render �Լ��� ȣ���Ͽ� �� ����� ���� �� �� �Լ��� ����Ͽ�
// ���ŵ� �� ����� ȣ�� �Լ��� ������ �� �ִ�.
// �� ����� HLSL ���ؽ� ���̴��� ���Ǵ� �� ���� �ֿ� ��� �� �ϳ��̴�.
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}