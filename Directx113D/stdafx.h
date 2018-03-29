// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include <wrl.h>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3d11.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <WICTextureLoader.h>
#include <vld.h>

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <mmsystem.h>
#include "dxerr.h"
#include "SGASpriteSheet.h"
#include "SGAActor.h"
#include "SGAActorManager.h"
#include "SGATimer.h"
#include "SGAFramework.h"
#include "SGAResourceManager.h"
#include "SGAProjectile.h"
#include "SGAFSM.h"
