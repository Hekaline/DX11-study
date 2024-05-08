/////////////////////////////////////////
// Filename: modelclass.h
/////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

//////////////
// INCLUDES //
//////////////
#include "stdafx.h"
#include <d3d11.h>

// 본래 D3DX10Math.h였으나 오래되어 없어지고 DirectXMath.h로 바뀌었다.
#include <DirectXMath.h> 


////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////

// 3D 모델들의 복잡한 기하학들을 캡슐화하는 클래스
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position; // 본래 D3DXVector3였으나 DXMath로 헤더파일이 변경됨에 따라 바뀜
		XMFLOAT4 color; // 마찬가지
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif