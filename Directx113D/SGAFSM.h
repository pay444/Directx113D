#pragma once

class SGAFSM;

class SGAState
{
protected:
	SGAFSM *mpFSM;

public:
	virtual ~SGAState() {};
	virtual void Enter() = 0;
	virtual void Execute(float dt) = 0;
	virtual void Exit() = 0;

public:
	void SetFSM(SGAFSM *pFSM) { this->mpFSM = pFSM; }
};

class SGAFSM
{
private:
	map<int, unique_ptr<SGAState>> mStateMap;
	SGAState* mpCurrent;
	SGAActor* mpOwner;

public:
	void SetOwner(SGAActor* pOwner) { this->mpOwner = pOwner; }
	SGAActor* GetOwner() { return this->mpOwner; }

public:
	template<class T>
	void AddState(int stateID);
	void ChangeState(int stateID);
	void Update(float dt);
};

template<class T>
inline void SGAFSM::AddState(int stateID)
{
	auto result = mStateMap.insert(pair<int, unique_ptr<SGAState>>(stateID, nullptr));

	if (result.second == true)
	{
		result.first->second = move(unique_ptr<SGAState>(new T()));
		result.first->second->SetFSM(this);
	}
	else
		DXTRACE_ERR_MSGBOX(L"State already Exists!", 0);
}