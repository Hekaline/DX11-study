// HLSL 셰이더를 코딩하는 법, 꼭짓점 및 인덱스 버퍼를 설정하는 법,
// ColorShaderClass를 사용하여 해당 버퍼를 그리기 위해 HLSL 셰이더를 호출하는 방법은 알지만
// DirectX11이 어디에서 어떻게 장면을 보고 있는지 알려주기 위해 카메라 클래스가 필요하다.
// 이 클래스는 카메라가 어디에 있는지, 어떻게 회전해 있는지를 추적한다.
// 위치 및 회전 정보를 사용하여 렌더링을 위해 HLSL 셰이더에 전달될 뷰 행렬을 생성한다.

////////////////////////////////
// Filename: CameraClass.h
////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
using namespace DirectX;

/////////////////////////////////
// Class name: CameraClass
/////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);


private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif