/*******************************************************************************
 H_EXEC for WINBINDER - The native Windows binding for PHP
 Copyright © Hypervisual
 Author: Rubem Pechansky
*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES
#include <windows.h>
#include <shellapi.h>
#include <winreg.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------- CONSTANTS
#define SE_ERROR		32		// See ShellExecute() doc
#define MAXSTR 			1024
#define INIPREFIX		"ini"

//----------------------------------------------------------------------- MACROS
#define INCRMAX(v,mx)	((v) = (v) < (mx) ? (v)+1 : (mx))	// Increment to a ceiling
#define DECRMIN(v,mn)	((v) = (v) > (mn) ? (v)-1 : (mn))	// Decrement to a floor
#define WHITESPACES		" \t\n\r\f\v"

//----------------------------------------------------------- PRIVATE PROTOTYPES
static char *GetTokenExt(const char *pszBuffer, int nToken, const char *pszSep, char chGroup, BOOL fBlock, char *pszToken, int nTokLen);
static char *GetToken(const char *pszBuffer, int nToken, char *pszToken, int nTokLen);
static BOOL ExistFile(const char *szFileName);

//-------------------------------------------------------------------- FUNCTIONS

/**
 * Main entry. Accept command line to program, reg key to read or nothing to run ini values
 */
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int bRet = (SE_ERROR + 1);
	char szRun[MAXSTR] = "";		// Command to run
	char szParam[MAXSTR] = "";		// Parameters
	char szIniFile[MAX_PATH];

    // Is there a command line?
    // No command line: must read from the INI file
	if(!lpCmdLine || !*lpCmdLine) {

		// Build the name of the INI file
		BOOL bSearchPath;
		GetModuleFileName(NULL, szIniFile, MAX_PATH);
		if(strrchr(szIniFile, '.'))
			*(strrchr(szIniFile, '.') + 1) = '\0';
		strcat(szIniFile, INIPREFIX);

		// Read data from the INI file
		GetPrivateProfileString("h_exec", "run", "", szRun, MAXSTR, szIniFile);
		GetPrivateProfileString("h_exec", "param", "", szParam, MAXSTR, szIniFile);
		bSearchPath = GetPrivateProfileInt("h_exec", "searchpath", FALSE, szIniFile);

		// Must prepend this path to the application name?
		if(bSearchPath) {

			char szPath[MAX_PATH];
			GetModuleFileName(NULL, szPath, MAX_PATH);
			if(strrchr(szPath, '\\'))
				*(strrchr(szPath, '\\') + 1) = '\0';
			strcat(szPath, szRun);

			bRet = ShellExecute(GetDesktopWindow(), NULL, szPath, szParam, NULL, SW_SHOWNORMAL);

		} else {
			bRet = ShellExecute(GetDesktopWindow(), NULL, szRun, szParam, NULL, SW_SHOWNORMAL);
		}

	} else {

		// Is it a registry key?
		if(!strncmp(lpCmdLine, "reg:", 4)) {
			HKEY hKey;
			DWORD dwSize = MAXSTR;
			char szKey[MAXSTR];
			char szEntry[MAXSTR];
			char szPgm[MAXSTR];
			char szAlt[MAXSTR];
			char szExec[MAXSTR];

			GetToken(lpCmdLine, 1, szKey, MAXSTR);
			GetToken(lpCmdLine, 2, szEntry, MAXSTR);
			GetToken(lpCmdLine, 3, szPgm, MAXSTR);
			GetToken(lpCmdLine, 4, szAlt, MAXSTR);

			// Read registry data
		    if(RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(szKey), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				RegQueryValueEx(hKey, TEXT(szEntry), NULL, NULL, (LPBYTE)szExec, &dwSize);
		    	RegCloseKey(hKey);
			}

			strcat(szExec, szPgm);
			if(ExistFile(szExec))
				bRet = ShellExecute(GetDesktopWindow(), "open", szExec, NULL, NULL, SW_SHOWNORMAL);
			else
				bRet = ShellExecute(GetDesktopWindow(), "open", szAlt, NULL, NULL, SW_SHOWNORMAL);
		} else

			// Must be a command line, then.
			bRet = ShellExecute(GetDesktopWindow(), "open", lpCmdLine, NULL, NULL, SW_SHOWNORMAL);
	}

	if(bRet <= SE_ERROR) {
		char sz[MAXSTR];
		sprintf(sz, "Error # %d", bRet);
		MessageBox(NULL, sz, "h_exec", MB_ICONEXCLAMATION);
	}
	return bRet;
}

//------------------------------------------------------------ PRIVATE FUNCTIONS

/**
 * Given a pszBuffer string, returns the nToken'th token in pszToken. Tokens are
 * separated with any character from pszSep. chGroup is generally a quote or double-
 * quote used to bypass pszSep
 */
static char *GetTokenExt(const char *pszBuffer, int nToken, const char *pszSep, char chGroup, BOOL fBlock, char *pszToken, int nTokLen)
{
	int nBufPtr, nSepPtr, nTokPtr, nToks;

	// For each character in the buffer
	for(nBufPtr = 0, nTokPtr = 0, nToks = 0; *(pszBuffer + nBufPtr); nBufPtr++) {

		if(chGroup && *(pszBuffer + nBufPtr) == chGroup) {

			// Start of group: waituntil it ends
			nBufPtr++;

			if(nToks == nToken) {
				for(; *(pszBuffer + nBufPtr) && *(pszBuffer + nBufPtr) != chGroup; nBufPtr++) {

					// Build token
					*(pszToken + nTokPtr) = *(pszBuffer + nBufPtr);
					INCRMAX(nTokPtr, nTokLen - 1);
				}
				break;
			} else
				for(; *(pszBuffer + nBufPtr) && *(pszBuffer + nBufPtr) != chGroup; nBufPtr++)
					;
		}

		// Test separator characters
		for(nSepPtr = 0; *(pszSep + nSepPtr); nSepPtr++) {
			if(*(pszBuffer + nBufPtr) == *(pszSep + nSepPtr)) {

				// Found a separator
				if(fBlock)

					// If fBlock is TRUE, skip next separators
					for(nSepPtr = 0; *(pszSep + nSepPtr); nSepPtr++)
						while(*(pszBuffer + nBufPtr + 1) == *(pszSep + nSepPtr))
							nBufPtr++;

				if(nToks++ > nToken) {

					// Token is finished, may end loop
					*(pszToken + nTokPtr) = '\0';
					return pszToken;
				}
				goto HHH;	// The good ol' goto still has its uses
			}
		}

		// Build token
		if(nToks == nToken) {
			*(pszToken + nTokPtr) = *(pszBuffer + nBufPtr);
			INCRMAX(nTokPtr, nTokLen - 1);
		}
HHH:
		;

	} // ~for

	// Reached the end of the loop
	*(pszToken + nTokPtr) = '\0';
	return pszToken;
}

/**
 * Return a whitepsace-separated token. Tokens may be grouped with double-quotes
 */
static char *GetToken(const char *pszBuffer, int nToken, char *pszToken, int nTokLen)
{
	return GetTokenExt(pszBuffer, nToken, WHITESPACES, '\"', TRUE, pszToken, nTokLen);
}

/**
 * File exists check
 */
static BOOL ExistFile(const char *szFileName)
{
	return access(szFileName, 0) != ((HFILE)-1);
}

//-------------------------------------------------------------------------- END
