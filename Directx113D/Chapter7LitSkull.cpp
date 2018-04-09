#include "stdafx.h"
#include "Chapter7LitSkull.h"
#include "d3dUtil.h"

#include <d3dcompiler.h>
#pragma comment (lib, "D3DCompiler.lib")

Chapter7LitSkull::Chapter7LitSkull() 
	:mShapesVB(0), mShapesIB(0), mSkullVB(0), mSkullIB(0), 
	mSkullIndexCount(0), mLightCount(1),
	mEyePosW(0.0f, 0.0f, 0.0f), mTheta(1.5f*XM_PI), 
	mPhi(0.1f*XM_PI), mRadius(15.0f),
	px(0), py(0), pz(0)
{
	mTitleCaption = L"LitSkull Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));

	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mGridMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mCylinderMat.Ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	mCylinderMat.Diffuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	mCylinderMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	mSphereMat.Ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
	mSphereMat.Diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
	mSphereMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

	mBoxMat.Ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mSkullMat.Ambient = XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f);
	mSkullMat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);


}

Chapter7LitSkull::~Chapter7LitSkull()
{
}

void Chapter7LitSkull::InitD3D(HWND hWnd)
{
	SGAFramework::InitD3D(hWnd);

	this->md3dDevice = this->mspDevice.Get();
	this->md3dImmediateContext = this->mspDeviceCon.Get();
	mSwapChain = mspSwapchain.Get();
	mDepthStencilBuffer = mspDepthStencilBuffer.Get();
	mRenderTargetView = mspTargetView.Get();
	mDepthStencilView = mspDepthStencilView.Get();

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);

	//D3D11_RASTERIZER_DESC wireframeDesc;
	//ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	//wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	//wireframeDesc.CullMode = D3D11_CULL_BACK;
	//wireframeDesc.FrontCounterClockwise = false;
	//wireframeDesc.DepthClipEnable = true;

	//HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

	BuildShapeGeometryBuffers();
	BuildSkullGeometryBuffers();
	//BuildFX();
	//BuildVertexLayout();
}

void Chapter7LitSkull::ClearD3D()
{
	SGAFramework::ClearD3D();

	ReleaseCOM(mShapesVB);
	ReleaseCOM(mShapesIB);
	ReleaseCOM(mSkullVB);
	ReleaseCOM(mSkullIB);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

void Chapter7LitSkull::OnResize()
{
	SGAFramework::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void Chapter7LitSkull::Update(float dt)
{
	//구면 좌표계를 데카르트 좌표로 변환
	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	//시야행렬을 만든다.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	float num1 = 0.2f;
	float num2 = -0.2f;
	auto state = Keyboard::Get().GetState();

	if (state.E)
	{
		pz += num1;
	}
	else if (state.R)
	{
		pz += num2;
	}
	else if (state.D0)
	{
		mLightCount = 0;
	}
	else if (state.D1)
	{
		mLightCount = 1;
	}
	else if (state.D2)
	{
		mLightCount = 2;
	}
	else if (state.D3)
	{
		mLightCount = 3;
	}
}

void Chapter7LitSkull::Render()
{
	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormal);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::PosNormal);
	UINT offset = 0;

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view * proj;

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mEyePosW);

	// Figure out which technique to use.
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light1Tech;
	switch (mLightCount)
	{
	case 1:
		activeTech = Effects::BasicFX->Light1Tech;
		break;
	case 2:
		activeTech = Effects::BasicFX->Light2Tech;
		break;
	case 3:
		activeTech = Effects::BasicFX->Light3Tech;
		break;
	}

	//md3dImmediateContext->RSSetState(mWireframeRS);
	//md3dImmediateContext->RSSetState(0);

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

		// Draw the grid.
		XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(mGridMat);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(mBoxMat);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(mCylinderMat);

			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetMaterial(mSphereMat);

			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}

		// Draw the skull.

		md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

		world = XMLoadFloat4x4(&mSkullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(mSkullMat);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);

		// Restore default.
		
	}

	//HR(mSwapChain->Present(0, 0));

	//SGAFramework::Render();
}

