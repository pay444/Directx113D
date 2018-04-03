// WinMain.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
//#include "GameMain.h" //기본 박스
//#include "Chapter6Box.h" //기본 박스 추가
//#include "Chapter6Grid.h" //지형출력
//#include "Chapter6Shapes.h" //모형들 출력
//#include "Chapter6Sukll.h"	//해골 출력
//#include "Chapter6Wave.h"		//파도 출력
//#include "Chapter7Light.h"		//조명 +파도
#include "Chapter7LitSkull.h"	//해골 + 조명
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int nCmdShow)
{
	//GameMain game;
	//Chapter6Box game; //박스
	//Chapter6Grid game;	//지형
	//Chapter6Shapes game;	//모양들 출력
	//Chapter6Sukll game;		//해골 출력
	//Chapter6Wave game;		//파도 출력
	//Chapter7Light game;			//조명 + 파도
	Chapter7LitSkull game;		//조명 + 해골

	game.InitWindow(hInstance, WS_OVERLAPPEDWINDOW);//& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX);
    
	return game.GameLoop();
}
