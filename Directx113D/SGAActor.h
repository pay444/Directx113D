#pragma once

namespace DirectX
{
	struct FrameInfo
	{
		string	FrameName;
		float	DelayPerFrame;
	};

	struct Animation
	{
		string				Name;
		int					TotalFrames;
		vector<FrameInfo>	FrameInfos;
	};
}

class SGAActor
{
public:
	SGAActor();
	SGAActor(SpriteBatch * pBatch, SGASpriteSheet * pSheet);
	virtual ~SGAActor();

public:
	virtual void Init(SpriteBatch* pBatch, SGASpriteSheet *pSheet, Animation* anims, int animCount);
	virtual void Init(Animation* anims, int animCount);
	virtual void Update(float dt);
	virtual void Draw();
	virtual void SetAnimation(string name);
	virtual bool IntersectRect(SGAActor* pActor);
	virtual void OnHit(SGAActor* pCollider) {}

protected:
	map<string, vector<FrameInfo>>	mAnimations;
	SGASpriteSheet*	mpSheet;
	SpriteBatch*	mpBatch;

protected:
	string						mCurAnim;
	vector<FrameInfo>*			mFrames;
	vector<FrameInfo>::iterator mCurFrame;
	const SpriteFrame*			mpSpriteFrame;
	float						mfElapsedTime;

protected:
	XMFLOAT2					mPosition;
	bool						mbDestroyed;
	bool						mbEnableCollision;
	int							mCamp;
	
public:
	void SetPosition(XMFLOAT2 pos)		{ mPosition = pos; }
	void SetPosition(int x, int y)		{ mPosition.x = static_cast<float>(x); mPosition.y = static_cast<float>(y); }
	XMFLOAT2 GetPosition()				{ return mPosition; }
	void SetDestroyed()					{ mbDestroyed = true; }
	bool GetDestroyed()					{ return mbDestroyed; }
	void SetEnableCollision(bool flag)	{ mbEnableCollision = flag; }
	bool GetEnableCollision()			{ return mbEnableCollision; }
	RECT inline GetBound()
	{
		RECT rct;
		ZeroMemory(&rct, sizeof(RECT));
		if (mpSpriteFrame != NULL)
		{
			rct = mpSpriteFrame->sourceRect;
			OffsetRect(&rct, static_cast<int>(mPosition.x - mpSpriteFrame->pivot.x) - rct.left, static_cast<int>(mPosition.y - mpSpriteFrame->pivot.y) - rct.top);
		}
		return rct;
	}
	virtual int GetCamp()				{ return mCamp; }
	virtual void SetCamp(int camp)		{ mCamp = camp; }

};

