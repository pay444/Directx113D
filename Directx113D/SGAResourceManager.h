#pragma once

class SGAResourceManager final
{
public:
	static SGAResourceManager& Instance()
	{
		static SGAResourceManager instance;
		return instance;
	}

private:
	SGAResourceManager();
	SGAResourceManager(SGAResourceManager const&);
	void operator =(SGAResourceManager const&);

public:
	~SGAResourceManager();

private:
	ID3D11Device*	mpDevice;
	HWND			mHWnd;

	map<wstring, ComPtr<ID3D11ShaderResourceView>>	mShaderResources;
	map<wstring, unique_ptr<SGASpriteSheet>>		mSpriteSheets;

public:
	void Init(HWND hwnd, ID3D11Device* pDevice) { mHWnd = hwnd; mpDevice = pDevice; }
	void Release();

public:
	ID3D11ShaderResourceView*	GetShaderResource(wstring fileName);
	SGASpriteSheet*				GetSpriteSheets(wstring fileName, ID3D11ShaderResourceView* pTexture);
	SGASpriteSheet*				GetSpriteSheets(wstring sheetName, wstring textureName);
};

