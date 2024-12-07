//-----------------------------------------------------------------------------
// File: cloth.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "resource.h"
#include "cloth.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include "ShaderConstants.h"
//#include <wxdebug.h>

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "Cloth simulation" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

	m_pVertexShader				= NULL;
	m_pVertexDeclaration = NULL;
    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	m_bPaused = false;
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	HRESULT hr;

    // TODO: perform one time initialization
	m_totalTime = 0.0f;

	// Initialize cloth system, and set up its internal vertex and index buffer.
	m_pClothSystem = new ClothSystem(m_Settings.iGridSize, m_Settings.iGridSize);

	if(FAILED(hr = m_pClothSystem->Initialize(m_Settings, true)))
		return hr;

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    // Initialize DirectInput
    InitInput( m_hWnd );

    m_bLoadingApp = FALSE;

    return S_OK;
}









//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

    // Create a IDirectInput8*
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return DXTRACE_ERR( "DirectInput8Create", hr );
    
    // Create a IDirectInputDevice8* for the keyboard
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return DXTRACE_ERR( "CreateDevice", hr );
    
    // Set the keyboard data format
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return DXTRACE_ERR( "SetDataFormat", hr );
    
    // Set the cooperative level on the keyboard
    if( FAILED( hr = m_pKeyboard->SetCooperativeLevel( hWnd, 
                                            DISCL_NONEXCLUSIVE | 
                                            DISCL_FOREGROUND | 
                                            DISCL_NOWINKEY ) ) )
        return DXTRACE_ERR( "SetCooperativeLevel", hr );

    // Acquire the keyboard
    m_pKeyboard->Acquire();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
		bCapsAcceptable = FALSE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;

    // TODO: create device objects
	if(FAILED(hr = m_pClothSystem->InitDeviceObjects( m_pd3dDevice )))
		return hr;

    // Init the font
    if(FAILED(hr = m_pFont->InitDeviceObjects( m_pd3dDevice )))
		return hr;

	// Create the shader declaration.
	D3DVERTEXELEMENT9 decl[] = 
	{
	  { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	  { 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	  { 0, 24,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	  D3DDECL_END()
	};

	m_pd3dDevice->CreateVertexDeclaration( decl, &m_pVertexDeclaration );

	// Assemble shader
	TCHAR        strShaderPath[512];
	LPD3DXBUFFER pCode;                  // Buffer with the assembled shader code
	LPD3DXBUFFER pErrorMsgs = NULL;      // Buffer with error messages
	DXUtil_FindMediaFileCb( strShaderPath, sizeof(strShaderPath), 
							_T("cloth.vsh") );

	D3DXAssembleShaderFromFile( strShaderPath, NULL, NULL, 0,  
								&pCode, &pErrorMsgs ); 
	
	if(pErrorMsgs) {
		TCHAR* errorString = new TCHAR[pErrorMsgs->GetBufferSize()+1];
		_tcsncpy(errorString, (TCHAR *) pErrorMsgs->GetBufferPointer(), pErrorMsgs->GetBufferSize());
		MessageBox(m_hWnd, errorString, NULL, MB_OK);
		delete errorString;
	}

	m_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),
									  &m_pVertexShader);
	SAFE_RELEASE(pCode);
	SAFE_RELEASE(pErrorMsgs);
	
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // TODO: setup render states

    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // don't cull faces of the cloth.

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
//    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
//    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
//    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX matViewProj = matView*m_matProj;
	D3DXMatrixTranspose(&matViewProj, &matViewProj);

    // Set up lighting states
//    D3DLIGHT9 light;
//    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
//    m_pd3dDevice->SetLight( 0, &light );
//    m_pd3dDevice->LightEnable( 0, TRUE );
//    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	D3DXVECTOR4 vecZero = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

	D3DXVECTOR4 vecLightDirection = D3DXVECTOR4(-1.0f, -1.0f, 2.0f, 0.0f);
	D3DXVec4Normalize(&vecLightDirection, &vecLightDirection);

	m_pd3dDevice->SetVertexShaderConstantF(CM_VIEWPROJMATRIX, (float *) &matViewProj, 4);
	m_pd3dDevice->SetVertexShaderConstantF(CV_LIGHT_DIRECTION, (float *) &vecLightDirection, 1);
	m_pd3dDevice->SetVertexShaderConstantF(CV_ZERO, (float *) &vecZero, 1);
	m_pd3dDevice->SetVertexShader( m_pVertexShader );


	// Restore cloth system vertex buffer
	m_pClothSystem->RestoreDeviceObjects();

    // Restore the font
    m_pFont->RestoreDeviceObjects();



    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: update world

    // Update user input state
    UpdateInput( &m_UserInput );

    // Update the world state according to user input
    D3DXMATRIX matRotLookFrom;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &matRotLookFrom, &matRotX, &matRotY );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR4 vFromPt   = D3DXVECTOR4( 0.0f, 0.0f, -5.0f, 1.0f );

    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR4 vUpVec    = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f );

	D3DXVec4Transform(&vFromPt, &vFromPt, &matRotLookFrom);
	D3DXVec4Transform(&vUpVec, &vUpVec, &matRotLookFrom);

    D3DXMatrixLookAtLH( &matView, (D3DXVECTOR3 *) &vFromPt, &vLookatPt, (D3DXVECTOR3 *) &vUpVec );


	D3DXMATRIX matViewProj = matView*m_matProj;
	D3DXMatrixTranspose(&matViewProj, &matViewProj);
	//    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	m_pd3dDevice->SetVertexShaderConstantF(CM_VIEWPROJMATRIX, (float *) &matViewProj, 4);

	m_pd3dDevice->SetVertexShaderConstantF(CV_EYE_WORLD, (float *) &vFromPt, 1);

	if(!m_bPaused) {
		m_pClothSystem->Update(m_Settings.timeStep);
		m_totalTime += m_Settings.timeStep;
	}

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: UpdateStats()
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateStats()
{
    const int cchMaxTimeStats = sizeof(m_strTimeStats) / sizeof(TCHAR);
    _sntprintf( m_strTimeStats, cchMaxTimeStats, _T("Elapsed simulation time: %f, Time step: %f"), m_totalTime, m_Settings.timeStep);
    m_strTimeStats[cchMaxTimeStats - 1] = TEXT('\0');

	CD3DApplication::UpdateStats();
}



