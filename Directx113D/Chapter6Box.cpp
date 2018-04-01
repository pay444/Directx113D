#include "stdafx.h"
#include "Chapter6Box.h"
#include "d3dUtil.h"


#include <d3dcompiler.h>
#pragma comment (lib, "D3DCompiler.lib")

Chapter6Box::Chapter6Box()
	: mBoxVB(0), mBoxIB(0), mFX(0), mTech(0),
	mfxWorldViewProj(0), mInputLayout(0),
	//mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
	mTheta(1.5f*XM_PI), mPhi(0.25f*XM_PI), mRadius(5.0f)
{
	mTitleCaption = L"Box Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}


Chapter6Box::~Chapter6Box()
{
}

void Chapter6Box::InitD3D(HWND hWnd)
{
	SGAFramework::InitD3D(hWnd);

	this->md3dDevice = this->mspDevice.Get();
	this->md3dImmediateContext = this->mspDeviceCon.Get();
	mSwapChain = mspSwapchain.Get();
	mDepthStencilBuffer = mspDepthStencilBuffer.Get();
	mRenderTargetView = mspTargetView.Get();
	mDepthStencilView = mspDepthStencilView.Get();

	BuildGeometryBuffers();
	BuildFX();
	//BuildVSPS();
	BuildVertexLayout();
}

void Chapter6Box::ClearD3D()
{
	SGAFramework::ClearD3D();

	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

void Chapter6Box::OnResize()
{
	SGAFramework::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void Chapter6Box::Update(float dt)
{
	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void Chapter6Box::Render()
{
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	//HR(mSwapChain->Present(0, 0));

	SGAFramework::Render();
}

void Chapter6Box::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	//SetCapture(mhMainWnd);
	SetCapture(mHwnd);
}

void Chapter6Box::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Chapter6Box::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mTheta += dx;
		mPhi += dy;

		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		mRadius += dx - dy;

		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Chapter6Box::BuildGeometryBuffers()
{
	/*Vertex vertices[] =
	{
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White },
	{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black },
	{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red },
	{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green },
	{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue },
	{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow },
	{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan },
	{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};*/

	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4((const float*)&DirectX::Colors::White) },
	{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4((const float*)&DirectX::Colors::Black) },
	{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4((const float*)&DirectX::Colors::Red) },
	{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4((const float*)&DirectX::Colors::Green) },
	{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4((const float*)&DirectX::Colors::Blue) },
	{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4((const float*)&DirectX::Colors::Yellow) },
	{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4((const float*)&DirectX::Colors::Cyan) },
	{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4((const float*)&DirectX::Colors::Magenta) }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}

void Chapter6Box::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	/*HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0",
	shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);*/
	HRESULT hr = D3DCompileFromFile(L"ComputeShader1.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 0, "fx_5_0",
		shaderFlags, 0, &compiledShader, &compilationMsgs);

	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	if (FAILED(hr))
	{
		//DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void Chapter6Box::BuildVSPS()
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// 이 함수에서 사용하는 포인터들을 null로 설정합니다.
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	// 정점 셰이더를 컴파일합니다.
	result = D3DCompileFromFile(L"FX\\color.vs", NULL, NULL, "ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더가 컴파일에 실패하면 에러 메세지를 기록합니다.
		if (errorMessage)
		{
			//OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		}
		// 에러 메세지가 없다면 셰이더 파일을 찾지 못한 것입니다.
		else
		{
			MessageBox(mHwnd, L"Colors.vs", L"Missing Shader File", MB_OK);
		}

	}

	// 픽셀 셰이더를 컴파일합니다.
	result = D3DCompileFromFile(L"FX\\color.ps", NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일이 실패하면 에러 메세지를 기록합니다.
		if (errorMessage)
		{
			MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		}
		// 에러 메세지가 없다면 단순히 셰이더 파일을 찾지 못한 것입니다.
		else
		{
			MessageBox(mHwnd, L"Colors.ps", L"Missing Shader File", MB_OK);
		}

	}

	// 버퍼로부터 정점 셰이더를 생성합니다.
	result = md3dDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreateVertexShader File error", MB_OK);

	// 버퍼로부터 픽셀 셰이더를 생성합니다.
	result = md3dDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreatePixelShader File error", MB_OK);


	// 정점 입력 레이아웃 description을 작성합니다.
	// 이 설정은 ModelClass와 셰이더에 있는 VertexType와 일치해야 합니다.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 갯수를 가져옵니다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	// 정점 입력 레이아웃을 생성합니다.
	result = md3dDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mInputLayout);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreateInputLayout File error", MB_OK);


	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 셰이더에 있는 행렬 상수 버퍼의 description을 작성합니다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	result = md3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &_MatrixBuffer);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreateBuffer File error", MB_OK);


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = md3dDevice->CreateSamplerState(&samplerDesc, &_SampleState);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreateSamplerState File error", MB_OK);
}

void Chapter6Box::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}
