#include "stdafx.h"
#include <windowsx.h>
#include <sstream>

SGAFramework* gpDispatch = 0;

DirectX::Keyboard::KeyboardStateTracker SGAFramework::KeyboardTracker;
DirectX::Mouse::ButtonStateTracker		SGAFramework::MouseTracker;

using namespace std;

LRESULT CALLBACK GlobalWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return gpDispatch->WindowProc(hWnd, message, wParam, lParam);
}

SGAFramework::SGAFramework() : 
	mScreenWidth(DEFAULT_SCREEN_WIDTH),
	mScreenHeight(DEFAULT_SCREEN_HEIGHT),
	mPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false)
{
	gpDispatch = this;
}

SGAFramework::~SGAFramework()
{
}

void SGAFramework::InitD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = mScreenWidth;
	scd.BufferDesc.Height = mScreenHeight;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&mspSwapchain,
		&mspDevice,
		NULL,
		&mspDeviceCon);

	SGAResourceManager::Instance().Init(mHwnd, mspDevice.Get());


	mspKeyboard = make_unique<DirectX::Keyboard>();
	mspMouse = make_unique<DirectX::Mouse>();
	mspMouse->SetWindow(hWnd);

	mspSpriteBatch = make_unique<SpriteBatch>(mspDeviceCon.Get());

	OnResize();
}

void SGAFramework::ClearD3D()
{
	mspSwapchain->SetFullscreenState(FALSE, NULL);

	SGAActorManager::Instance().Release();
	SGAResourceManager::Instance().Release();
}

void SGAFramework::Update(float dt)
{
	SGAActorManager::Instance().Update(dt);
}

void SGAFramework::BeginRender()
{
	mspDeviceCon->ClearRenderTargetView(mspTargetView.Get(), Colors::LawnGreen);
	mspDeviceCon->ClearDepthStencilView(mspDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

void SGAFramework::EndRender()
{
	mspSwapchain->Present(0, 0);
}

void SGAFramework::Render()
{
	mspSpriteBatch->Begin();

	SGAActorManager::Instance().Draw();

	mspSpriteBatch->End();
}

void SGAFramework::OnResize()
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	mspDeviceCon->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	mspTargetView.Reset();
	mspDepthStencilView.Reset();
	mspDeviceCon->Flush();

	mspSwapchain->ResizeBuffers(1, mScreenWidth, mScreenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	mspSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());

	mspDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, mspTargetView.ReleaseAndGetAddressOf());

	D3D11_TEXTURE2D_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_TEXTURE2D_DESC));
	dsd.Width = mScreenWidth;
	dsd.Height = mScreenHeight;
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = 4;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	mspDevice->CreateTexture2D(&dsd, 0, mspDepthStencilBuffer.ReleaseAndGetAddressOf());
	mspDevice->CreateDepthStencilView(mspDepthStencilBuffer.Get(), 0, mspDepthStencilView.ReleaseAndGetAddressOf());

	mspDeviceCon->OMSetRenderTargets(1, mspTargetView.GetAddressOf(), mspDepthStencilView.Get());

	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = static_cast<FLOAT>(mScreenWidth);
	mViewport.Height = static_cast<FLOAT>(mScreenHeight);
	mViewport.MinDepth = D3D11_MIN_DEPTH;
	mViewport.MaxDepth = D3D11_MAX_DEPTH;

	mspDeviceCon->RSSetViewports(1, &mViewport);
}

void SGAFramework::InitWindow(HINSTANCE hInstance, UINT style, LPCTSTR title, UINT width, UINT height)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = L"SGAWindowClass";
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = GlobalWindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, static_cast<LONG>(mScreenWidth), static_cast<LONG>(mScreenHeight) };
	AdjustWindowRect(&wr, style, FALSE);

	mTitleCaption = title;
	mHwnd = CreateWindowEx(NULL,
		L"SGAWindowClass",
		mTitleCaption.c_str(),
		style,
		100,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(mHwnd, SW_SHOW);

	InitD3D(mHwnd);
}

LRESULT SGAFramework::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				mPaused = true;
				mTimer.Stop();
			}
			else
			{
				mPaused = false;
				mTimer.Resume();
			}
		break;

		case WM_SIZE:
			mScreenWidth = LOWORD(lParam);
			mScreenHeight = HIWORD(lParam);

			if (mspDevice)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					mPaused = true;
					mMinimized = true;
					mMaximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					mPaused = false;
					mMinimized = false;
					mMaximized = true;
					OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{
					if (mMinimized)
					{
						mPaused = false;
						mMinimized = false;
						OnResize();
					}
					else if (mMaximized)
					{
						mPaused = false;
						mMaximized = false;
						OnResize();
					}
					else if (mResizing)
					{

					}
					else
						OnResize();
				}
			}

			return 0;
			break;

		case WM_ENTERSIZEMOVE:
			mPaused = true;
			mResizing = true;
			mTimer.Stop();
			return 0;
			break;

		case WM_EXITSIZEMOVE:
			mPaused = false;
			mResizing = false;
			mTimer.Resume();
			OnResize();
			return 0;
			break;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);
			break;

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 640;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 480;
			return 0;
			break;

		case WM_ACTIVATEAPP:
			DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			SGAFramework::KeyboardTracker.Reset();
			SGAFramework::MouseTracker.Reset();
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			break;

		case WM_INPUT:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
			break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int SGAFramework::GameLoop()
{
	mTimer.Start();

	MSG msg = { 0 };
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			mTimer.Update();

			if (!mPaused)
			{
				CalculateFPS();

				SGAFramework::KeyboardTracker.Update(mspKeyboard->GetState());
				SGAFramework::MouseTracker.Update(mspMouse->GetState());

				Update(mTimer.DeltaTime());
				
				BeginRender();
				Render();
				EndRender();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	ClearD3D();

	return (int)msg.wParam;
}

void SGAFramework::CalculateFPS()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mTitleCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mHwnd, outs.str().c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}