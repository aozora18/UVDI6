/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CMyFolder  - Folder Selection Dialog Class
	Copyright(C) Armen Hakobyan, 2002 - 2005
	http://www.codeproject.com/dialog/cMyFolder.asp

VERSION HISTORY:
	24 Mar 2002 - First release
	30 Mar 2003 - Some minor changes
				- Added missing in old Platform SDK new flag definitions  
				- Added support for both MFC 6.0 and 7.0
				- Added OnIUnknown handler for Windows XP folder filtration
				- Added SetExpanded and SetOKText and GetSelectedFolder functions
	24 May 2003 - Added OnSelChanged implementation
	14 Jul 2003 - Added custom filtration for Windows XP, thanks to Arik Poznanski
	29 Nov 2003 - Added SetRootFolder, thanks to Eckhard Schwabe ( and Jose Insa )
	02 Jan 2004 - Added GetRootFolder, uncomment if needed
	15 Feb 2005 - Small bug fix in DoModal, thanks to WindSeven
*/
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "MyFolder.h"

/////////////////////////////////////////////////////////////////////////////

#ifndef BFFM_VALIDATEFAILED
	#ifndef UNICODE
		#define BFFM_VALIDATEFAILED		3
	#else
		#define BFFM_VALIDATEFAILED		4	
	#endif
#endif

#ifndef BFFM_IUNKNOWN
	#define BFFM_IUNKNOWN				5
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyFolder

IMPLEMENT_DYNAMIC( CMyFolder, CDialog )

CMyFolder::CMyFolder( IN LPCTSTR pszTitle	 /*NULL*/,
							  IN LPCTSTR pszSelPath	 /*NULL*/,
							  IN CWnd*	 pWndParent	 /*NULL*/,
							  IN UINT	 uFlags		 /*BIF_RETURNONLYFSDIRS*/ )
			 : CCommonDialog( pWndParent )
			 , m_hWnd( NULL )
{
	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );
	::ZeroMemory( m_szFolPath, _MAX_PATH );
	::ZeroMemory( m_szSelPath, _MAX_PATH );
	
	// Fill data

	m_bi.hwndOwner	= pWndParent->GetSafeHwnd();
	m_bi.pidlRoot	= NULL;
	m_bi.lpszTitle	= pszTitle;
	m_bi.ulFlags	= uFlags;
	m_bi.lpfn		= (BFFCALLBACK)&BrowseCallbackProc;
	m_bi.lParam		= (LPARAM)this;

	// The size of this buffer is assumed to be _MAX_PATH bytes:

	m_bi.pszDisplayName = new TCHAR[ _MAX_PATH ];
	ASSERT( m_bi.pszDisplayName != NULL );

	SAFE_ZEROMEMORY( 
		m_bi.pszDisplayName, ( _MAX_PATH * sizeof( TCHAR ) )
	);

	if( pszSelPath )
		SetSelectedFolder( pszSelPath );
}

CMyFolder::~CMyFolder( VOID )
{	
	SAFE_COTASKMEMFREE( m_bi.pidlRoot );
	SAFE_DELETE2( m_bi.pszDisplayName );

	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );
}

