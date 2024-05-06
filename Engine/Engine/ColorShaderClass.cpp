#include "stdafx.h"
#include "ColorShaderClass.h"

// private ������ �ʱ�ȭ
ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// ������ ���̴��� �ȼ� ���̴� ���� �̸� ����
	error = wcscpy_s(vsFilename, 128, L"../Engine/Color.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../Engine/Color.ps");
	if (error != 0)
	{
		return false;
	}

	// ������ ���̴��� �ȼ� ���̴� �ʱ�ȭ
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	// ���ؽ��� �ȼ� ���̴� �� ���� ��ü ����
	ShutdownShader();
	
	return;
}

// Render�� ���� SetShaderParameters �Լ��� ����Ͽ� ���̴� ������ �Ű������� ����
// �Ű������� �����Ǹ� RenderShader�� ȣ���Ͽ� HLSL ���̴��� ����Ͽ� ��� �ﰢ���� �׸�
bool ColorShaderClass::Render
(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	// �������� ����� ���̴� �Ķ���� ����
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// ���̴��� �Բ� �غ�� ���۸� ������
	RenderShader(deviceContext, indexCount);

	return true;
}

// ���̴� ������ �ε��ϰ� DX�� GPU���� ����� �� �ֵ��� ����� �Լ�
// ���̾ƿ� ������ ������ ���� �����Ͱ� GPU�� �׷��� ���������ο��� ��� ���� ���� �� �� ����
// ���̾ƿ��� color.vs ���Ͽ� ���ǵ� �ͻӸ� �ƴ϶� ModelClass.h ������ VertexType�� ��ġ�ؾ� �Ѵ�
bool ColorShaderClass::InitializeShader
(ID3D11Device* device, HWND hwnd, 
    WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// �� �Լ��� ����� �����͸� �η� ����
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���⼭ ���̴� ���α׷��� ���۷� ������
	// ���̴� ���� �̸�, ���̴� �̸�, ���̴� ����, ���̴��� �����ϵ� ���۰� �����̴�
	
	// ������ ���̴� ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", 
		"vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� ������ ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޽����� �ƹ��͵� �����ٸ� �ܼ��� ���̴� ������ ã�� �� ������ ��
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ� ������
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// ���̴� �����Ͽ� �����ϸ� ���� �޽����� ���𰡰� ���� ���� ��
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޽����� �ƹ��͵� �����ٸ� �ܼ��� ������ �� ã�� ��
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ������ ���̴��� �ȼ� ���̴��� ���۷� ���������� �����ϵǸ�, �� ���۸� ����Ͽ� ���̴� ��ü ���� �����
	// ���⼭ ���� �����͸� ������ �� �ȼ� ���̴��� �������̽��� ����Ѵ�.

	// ���ۿ��� ������ ���̴� �����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴� �����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���̴����� ����� ������ �������� ���̾ƿ� ����
	// ��ġ ���Ϳ� ���� ���͸� ����ϹǷ� ������ ������ ũ�⸦ �����ϴ� �� ���̾ƿ��� �����
	// SemanticName�� �� ��Ұ� ���̾ƿ����� ��� ���Ǵ��� �˷��ֹǷ� ���� �ۼ��ؾ� �� �׸��̴�.
	// position�� ����, color�� �� ��°�� ó���Ѵ�.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;

	// �� ���̾ƿ������� ó�� 12����Ʈ�� ��ġ ���Ϳ��� ����ϰ� ���� 16����Ʈ�� �������� ����ؾ� �Ѵٴ� ���� �˷���� ��
	// ���� ���� �Է��ϱ⺸�� ��ũ�θ� �����Ͽ� �ڵ����� �˾Ƴ����� ��
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; 
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ��� Desc�� ä������ �̰��� ũ�⸦ ������ D3D ����̽��� ����Ͽ� �Է� ���̾ƿ� ����
	// ���̾ƿ��� �����Ǹ� ������/�ȼ� ���̴� ���۵��� �� �̻� ������ �����Ƿ� �Ҵ� ����
	// ���̾ƿ� ��� ���� ��������
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	// ������ �Է� ���̾ƿ� ����
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
									   vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ������ ���̴� ���ۿ� �ȼ� ���̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���̴��� ����ϱ� ���� ������ �ܰ�� ��� �����̴�.
	// ������ ���̴�����ó�� ���� �� �ϳ��� ��� ���۸� ������ �ִ�.
	
	// ������ ���̴��� �ִ� ��� ��� ������ description �ۼ�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // ��� ���۸� �� ���۷� �Ѵ�.
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // cpu�� ���� �׼����� �� �� �ִ�.
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ������ ���̴� ��� ���ۿ� ������ �� �ְ� ��
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// ��� ���� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// ���̾ƿ� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ������ ���̴� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

// �������̳� �ȼ� ���̴� ������ ���� ���� �޽��� ���
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// ���� �޽����� ��� �ִ� ���ڿ� ������ ������ ��������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽����� ���� ��������
	bufferSize = errorMessage->GetBufferSize();

	// ������ ���� �ȿ� �޽��� ���
	fout.open("shader-error.txt");

	// ���� �޽��� �ۼ�
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// ���� �ݱ�
	fout.close();

	// ���� �޽��� ����
	errorMessage->Release();
	errorMessage = 0;

	// ������ ������ ������ �˾����� �˸�
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFileName, MB_OK);

	return;
}

// ���̴��� ������ ���� �ٷ�� ���� ���� �Լ�
// �� �Լ��� ���� ��ĵ��� GraphicsClass���� �������
bool ColorShaderClass::SetShaderParameters
(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, 
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� ��
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// m_matrixBuffer�� ��װ� ��ĵ��� ������ �� ��� ����
	// ��� ���۸� �ᰡ�� �ۼ��� �� �ִ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ��� ������ �����͸� �����ͷ� �޾ƿ´�
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���۷� ��ĵ� ����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ���� ��� ����
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� HLSL ������ ���̴� �ȿ� �ִ� ���ŵ� ����� �����Ѵ�.
	// ������ ���̴� ���� ��� ������ ��ġ ����
	bufferNumber = 0;

	// ������ ���̴� ���� ��� ���۸� ���ŵ� ������ ����
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

// �� �Լ��� Render �Լ����� �� ��°�� ȣ��ȴ�. 
// �̰��� ȣ��Ǳ� �� SetShaderParameter�� ȣ��Ǿ�� ���̴� �������� ����� �����ȴ�.
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ������ �Է� ���̾ƿ� ����
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �������ϴ� �� ���� �������� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �����Ѵ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

