#include "stdafx.h"
#include "SGAResourceManager.h"


SGAResourceManager::SGAResourceManager() : 
	mHWnd(0),
	mpDevice(0)
{
}

SGAResourceManager::~SGAResourceManager()
{
	
}

void SGAResourceManager::Release()
{
	for (auto& resource : mShaderResources)
	{
		resource.second.Reset();
	}

	for (auto& sheet : mSpriteSheets)
	{
		sheet.second.reset();
	}
}

ID3D11ShaderResourceView * SGAResourceManager::GetShaderResource(wstring fileName)
{
	if (mpDevice == NULL)
	{
		DXTRACE_ERR_MSGBOX(L"mpDevice is NULL.\nCall Init() first!", 0);
		return NULL;
	}

	ID3D11ShaderResourceView *pResource = NULL;
	
	auto result = mShaderResources.insert(pair<wstring, ComPtr<ID3D11ShaderResourceView>>(fileName, nullptr));

	if (result.second == true)
	{
		result.first->second = ComPtr<ID3D11ShaderResourceView>();
		CreateWICTextureFromFile(mpDevice, fileName.c_str(), NULL, result.first->second.GetAddressOf());
		pResource = result.first->second.Get();
	}
	else
		pResource = result.first->second.Get();

	return pResource;
}

SGASpriteSheet * SGAResourceManager::GetSpriteSheets(wstring fileName, ID3D11ShaderResourceView * pTexture)
{
	SGASpriteSheet *pSheet = NULL;

	auto result = mSpriteSheets.insert(pair<wstring, unique_ptr<SGASpriteSheet>>(fileName, nullptr));
	
	if (result.second == true)
	{
		result.first->second = move(make_unique<SGASpriteSheet>());
		pSheet = result.first->second.get();
		pSheet->Load(pTexture, fileName.c_str());
	}
	else
		pSheet = result.first->second.get();

	return pSheet;
}

SGASpriteSheet * SGAResourceManager::GetSpriteSheets(wstring sheetName, wstring textureName)
{
	return GetSpriteSheets(sheetName, GetShaderResource(textureName));
}