void Chapter7LitSkull::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	//SetCapture(mhMainWnd);
	SetCapture(mHwnd);
}


void Chapter7LitSkull::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Chapter7LitSkull::OnMouseMove(WPARAM btnState, int x, int y)
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

float Chapter7LitSkull::GetHillHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}

DirectX::XMFLOAT3 Chapter7LitSkull::GetHillNormal(float x, float z) const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void Chapter7LitSkull::BuildLandGeometryBuffers()
{
	//GeometryGenerator::MeshData grid;

	//GeometryGenerator geoGen;

	//geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	//mLandIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  In addition, color the vertices based on their height so we have
	// sandy looking beaches, grassy low hills, and snow mountain peaks.
	//

	//std::vector<Vertex> vertices(grid.Vertices.size());
	//for (size_t i = 0; i < grid.Vertices.size(); ++i)
	//{
	//	XMFLOAT3 p = grid.Vertices[i].Position;

	//	p.y = GetHillHeight(p.x, p.z);

	//	vertices[i].Pos = p;
	//	vertices[i].Normal = GetHillNormal(p.x, p.z);
	//}

	//D3D11_BUFFER_DESC vbd;
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	//vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();
	//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbd.CPUAccessFlags = 0;
	//vbd.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA vinitData;
	//vinitData.pSysMem = &vertices[0];
	//HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));

	////
	//// Pack the indices of all the meshes into one index buffer.
	////

	//D3D11_BUFFER_DESC ibd;
	//ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//ibd.ByteWidth = sizeof(UINT) * mLandIndexCount;
	//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//ibd.CPUAccessFlags = 0;
	//ibd.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA iinitData;
	//iinitData.pSysMem = &grid.Indices[0];
	//HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
}

void Chapter7LitSkull::BuildWavesGeometryBuffers()
{
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.

	//D3D11_BUFFER_DESC vbd;
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	//vbd.ByteWidth = sizeof(Vertex) * mWaves.VertexCount();
	//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//vbd.MiscFlags = 0;
	//HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVB));


	//// Create the index buffer.  The index buffer is fixed, so we only 
	//// need to create and set once.

	//std::vector<UINT> indices(3 * mWaves.TriangleCount()); // 3 indices per face

	//													   // Iterate over each quad.
	//UINT m = mWaves.RowCount();
	//UINT n = mWaves.ColumnCount();
	//int k = 0;
	//for (UINT i = 0; i < m - 1; ++i)
	//{
	//	for (DWORD j = 0; j < n - 1; ++j)
	//	{
	//		indices[k] = i * n + j;
	//		indices[k + 1] = i * n + j + 1;
	//		indices[k + 2] = (i + 1)*n + j;

	//		indices[k + 3] = (i + 1)*n + j;
	//		indices[k + 4] = i * n + j + 1;
	//		indices[k + 5] = (i + 1)*n + j + 1;

	//		k += 6; // next quad
	//	}
	//}

	//D3D11_BUFFER_DESC ibd;
	//ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//ibd.ByteWidth = sizeof(UINT) * indices.size();
	//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//ibd.CPUAccessFlags = 0;
	//ibd.MiscFlags = 0;
	//D3D11_SUBRESOURCE_DATA iinitData;
	//iinitData.pSysMem = &indices[0];
	//HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWavesIB));
}

void Chapter7LitSkull::BuildFX()
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
	HRESULT hr = D3DCompileFromFile(L"FX\\Basic.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 0, "fx_5_0",
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

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	ReleaseCOM(compiledShader);

}

void Chapter7LitSkull::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));

}

void Chapter7LitSkull::BuildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosNormal> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormal) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mShapesVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mShapesIB));

}

void Chapter7LitSkull::BuildSkullGeometryBuffers()
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex::PosNormal> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	mSkullIndexCount = 3 * tcount;
	std::vector<UINT> indices(mSkullIndexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormal) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSkullVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mSkullIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSkullIB));

}