//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    HRESULT hr;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    // TODO: Process user input as needed
    pUserInput->bRotateLeft  = ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );
    pUserInput->bRotateRight = ( (pUserInput->diks[DIK_RIGHT] & 0x80) == 0x80 );
    pUserInput->bRotateUp    = ( (pUserInput->diks[DIK_UP] & 0x80)    == 0x80 );
    pUserInput->bRotateDown  = ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: render world        
		m_pClothSystem->Render();

        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 60.0f; 

    lstrcpy( szMsg, m_strTimeStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 

    lstrcpy( szMsg, TEXT("Use arrow keys to rotate camera, C: reset position") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, TEXT("F3: simulation settings") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("F2: configure display, P: pause/unpause, R: reset simulation") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: UserSelectNewDevice()
// Desc: Displays a dialog so the user can select a new adapter, device, or
//       display mode, and then recreates the 3D environment if needed
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ShowSettingsDialog()
{

    // Can't display dialogs in fullscreen mode
    if( m_bWindowed == false )
    {
        if( FAILED( ToggleFullscreen() ) )
        {
            DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
            return E_FAIL;
        }
    }

    SettingsDialog settingsDialog( &m_Settings);
    if( settingsDialog.ShowDialog( m_hWnd ) != IDOK )
        return S_OK;

	int oldGridSize = m_Settings.iGridSize;
    settingsDialog.GetFinalSettings( &m_Settings );

	// Changing grid size requires creating a new ClothSystem object.
	if(m_Settings.iGridSize != oldGridSize) {
		delete m_pClothSystem;
		m_pClothSystem = new ClothSystem(m_Settings.iGridSize, m_Settings.iGridSize);
		m_pClothSystem->Initialize(m_Settings, true);
		m_pClothSystem->InitDeviceObjects(m_pd3dDevice);
		m_pClothSystem->RestoreDeviceObjects();
		m_totalTime = 0.0f;
	} else {
		m_pClothSystem->Initialize(m_Settings, false);
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

        case WM_COMMAND:
		{
            switch( LOWORD(wParam) )
            {
                case IDM_PAUSE:
					m_bPaused = !m_bPaused;
                    return 0;

				case IDM_SETTINGS:
                    Pause(true);
                    ShowSettingsDialog();
                    Pause(false);
                    return 0;

				case IDM_RESET: 
					{
						m_pClothSystem->Initialize(m_Settings, true);
						m_totalTime = 0.0f;
						return 0;
					}
				case IDM_RESETCAMERA:
					{
						m_fWorldRotX = m_fWorldRotY = 0.0;
						return 0;
					}
            }
            break;
		}

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
	m_pClothSystem->InvalidateDeviceObjects();
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{


    // TODO: Cleanup any objects created in InitDeviceObjects()
    m_pClothSystem->DeleteDeviceObjects();
	m_pFont->DeleteDeviceObjects();
    SAFE_RELEASE( m_pD3DXMesh );
	SAFE_RELEASE( m_pVertexShader);
	SAFE_RELEASE( m_pVertexDeclaration);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed
	SAFE_DELETE(m_pClothSystem); 

	// Cleanup D3D font
    SAFE_DELETE( m_pFont );

    // Cleanup DirectInput
    CleanupDirectInput();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    if(m_pKeyboard != NULL)
		m_pKeyboard->Unacquire();

    // Cleanup DirectX input objects
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}




