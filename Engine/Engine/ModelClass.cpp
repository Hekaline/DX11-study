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

	// ���� ���ۿ� �ε��� ���� �ʱ�ȭ
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// ���� ���ۿ� �ε��� ���� ����
	ShutdownBuffers();

	return;
}

// GraphicsClass::Render()���� ȣ��
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// ���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־�
	// ȭ�鿡 �׸� �غ��ϱ�
	RenderBuffers(deviceContext);
	return;
}

// �ε��� ���� Getter
// �÷� ���̴����� ���� �׸����� �ε��� ���� �ʿ�
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


// ���� ���ۿ� �ε��� ���۸� �����ϴ� �۾� ����
// ������ ���Ϸκ��� ���� ������ �о�� ���۸� ����� ���� �Ѵ�.
// ���� �ﰢ���� �׸��µ�, ������ ���� �ﰢ���� ������ �ð� �������� ������ �Ѵٴ� ���̴�.
// �׷��� ������ DX���� �ﰢ���� �ݴ����� ���� �����Ͽ� �׸��� �ʰ� �ȴ�.
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices; // ���� �迭
	unsigned long* indices; // �ε��� �迭 ("indices"�� �ε��� ������)
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ���� �迭�� ���� (�ﰢ���̹Ƿ� 3��)
	m_vertexCount = 3;

	// �ε��� �迭�� ���� (�ﰢ���̹Ƿ� 3��)
	m_indexCount = 3;

	// ���� �迭 ���� ����
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

	// ���� �迭�� �� �ֱ�
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // ������ ��ġ
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // RGBA

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// �ε��� �迭�� �� �ֱ�
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// ���� ������ ����(desc) �ۼ�
	// desc���� ���� ũ��(ByteWidth), ���� Ÿ��(BindFlags)�� ��Ȯ�� �Է��ؾ� ��
	// ä������ ���� ���� �迭�� �ε��� �迭�� subresource �����Ϳ� ����
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	
	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ε��� ������ description �ۼ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ���ۿ� �ε��� ���۸� ���� ���Ŀ��� �̹� ���� ����Ǿ�
	// �ʿ� ������ ���� �迭�� �ε��� �迭 ���� ����
	delete[] vertices;
	vertices = 0;
	
	delete[] indices;
	indices = 0;

	return true;
}

// InitializeBuffer �Լ����� ������� ���� ���ۿ� �ε��� ���۸� ����
void ModelClass::ShutdownBuffers()
{
	// �ε��� ���� ����
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���� ����
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

	// ���� ������ ������ ������ ����
	stride = sizeof(VertexType);
	offset = 0;

	// input ��������� ���� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� ��
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	
	// input ��������� �ε��� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� ��
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ���� (���� �ﰢ��)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}