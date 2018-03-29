#pragma once
#include "SGAActor.h"
class SGAProjectile :
	public SGAActor
{
public:
	SGAProjectile();
	SGAProjectile(SpriteBatch * pBatch, SGASpriteSheet * pSheet);
	virtual ~SGAProjectile();

public:
	virtual void Init(XMFLOAT2 position, XMFLOAT2 direction, float speed, float lifetime, float distance);
	virtual void Update(float dt);

protected:
	XMFLOAT2	mvDirection;
	float		mfSpeed;
	float		mfLifeTime;
	float		mfMaxDistance;

protected:
	float		mfElapsedTime;
	float		mfDistance;
};

