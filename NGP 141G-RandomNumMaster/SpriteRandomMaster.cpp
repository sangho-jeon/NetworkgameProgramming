#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>

#include "ddutil.h"

#include <dsound.h>
#include "dsutil.h"


#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))

HWND MainHwnd;

LPDIRECTDRAW         DirectOBJ;
LPDIRECTDRAWSURFACE  RealScreen;
LPDIRECTDRAWSURFACE  BackScreen;
LPDIRECTDRAWSURFACE  SpriteImage;
LPDIRECTDRAWSURFACE  BackGround;
LPDIRECTDRAWSURFACE  Gunship;
LPDIRECTDRAWSURFACE  heart;
LPDIRECTDRAWSURFACE  start;
LPDIRECTDRAWCLIPPER	ClipScreen;

int gFullScreen=0, Click=0;
int gWidth=640, gHeight=480*2;
int MouseX=gWidth/2, MouseY=gHeight-50;
int gamemode = 0;
int life = 3;
RECT SpriteRect, dstRect; // 0은 
////////////////////

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

HSNDOBJ Sound[10];


BOOL _InitDirectSound( void )
{
    if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
    {
        if (SoundOBJ->SetCooperativeLevel(MainHwnd,DSSCL_PRIORITY)!=DS_OK) return FALSE;

        memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
        DSB_desc.dwSize = sizeof(DSBUFFERDESC);
        DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;

        if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return FALSE;
        SoundDSB -> SetVolume(DSBVOLUME_MAX); // DSBVOLUME_MIN
        SoundDSB -> SetPan(DSBPAN_RIGHT);
        return TRUE;
    }
    return FALSE;
}

void _Play( int num )
{
    SndObjPlay( Sound[num], NULL );
}

////////////////////////


BOOL Fail( HWND hwnd )
{
    ShowWindow( hwnd, SW_HIDE );
    MessageBox( hwnd, "DIRECT X 초기화에 실패했습니다.", "게임 디자인", MB_OK );
    DestroyWindow( hwnd );
    return FALSE;
}

void _ReleaseAll( void )
{
    if ( DirectOBJ != NULL )
    {
        if ( RealScreen != NULL )
        {
            RealScreen->Release();
            RealScreen = NULL;
        }
        if ( SpriteImage != NULL )
        {
            SpriteImage->Release();
            SpriteImage = NULL;
        }
        if ( BackGround != NULL )
        {
            BackGround->Release();
            BackGround = NULL;
        }
        DirectOBJ->Release();
        DirectOBJ = NULL;
    }
}

long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int Step=5;


    switch ( message )
    {
/*
        case    WM_MOUSEMOVE    :   MouseX = LOWORD(lParam);
                                    MouseY = HIWORD(lParam);
                                    break;
*/
		case	WM_LBUTTONDOWN	: 	Click=1;
									_Play( 3 );
									break;
        case	WM_KEYDOWN:            
            switch (wParam)
            {
			case	'x':
			case	'X':
				gamemode = 1;
				return 0;
			
			case VK_ESCAPE:
                case VK_F12: 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;            

				case VK_LEFT: 
					MouseX-=Step;
					
					return 0;

                case VK_RIGHT: 
					MouseX+=Step;
					
					return 0;

                case VK_UP:
					//MouseY-=Step;	
					return 0;

                case VK_DOWN: 
					//MouseY+=Step;
					return 0;

				case VK_SPACE:
					Click=1;
					_Play( 3 );
					break;
			}
            break;

        case    WM_DESTROY      :  _ReleaseAll();
                                    PostQuitMessage( 0 );
                                    break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}

BOOL _GameMode( HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp )
{
    HRESULT result;
    WNDCLASS wc;
    DDSURFACEDESC ddsd;
    DDSCAPS ddscaps;
    LPDIRECTDRAW pdd;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TEST";
    RegisterClass( &wc );


	if(gFullScreen){
		if((MainHwnd=CreateWindowEx (0, "TEST", NULL, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), 
				GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL ))==NULL)
			ExitProcess(1);
	}
	else{
		if((MainHwnd=CreateWindow("TEST", "Master", WS_OVERLAPPEDWINDOW, 0, 0, x, 
									y, NULL, NULL, hInstance, NULL))==NULL)
			ExitProcess(1);
		SetWindowPos(MainHwnd, NULL, 100, 100, x, y, SWP_NOZORDER);
	}

    SetFocus( MainHwnd );
    ShowWindow( MainHwnd, nCmdShow );
    UpdateWindow( MainHwnd );
