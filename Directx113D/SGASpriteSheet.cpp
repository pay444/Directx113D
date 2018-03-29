#include "stdafx.h"
#include <fstream>
#include <string>
#include <regex>
#include "SGASpriteSheet.h"


SGASpriteSheet::SGASpriteSheet()
{
}


SGASpriteSheet::~SGASpriteSheet()
{
}

void SGASpriteSheet::Load(ID3D11ShaderResourceView * texture, const TCHAR * fileName)
{
	mFrames.clear();
	mspTexture = texture;

	ifstream file(fileName);

	if (!file) throw exception("Can not find sheets file");

	regex pattern("\"([^\"]*)\"");
	string line;

	getline(file, line);
	getline(file, line);

	while (!file.eof())
	{
		getline(file, line);
		if (file.eof())
			break;

		sregex_iterator current(line.begin(), line.end(), pattern);
		sregex_iterator end;
		int index = 0;
		string name;
		SpriteFrame frame;

		while (current != end)
		{
			string result = (*current)[1];

			switch (index)
			{
			case 0: name = result;		break;
			case 1: frame.sourceRect.left = stol(result);	break;
			case 2: frame.sourceRect.top = stol(result);	break;
			case 3:
				frame.size.x = stof(result);
				frame.sourceRect.right = frame.sourceRect.left + static_cast<long>(frame.size.x);
				break;
			case 4:
				frame.size.y = stof(result);
				frame.sourceRect.bottom = frame.sourceRect.top + static_cast<long>(frame.size.y);
				break;
			case 5: frame.pivot.x = frame.size.x * stof(result);	break;
			case 6: frame.pivot.y = frame.size.y * stof(result);	break;
			default:
				throw exception("Invalid file format");
				break;
			}

			index++;
			current++;
		}

		if (index > 6)
		{
			mFrames.insert(pair<string, SpriteFrame>(name.substr(0, name.find_last_of(".")), frame));
		}
	}
}

const SpriteFrame * SGASpriteSheet::Find(const char * name) const
{
	auto it = mFrames.find(name);
	
	if (it == mFrames.cend())
		return nullptr;

	return &it->second;
}

void XM_CALLCONV SGASpriteSheet::Draw(SpriteBatch * batch, const SpriteFrame & frame, XMFLOAT2 const & position, FXMVECTOR color, float rotation, float scale, SpriteEffects effects, float layer) const
{
	assert(batch != 0);

	XMFLOAT2 pivot = frame.pivot;
	switch (effects)
	{
	case SpriteEffects_FlipHorizontally:
		pivot.x = frame.sourceRect.right - frame.sourceRect.left - pivot.x;
		break;
	case SpriteEffects_FlipVertically:
		pivot.y = frame.sourceRect.bottom - frame.sourceRect.top - pivot.y;
		break;
	}

	batch->Draw(mspTexture.Get(), position, &frame.sourceRect, color, rotation, pivot, scale, effects, layer);
}

void XM_CALLCONV SGASpriteSheet::Draw(SpriteBatch * batch, const SpriteFrame & frame, XMFLOAT2 const & position, FXMVECTOR color, float rotation, XMFLOAT2 const & scale, SpriteEffects effects, float layer) const
{
	assert(batch != 0);

	XMFLOAT2 pivot = frame.pivot;
	switch (effects)
	{
	case SpriteEffects_FlipHorizontally:
		pivot.x = frame.sourceRect.right - frame.sourceRect.left - pivot.x;
		break;
	case SpriteEffects_FlipVertically:
		pivot.y = frame.sourceRect.bottom - frame.sourceRect.top - pivot.y;
		break;
	}

	batch->Draw(mspTexture.Get(), position, &frame.sourceRect, color, rotation, pivot, scale, effects, layer);
}

void XM_CALLCONV SGASpriteSheet::Draw(SpriteBatch * batch, const SpriteFrame & frame, FXMVECTOR position, FXMVECTOR color, float rotation, float scale, SpriteEffects effects, float layer) const
{
	assert(batch != 0);

	XMFLOAT2 pivot = frame.pivot;
	switch (effects)
	{
	case SpriteEffects_FlipHorizontally:
		pivot.x = frame.sourceRect.right - frame.sourceRect.left - pivot.x;
		break;
	case SpriteEffects_FlipVertically:
		pivot.y = frame.sourceRect.bottom - frame.sourceRect.top - pivot.y;
		break;
	}

	batch->Draw(mspTexture.Get(), position, &frame.sourceRect, color, rotation, XMLoadFloat2(&pivot), scale, effects, layer);
}

void XM_CALLCONV SGASpriteSheet::Draw(SpriteBatch * batch, const SpriteFrame & frame, FXMVECTOR position, FXMVECTOR color, float rotation, GXMVECTOR scale, SpriteEffects effects, float layer) const
{
	assert(batch != 0);

	XMFLOAT2 pivot = frame.pivot;
	switch (effects)
	{
	case SpriteEffects_FlipHorizontally:
		pivot.x = frame.sourceRect.right - frame.sourceRect.left - pivot.x;
		break;
	case SpriteEffects_FlipVertically:
		pivot.y = frame.sourceRect.bottom - frame.sourceRect.top - pivot.y;
		break;
	}

	batch->Draw(mspTexture.Get(), position, &frame.sourceRect, color, rotation, XMLoadFloat2(&pivot), scale, effects, layer);
}

void XM_CALLCONV SGASpriteSheet::Draw(SpriteBatch * batch, const SpriteFrame & frame, RECT const & destRect, FXMVECTOR color, float rotation, SpriteEffects effects, float layer) const
{
	assert(batch != 0);

	XMFLOAT2 pivot = frame.pivot;
	switch (effects)
	{
	case SpriteEffects_FlipHorizontally:
		pivot.x = frame.sourceRect.right - frame.sourceRect.left - pivot.x;
		break;
	case SpriteEffects_FlipVertically:
		pivot.y = frame.sourceRect.bottom - frame.sourceRect.top - pivot.y;
		break;
	}

	batch->Draw(mspTexture.Get(), destRect, &frame.sourceRect, color, rotation, pivot, effects, layer);
}
