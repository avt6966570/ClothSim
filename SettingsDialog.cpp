//-----------------------------------------------------------------------------
// File: SettingsDialog.cpp
//
// Desc: Dialog box handler for simulation settings.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "SettingsDialog.h"
#include "resource.h"

SettingsDialog* s_pClothSettingsDialog = NULL;



BOOL SimpleFloatParse(LPCTSTR lpszText, double& d)
{
	while (*lpszText == ' ' || *lpszText == '\t')
	 lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
	 return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
	 lpszText++;

	if (*lpszText != '\0')
	 return FALSE;   // not terminated properly

	return TRUE;
}

BOOL SimpleIntParse(LPCTSTR lpszText, int& i)
{
	while (*lpszText == ' ' || *lpszText == '\t')
	 lpszText++;

	TCHAR chFirst = lpszText[0];
	i = _ttoi(lpszText);
	if (i == 0 && chFirst != '0')
	 return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
	 lpszText++;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: IntegratorTypeToString
// Desc: Returns the string for the given IntegratorType.
//-----------------------------------------------------------------------------
TCHAR* IntegratorTypeToString(IntegratorType integratorType)
{
    switch (integratorType)
    {
		case INT_EULER:        return TEXT("Euler");
		case INT_VERLET:       return TEXT("Verlet");
		case INT_RK:			return TEXT("4th order RK");
		default:               return TEXT("Unknown");
    }
}





//-----------------------------------------------------------------------------
// Name: DialogProcHelper
// Desc: 
//-----------------------------------------------------------------------------
INT_PTR CALLBACK SettingsDialogProcHelper( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return s_pClothSettingsDialog->DialogProc( hDlg, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: CD3DSettingsDialog constructor
// Desc: 
//-----------------------------------------------------------------------------
SettingsDialog::SettingsDialog(Settings* pSettings)
{
    s_pClothSettingsDialog = this;
    m_settings = *pSettings;
}




//-----------------------------------------------------------------------------
// Name: ComboBoxAdd
// Desc: Adds an entry to the combo box.
//-----------------------------------------------------------------------------
void SettingsDialog::ComboBoxAdd( int id, void* pData, TCHAR* pstrDesc )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    DWORD dwItem = ComboBox_AddString( hwndCtrl, pstrDesc );
    ComboBox_SetItemData( hwndCtrl, dwItem, pData );
}




//-----------------------------------------------------------------------------
// Name: ComboBoxSelect
// Desc: Selects an entry in the combo box.
//-----------------------------------------------------------------------------
void SettingsDialog::ComboBoxSelect( int id, void* pData )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    UINT count = ComboBoxCount( id );
    for( UINT iItem = 0; iItem < count; iItem++ )
    {
        if( (void*)ComboBox_GetItemData( hwndCtrl, iItem ) == pData )
        {
            ComboBox_SetCurSel( hwndCtrl, iItem );
            PostMessage( m_hDlg, WM_COMMAND, 
                MAKEWPARAM( id, CBN_SELCHANGE ), (LPARAM)hwndCtrl );
            return;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: ComboBoxSelectIndex
// Desc: Selects an entry in the combo box.
//-----------------------------------------------------------------------------
void SettingsDialog::ComboBoxSelectIndex( int id, int index )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    ComboBox_SetCurSel( hwndCtrl, index );
    PostMessage( m_hDlg, WM_COMMAND, MAKEWPARAM( id, CBN_SELCHANGE ), 
        (LPARAM)hwndCtrl );
}




//-----------------------------------------------------------------------------
// Name: ComboBoxSelected
// Desc: Returns the data for the selected entry in the combo box.
//-----------------------------------------------------------------------------
void* SettingsDialog::ComboBoxSelected( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    int index = ComboBox_GetCurSel( hwndCtrl );
    if( index < 0 )
        return NULL;
    return (void*)ComboBox_GetItemData( hwndCtrl, index );
}




//-----------------------------------------------------------------------------
// Name: ComboBoxSomethingSelected
// Desc: Returns whether any entry in the combo box is selected.  This is 
//       more useful than ComboBoxSelected() when you need to distinguish 
//       between having no item selected vs. having an item selected whose 
//       itemData is NULL.
//-----------------------------------------------------------------------------
bool SettingsDialog::ComboBoxSomethingSelected( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    int index = ComboBox_GetCurSel( hwndCtrl );
    return ( index >= 0 );
}




//-----------------------------------------------------------------------------
// Name: ComboBoxCount
// Desc: Returns the number of entries in the combo box.
//-----------------------------------------------------------------------------
UINT SettingsDialog::ComboBoxCount( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    return ComboBox_GetCount( hwndCtrl );
}




//-----------------------------------------------------------------------------
// Name: ComboBoxClear
// Desc: Clears the entries in the combo box.
//-----------------------------------------------------------------------------
void SettingsDialog::ComboBoxClear( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    ComboBox_ResetContent( hwndCtrl );
}




//-----------------------------------------------------------------------------
// Name: ComboBoxContainsText
// Desc: Returns whether the combo box contains the given text.
//-----------------------------------------------------------------------------
bool SettingsDialog::ComboBoxContainsText( int id, TCHAR* pstrText )
{
    TCHAR strItem[200];
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    UINT count = ComboBoxCount( id );
    for( UINT iItem = 0; iItem < count; iItem++ )
    {
        if( ComboBox_GetLBTextLen( hwndCtrl, iItem ) >= 200 )
            continue; // shouldn't happen, but don't overwrite buffer if it does
        ComboBox_GetLBText( hwndCtrl, iItem, strItem );
        if( lstrcmp( strItem, pstrText ) == 0 )
            return true;
    }
    return false;
}

// Populate edit fields
void SettingsDialog::SetEditBoxFloatValue(int id, float f)
{
	TCHAR strText[50];

	_sntprintf( strText, 50, TEXT("%f"), f );
	strText[49] = 0;
	SetDlgItemText(m_hDlg, id, strText);
}

void SettingsDialog::SetEditBoxIntValue(int id, int i)
{
	TCHAR strText[50];

	_sntprintf( strText, 50, TEXT("%d"), i );
	strText[49] = 0;
	SetDlgItemText(m_hDlg, id, strText);
}


void SettingsDialog::ValidateAndSet(int id, float &f, bool bZeroOK)
{
	double d;
	TCHAR strText[50];

	GetDlgItemText(m_hDlg, id, strText, sizeof(strText) / sizeof(TCHAR));

	if(!SimpleFloatParse(strText, d) || ((bZeroOK) ? (d < 0.0) : (d <= 0.0))) {
		MessageBeep(MB_ICONEXCLAMATION);
	} else {
		f = (SCALAR) d;
	}
	SetEditBoxFloatValue(id, f);
}

void SettingsDialog::ValidateAndSet(int id, int &i, int minValue, int maxValue)
{
	int tmp;
	TCHAR strText[50];
	GetDlgItemText(m_hDlg, id, strText, sizeof(strText) / sizeof(TCHAR));

	if(!SimpleIntParse(strText, tmp) || (tmp < minValue) || (tmp > maxValue)) {
		MessageBeep(MB_ICONEXCLAMATION);
	} else {
		i = tmp;
	}
	SetEditBoxIntValue(id, i);
}


void SettingsDialog::SetFromCheckbox(int id, bool &b)
{
	UINT isChecked = IsDlgButtonChecked(m_hDlg, id);
	b = (isChecked == BST_CHECKED);
}

//-----------------------------------------------------------------------------
// Name: ShowDialog
// Desc: Show the D3D settings dialog.
//-----------------------------------------------------------------------------
INT_PTR SettingsDialog::ShowDialog( HWND hwndParent )
{
    return DialogBox( NULL, MAKEINTRESOURCE( IDD_SETTINGS ), 
        hwndParent, SettingsDialogProcHelper );
}


void SettingsDialog::SetControls( Settings& settings )
{


    // List our known integrator types
	ComboBoxClear(IDC_COMBOINTEGRATOR);
	for(IntegratorType it = 0; it < INT_INVALIDLAST; it++) {
	    ComboBoxAdd( IDC_COMBOINTEGRATOR, (void *) it, IntegratorTypeToString(it) );
	}

    ComboBoxSelect( IDC_COMBOINTEGRATOR, (void *) settings.integrator );            
    if( !ComboBoxSomethingSelected( IDC_COMBOINTEGRATOR ) &&
        ComboBoxCount( IDC_COMBOINTEGRATOR ) > 0 )
    {
        ComboBoxSelectIndex( IDC_COMBOINTEGRATOR, 0 );
    }

	// Populate checkboxes
	CheckDlgButton(m_hDlg, IDC_CHECKTOPLEFT, (settings.bConstrainTopLeft ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(m_hDlg, IDC_CHECKTOPRIGHT, (settings.bConstrainTopRight ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(m_hDlg, IDC_CHECKBOTLEFT, (settings.bConstrainBotLeft ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(m_hDlg, IDC_CHECKBOTRIGHT, (settings.bConstrainBotRight ? BST_CHECKED : BST_UNCHECKED));

	CheckDlgButton(m_hDlg, IDC_CHECKSTRETCH, (settings.bStretch ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(m_hDlg, IDC_CHECKSHEAR, (settings.bShear ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(m_hDlg, IDC_CHECKBEND, (settings.bBend ? BST_CHECKED : BST_UNCHECKED));

	// Populate edit fields
	SetEditBoxIntValue(IDC_EDITCLOTHSIZE, settings.iGridSize);

	SetEditBoxFloatValue(IDC_EDITGRAVITY, settings.kGravityAccel);
	SetEditBoxFloatValue(IDC_EDITMASS, settings.kMassPerParticle);
	SetEditBoxFloatValue(IDC_EDITTIMESTEP, settings.timeStep);

	SetEditBoxFloatValue(IDC_EDITSTRETCHSPRING, settings.kStretchSpring);
	EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSTRETCHSPRING ), settings.bStretch );
	SetEditBoxFloatValue(IDC_EDITSTRETCHDAMP, settings.kStretchDamp);
	EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSTRETCHDAMP ), settings.bStretch );

	SetEditBoxFloatValue(IDC_EDITSHEARSPRING, settings.kShearSpring);
	EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSHEARSPRING ), settings.bShear );
	SetEditBoxFloatValue(IDC_EDITSHEARDAMP, settings.kShearDamp);
	EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSHEARDAMP ), settings.bShear );

	SetEditBoxFloatValue(IDC_EDITBENDSPRING, settings.kBendSpring);
	EnableWindow( GetDlgItem( m_hDlg, IDC_EDITBENDSPRING ), settings.bBend );
	SetEditBoxFloatValue(IDC_EDITBENDDAMP, settings.kBendDamp);
	EnableWindow( GetDlgItem( m_hDlg, IDC_EDITBENDDAMP ), settings.bBend );

}


//-----------------------------------------------------------------------------
// Name: DialogProc
// Desc: Handle window messages in the dialog.
//-----------------------------------------------------------------------------
INT_PTR SettingsDialog::DialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER( lParam );

    switch( msg )
    {
    case WM_INITDIALOG:
        {
		    m_hDlg = hDlg;
			SetControls(m_settings);
        }
        return TRUE;

    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case IDOK:
            EndDialog( hDlg, IDOK );
            break;
        case IDCANCEL:
            EndDialog( hDlg, IDCANCEL );
            break;
		case IDRESTOREDEFAULTS:
			{
				Settings defSettings;
				m_settings = defSettings;
				SetControls(m_settings);
			}
			break;

        case IDC_COMBOINTEGRATOR:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                IntegratorTypeChanged();
            break;

		case IDC_CHECKTOPLEFT:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKTOPLEFT, m_settings.bConstrainTopLeft);
			}
			break;

		case IDC_CHECKTOPRIGHT:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKTOPRIGHT, m_settings.bConstrainTopRight);
			}
			break;

		case IDC_CHECKBOTLEFT:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKTOPLEFT, m_settings.bConstrainBotLeft);
			}
			break;

		case IDC_CHECKBOTRIGHT:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKBOTRIGHT, m_settings.bConstrainBotRight);
			}
			break;


		case IDC_CHECKSTRETCH:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKSTRETCH, m_settings.bStretch);
				EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSTRETCHSPRING ), m_settings.bStretch );
				EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSTRETCHDAMP ), m_settings.bStretch );
			}
			break;

		case IDC_CHECKSHEAR:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKSHEAR, m_settings.bShear);
				EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSHEARSPRING ), m_settings.bShear );
				EnableWindow( GetDlgItem( m_hDlg, IDC_EDITSHEARDAMP ), m_settings.bShear );
			}
			break;

		case IDC_CHECKBEND:
			if( BN_CLICKED == HIWORD(wParam) ) {
				SetFromCheckbox(IDC_CHECKBEND, m_settings.bBend);
				EnableWindow( GetDlgItem( m_hDlg, IDC_EDITBENDSPRING ), m_settings.bBend );
				EnableWindow( GetDlgItem( m_hDlg, IDC_EDITBENDDAMP ), m_settings.bBend );
			}
			break;

		case IDC_EDITCLOTHSIZE:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITCLOTHSIZE, m_settings.iGridSize, 2, 256);
			}
			break;

		case IDC_EDITGRAVITY:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITGRAVITY, m_settings.kGravityAccel, true);
			}
			break;

		case IDC_EDITTIMESTEP:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITTIMESTEP, m_settings.timeStep, false);
			}
			break;

		case IDC_EDITMASS:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITMASS, m_settings.kMassPerParticle, false);
			}
			break;
			
		case IDC_EDITSTRETCHSPRING:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITSTRETCHSPRING, m_settings.kStretchSpring, false);
			}
			break;

		case IDC_EDITSTRETCHDAMP:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITSTRETCHDAMP, m_settings.kStretchDamp, true);
			}
			break;

		case IDC_EDITSHEARSPRING:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITSHEARSPRING, m_settings.kShearSpring, false);
			}
			break;

		case IDC_EDITSHEARDAMP:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITSHEARDAMP, m_settings.kShearDamp, true);
			}
			break;

		case IDC_EDITBENDSPRING:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITBENDSPRING, m_settings.kBendSpring, false);
			}
			break;

		case IDC_EDITBENDDAMP:
			if( EN_KILLFOCUS == HIWORD(wParam) ) {
				ValidateAndSet(IDC_EDITBENDDAMP, m_settings.kBendDamp, true);
			}
			break;

        }
        return TRUE;

    default:
        return FALSE;
    }
}



//-----------------------------------------------------------------------------
// Name: IntegratorTypeChanged
// Desc: Respond to a change of integrator.
//-----------------------------------------------------------------------------
void SettingsDialog::IntegratorTypeChanged( void )
{
    IntegratorType it = (IntegratorType) (ComboBoxSelected( IDC_COMBOINTEGRATOR )) ;
    m_settings.integrator = it;
}