BEGIN_MESSAGE_MAP( CMyFolder, CCommonDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyFolder message handlers

// SetRootFolder By Jose Insa
// Microsoft knowledge Base Article
// ID Q132750: Convert a File Path to an ITEMIDLIST

BOOL CMyFolder::SetRootFolder( IN LPCTSTR pszPath )
{
	ASSERT_VALID( this );

	if( !pszPath )
	{
		SAFE_COTASKMEMFREE( m_bi.pidlRoot );
		return TRUE;
	}

	ASSERT( AfxIsValidString( pszPath, _MAX_PATH ) );

	HRESULT		  hResult	  = S_FALSE;
	IShellFolder* pDeskFolder = NULL;

	hResult = ::SHGetDesktopFolder( &pDeskFolder );
	if ( hResult == S_OK )
	{
		LPITEMIDLIST pidlRoot = NULL;
		LPTSTR       pszRoot  = const_cast< LPTSTR >( pszPath );

		// Convert the path to an ITEMIDLIST:
				
		USES_CONVERSION;

		hResult = pDeskFolder->ParseDisplayName(
			NULL, NULL, T2W( pszRoot ), NULL, &pidlRoot, NULL 
		);

		if( hResult == S_OK )
		{
			SAFE_COTASKMEMFREE( m_bi.pidlRoot );
			m_bi.pidlRoot = pidlRoot;
		}

		SAFE_RELEASE( pDeskFolder );		
	}
	
	return ( hResult == S_OK );
} 

// NOTE: pszPath buffer must be at least
// _MAX_PATH characters in size:

BOOL CMyFolder::GetRootFolder( IN OUT LPTSTR pszPath )
{
	ASSERT_VALID( this );
	ASSERT( AfxIsValidString( pszPath, _MAX_PATH ) );

	return ::SHGetPathFromIDList( m_bi.pidlRoot, pszPath );
} 

LPTSTR CMyFolder::SetSelectedFolder( IN LPCTSTR pszPath )
{
	ASSERT( AfxIsValidString( pszPath, _MAX_PATH ) );
	return ::lstrcpy( m_szSelPath, pszPath );
}

/////////////////////////////////////////////////////////////////////////////

#if ( _MFC_VER >= 0x0700 )
	INT_PTR CMyFolder::DoModal( VOID )
#else
	INT		CMyFolder::DoModal( VOID )
#endif
{
	ASSERT_VALID( this );	
	ASSERT( m_bi.lpfn != NULL );
	
	INT_PTR nRet	= -1;
	m_bi.hwndOwner	= PreModal();
	m_bi.ulFlags	= BIF_NEWDIALOGSTYLE|BIF_RETURNONLYFSDIRS|BIF_DONTGOBELOWDOMAIN;
	m_bi.lpfn		= BrowseCallbackProc;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder( &m_bi );
	if( pItemIDList )
	{
		if( ::SHGetPathFromIDList( pItemIDList, m_szFolPath ) )
			nRet = IDOK;

		SAFE_COTASKMEMFREE( pItemIDList );
	}
	else
		nRet = IDCANCEL;


	PostModal();
	return ( nRet );
}

/////////////////////////////////////////////////////////////////////////////
// Overridables:

VOID CMyFolder::OnInitialized( VOID )
{
	if( ::lstrlen( m_szSelPath ) > 0 )
		SetSelection( m_szSelPath );
}

VOID CMyFolder::OnSelChanged( IN LPITEMIDLIST pItemIDList )
{
	if( m_bi.ulFlags & BIF_STATUSTEXT )
	{
		TCHAR szSelFol[ _MAX_PATH ] = { 0 };
		if( ::SHGetPathFromIDList( pItemIDList, szSelFol ) )
			SetStatusText( szSelFol );
	}
}

INT CMyFolder::OnValidateFailed( IN LPCTSTR /*pszPath*/ )
{	
	::MessageBeep( MB_ICONHAND );
	return 1; // Return 1 to leave dialog open, 0 - to end one
}

VOID CMyFolder::OnIUnknown( IN IUnknown* /*pIUnknown*/ )
{
}

/////////////////////////////////////////////////////////////////////////////
// Callback function used with the SHBrowseForFolder function. 

INT CALLBACK CMyFolder::BrowseCallbackProc( IN HWND   hWnd, 
												IN UINT   uMsg, 
												IN LPARAM lParam, 
												IN LPARAM lpData )
{
	CMyFolder* pThis = (CMyFolder*)lpData;
	ASSERT( pThis != NULL );

	INT nRet = 0;
	pThis->m_hWnd = hWnd;

	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		pThis->OnInitialized();
		break;
	case BFFM_SELCHANGED:
		pThis->OnSelChanged( (LPITEMIDLIST)lParam );
		break;
	case BFFM_VALIDATEFAILED:
		nRet = pThis->OnValidateFailed( (LPCTSTR)lParam );
		break;
	case BFFM_IUNKNOWN:
		pThis->OnIUnknown( (IUnknown*)lParam );
		break;
	default:
		ASSERT( FALSE );
		break;
	}

	pThis->m_hWnd = NULL;
	return nRet;	
}

/////////////////////////////////////////////////////////////////////////////
// Commands, valid to call only from handlers

VOID CMyFolder::SetExpanded( IN LPCTSTR pszPath )
{
	USES_CONVERSION;

	ASSERT( m_hWnd != NULL );
	ASSERT( AfxIsValidString( pszPath, _MAX_PATH ) );

	::SendMessage( 
		m_hWnd, BFFM_SETEXPANDED, 
		(WPARAM)TRUE, (LPARAM)T2CW( pszPath )
	);
}

VOID CMyFolder::SetOKText( IN LPCTSTR pszText )
{
	USES_CONVERSION;

	ASSERT( m_hWnd != NULL );
	
	::SendMessage( 
		m_hWnd, BFFM_SETOKTEXT, 
		(WPARAM)0, (LPARAM)T2CW( pszText )
	);
}

VOID CMyFolder::EnableOK( IN BOOL bEnable /*TRUE*/ )
{
	ASSERT( m_hWnd != NULL ); 

	::SendMessage( 
		m_hWnd, BFFM_ENABLEOK, (WPARAM)bEnable, 0L
	);
}

VOID CMyFolder::SetSelection( IN LPITEMIDLIST pItemIDList )
{
	ASSERT( m_hWnd != NULL );

	::SendMessage( 
		m_hWnd, BFFM_SETSELECTION, 
		(WPARAM)FALSE, (LPARAM)pItemIDList
	);
}

VOID CMyFolder::SetSelection( IN LPCTSTR pszPath )
{
	ASSERT( m_hWnd != NULL );
	ASSERT( AfxIsValidString( pszPath, _MAX_PATH ) );
	
	::SendMessage( 
		m_hWnd, BFFM_SETSELECTION, 
		(WPARAM)TRUE, (LPARAM)pszPath
	);
}

VOID CMyFolder::SetStatusText( IN LPCTSTR pszText )
{
	ASSERT( m_hWnd != NULL );

	::SendMessage( 
		m_hWnd, BFFM_SETSTATUSTEXT, 
		(WPARAM)0, (LPARAM)pszText
	);
}

// Shell version 5.0 or later:

VOID CMyFolder::SetExpanded( IN LPITEMIDLIST pItemIDList )
{
	ASSERT( m_hWnd != NULL ); 

	::SendMessage( 
		m_hWnd, BFFM_SETEXPANDED, 
		(WPARAM)FALSE, (LPARAM)pItemIDList
	);
}

/////////////////////////////////////////////////////////////////////////////