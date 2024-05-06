#include "stdafx.h"
#include "ColorShaderClass.h"

// private 포인터 초기화
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

	// 꼭짓점 셰이더와 픽셀 셰이더 파일 이름 설정
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

	// 꼭짓점 셰이더와 픽셀 셰이더 초기화
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	// 버텍스와 픽셀 셰이더 및 관련 객체 종료
	ShutdownShader();
	
	return;
}

// Render는 먼저 SetShaderParameters 함수를 사용하여 셰이더 내부의 매개변수를 설정
// 매개변수가 설정되면 RenderShader를 호출하여 HLSL 셰이더를 사용하여 녹색 삼각형을 그림
bool ColorShaderClass::Render
(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	// 렌더링에 사용할 셰이더 파라미터 설정
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// 셰이더와 함께 준비된 버퍼를 렌더링
	RenderShader(deviceContext, indexCount);

	return true;
}

// 셰이더 파일을 로드하고 DX와 GPU에서 사용할 수 있도록 만드는 함수
// 레이아웃 설정과 꼭짓점 버퍼 데이터가 GPU의 그래픽 파이프라인에서 어떻게 보일 지도 볼 수 있음
// 레이아웃은 color.vs 파일에 정의된 것뿐만 아니라 ModelClass.h 파일의 VertexType과 일치해야 한다
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

	// 이 함수가 사용할 포인터를 널로 설정
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 이제 여기서 셰이더 프로그램을 버퍼로 컴파일
	// 셰이더 파일 이름, 셰이더 이름, 셰이더 버전, 셰이더가 컴파일될 버퍼가 인자이다
	
	// 꼭짓점 셰이더 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", 
		"vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일 실패
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메시지가 아무것도 없었다면 단순히 셰이더 파일을 찾을 수 없었던 것
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더 코드 컴파일
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// 셰이더 컴파일에 실패하면 에러 메시지에 무언가가 쓰여 있을 것
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 에러 메시지에 아무것도 없었다면 단순히 파일을 못 찾은 것
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 꼭짓점 셰이더와 픽셀 셰이더가 버퍼로 성공적으로 컴파일되면, 그 버퍼를 사용하여 셰이더 객체 직접 만들기
	// 여기서 나온 포인터를 꼭짓점 및 픽셀 셰이더의 인터페이스로 사용한다.

	// 버퍼에서 꼭짓점 셰이더 만들기
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼에서 픽셀 셰이더 만들기
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 셰이더에서 사용할 꼭짓점 데이터의 레이아웃 생성
	// 위치 벡터와 색상 벡터를 사용하므로 각각의 벡터의 크기를 포함하는 두 레이아웃을 만든다
	// SemanticName은 이 요소가 레이아웃에서 어떻게 사용되는지 알려주므로 먼저 작성해야 할 항목이다.
	// position을 먼저, color를 두 번째로 처리한다.
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

	// 이 레이아웃에서는 처음 12바이트를 위치 벡터에서 사용하고 다음 16바이트를 색상으로 사용해야 한다는 것을 알려줘야 함
	// 직접 값을 입력하기보다 매크로를 지정하여 자동으로 알아내도록 함
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; 
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 Desc가 채워지면 이것의 크기를 가지고 D3D 디바이스를 사용하여 입력 레이아웃 생성
	// 레이아웃이 생성되면 꼭짓점/픽셀 셰이더 버퍼들은 더 이상 사용되지 않으므로 할당 해제
	// 레이아웃 요소 개수 가져오기
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	// 꼭짓점 입력 레이아웃 생성
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
									   vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 사용되지 않는 꼭짓점 셰이더 버퍼와 픽셀 셰이더 버퍼를 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 셰이더를 사용하기 위한 마지막 단계는 상수 버퍼이다.
	// 꼭짓점 셰이더에서처럼 현재 단 하나의 상수 버퍼만 가지고 있다.
	
	// 꼭짓점 셰이더에 있는 행렬 상수 버퍼의 description 작성
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 상수 버퍼를 이 버퍼로 한다.
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // cpu에 쓰기 액세스를 할 수 있다.
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 꼭짓점 셰이더 상수 버퍼에 접근할 수 있게 함
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// 상수 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 꼭짓점 셰이더 해제
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

// 꼭짓점이나 픽셀 셰이더 컴파일 도중 오류 메시지 출력
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// 에러 메시지를 담고 있는 문자열 버퍼의 포인터 가져오기
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지의 길이 가져오기
	bufferSize = errorMessage->GetBufferSize();

	// 파일을 열고 안에 메시지 기록
	fout.open("shader-error.txt");

	// 에러 메시지 작성
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 파일 닫기
	fout.close();

	// 에러 메시지 해제
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업으로 알림
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFileName, MB_OK);

	return;
}

// 셰이더의 변수를 쉽게 다루기 위해 만든 함수
// 이 함수에 사용된 행렬들은 GraphicsClass에서 만들어짐
bool ColorShaderClass::SetShaderParameters
(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, 
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 함
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// m_matrixBuffer를 잠그고 행렬들을 설정한 후 잠금 해제
	// 상수 버퍼를 잠가야 작성될 수 있다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼의 데이터를 포인터로 받아온다
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼로 행렬들 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼 잠금 해제
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 이제 HLSL 꼭짓점 셰이더 안에 있는 갱신된 행렬을 설정한다.
	// 꼭짓점 셰이더 내의 상수 버퍼의 위치 설정
	bufferNumber = 0;

	// 꼭짓점 셰이더 내의 상수 버퍼를 갱신된 값으로 설정
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

// 이 함수는 Render 함수에서 두 번째로 호출된다. 
// 이것이 호출되기 전 SetShaderParameter가 호출되어야 셰이더 변수들이 제대로 설정된다.
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 꼭짓점 입력 레이아웃 설정
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 렌더링하는 데 쓰일 꼭짓점과 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 렌더한다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

