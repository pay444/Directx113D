#pragma once
#include "SGAFramework.h"
#include <d3dx11effect.h>
#include "GeometryGenerator.h"
#include "Waves.h"
#include "MathHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "d3dx11Effect.h"

//struct Vertex
//{
//	XMFLOAT3 Pos;
//	XMFLOAT3 Normal;
//};

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

class Chapter7LitSkull :
	public SGAFramework
{
public:
	Chapter7LitSkull();
	virtual ~Chapter7LitSkull();
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
	float GetHillHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	void BuildLandGeometryBuffers();
	void BuildWavesGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();
	void BuildShapeGeometryBuffers();
	void BuildSkullGeometryBuffers();

private:
	ID3D11Buffer * mShapesVB;
	ID3D11Buffer* mShapesIB;

	ID3D11Buffer* mSkullVB;
	ID3D11Buffer* mSkullIB;

	DirectionalLight mDirLights[3];
	Material mGridMat;
	Material mBoxMat;
	Material mCylinderMat;
	Material mSphereMat;
	Material mSkullMat;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	ID3D11InputLayout* mInputLayout;

	ID3D11RasterizerState* mWireframeRS;

	// Define transformations from local spaces to world space.
	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mSkullWorld;

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

	UINT mSkullIndexCount;

	UINT mLightCount;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;
	float px;
	float py;
	float pz;

	POINT mLastMousePos;
};

