#pragma once

#include "SGATimer.h"

#pragma comment (lib, "d3d11.lib")

#define DEFAULT_SCREEN_WIDTH	800
#define DEFAULT_SCREEN_HEIGHT	600

class SGAFramework
{
public:
	SGAFramework();
	virtual ~SGAFramework();

protected:
	Microsoft::WRL::ComPtr<IDXGISwapChain>			mspSwapchain;
	Microsoft::WRL::ComPtr<ID3D11Device>			mspDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		mspDeviceCon;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	mspTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mspDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	mspDepthStencilView;
	D3D11_VIEWPORT									mViewport;
	std::unique_ptr<DirectX::Keyboard>				mspKeyboard;
	std::unique_ptr<DirectX::Mouse>					mspMouse;

	unique_ptr<SpriteBatch>							mspSpriteBatch;

	UINT		mScreenWidth;
	UINT		mScreenHeight;
	bool		mMinimized;
	bool		mMaximized;
	bool		mResizing;

	bool		mPaused;
	HWND		mHwnd;
	wstring		mTitleCaption;
	SGATimer	mTimer;

protected:
	void CalculateFPS();

public:
	static DirectX::Keyboard::KeyboardStateTracker	KeyboardTracker;
	static DirectX::Mouse::ButtonStateTracker		MouseTracker;

public:
	virtual void InitD3D(HWND hWnd);
	virtual void ClearD3D();
	virtual void Update(float dt);
	virtual void BeginRender();
	virtual void EndRender();
	virtual void Render();
	virtual void OnResize();
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }
	
public:
	void						InitWindow(HINSTANCE hInstance, UINT style = WS_OVERLAPPEDWINDOW, LPCTSTR title = L"SBSGameAcademySample", UINT width = 1280, UINT height = 720);
	virtual LRESULT CALLBACK	WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int							GameLoop();
	float						AspectRatio() const { return static_cast<float>(mScreenWidth) / mScreenHeight; }
};

