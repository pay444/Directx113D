#include "stdafx.h"
#include "SGAProjectile.h"


SGAProjectile::SGAProjectile()
{
}

SGAProjectile::SGAProjectile(SpriteBatch * pBatch, SGASpriteSheet * pSheet) :
	SGAActor(pBatch, pSheet)
{
}

SGAProjectile::~SGAProjectile()
{
}

void SGAProjectile::Init(XMFLOAT2 position, XMFLOAT2 direction, float speed, float lifetime, float distance)
{
	if (lifetime == 0.0f && distance == 0.0f)
		DXTRACE_ERR_MSGBOX(L"Projectile should not set Lifetime and MaxDistance to 0 at same time!!!", 0);

	mPosition = position;
	mvDirection = direction;
	mfSpeed = speed;
	mfLifeTime = lifetime;
	mfMaxDistance = distance;

	mfElapsedTime = 0.0f;
	mfDistance = 0.0f;
}

void SGAProjectile::Update(float dt)
{
	SGAActor::Update(dt);

	float moveScalar = mfSpeed * dt;
	mPosition = mPosition + (mvDirection * moveScalar);
	
	mfElapsedTime += dt;
	mfDistance += moveScalar;

	if ((mfLifeTime != 0.0f && mfElapsedTime > mfLifeTime) ||
		(mfMaxDistance != 0.0f && mfDistance > mfMaxDistance))
	{
		SetDestroyed();
	}
}