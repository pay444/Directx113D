#include "stdafx.h"
#include "SGAActor.h"


SGAActor::SGAActor() : 
	mpSheet(NULL),
	mFrames(NULL),
	mpSpriteFrame(NULL),
	mfElapsedTime(0.0f),
	mPosition(0.0f, 0.0f),
	mbDestroyed(false),
	mbEnableCollision(true),
	mCamp(0)
{
	mAnimations.clear();
}

SGAActor::SGAActor(SpriteBatch * pBatch, SGASpriteSheet * pSheet) : SGAActor()
{
	mpSheet = pSheet;
	mpBatch = pBatch;
}


SGAActor::~SGAActor()
{
}

void SGAActor::Init(SpriteBatch* pBatch, SGASpriteSheet *pSheet, Animation *anims, int animCount)
{
	mpSheet = pSheet;
	mpBatch = pBatch;

	Init(anims, animCount);
}

void SGAActor::Init(Animation * anims, int animCount)
{
	for (int i = 0; i < animCount; ++i)
	{
		vector<FrameInfo> frames;
		for (int j = 0; j < anims[i].TotalFrames; ++j)
		{
			FrameInfo frame;
			frame.DelayPerFrame = anims[i].FrameInfos[j].DelayPerFrame;
			frame.FrameName = anims[i].FrameInfos[j].FrameName;
			frames.push_back(frame);
		}

		mAnimations.insert(pair<string, vector<FrameInfo>>(anims[i].Name, frames));
	}
}

void SGAActor::Update(float dt)
{
	if (mpSheet == NULL)
		return;

	mfElapsedTime += dt;

	if (mFrames == NULL)
		return;

	if (mCurFrame->DelayPerFrame != 0.0f && mfElapsedTime > mCurFrame->DelayPerFrame)
	{
		mCurFrame++;
		mfElapsedTime = 0.0f;
		if (mCurFrame == mFrames->cend())
			mCurFrame = mFrames->begin();

		mpSpriteFrame = mpSheet->Find(mCurFrame->FrameName.c_str());
	}
}

void SGAActor::Draw()
{
	if (mpBatch == NULL || mpSheet == NULL)
		return;

	if (mpSpriteFrame == NULL)
		return;

	mpSheet->Draw(mpBatch, *mpSpriteFrame, mPosition);
}

void SGAActor::SetAnimation(string name)
{
	if (mCurAnim == name)
		return;

	mCurAnim = name;

	auto result = mAnimations.find(name);

	if (result == mAnimations.cend())
	{
		OutputDebugString(L"Can not find animation!");
		return;
	}

	mFrames = &result->second;
	mCurFrame = result->second.begin();
	mfElapsedTime = 0.0f;
	mpSpriteFrame = mpSheet->Find(mCurFrame->FrameName.c_str());
}

bool SGAActor::IntersectRect(SGAActor * pActor)
{
	RECT src = GetBound();
	RECT trg = pActor->GetBound();
	RECT intersect;

	return ::IntersectRect(&intersect, &src, &trg);
}