//    ShowCursor( FALSE );

    result = DirectDrawCreate( NULL, &pdd, NULL );
    if ( result != DD_OK ) return Fail( MainHwnd );

    result = pdd->QueryInterface(IID_IDirectDraw, (LPVOID *) &DirectOBJ);
    if ( result != DD_OK ) return Fail( MainHwnd );


	// 윈도우 핸들의 협력 단계를 설정한다.
	if(gFullScreen){
	    result = DirectOBJ->SetCooperativeLevel( MainHwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = DirectOBJ->SetDisplayMode( x, y, bpp);
		if ( result != DD_OK ) return Fail( MainHwnd );

		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

	    result = DirectOBJ -> CreateSurface( &ddsd, &RealScreen, NULL );
	   if ( result != DD_OK ) return Fail( MainHwnd );

		memset( &ddscaps, 0, sizeof(ddscaps) );
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		result = RealScreen -> GetAttachedSurface( &ddscaps, &BackScreen );
		if ( result != DD_OK ) return Fail( MainHwnd );
	}
	else{
	    result = DirectOBJ->SetCooperativeLevel( MainHwnd, DDSCL_NORMAL );
		if ( result != DD_OK ) return Fail( MainHwnd );

		memset( &ddsd, 0, sizeof(ddsd) );
	    ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 0;

		result = DirectOBJ -> CreateSurface( &ddsd, &RealScreen, NULL );
	    if(result != DD_OK) return Fail(MainHwnd);

		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof(ddsd);
	    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = x; 
		ddsd.dwHeight = y;
		result = DirectOBJ->CreateSurface( &ddsd, &BackScreen, NULL );
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = DirectOBJ->CreateClipper( 0, &ClipScreen, NULL);
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = ClipScreen->SetHWnd( 0, MainHwnd );
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = RealScreen->SetClipper( ClipScreen );
		if ( result != DD_OK ) return Fail( MainHwnd );

		SetWindowPos(MainHwnd, NULL, 100, 100, x, y, SWP_NOZORDER | SWP_NOACTIVATE); 
	}


    return TRUE;
}


extern void CommInit(int argc, char **argv);
extern void CommSend(char *sending);
extern void CommRecv(char *recvData);
void setSprite() {

}

