/////////////////////////////////////////////
// Filename: ColorShaderClass.h
/////////////////////////////////////////////
#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

///////////////////////////////////////
// Class name: ColorShaderClass
///////////////////////////////////////

// GPU 상에 존재하는 3D 모델들을 그리는 데 사용하는
// HLSL 셰이더를 호출하는 클래스
class ColorShaderClass
{
private:
	// 정점 셰이더에 사용될 cBuffer 구조체의 정의
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

	// 셰이더의 초기화 및 마무리 제어
	// Render 함수는 셰이더에 사용되는 변수를 설정하고
	// 셰이더를 이용해 준비된 모델들의 정점을 그려낸다
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};

#endif