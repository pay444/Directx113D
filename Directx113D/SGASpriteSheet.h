#pragma once

namespace DirectX
{
	struct SpriteFrame
	{
		RECT		sourceRect;
		XMFLOAT2	pivot;
		XMFLOAT2	size;
	};
};

class SGASpriteSheet
{
public:
	SGASpriteSheet();
	~SGASpriteSheet();

private:
	ComPtr<ID3D11ShaderResourceView>	mspTexture;
	map<string, SpriteFrame>			mFrames;

public:
	void				Load(ID3D11ShaderResourceView *texture, const TCHAR* fileName);
	const SpriteFrame*	Find(const char* name) const;


	void XM_CALLCONV	Draw(SpriteBatch *batch, const SpriteFrame& frame,
							XMFLOAT2 const& position,
							FXMVECTOR color = Colors::White,
							float rotation = 0.0f,
							float scale = 1.0f,
							SpriteEffects effects = SpriteEffects_None,
							float layer = 0.0f) const;
	void XM_CALLCONV	Draw(SpriteBatch *batch, const SpriteFrame& frame,
							XMFLOAT2 const& position,
							FXMVECTOR color,
							float rotation,
							XMFLOAT2 const& scale,
							SpriteEffects effects = SpriteEffects_None,
							float layer = 0.0f) const;
	void XM_CALLCONV	Draw(SpriteBatch *batch, const SpriteFrame& frame,
							FXMVECTOR position,
							FXMVECTOR color = Colors::White,
							float rotation = 0.0f,
							float scale = 1.0f,
							SpriteEffects effects = SpriteEffects_None,
							float layer = 0.0f) const;
	void XM_CALLCONV	Draw(SpriteBatch *batch, const SpriteFrame& frame,
							FXMVECTOR position,
							FXMVECTOR color,
							float rotation,
							GXMVECTOR scale,
							SpriteEffects effects = SpriteEffects_None,
							float layer = 0.0f) const;
	void XM_CALLCONV	Draw(SpriteBatch *batch, const SpriteFrame& frame,
							RECT const& destRect,
							FXMVECTOR color = Colors::White,
							float rotation = 0.0f,
							SpriteEffects effects = SpriteEffects_None,
							float layer = 0.0f) const;
};

