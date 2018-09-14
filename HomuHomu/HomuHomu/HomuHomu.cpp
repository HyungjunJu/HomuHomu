// HomuHomu.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Win32.h"
#include "egl/egl.h"
#include "vg/openvg.h"
#include "vgLoadImage.h"
#include "stdlib.h"
#include "dxutsound.h"

#define KEY_DOWN(code) ((GetAsyncKeyState(code)&0x8000)?1:0)
#define MAX_LOADSTRING 100


CSoundManager* sm = NULL;
CSound* BumbSound = NULL;
CSound* GunSound = NULL;
CSound* MachinSound = NULL;
CSound* breakSound = NULL;
CSound* KillSound = NULL;
CSound* JumpSound = NULL;
CSound* playBackSound = NULL;
CSound* StartBackSound = NULL;
CSound* EpiBackSound = NULL;
CSound* TimeStopSound = NULL;

EGLDisplay display;
EGLSurface surface;
EGLContext context;

//호무라 걷기 
VGImage PlayerWalkingImages[7];

//호무라 권총
VGImage PlayerGun[5];

//호무라 달리기
VGImage PlayerRunning[7];

//호무라 머신건
VGImage PlayerMachin[10];

//호무라 대포
VGImage PlayerBumb[12];

//호무라 넘어지기
VGImage PlayerSick[4];

//몬스터- 큐베
VGImage enemy1Images[6];

//몬스터- 신발마녀
VGImage enemy2Image;

//총 발사, 대포 발사
VGImage GunShootImage;
VGImage BumbShootImage;

//지우기
VGImage GunShoot_1Image;

//배경화면
VGImage BK_1Image;

//시간정지 배경화면
VGImage BK_2Image;

//타이틀화면
VGImage TitleImage;

//게임오버화면
VGImage GameOverImage;

//튜토리얼화면
VGImage TutorialImage;

//하트게이지
VGImage H0Image;
VGImage H1Image;
VGImage H2Image;
VGImage H3Image;

void timerProc();
void draw();

struct Player
{	
	int x, y,  countS, countR, countG;
	bool ground;
} player;

struct MISSILE1
{
	int x, y, shot;
} missile1[100];

struct MISSILE2
{
	int x, y, shot;
} missile2[100];

//지우기
struct MISSILE3
{
	int x, y, shot;
} missile3[100];


struct ENEMY1
{
		int x, y, shot, count;
} enemy1[100];

struct ENEMY2
{		
	int x, y, shot;
} enemy2[100];

int cx = 0;
int HNum = 3, ENum = 0;
int GameState = 0;
int count = 0;
int m=0;
float gravity = -10;
float jumpVel=0;
int timeStop=0;
int runAction = 0; // 달리기 액션 유무
int gunAction = 0; // 권총 쏘는 액션 유무
static float frame = 0;
static float mapFrame = 0;


// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HOMUHOMU, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HOMUHOMU));

	// 기본 메시지 루프입니다.
	DWORD lastTime = GetTickCount();
	while (1)
	{
		if( PeekMessage(&msg, NULL, 0, 0, 1) )
		{
			if(!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{ TranslateMessage(&msg);
			DispatchMessage(&msg); }
			if( msg.message == WM_QUIT ) break;
		}
		DWORD curTime = GetTickCount();
		if( curTime-lastTime>80 ) // 30 frame per second!
		{
			lastTime = lastTime+81;
			timerProc();
		}
	}


	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HOMUHOMU));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HOMUHOMU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      300, 200, 1300, 800, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		{
			display = eglGetDisplay( GetDC(hWnd));
			
			eglInitialize( display, NULL, NULL );
			eglBindAPI( EGL_OPENVG_API );
			
			EGLint conf_list[] = { EGL_RED_SIZE, 8,
			 EGL_GREEN_SIZE, 8,
			 EGL_BLUE_SIZE, 8,
			 EGL_ALPHA_SIZE, 8,
			 EGL_NONE };

			EGLConfig config;
			EGLint num_config;
			eglChooseConfig( display, conf_list, &config, 1, &num_config );
			
			surface = eglCreateWindowSurface( display, config, hWnd, NULL );
			context = eglCreateContext( display, 0, NULL, NULL );
			eglMakeCurrent(display, surface, surface, context) ;

			
			//호무라 걷기 이미지
			PlayerWalkingImages[0] = vgLoadImage(TEXT("frame0.png"));
			PlayerWalkingImages[1] = vgLoadImage(TEXT("frame1.png"));
			PlayerWalkingImages[2] = vgLoadImage(TEXT("frame2.png"));
			PlayerWalkingImages[3] = vgLoadImage(TEXT("frame3.png"));
			PlayerWalkingImages[4] = vgLoadImage(TEXT("frame4.png"));
			PlayerWalkingImages[5] = vgLoadImage(TEXT("frame5.png"));
			PlayerWalkingImages[6] = vgLoadImage(TEXT("frame6.png"));
			PlayerWalkingImages[7] = vgLoadImage(TEXT("frame7.png"));

			//호무라 달리기 이미지
			PlayerRunning[0] = vgLoadImage(TEXT("humuRunning_1.png"));
			PlayerRunning[1] = vgLoadImage(TEXT("humuRunning_2.png"));
			PlayerRunning[2] = vgLoadImage(TEXT("humuRunning_3.png"));
			PlayerRunning[3] = vgLoadImage(TEXT("humuRunning_4.png"));
			PlayerRunning[4] = vgLoadImage(TEXT("humuRunning_5.png"));
			
			//호무라 넘어지기 이미지
			PlayerSick[0] = vgLoadImage(TEXT("homuSick_1.png"));
			PlayerSick[1] = vgLoadImage(TEXT("homuSick_2.png"));
			PlayerSick[2] = vgLoadImage(TEXT("homuSick_3.png"));
			PlayerSick[3] = vgLoadImage(TEXT("homuSick_4.png"));
			PlayerSick[4] = vgLoadImage(TEXT("homuSick_5.png"));


			//
			

			//호무라 권총 이미지
			PlayerGun[0] = vgLoadImage(TEXT("homuGun_1.png"));
			PlayerGun[1] = vgLoadImage(TEXT("homuGun_2.png"));
			PlayerGun[2] = vgLoadImage(TEXT("homuGun_3.png"));
			PlayerGun[3] = vgLoadImage(TEXT("homuGun_4.png"));
			PlayerGun[4] = vgLoadImage(TEXT("homuGun_5.png"));
			PlayerGun[5] = vgLoadImage(TEXT("homuGun_6.png"));

			
			//호무라 기관총 이미지

			PlayerMachin[0] = vgLoadImage(TEXT("homuMachin_1.png"));
			PlayerMachin[1] = vgLoadImage(TEXT("homuMachin_2.png"));
			PlayerMachin[2] = vgLoadImage(TEXT("homuMachin_3.png"));
			PlayerMachin[3] = vgLoadImage(TEXT("homuMachin_4.png"));
			PlayerMachin[4] = vgLoadImage(TEXT("homuMachin_5.png"));
			PlayerMachin[5] = vgLoadImage(TEXT("homuMachin_6.png"));
			PlayerMachin[6] = vgLoadImage(TEXT("homuMachin_7.png"));
			PlayerMachin[7] = vgLoadImage(TEXT("homuMachin_8.png"));
			PlayerMachin[8] = vgLoadImage(TEXT("homuMachin_9.png"));
			PlayerMachin[9] = vgLoadImage(TEXT("homuMachin_10.png"));
			PlayerMachin[10] = vgLoadImage(TEXT("homuMachin_11.png"));
		
			//호무라 대포 이미지
			PlayerBumb[0] = vgLoadImage(TEXT("homuBumb_1.png"));
			PlayerBumb[1] = vgLoadImage(TEXT("homuBumb_2.png"));
			PlayerBumb[2] = vgLoadImage(TEXT("homuBumb_3.png"));
			PlayerBumb[3] = vgLoadImage(TEXT("homuBumb_4.png"));
			PlayerBumb[4] = vgLoadImage(TEXT("homuBumb_5.png"));
			PlayerBumb[5] = vgLoadImage(TEXT("homuBumb_6.png"));
			PlayerBumb[6] = vgLoadImage(TEXT("homuBumb_7.png"));
			PlayerBumb[7] = vgLoadImage(TEXT("homuBumb_8.png"));
			PlayerBumb[8] = vgLoadImage(TEXT("homuBumb_9.png"));
			PlayerBumb[9] = vgLoadImage(TEXT("homuBumb_10.png"));
			PlayerBumb[10] = vgLoadImage(TEXT("homuBumb_11.png"));
			PlayerBumb[11] = vgLoadImage(TEXT("homuBumb_12.png"));
			PlayerBumb[12] = vgLoadImage(TEXT("homuBumb_13.png"));
			

			//큐베 서있기 이미지
			enemy1Images[0] = vgLoadImage(TEXT("Kyu_1.png"));
			enemy1Images[1] = vgLoadImage(TEXT("Kyu_2.png"));
			enemy1Images[2] = vgLoadImage(TEXT("Kyu_3.png"));
			enemy1Images[3] = vgLoadImage(TEXT("Kyu_4.png"));
			enemy1Images[4] = vgLoadImage(TEXT("Kyu_5.png"));
			enemy1Images[5] = vgLoadImage(TEXT("Kyu_6.png"));
			enemy1Images[6] = vgLoadImage(TEXT("Kyu_7.png"));
			
			//총, 대포 발사 이미지
			GunShootImage = vgLoadImage(TEXT("gunShoot.png"));
			BumbShootImage = vgLoadImage(TEXT("bumb.png"));
			
			//지우기
			GunShoot_1Image = vgLoadImage(TEXT("gunShoot.png"));


			//신발 마녀 이미지
			enemy2Image = vgLoadImage(TEXT("ShosesMonster.png"));
	
			//화면 이미지
			TitleImage = vgLoadImage(TEXT("TitleBk.png"));
			GameOverImage = vgLoadImage(TEXT("GameOver.png"));		
			BK_1Image = vgLoadImage(TEXT("GameBK_01.png"));
			TutorialImage = vgLoadImage(TEXT("Tutorial.png"));
			BK_2Image = vgLoadImage(TEXT("GameBK_02.png"));
			
			//등등 이미지
			H0Image = vgLoadImage(TEXT("SoulJam.png"));
			H1Image = vgLoadImage(TEXT("heart1.png"));
			H2Image = vgLoadImage(TEXT("heart2.png"));
			H3Image = vgLoadImage(TEXT("heart3.png"));
		


			//플레이어 초기 위치 설정
			player.x = 30;
			player.y = 30;

			//카운트 및 땅에 서 있을 때
			player.countS = 0;
			player.countR = 0;
			player.countG = 0;
			player.ground = true;
			/*
			sm = new CSoundManager();
			sm -> Initialize(hWnd, DSSCL_NORMAL);
			sm -> SetPrimaryBufferFormat(2, 22050, 16);
			sm -> Create(&GunSound, TEXT("gun.wav"), 0, GUID_NULL);
			sm -> Create(&BumbSound, TEXT("bumb.wav"), 0, GUID_NULL);
			sm -> Create(&MachinSound, TEXT("mach.wav"), 0, GUID_NULL);
			sm -> Create(&breakSound, TEXT("powerdown2.wav"), 0, GUID_NULL);
			sm -> Create(&KillSound, TEXT("powerdown.wav"), 0, GUID_NULL);
			sm -> Create(&playBackSound, TEXT("Puella.wav"), 0, GUID_NULL); 
			sm -> Create(&StartBackSound, TEXT("credens.wav"), 0, GUID_NULL);
			sm -> Create(&EpiBackSound, TEXT("Magia.wav"), 0, GUID_NULL);
			sm -> Create(&JumpSound, TEXT("jump.wav"), 0, GUID_NULL);	
			sm -> Create(&TimeStopSound, TEXT("time.wav"), 0, GUID_NULL);
			*/
			for(int i=0; i<100; i++)
				missile1[i].shot = false;
			for(int i=0; i<100; i++)
				missile2[i].shot = false;
			for(int i=0; i<100; i++)
				enemy1[i].shot = false;
			for(int i=0; i<100; i++)
				enemy2[i].shot = false;

			//지우기
			for(int i=0; i<100; i++)
				missile3[i].shot = false;
		}
		break;
	case WM_KEYDOWN:
		{
		//Z 키 눌렀을 때 총 발사
		if(wParam == 'Z')
		{	
			
			//	GunSound -> Stop();
			//	GunSound -> Reset();
			//	GunSound -> Play(0);

				if(gunAction==0)
				gunAction=1;
			
				for(int i=0; i<100; i++)
				{
					if(!missile1[i].shot)
					{
						missile1[i].shot = true;
						missile1[i].y = player.y+30;
						missile1[i].x = player.x;
						break;
					}
				}
		}
		

			//x키 눌렀을 때 대포 발사
		if(wParam == 'X')
		{
			//	BumbSound -> Stop();
			//	BumbSound -> Reset();
			//	BumbSound -> Play(0);
		
				if(gunAction==0)
				gunAction=2;

				for(int i=0; i<100; i++)
				{
					if(!missile2[i].shot)
					{
						missile2[i].shot = true;
						missile2[i].y = player.y+30;
						missile2[i].x = player.x;
						break;
					}
					
				}
		}
		
		if(wParam == 'C')
		{
				//MachinSound -> Stop();
			//	MachinSound -> Reset();
			//	MachinSound -> Play(0);

				if(gunAction==0)
				gunAction=3;

			for(int i= 0; i<100; i++)
				if(!missile3[i].shot)
				{
					missile3[i].shot = true;
					missile3[i].y = player.y+30;
					missile3[i].x=player.x;
					break;
				}
		}
		//break;

		//스페이스 누르면 시간 멈춤
		if(wParam == VK_SPACE)
		{
			
			//TimeStopSound -> Stop();
			//TimeStopSound -> Reset();
			//TimeStopSound -> Play(0);
			
			if(gunAction==0)
				gunAction=5;
			if(timeStop==0)
				timeStop=1;
			else timeStop=0;
			break;

		}
		//오른쪽 누르면 액션 
		if(wParam == VK_RIGHT)
		{
			if(runAction == 0)
				runAction = 1;
			else runAction = 0;
			break;
		}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void draw(){
	if(GameState == 0)
	{
		//EpiBackSound -> Stop();
		//StartBackSound -> Play(0, DSBPLAY_LOOPING);

		eglMakeCurrent(display,surface,surface,context);
		vgClear(0,0,10000,10000);
		 VGfloat color[4] = {1,1,1,1};
		vgSetfv(VG_CLEAR_COLOR, 4, color);
		vgSeti( VG_MATRIX_MODE,VG_MATRIX_IMAGE_USER_TO_SURFACE);

		vgLoadIdentity();
		vgDrawImage(TitleImage);

		eglSwapBuffers(display, surface);
	}
	else if(GameState == 2)
	{
		//StartBackSound -> Play(0, DSBPLAY_LOOPING);

		 vgClear(0,0,10000,10000);
		 VGfloat color[4] = {1,1,1,1};
		 vgSetfv(VG_CLEAR_COLOR, 4, color);
		 vgSeti( VG_MATRIX_MODE,VG_MATRIX_IMAGE_USER_TO_SURFACE);

		 vgLoadIdentity();
		 vgDrawImage(TutorialImage);

		eglSwapBuffers(display, surface);

	}
	else if(GameState == 3)
	{

		//StartBackSound -> Stop();
		//playBackSound -> Play(0, DSBPLAY_LOOPING);
		eglMakeCurrent(display,surface,surface,context);
		vgClear(0,0,10000,10000);
		VGfloat color[4] = {1,1,1,1};
		vgSetfv(VG_CLEAR_COLOR, 4, color);
		vgSeti( VG_MATRIX_MODE,VG_MATRIX_IMAGE_USER_TO_SURFACE);
	
		//배경화면 이동시키기
		int k = (3*m)/1300;
		
		if (timeStop == 1)
		{
			vgLoadIdentity();
			vgTranslate(k*1300-3*m,0);
			vgDrawImage(BK_2Image);
		

			vgLoadIdentity();
			vgTranslate((k+1)*1300-3*m,0);
			vgDrawImage(BK_2Image);
		}else
		{

			vgLoadIdentity();
			vgTranslate(k*1300-3*m,0);
			vgDrawImage(BK_1Image);


			vgLoadIdentity();
			vgTranslate((k+1)*1300-3*m,0);
			vgDrawImage(BK_1Image); 
		}

		//목숨 이미지
		if(HNum == 3)
		{
			vgLoadIdentity();
			vgTranslate(1000, 600);
			vgDrawImage(H0Image);

		

		}

		else if (HNum == 2)
		{
			vgLoadIdentity();
			vgTranslate(1000, 600);
			vgDrawImage(H1Image);
		}

		else if (HNum == 1)
		{
			vgLoadIdentity();
			vgTranslate(1000, 600);
			vgDrawImage(H2Image);
		}

	

		//권총쏘는 호무라 이미지 부르기
		if(gunAction == 1)
		{
			vgLoadIdentity();
			vgTranslate(player.x, player.y);
			vgDrawImage(PlayerGun[player.countG]);
			player.countG += 1;
			if(player.countG >4) 
			{
				player.countG = 0;
				gunAction=0;
			}
		}

		//기관총 쏘는 호무라 이미지 부르기
		if(gunAction == 3)
		{
			vgLoadIdentity();
			vgTranslate(player.x, player.y);
			vgDrawImage(PlayerMachin[player.countG]);
			player.countG += 1;
		if(player.countG >9) 
			{
				player.countG = 0;
				gunAction=0;
			}
		
		}

		//대포 쏘는 호무라 이미지 부르기
		if(gunAction == 2)
		{
			vgLoadIdentity();
			vgTranslate(player.x, player.y);
			vgDrawImage(PlayerBumb[player.countG]);
			player.countG += 1;
		if(player.countG >12) 
			{
				player.countG = 0;
				gunAction=0;
			}
		
		}



		//걸어다니는 호무라 이미지 부르기
		if(runAction == 0 && gunAction==0){
			vgLoadIdentity();
			vgTranslate(player.x, player.y);
			vgDrawImage(PlayerWalkingImages[player.countS]);
			player.countS += 1;
		if(player.countS >6) // 이거 맞나?? 
			player.countS = 0;
		}

		//달리는 호무라 이미지 부르기
		if(runAction == 1&& gunAction==0)
		{
			vgLoadIdentity();
			vgTranslate(player.x, player.y);
			vgDrawImage(PlayerRunning[player.countR]);
			player.countR += 1;
			if(player.countR >4) 
				player.countR = 0;
		} 

		
		//넘어지는 호무라 이미지 부르기
		if(gunAction == 4)
		{
			vgLoadIdentity();
			vgTranslate(player.x, player.y);
			vgDrawImage(PlayerSick[player.countG]);
			player.countG += 1;
			if(player.countG >3) 
				{
					player.countG = 0;
					gunAction=0;
				}
		
		}

		

		//총알 이미지 그려주기
		for (int i=0; i<100; i++)
		{
			if(missile1[i].shot)
			{
				vgLoadIdentity();
				vgTranslate(missile1[i].x+60, missile1[i].y+50);
				vgDrawImage(GunShootImage);
			}
		}
	
		//대포 이미지 그려주기
		for (int i=0; i<100; i++)
		{ 
			if(missile2[i].shot)
			{
				vgLoadIdentity();
				vgTranslate(missile2[i].x+75, missile2[i].y+55);
				vgDrawImage(BumbShootImage);
			}
		}



		//머신건 총알 이미지 그려주기

		for (int i=0; i<100; i++)
		{
			if(missile3[i].shot)
			{
				vgLoadIdentity();
				vgTranslate(missile3[i].x+35, missile3[i].y+25);
				vgDrawImage(GunShoot_1Image);
			}
		}

		//큐베 이미지 그려주기
		for (int i=0; i<100; i++)
		{
			if(enemy1[i].shot)
			{
				vgLoadIdentity();
				vgTranslate(enemy1[i].x, enemy1[i].y);
				vgDrawImage(enemy1Images[enemy1[i].count]);
				if(timeStop==0)
				enemy1[i].count+=1;
				if(enemy1[i].count>5)
				enemy1[i].count=0; //맞을지 모르겠음. 
			}
		}

		//신발 마녀 이미지 그려주기
		for (int i =0; i<100; i++)
		{
			if(enemy2[i].shot)
			{
				vgLoadIdentity();
				vgTranslate(enemy2[i].x, enemy2[i].y);
				vgDrawImage(enemy2Image);
			}
		}
		eglSwapBuffers(display, surface);
	}
	if(GameState == 5)
	{
		//playBackSound -> Stop();
		//EpiBackSound -> Play(0, DSBPLAY_LOOPING);
		
		eglMakeCurrent(display,surface,surface,context);
		VGfloat clearColor[4] = {1, 1, 0.1, 1};
	
		vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
		vgClear(0, 0, 2000,2000);

		vgLoadIdentity();
		vgDrawImage(GameOverImage);

		eglSwapBuffers(display, surface);
	}
}

void timerProc(){
	runAction = 0;
	if(GameState == 0)
	{
		if(KEY_DOWN('2'))
		{
			GameState = 2;
		}
		if(KEY_DOWN('3'))
		{
			GameState = 3;
		}
		if(KEY_DOWN('4'))
		{
			PostQuitMessage(0);
		}
	}
	if(GameState == 2)
	{
		if(KEY_DOWN(VK_RETURN))
		{
			GameState = 3;
		}
	}
	if(GameState == 3)
	{
		jumpVel=250;

		if(player.y<=0)
		{
			player.ground = true;
			gravity=0;
		}
		else
		{
			gravity=-20;
			player.y+=gravity;
			player.ground=false;
		}
		if(HNum == 0)
			GameState = 5;

		// 위, 아래 버튼 눌렀을 때
	//	if(KEY_DOWN(VK_UP) && player.y<80)
	//	{

	//		player.y +=30;
	//	}
	//	if(KEY_DOWN(VK_DOWN) && player.y>40)
	//	{
		
	//		player.y -= 30;
	//	}

		if(KEY_DOWN(VK_RIGHT) )
		{
			if(runAction == 0)
			{
				runAction = 1;
			} 
			if(player.x>390)
			m+=5;
			if(player.x<400)
			{
				player.x +=10;
				m+=3;
			}
		} 
		if(KEY_DOWN(VK_LEFT)  )
		{
			m-=3;
			if(player.x>0)
			player.x -= 10;
		}
		if(KEY_DOWN(VK_UP))
		{
			if(player.ground)
			{
				player.y = jumpVel++;
				player.ground=false;
			}
		}
		//공격하는 미사일 속도
		if(timeStop==0)
		{
			for(int i=0; i<100; i++)
			{
				if(missile1[i].shot)
				{
					missile1[i].x += 30;
					if(missile1[i].x > 1500) missile1[i].shot=0;
				}
			}
			for(int i=0; i<100; i++)
			{
				if(missile2[i].shot)
				{
					missile2[i].x += 20;
					if(missile2[i].x > 1500) missile2[i].shot=0;
				}
			}

			for(int i=0; i<100; i++)
			{
				if(missile3[i].shot)
				{
					missile3[i].x += 50;
					if(missile3[i].x > 1500) missile3[i].shot=0;
				}
			}
			//큐베 속도
			for(int i=0; i<100; i++)
			{
					if(enemy1[i].shot)
					{
						enemy1[i].x -= 20;
						if(enemy1[i].x < -1500) enemy1[i].shot=0;
					}
			}

			//신발 마녀 속도
			for(int i=0; i<100; i++)
			{
					if(enemy2[i].shot)
					{
						enemy2[i].y-= 20;

						if(enemy2[i].y < -100) enemy2[i].shot=0;
					}
			}


			unsigned int randomNum = rand()%100;
			unsigned int randomNum2 = rand()%55;
			unsigned int randomNum3 = rand()%200;
	
			// 큐베 랜덤발생
			if(randomNum == 1)
			{
				for(int i=0; i<100; i++)
				{
					if(!enemy1[i].shot)
					{
						enemy1[i].shot = true;
						enemy1[i].y = 10 * (rand()%30) +1;
						enemy1[i].x = 1400;
						break;
					}
				}
			}
		

			//신발마녀 랜덤발생
			if(randomNum2 == 1)
			{
				for(int i=0; i<100; i++)
				{
					if(!enemy2[i].shot)
					{
						enemy2[i].shot = true;
						enemy2[i].x= rand()%1500;
						enemy2[i].y = 900;
						break;
					}
				}
			}
		
		}
		//////////////////////////////////////////////////////////////////공격
		//총 + 신발마녀
		for(int i=0; i<100; i++){
			for(int j=0; j<100; j++){
				if(missile1[i].shot && enemy2[j].shot){
					int x11 = missile1[i].x;
					int x12 = missile1[i].x+25;
					int y11 = missile1[i].y;
					int y12 = missile1[i].y+35;

					int x21 = enemy2[j].x;
					int x22 = enemy2[j].x+60;
					int y21 = enemy2[j].y;
					int y22 = enemy2[j].y+200;

					if(x11<x22 && x21<x12 && y11<y22 && y21<y12){
						//KillSound -> Play(0);
						missile1[i].shot = 0;
						enemy2[j].shot = 0;
					}
				}
			}
		}
		
		//대포 + 큐베
		for(int i=0; i<100; i++){
			for(int j=0; j<100; j++){
				if(missile2[i].shot && enemy1[j].shot){
					int x11 = missile2[i].x;
					int x12 = missile2[i].x+40;
					int y11 = missile2[i].y;
					int y12 = missile2[i].y+35;

					int x21 = enemy1[j].x;
					int x22 = enemy1[j].x+60;
					int y21 = enemy1[j].y;
					int y22 = enemy1[j].y+120;

					if(x11<x22 && x21<x12 && y11<y22 && y21<y12){
						//KillSound -> Play(0);
						missile2[i].shot = 0;
						enemy1[j].shot = 0;
					}
				}
			}
		}
		//대포 + 신발마녀
		for(int i=0; i<100; i++){
			for(int j=0; j<100; j++){
				if(missile2[i].shot && enemy2[j].shot){
					int x11 = missile2[i].x;
					int x12 = missile2[i].x+40;
					int y11 = missile2[i].y;
					int y12 = missile2[i].y+35;

					int x21 = enemy2[j].x;
					int x22 = enemy2[j].x+60;
					int y21 = enemy2[j].y;
					int y22 = enemy2[j].y+200;

					if(x11<x22 && x21<x12 && y11<y22 && y21<y12){
						//KillSound -> Play(0);
						missile2[i].shot = 0;
						enemy2[j].shot = 0;
					}
				}
			}
		}
		
		//////////////////////////////////////////////////////////////////부딪히는 경우
		//호무라 + 큐베
		for(int i=0; i<100; i++)
		{
			if(enemy1[i].shot)
			{
				int x11 = enemy1[i].x;
				int x12 = enemy1[i].x+150;
				int y11 = enemy1[i].y;
				int y12 = enemy1[i].y+120;

				int x21 = player.x;
				int x22 = player.x+60;
				int y21 = player.y;
				int y22 = player.y+12;

				if(x11<x22 && x21<x12 && y11<y22 && y21<y12)
				{
					//breakSound -> Play(0);
					enemy1[i].shot = 0;
					HNum = HNum-1;
					if(gunAction==0)
					gunAction=4;
				}
			}
		}

		//호무라 + 신발마녀
		for(int i=0; i<100; i++)
		{
			if(enemy2[i].shot)
			{
				int x11 = enemy2[i].x;
				int x12 = enemy2[i].x+60;
				int y11 = enemy2[i].y;
				int y12 = enemy2[i].y+200;

				int x21 = player.x;
				int x22 = player.x+90;
				int y21 = player.y;
				int y22 = player.y+208;

				if(x11<x22 && x21<x12 && y11<y22 && y21<y12)
				{
					//breakSound -> Play(0);
					enemy2[i].shot = 0;
					HNum = HNum-1;
					if(gunAction==0)
					gunAction=4;
				}
			}
		}
	}
	if(GameState == 5)
	{
		if(KEY_DOWN(VK_BACK))
		{
			GameState = 0;


			cx = 0;
			HNum = 3; ENum = 0;

			player.x = 250;
			player.y = 30;
			player.countR = 0;
			player.countS = 0;

			for(int i=0; i<100; i++)
				missile1[i].shot = false;
			for(int i=0; i<100; i++)
				missile2[i].shot = false;
			for(int i=0; i<100; i++)
				enemy1[i].shot = false;
			for(int i=0; i<100; i++)
				enemy2[i].shot = false;
		}
		if(KEY_DOWN(VK_RETURN))
		{
			PostQuitMessage(0);
		}
	}
	draw();
}