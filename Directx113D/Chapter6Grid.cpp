#include "stdafx.h"
#include "Chapter6Grid.h"
#include "d3dUtil.h"


#include <d3dcompiler.h>
#pragma comment (lib, "D3DCompiler.lib")

Chapter6Grid::Chapter6Grid()
	: mHillVB(0), mHillIB(0), mFX(0), mTech(0),
	mfxWorldViewProj(0), mInputLayout(0), mGridIndexCount(0),
	//mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
	mTheta(1.5f*XM_PI), mPhi(0.25f*XM_PI), mRadius(5.0f)
{
	mTitleCaption = L"Grid Hill Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}


Chapter6Grid::~Chapter6Grid()
{

}

void Chapter6Grid::InitD3D(HWND hWnd)
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

void Chapter6Grid::ClearD3D()
{
	SGAFramework::ClearD3D();

	ReleaseCOM(mHillVB);
	ReleaseCOM(mHillIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}

void Chapter6Grid::OnResize()
{
	SGAFramework::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void Chapter6Grid::Update(float dt)
{
	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	float num1 = 0.2f;
	float num2 = -0.2f;
	auto state = Keyboard::Get().GetState();

	if (state.E)
	{
		z += num1;
	}
	else if (state.R)
	{
		z += num2;
	}

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void Chapter6Grid::Render()
{
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexC);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mHillVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mHillIB, DXGI_FORMAT_R32_UINT, 0);


	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
	XMMATRIX worldViewProj = world * view*proj;



	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);
	}

	//HR(mSwapChain->Present(0, 0));

	SGAFramework::Render();
}

void Chapter6Grid::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	//SetCapture(mhMainWnd);
	SetCapture(mHwnd);
}

void Chapter6Grid::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Chapter6Grid::OnMouseMove(WPARAM btnState, int x, int y)
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

float Chapter6Grid::GetHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}

void Chapter6Grid::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	//�׸��� ����
	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mGridIndexCount = grid.Indices.size();

	//�ʿ��� ���� Ư������ �����ϰ�, �� ������ ���� �Լ��� �����Ѵ�,
	//���� �׳��̿� �����ؼ� ������ ���� ������ �����Ѵ�. 
	//�̸� ���ؼ� �� ���� �غ��� ����� ���, 
	//�׸��� �� ���� ���츮 ���� ����� ���������.

	std::vector<VertexC> vertices(grid.Vertices.size());
	//size_t = unsigned int
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		//������ ���� ���
		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;

		//���̿� �����ؼ� ������ ������ �����Ѵ�.
		if (p.y < -10.0f)
		{
			//�غ��� �� ��.
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (p.y < 5.0f)
		{
			// ���� ��Ȳ��
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (p.y < 12.0f)
		{
			// £�� ��Ȳ��
			vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (p.y < 20.0f)
		{
			// £�� ����
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// ���
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexC) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mHillVB));

	//��� �޽��� ���ε��� �ϳ��� ���� ���ۿ� ���� �ִ´�

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mHillIB));
}

void Chapter6Grid::BuildFX()
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
	HRESULT hr = D3DCompileFromFile(L"FX\\color.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 0, "fx_5_0",
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

void Chapter6Grid::BuildVSPS()
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// �� �Լ����� ����ϴ� �����͵��� null�� �����մϴ�.
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	// ���� ���̴��� �������մϴ�.
	result = D3DCompileFromFile(L"FX\\color.vs", NULL, NULL, "ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����մϴ�.
		if (errorMessage)
		{
			//OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���� ���Դϴ�.
		else
		{
			MessageBox(mHwnd, L"Colors.vs", L"Missing Shader File", MB_OK);
		}

	}

	// �ȼ� ���̴��� �������մϴ�.
	result = D3DCompileFromFile(L"FX\\color.ps", NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �������� �����ϸ� ���� �޼����� ����մϴ�.
		if (errorMessage)
		{
			MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		}
		// ���� �޼����� ���ٸ� �ܼ��� ���̴� ������ ã�� ���� ���Դϴ�.
		else
		{
			MessageBox(mHwnd, L"Colors.ps", L"Missing Shader File", MB_OK);
		}

	}

	// ���۷κ��� ���� ���̴��� �����մϴ�.
	result = md3dDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreateVertexShader File error", MB_OK);

	// ���۷κ��� �ȼ� ���̴��� �����մϴ�.
	result = md3dDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreatePixelShader File error", MB_OK);


	// ���� �Է� ���̾ƿ� description�� �ۼ��մϴ�.
	// �� ������ ModelClass�� ���̴��� �ִ� VertexType�� ��ġ�ؾ� �մϴ�.
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

	// ���̾ƿ��� ��� ������ �����ɴϴ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	// ���� �Է� ���̾ƿ��� �����մϴ�.
	result = md3dDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mInputLayout);
	if (FAILED(result))
		MessageBox(mHwnd, L"Colors.ps", L"CreateInputLayout File error", MB_OK);


	// �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ���̴��� �ִ� ��� ��� ������ description�� �ۼ��մϴ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
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

void Chapter6Grid::BuildVertexLayout()
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
