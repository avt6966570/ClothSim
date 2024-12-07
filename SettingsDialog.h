//-----------------------------------------------------------------------------
// Name: class SettingsDialog
// Desc: Dialog box to allow the user to change the cloth simulation settings
//-----------------------------------------------------------------------------

#ifndef _SETTINGSDIALOG_H_
#define _SETTINGSDIALOG_H_

#include "Settings.h"

class SettingsDialog
{
private:
    HWND m_hDlg;
    Settings m_settings;

private:
    // ComboBox helper functions
    void ComboBoxAdd( int id, void* pData, TCHAR* pstrDesc );
    void ComboBoxSelect( int id, void* pData );
    void* ComboBoxSelected( int id );
    bool ComboBoxSomethingSelected( int id );
    UINT ComboBoxCount( int id );
    void ComboBoxSelectIndex( int id, int index );
    void ComboBoxClear( int id );
    bool ComboBoxContainsText( int id, TCHAR* pstrText );

    void IntegratorTypeChanged( void );

	void SetControls( Settings& settings );
	void SetEditBoxFloatValue(int id, float f);
	void SetEditBoxIntValue(int id, int i);
	void ValidateAndSet(int id, float &f, bool bZeroOK);
	void ValidateAndSet(int id, int &i, int minValue, int maxValue);
	void SetFromCheckbox(int id, bool &b);

public:
    SettingsDialog( Settings* pSettings);
    INT_PTR ShowDialog( HWND hwndParent );
    INT_PTR DialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
    void GetFinalSettings( Settings* pSettings ) { *pSettings = m_settings; }
};

#endif // #ifndef _SETTINGSDIALOG_H_

