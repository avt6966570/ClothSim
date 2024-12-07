//-----------------------------------------------------------------------------
// File: cloth.h
//
// Desc: Header file cloth sample app
//-----------------------------------------------------------------------------
#pragma once

#include "ClothSystem.h"
#include "Settings.h"


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\cloth")

// Struct to store the current input state
struct UserInput
{
    BYTE diks[256];   // DirectInput keyboard state buffer 

    // TODO: change as needed
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    ID3DXMesh*              m_pD3DXMesh;            // D3DX mesh to store teapot

    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
    UserInput               m_UserInput;            // Struct for storing user input 

    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis
    D3DXMATRIX m_matProj;

	ClothSystem*			m_pClothSystem;

	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;

	FLOAT					m_totalTime;
    TCHAR		            m_strTimeStats[90]; // String to hold time stats

	BOOL					m_bPaused;

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
	virtual void UpdateStats();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();

	HRESULT ShowSettingsDialog();

	Settings m_Settings;

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

