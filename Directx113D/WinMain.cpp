// WinMain.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
//#include "GameMain.h" //�⺻ �ڽ�
//#include "Chapter6Box.h" //�⺻ �ڽ� �߰�
//#include "Chapter6Grid.h" //�������
//#include "Chapter6Shapes.h" //������ ���
//#include "Chapter6Sukll.h"	//�ذ� ���
//#include "Chapter6Wave.h"		//�ĵ� ���
//#include "Chapter7Light.h"		//���� +�ĵ�
#include "Chapter7LitSkull.h"	//�ذ� + ����
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int nCmdShow)
{
	//GameMain game;
	//Chapter6Box game; //�ڽ�
	//Chapter6Grid game;	//����
	//Chapter6Shapes game;	//���� ���
	//Chapter6Sukll game;		//�ذ� ���
	//Chapter6Wave game;		//�ĵ� ���
	//Chapter7Light game;			//���� + �ĵ�
	Chapter7LitSkull game;		//���� + �ذ�

	game.InitWindow(hInstance, WS_OVERLAPPEDWINDOW);//& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX);
    
	return game.GameLoop();
}
