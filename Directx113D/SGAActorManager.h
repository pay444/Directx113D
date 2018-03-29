#pragma once
#include <list>

class SGAActorManager final
{
public:
	static SGAActorManager& Instance()
	{
		static SGAActorManager instance;
		return instance;
	}

private:
	SGAActorManager();
	SGAActorManager(SGAActorManager const&);
	void operator =(SGAActorManager const&);

public:
	~SGAActorManager();

public:
	void		Update(float dt);
	void		CheckCollision();
	void		Draw();
	void		Release();
	
public:
	template<class T, class... Types> inline
	T* Create(Types&& ... args)
	{
		mActors.push_back(unique_ptr<T>(new T(forward<Types>(args)...)));
		return dynamic_cast<T*>(mActors.back().get());
	}

private:
	list<unique_ptr<SGAActor>> mActors;
};