#include "stdafx.h"
#include "SGAActorManager.h"


SGAActorManager::SGAActorManager()
{
}


SGAActorManager::~SGAActorManager()
{
}

void SGAActorManager::Update(float dt)
{
	for (const auto& actor : mActors)
	{
		actor->Update(dt);
	}

	CheckCollision();

	auto iter = mActors.begin();
	while (iter != mActors.cend())
	{
		auto pActor = iter->get();

		if (pActor->GetDestroyed())
		{
			iter->reset();
			iter = mActors.erase(iter);
		} else
			++iter;
	}
}

void SGAActorManager::CheckCollision()
{
	auto iter1 = mActors.begin();
	SGAActor *pCollider, *pCollidee;

	while (iter1 != mActors.cend())
	{
		pCollider = iter1->get();

		if (pCollider->GetEnableCollision() == false)
			continue;

		auto iter2 = iter1;
		std::advance(iter2, 1);

		while (iter2 != mActors.cend())
		{
			pCollidee = iter2->get();
			if (pCollider->IntersectRect(pCollidee) )
				pCollider->OnHit(pCollidee);

			iter2++;
		}

		iter1++;
	}
}

void SGAActorManager::Draw()
{
	for (const auto& actor : mActors)
	{
		actor->Draw();
	}
}

void SGAActorManager::Release()
{
	for (auto& actor : mActors)
	{
		actor.reset();
	}
}