void CALLBACK _GameProc(HWND hWnd, UINT message, UINT wParam, DWORD lParam)
{
    RECT BackRect = { 0, 0, 640, 960 };
	RECT DispRect = { 0, 0, gWidth, gHeight };
	RECT SpriteRect, dstRect, WinRect;
	char sendData[200];
	sprintf(sendData, "%d", gamemode + 10);
	CommSend(sendData);
	static int Frame = 0;

	if (gamemode == 0) {
		BackScreen->BltFast(0, 0, BackGround, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		
		SpriteRect.left = 108;
		SpriteRect.top = 115;
		SpriteRect.right = 330;
		SpriteRect.bottom = 159;
		
		dstRect.left = 180;
		dstRect.top = 300;
		dstRect.right = dstRect.left + 272;
		dstRect.bottom = dstRect.top + 64;

		BackScreen->Blt(&dstRect, start, &SpriteRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
		
	}

	else if (gamemode == 1) {

		BackScreen->BltFast(0, 0, BackGround, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);

		SpriteRect.left = Frame * 100;
		SpriteRect.top = 0;
		SpriteRect.right = SpriteRect.left + 100;
		SpriteRect.bottom = 100;

		if (Click) {
			if (++Frame >= 4) {
				Frame = 0;
				Click = 0;
			}
		}

		if (MouseX <= 50) MouseX = 50;
		if (MouseX > gWidth - 50) MouseX = gWidth - 50;
		if (MouseY <= 50) MouseY = 50;
		if (MouseY > gHeight - 50) MouseY = gHeight - 50;

		BackScreen->BltFast(MouseX - 50, MouseY - 50, SpriteImage, &SpriteRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);

		sprintf(sendData, "1,%d,%d,%d,%d,%d,%d,%d,%d", MouseX - 50, MouseY - 50, MouseX +50, MouseY +50, SpriteRect.left, SpriteRect.top, SpriteRect.right, SpriteRect.bottom);
		CommSend(sendData);


		//////////////////////////////
		// Enter splite animation here
		// use srand(int seed); rand();
		// stone            (380, 375)            (405, 395)

		static int SrcX = 0, SrcY = 0, Collision = 0;
		int posx, posy, size, coll = 0;
		srand(11);
		
		if (life <= 0) {
			gamemode = 2;
		}
		for (int i = 0; i < life; i++) {
			SpriteRect.left =401 ;
			SpriteRect.top = 167;
			SpriteRect.right = SpriteRect.left + 217;
			SpriteRect.bottom = SpriteRect.top + 178;
			dstRect.left = 20 + i * 30;
			dstRect.top = 60;
			dstRect.right = dstRect.left + 30;
			dstRect.bottom = dstRect.top + 30;

			BackScreen->Blt(&dstRect, heart, &SpriteRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d, %d, %d, %d, %d, %d, %d, %d, %d", 3, dstRect.left, dstRect.top, dstRect.right, dstRect.bottom, SpriteRect.left, SpriteRect.top, SpriteRect.right, SpriteRect.bottom);
			CommSend(sendData);
		}
		for (int k = 0; k < 20; k++) {
			posx = (SrcX + rand()) % (gWidth - 30) + 15;
			posy = (SrcY + rand()) % (gHeight - 30) + 15;
			size = (rand() % 20) - 10;

			if (abs(MouseX - (posx + 15)) < 40 && abs(MouseY - (posy + 15)) < 30) {	// collision
				// flare            (360, 425)            (390, 455)
				SpriteRect.left = 360;
				SpriteRect.top = 425;
				SpriteRect.right = 390;
				SpriteRect.bottom = 455;
				coll = 1;
			}
			else {	// stone
				// stone            (380, 375)            (405, 395)
				SpriteRect.left = 380;
				SpriteRect.top = 375;
				SpriteRect.right = 405;
				SpriteRect.bottom = 395;
			}

			dstRect.left = posx;
			dstRect.top = posy;
			dstRect.right = dstRect.left + 30 + size;
			dstRect.bottom = dstRect.top + 30 + size;

			BackScreen->Blt(&dstRect, Gunship, &SpriteRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);

			sprintf(sendData, "2,%d,%d,%d,%d,%d,%d,%d,%d", dstRect.left, dstRect.top, dstRect.right, dstRect.bottom, SpriteRect.left, SpriteRect.top, SpriteRect.right, SpriteRect.bottom);
			CommSend(sendData);
		}


		if (coll) {
			if (!Collision) {
				Collision = 1;
				_Play(4);
				life--;
			}
		}
		else
			Collision = 0;

		SrcY = SrcY + 3;
		
		
		

	}
	else if (gamemode == 2) {
		BackScreen->BltFast(0, 0, BackGround, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		life = 3;
		SpriteRect.left = 108;
		SpriteRect.top = 115;
		SpriteRect.right = 330;
		SpriteRect.bottom = 159;

		dstRect.left = 50;
		dstRect.top = 300;
		dstRect.right = dstRect.left + 222;
		dstRect.bottom = dstRect.top + 44;

		BackScreen->Blt(&dstRect, start, &SpriteRect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	}
	if (gFullScreen)
		RealScreen->Flip(NULL, DDFLIP_WAIT);
	else {
		GetWindowRect(MainHwnd, &WinRect);
		RealScreen->Blt(&WinRect, BackScreen, &DispRect, DDBLT_WAIT, NULL);
	}
	
}




int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG msg;

    if ( !_GameMode(hInstance, nCmdShow, gWidth, gHeight, 32) ) return FALSE;

    SpriteImage = DDLoadBitmap( DirectOBJ, "cat_right.BMP", 0, 0 );
    BackGround  = DDLoadBitmap( DirectOBJ, "faill2.BMP", 0, 0 );
    Gunship  = DDLoadBitmap( DirectOBJ, "EXAM3_3.BMP", 0, 0 );
	heart = DDLoadBitmap(DirectOBJ, "heart.BMP", 0, 0);
	start = DDLoadBitmap(DirectOBJ, "start_text.BMP", 0, 0);
	
	DDSetColorKey(start, RGB(0, 0, 0));
	DDSetColorKey(heart, RGB(0, 0, 0));
    DDSetColorKey( SpriteImage, RGB(0,0,0) );
    DDSetColorKey( Gunship, RGB(0,0,0) );

	SetTimer(MainHwnd, 1, 30, _GameProc);

	CommInit(NULL, NULL);

///////////////////

    if ( _InitDirectSound() )
    {
        Sound[0] = SndObjCreate(SoundOBJ,"MUSIC.WAV",1);
        Sound[1] = SndObjCreate(SoundOBJ,"LAND.WAV",2);
        Sound[2] = SndObjCreate(SoundOBJ,"GUN1.WAV",2);
        Sound[3] = SndObjCreate(SoundOBJ,"KNIFE1.WAV",2);
        Sound[4] = SndObjCreate(SoundOBJ,"DAMAGE1.WAV",2);
        Sound[5] = SndObjCreate(SoundOBJ,"DAMAGE2.WAV",2);

//        SndObjPlay( Sound[0], DSBPLAY_LOOPING );
    }
//////////////////

    while ( !_GetKeyState(VK_ESCAPE) )
    {

        if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
        {
            if ( !GetMessage(&msg, NULL, 0, 0) ) return msg.wParam;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
//        else _GameProc();
    }
    DestroyWindow( MainHwnd );

    return TRUE;
}
