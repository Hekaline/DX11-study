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

// ���� D3DX10Math.h������ �����Ǿ� �������� DirectXMath.h�� �ٲ����.
#include <DirectXMath.h> 


////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////

// 3D �𵨵��� ������ �����е��� ĸ��ȭ�ϴ� Ŭ����
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position; // ���� D3DXVector3������ DXMath�� ��������� ����ʿ� ���� �ٲ�
		XMFLOAT4 color; // ��������
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