#pragma once
#include "SGAFramework.h"
#include <d3dx11effect.h>
#include "GeometryGenerator.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};
class Chapter6Shapes :
	public SGAFramework
{
public:
	Chapter6Shapes();
	virtual ~Chapter6Shapes();

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
	ID3D11Buffer * mVB;
	ID3D11Buffer* mIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	ID3D11RasterizerState* mWireframeRS;

	//국소 공간에서 세계 공간으로의 변환 행렬을 정의한다.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mCenterSphere;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	float mTheta;
	float mPhi;
	float mRadius;
	float px;
	float py;
	float pz;
	POINT mLastMousePos;
};

