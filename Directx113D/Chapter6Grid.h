#pragma once
#include "SGAFramework.h"
#include <d3dx11effect.h>
#include "GeometryGenerator.h"

struct VertexC
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	//XMFLOAT3 NormalL;
};

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};
class Chapter6Grid :
	public SGAFramework
{
public:
	Chapter6Grid();
	virtual ~Chapter6Grid();

private:
	ID3D11Device * md3dDevice;
	ID3D11DeviceContext*	md3dImmediateContext;
	IDXGISwapChain*			mSwapChain;
	ID3D11Texture2D*		mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;

	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11Buffer* _MatrixBuffer;
	ID3D11SamplerState* _SampleState;
public:
	void InitD3D(HWND hWnd);
	void ClearD3D();
	void OnResize();
	void Update(float dt);
	void Render();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHeight(float x, float z)const;
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVSPS();
	void BuildVertexLayout();

private:
	ID3D11Buffer * mHillVB;
	ID3D11Buffer* mHillIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mGridWorld;

	UINT mGridIndexCount;

	//XMFLOAT4X4 mWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

