#include "stdafx.h"
#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// 정점 버퍼와 인덱스 버퍼 초기화
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// 정점 버퍼와 인덱스 버퍼 해제
	ShutdownBuffers();

	return;
}

// GraphicsClass::Render()에서 호출
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어
	// 화면에 그릴 준비하기
	RenderBuffers(deviceContext);
	return;
}

// 인덱스 개수 Getter
// 컬러 셰이더에서 모델을 그리려면 인덱스 정보 필요
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


// 정점 버퍼와 인덱스 버퍼를 생성하는 작업 제어
// 데이터 파일로부터 모델의 정보를 읽어와 버퍼를 만드는 일을 한다.
// 현재 삼각형을 그리는데, 주의할 점은 삼각형의 점들을 시계 방향으로 만들어야 한다는 점이다.
// 그렇지 않으면 DX에서 삼각형이 반대편을 본다 생각하여 그리지 않게 된다.
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices; // 정점 배열
	unsigned long* indices; // 인덱스 배열 ("indices"는 인덱스 복수형)
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 정점 배열의 길이 (삼각형이므로 3개)
	m_vertexCount = 3;

	// 인덱스 배열의 길이 (삼각형이므로 3개)
	m_indexCount = 3;

	// 정점 배열 동적 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열에 값 넣기
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // 정점의 위치
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // RGBA

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// 인덱스 배열에 값 넣기
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// 정점 버퍼의 설명(desc) 작성
	// desc에는 버퍼 크기(ByteWidth), 버퍼 타입(BindFlags)를 정확히 입력해야 함
	// 채워넣은 이후 정점 배열과 인덱스 배열을 subresource 포인터에 연결
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	
	// 정점 데이터를 가리키는 보조 리소스 구조체 작성
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼의 description 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체 작성
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 버퍼와 인덱스 버퍼를 만든 이후에는 이미 값이 복사되어
	// 필요 없어진 정점 배열과 인덱스 배열 제거 가능
	delete[] vertices;
	vertices = 0;
	
	delete[] indices;
	indices = 0;

	return true;
}

// InitializeBuffer 함수에서 만들었던 정점 버퍼와 인덱스 버퍼를 해제
void ModelClass::ShutdownBuffers()
{
	// 인덱스 버퍼 해제
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼 해제
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 정점 버퍼의 단위와 오프셋 설정
	stride = sizeof(VertexType);
	offset = 0;

	// input 어셈블러에 정점 버퍼를 활성화하여 그려질 수 있게 함
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	
	// input 어셈블러에 인덱스 버퍼를 활성화하여 그려질 수 있게 함
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정 (현재 삼각형)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}