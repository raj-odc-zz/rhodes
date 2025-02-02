#include "../../../shared/SystemImplBase.h"

#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include "common/app_build_capabilities.h"
#include "common/RhodesApp.h"
#include "common/StringConverter.h"
#include "common/RhoFilePath.h"
#include "common/RhoFile.h"
#include "common/RhoDefs.h"
#if defined( OS_WINCE )
#include "EmdkDefines.h"
#include "Keyboard.h"
#endif
#include <algorithm>
#include "Registry.h"

#if defined( OS_WINCE ) && !defined( OS_PLATFORM_MOTCE )
#include <cfgmgrapi.h>
#include <getdeviceuniqueid.h>
#endif

//  Defines for Unique Device ID (not present in MC3000c50b)
#ifndef _GETDEVICEUNIQUEID_INC
#define _GETDEVICEUNIQUEID_INC
#define GETDEVICEUNIQUEID_V1                1
#define GETDEVICEUNIQUEID_V1_MIN_APPDATA    8
#define GETDEVICEUNIQUEID_V1_OUTPUT         20
HRESULT
GetDeviceUniqueID(LPBYTE pbApplicationData, 
                  DWORD cbApplicationData, 
                  DWORD dwDeviceIDVersion,
                  LPBYTE pbDeviceIDOutput, 
                  DWORD *pcbDeviceIDOutput);
#endif

#undef DEFAULT_LOGCATEGORY
#define DEFAULT_LOGCATEGORY "System"


extern "C"
{
void rho_sys_bring_to_front();
void rho_wmsys_run_appW(const wchar_t* szPath, const wchar_t* szParams );
int rho_sys_get_screen_width();
int rho_sys_get_screen_height();
const char* rho_sys_win32_getWebviewFramework();

#if !defined( OS_WINDOWS_DESKTOP )
typedef DWORD (*RCM_GETUNIQUEUNITIDEX)(LPUNITID_EX);
RCM_GETUNIQUEUNITIDEX lpfnRCM_GetUniqueUnitIdEx = NULL; ///< pointer to the RCM get uuid function
#endif

#if defined(OS_WINDOWS_DESKTOP)
void rho_sys_set_window_frame(int x0, int y0, int width, int height);
void rho_sys_set_window_position(int x0, int y0);
void rho_sys_set_window_size(int width, int height);
void rho_sys_lock_window_size(int locked);
#endif
}

namespace rho {

using namespace apiGenerator;
using namespace common;

class CSystemImpl: public CSystemImplBase
{
private:
#if defined( OS_WINCE )
	CSIP* m_pSip;
#endif
    bool m_bLockedWindowSize;
public:
    CSystemImpl(): CSystemImplBase()
	{
#if defined( OS_WINCE )
	    m_pSip = NULL;
#endif
        m_bLockedWindowSize = false;
	}

    virtual void getScreenWidth(CMethodResult& oResult);
    virtual void getScreenHeight(CMethodResult& oResult);
    virtual void getScreenOrientation(CMethodResult& oResult);
    virtual void getPpiX(CMethodResult& oResult);
    virtual void getPpiY(CMethodResult& oResult);
    virtual void getPhoneId(CMethodResult& oResult);
    virtual void getDeviceName(CMethodResult& oResult);
    virtual void getOsVersion(CMethodResult& oResult);
    virtual void getIsMotorolaDevice(CMethodResult& oResult);
    virtual void getLocale(CMethodResult& oResult);
    virtual void getCountry(CMethodResult& oResult);
    virtual void getIsEmulator(CMethodResult& oResult);
    virtual void getHasCalendar(CMethodResult& oResult);
    virtual void getHasCamera(CMethodResult& oResult);
    virtual void getOemInfo(CMethodResult& oResult);
    virtual void getUuid(CMethodResult& oResult);
#if defined( OS_WINCE )
	bool populateUUID(UNITID_EX* uuid);
	void bytesToHexStr(LPTSTR lpHexStr, LPBYTE lpBytes, int nSize);
#endif
    virtual void getHttpProxyURI(CMethodResult& oResult);
    virtual void setHttpProxyURI( const rho::String& value, CMethodResult& oResult);
    virtual void getLockWindowSize(CMethodResult& oResult);
    virtual void setLockWindowSize( bool value, CMethodResult& oResult);
    virtual void getKeyboardState(CMethodResult& oResult);
	virtual void setKeyboardState( const rho::String &, CMethodResult& oResult);
    virtual void getScreenAutoRotate(CMethodResult& oResult);
    virtual void setScreenAutoRotate( bool value, CMethodResult& oResult);
    virtual void getHasTouchscreen(rho::apiGenerator::CMethodResult& oResult);
    virtual void getScreenSleeping(rho::apiGenerator::CMethodResult& oResult);
    virtual void setScreenSleeping( bool value, rho::apiGenerator::CMethodResult& oResult);

    virtual void applicationInstall( const rho::String& applicationUrl, CMethodResult& oResult);
    virtual void isApplicationInstalled( const rho::String& applicationName, CMethodResult& oResult);
    virtual void applicationUninstall( const rho::String& applicationName, CMethodResult& oResult);
    virtual void openUrl( const rho::String& url, CMethodResult& oResult);
    virtual void setRegistrySetting( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
    virtual void getRegistrySetting( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
    virtual void deleteRegistrySetting(const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult);
	virtual void setWindowFrame( int x,  int y,  int width,  int height, CMethodResult& oResult);
    virtual void setWindowPosition( int x,  int y, CMethodResult& oResult);
    virtual void setWindowSize( int width,  int height, rho::apiGenerator::CMethodResult& oResult);
    virtual void getWebviewFramework(rho::apiGenerator::CMethodResult& oResult);
    virtual void bringToFront(rho::apiGenerator::CMethodResult& oResult);
    virtual void runApplication( const rho::String& appName,  const rho::String& params,  bool blockingCall, rho::apiGenerator::CMethodResult& oResult);

    virtual void set_http_proxy_url( const rho::String& proxyURI, rho::apiGenerator::CMethodResult& oResult);
    virtual void unset_http_proxy(rho::apiGenerator::CMethodResult& oResult);
	virtual long OnSIPState(bool bSIPState, const CRhoExtData& oExtData)
	{
#if defined( OS_WINCE )
		if (!m_pSip)
			m_pSip = new CSIP();
		m_pSip->ToggleSIPReliably(bSIPState);
#endif
		return S_OK;
	}
};

void CSystemImpl::getOsVersion(CMethodResult& oResult)
{
#ifdef RHODES_EMULATOR
    CSystemImplBase::getOsVersion(oResult);
#else
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	if (!GetVersionEx(&osv))
    {
        LOG(ERROR) + "GetVersionEx failed.";
		oResult.set("0");
    }
    else
    {
	    wchar_t buf[50];
        memset(buf, 0, sizeof(buf));
	    _snwprintf(buf, sizeof(buf) - 1, L"%u.%u.%u", (unsigned)osv.dwMajorVersion,
		    (unsigned)osv.dwMinorVersion, (unsigned)osv.dwBuildNumber);

        oResult.set( StringW(buf));
    }
#endif
}

void CSystemImpl::getIsEmulator(CMethodResult& oResult)
{
#ifdef RHODES_EMULATOR
    oResult.set(true);
#else

#ifdef OS_WINDOWS_DESKTOP
    oResult.set(false);
#else

	TCHAR buf[255];
	SystemParametersInfo(SPI_GETOEMINFO, 255, &buf, NULL);
	bool bEmulator = (wcsnicmp(buf, _T("Microsoft DeviceEmulator"), 24)==0);

    oResult.set(bEmulator);
#endif

#endif
}

void CSystemImpl::getScreenWidth(CMethodResult& oResult)
{
    oResult.set(rho_sys_get_screen_width());
}

void CSystemImpl::getScreenHeight(CMethodResult& oResult)
{
    oResult.set(rho_sys_get_screen_height());
}

void CSystemImpl::getScreenOrientation(CMethodResult& oResult)
{
    if (rho_sys_get_screen_width() <= rho_sys_get_screen_height()) 
	    oResult.set(SCREEN_PORTRAIT);
    else
        oResult.set(SCREEN_LANDSCAPE);
}

void CSystemImpl::getPpiX(CMethodResult& oResult)
{
	HWND hWndDesktop = GetDesktopWindow();
	HDC hdcDesktop = GetDC(hWndDesktop);
	int mms = GetDeviceCaps(hdcDesktop, HORZSIZE);
	int pixels = GetDeviceCaps(hdcDesktop, HORZRES);
	double ret = (pixels*25.4)/mms;

    oResult.set(ret);
}

void CSystemImpl::getPpiY(CMethodResult& oResult)
{
	HWND hWndDesktop = GetDesktopWindow();
	HDC hdcDesktop = GetDC(hWndDesktop);
	int mms = GetDeviceCaps(hdcDesktop, VERTSIZE);
	int pixels = GetDeviceCaps(hdcDesktop, VERTRES);
	double ret = (pixels*25.4)/mms;

    oResult.set(ret);
}

static void _toHexString(int i, String& strRes, int radix)
{
    char buf[33];
	bool neg= false;
	int f, n;
	if(i<0) { neg= true; i= -i; };
	f= 32;
	buf[f--]= 0;
	do
	{
		n= i%radix;
		if(n<10) buf[f]= '0'+n;
		else buf[f]= 'a'+n-10;
		i= i/radix;
		f--;
	}
	while(i>0);

	if(neg) 
        buf[f--]= '-';

    strRes += (buf+f+1);
}

void CSystemImpl::getPhoneId(CMethodResult& oResult)
{
    String strDeviceID;

#if defined( OS_WINCE ) && !defined( OS_PLATFORM_MOTCE )
    String strAppData = "RHODES_" + RHODESAPP().getAppName() + "_DEVICEID";

    BYTE rgDeviceId[20];
    DWORD cbDeviceId = sizeof(rgDeviceId);
    HRESULT hr = GetDeviceUniqueID( (PBYTE)(strAppData.c_str()),
       strAppData.length(),
       GETDEVICEUNIQUEID_V1,
       rgDeviceId,
       &cbDeviceId);

    if ( SUCCEEDED(hr) )
    {
        for(unsigned int i = 0; i < cbDeviceId; i++)
        {
            _toHexString( rgDeviceId[i], strDeviceID, 16);
        }
    }
#endif

    oResult.set( strDeviceID );
}

void CSystemImpl::getDeviceName(CMethodResult& oResult)
{
    String strRes;
#ifdef OS_WINDOWS_DESKTOP
	strRes = "Win32";
#else
	HKEY hKey;
	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Ident"), 0, KEY_READ, &hKey ) == ERROR_SUCCESS)
    {
        DWORD dwType = REG_SZ;
        DWORD dwDataSize = 0;
        if ( RegQueryValueEx( hKey, _T("name"), 0, &dwType, (PBYTE)NULL, &dwDataSize ) == ERROR_SUCCESS)
        {
            CAtlString deviceName;
            RegQueryValueEx( hKey, _T("name"), 0, &dwType, 
                (PBYTE)deviceName.GetBuffer((dwDataSize/sizeof(TCHAR) + sizeof(TCHAR))), &dwDataSize );
            deviceName.ReleaseBuffer();

            strRes = convertToStringA(deviceName.GetString());
        }
    }
	RegCloseKey(hKey);
#endif
    oResult.set(strRes);
}

void CSystemImpl::getLocale(CMethodResult& oResult)
{
	wchar_t szLang[20];
	int nRes = GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SABBREVLANGNAME , szLang, sizeof(szLang)/sizeof(szLang[0]));
	szLang[2] = 0;
	wcslwr(szLang);

	oResult.set(szLang);
}

void CSystemImpl::getCountry(CMethodResult& oResult)
{
    wchar_t szCountry[20];
    int nRes = GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SISO3166CTRYNAME , szCountry, sizeof(szCountry)/sizeof(szCountry[0]));
    szCountry[2] = 0;

  	oResult.set(szCountry);
}

void CSystemImpl::getHasCalendar(CMethodResult& oResult)
{
#if defined( OS_PLATFORM_MOTCE )
    oResult.set(false);
#else
    oResult.set(true);
#endif
}

void CSystemImpl::getIsMotorolaDevice(CMethodResult& oResult)
{
#if defined( APP_BUILD_CAPABILITY_MOTOROLA ) && defined( OS_WINCE )
    //get the system OEM string
    TCHAR szPlatform[MAX_PATH+1];
    memset(szPlatform, 0, MAX_PATH*sizeof(TCHAR));
    SystemParametersInfo(SPI_GETOEMINFO, MAX_PATH, szPlatform, 0);
    _wcslwr(szPlatform);
    if(wcsstr(szPlatform, L"symbol") || wcsstr(szPlatform, L"motorola"))
        oResult.set(true);
    else
        oResult.set(false);
#else
    oResult.set(false);
#endif
}

void CSystemImpl::getOemInfo(CMethodResult& oResult)
{
#if defined( OS_WINCE )
	WCHAR info [64];
	if (SystemParametersInfo (SPI_GETOEMINFO, 64, info, 0))
		oResult.set(info);
	else
		oResult.set(L"unknown");
#else
    oResult.set(L"unknown");
#endif
}


void CSystemImpl::getUuid(CMethodResult& oResult)
{
#if defined( OS_WINCE )
	UNITID_EX uuid;
    memset(&uuid, 0, sizeof uuid);
    uuid.StructInfo.dwAllocated = sizeof uuid;
	if (populateUUID(&uuid))
	{
		//  RCM Library successfully loaded and uuid populated
		WCHAR tcUUID[33];
		memset(tcUUID, 0, 33 * sizeof(WCHAR));
	    if (uuid.StructInfo.dwUsed)
		{
		    bytesToHexStr(tcUUID, uuid.byUUID, uuid.StructInfo.dwUsed - sizeof(STRUCT_INFO)); 
			if(tcUUID[16] == 0)
			{
				WCHAR tc1[17], tc2[17];
				wcsncpy(tc1, tcUUID, 8);
				wcsncpy(&tc1[8], L"00000000", 8);
				wcsncpy(tc2, &tcUUID[8], 8);
				wcscpy(&tc2[8], L"00000000");
				wcsncpy(tcUUID, tc1, 16);
				tcUUID[16] = L'\0';
				wcsncat(tcUUID, tc2, 16);
			}
			oResult.set(tcUUID);
		}
	}
	else
	{
		//  RCM Library could not be loaded and all other attempts to ascertain the uuid failed
		LOG(WARNING) + "UUID could not be obtained for this device";
		oResult.set(L"GetSerialNumber: not supported");
	}
#endif
}

#if defined( OS_WINCE )

bool CSystemImpl::populateUUID(UNITID_EX* uuid)
{
	//  For Motorola devices the UUID is obtained from the RCM library.  For non-motorola devices the UUID is 
	//  obtained from a system call.
	bool bRetVal = false;
	HMODULE hLib = LoadLibrary(TEXT("Rcm2API32.dll"));
	if (hLib != NULL)
	{
		lpfnRCM_GetUniqueUnitIdEx = (RCM_GETUNIQUEUNITIDEX)GetProcAddress(hLib,L"RCM_GetUniqueUnitIdEx");
		uuid->StructInfo.dwAllocated = sizeof uuid;
		DWORD dwRes = (*lpfnRCM_GetUniqueUnitIdEx) (uuid);
		if( dwRes == E_RCM_NOTSUPPORTED )
		{
			FreeLibrary (hLib);
			hLib = LoadLibrary(TEXT("RcmAPI32.dll"));
			if (hLib != NULL)
			{
				lpfnRCM_GetUniqueUnitIdEx = (RCM_GETUNIQUEUNITIDEX)GetProcAddress(hLib,L"RCM_GetUniqueUnitIdEx");
				if(	lpfnRCM_GetUniqueUnitIdEx )
				{
					DWORD dwRes = (*lpfnRCM_GetUniqueUnitIdEx) (uuid);
					if (dwRes == E_RCM_SUCCESS)
						bRetVal = true;
				}
			}
		}
		else if (dwRes == E_RCM_SUCCESS)
			bRetVal = true;
	}
	else
	{
		//  Unable to load Rcm2API32.dll, it is possible we are running on a Non Motorola 
		//  WM / CE device, use the Windows function for retrieving UUID:
		//  Load CoreDLL
		typedef HRESULT (WINAPI* GET_DEVICE_ID_T)(LPBYTE pbApplicationData, 
												  DWORD cbApplicationData, 
												  DWORD dwDeviceIDVersion,
												  LPBYTE pbDeviceIDOutput, 
												  DWORD *pcbDeviceIDOutput);
		GET_DEVICE_ID_T lpfn_device_id = NULL; 
		hLib = LoadLibrary(TEXT("CoreDLL.dll"));
		if (hLib)
		{
			lpfn_device_id = (GET_DEVICE_ID_T)GetProcAddress(hLib, L"GetDeviceUniqueID");
			if (lpfn_device_id != NULL)
			{
				BYTE            rgDeviceId[GETDEVICEUNIQUEID_V1_OUTPUT];
				DWORD           cbDeviceId      = sizeof(rgDeviceId);
				HRESULT hr = lpfn_device_id(reinterpret_cast<PBYTE>("RhoElements"), 
					wcslen(L"RhoElements"), GETDEVICEUNIQUEID_V1, 
					rgDeviceId,
					&cbDeviceId);
				
				if (hr == S_OK)
				{
					//  Got UUID, populate m_UUID
					uuid->StructInfo.dwUsed = 24;	//  24 is used by EMDK device ID, even though 
													//  the Windows ID has only 20 digits.  Rest are 0's
					for (int i = 0; i < GETDEVICEUNIQUEID_V1_OUTPUT; i++)
					{
						uuid->byUUID[i] = rgDeviceId[i];
					}
					bRetVal = true;
				}
			}
			//  Failed to load CoreDLL
		}
	}
	if (hLib)
		FreeLibrary (hLib);

	return bRetVal;
}

void CSystemImpl::bytesToHexStr(LPTSTR lpHexStr, LPBYTE lpBytes, int nSize)
{
	//this function is lifted from the EMDK and converts the UUID into a sensible HEX string
	int		i;
	TCHAR	szByteStr[5];
	lpHexStr[0] = 0;
	for (i=0; i<nSize && (i < sizeof(UNITID_EX) - sizeof(STRUCT_INFO)); i++)
	{
		wsprintf(szByteStr, TEXT("%02X"), lpBytes[i]);
		_tcscat(lpHexStr, szByteStr);
	}
}
#endif

void CSystemImpl::getLockWindowSize(CMethodResult& oResult)
{
    oResult.set(m_bLockedWindowSize);
}

void CSystemImpl::setLockWindowSize( bool value, CMethodResult& oResult)
{
#if defined(OS_WINDOWS_DESKTOP)
    m_bLockedWindowSize = value;
    rho_sys_lock_window_size(value?1:0);
#endif
}

void CSystemImpl::getKeyboardState(CMethodResult& oResult)
{
    oResult.set("automatic");

#if defined( OS_WINCE )
	if (!m_pSip)
		m_pSip = new CSIP();
	if (m_pSip->getCurrentStatus() == SIP_CONTROL_AUTOMATIC)
		oResult.set("automatic");
	else
	{
		bool bSIPVisible = false;
		HWND sipHWND = FindWindow(L"SipWndClass", NULL);
		if (sipHWND && IsWindowVisible(sipHWND))
			bSIPVisible = true;
		if (bSIPVisible)
			oResult.set("shown");
		else
			oResult.set("hidden");
	}
#endif
}

void CSystemImpl::setKeyboardState( const rho::String & value, CMethodResult& oResult)
{
#if defined( OS_WINCE )
	if (!m_pSip)
		m_pSip = new CSIP();
	
	//  User has instructed the keyboard to show / hide, now assume the control is manual
	bool bUserRequestToShow = false;
	bool bSuccess = true;
	if (value.compare("shown") == 0)
	{
		//  Show the keyboard
		bUserRequestToShow = true;
		m_pSip->setCurrentStatus(SIP_CONTROL_MANUAL);
	}
	else if (value.compare("hidden") == 0)
	{
		//  Hide the keyboard
		bUserRequestToShow = false;
		m_pSip->setCurrentStatus(SIP_CONTROL_MANUAL);
	}
	else if (value.compare("automatic") == 0)
	{
		//  Respond to WebKit requests to show / hide the SIP automatically
		m_pSip->setCurrentStatus(SIP_CONTROL_AUTOMATIC);
	}
	else
	{
		bSuccess = false;
		LOG(WARNING) + "Unrecognised keyboard state: " + value;
	}

	if (m_pSip->getCurrentStatus() == SIP_CONTROL_MANUAL)
	{
		m_pSip->ToggleSIPReliably(bUserRequestToShow);
	}
	oResult.set(bSuccess);
#endif
}

void CSystemImpl::getScreenAutoRotate(CMethodResult& oResult)
{
    oResult.set(true);
}

void CSystemImpl::setScreenAutoRotate( bool value, CMethodResult& oResult)
{
    //TODO:setScreenAutoRotate
    //HKLM\System\GDI\Rotation\
    //HKCU\Software\HTC\HTCSensor\Gsensor:AutoRotation = 0 
}

extern "C" int rho_wmsys_has_touchscreen()
{
#if defined( OS_WINDOWS_DESKTOP ) || defined( OS_PLATFORM_MOTCE )
        return 1;
#else
        BOOL bRet;
        TCHAR oem[257];
#ifndef WIN32_PLATFORM_WFSP
        // fix for all supported platforms expect Smartphone 2003:
        // DeviceEmulator sould report it has a touch screen
        bRet = SystemParametersInfo(SPI_GETOEMINFO, sizeof(oem), oem, 0);
        if (bRet && wcsstr(oem, _T("DeviceEmulator"))!=NULL)
            return 1;
#endif

        bRet = SystemParametersInfo(SPI_GETPLATFORMTYPE, sizeof(oem), oem, 0);
        // SmartPhone has no touch screen
        if (bRet && wcsstr(oem, _T("SmartPhone"))!=NULL)
            return 0;
        // PocketPC has touch screen
        if (bRet && wcsstr(oem, _T("PocketPC"))!=NULL)
            return 1;

        // otherwise check mouse info
        int aMouseInfo[3] = {0};
        bRet = SystemParametersInfo(SPI_GETMOUSE, sizeof(aMouseInfo), aMouseInfo, 0);
        return (bRet && aMouseInfo[0] != 0) ? 1 : 0;
#endif
}

void CSystemImpl::getHasTouchscreen(CMethodResult& oResult)
{
    oResult.set( rho_wmsys_has_touchscreen() == 1 ? true : false );
}

void CSystemImpl::getScreenSleeping(rho::apiGenerator::CMethodResult& oResult)
{
    oResult.set(false);
}

void CSystemImpl::setScreenSleeping( bool value, rho::apiGenerator::CMethodResult& oResult)
{
    //TODO: setScreenSleeping
    // http://stackoverflow.com/questions/246407/disable-sleep-mode-in-windows-mobile-6
}

void CSystemImpl::applicationInstall( const rho::String& applicationUrl, CMethodResult& oResult)
{
#ifdef OS_WINDOWS_DESKTOP
	String sUrl = applicationUrl;
    CFilePath oFile(sUrl);
	String filename = RHODESAPP().getRhoUserPath()+ oFile.getBaseName();
	if (CRhoFile::isFileExist(filename.c_str()) && (CRhoFile::deleteFile(filename.c_str()) != 0)) {
		LOG(ERROR) + "rho_sys_app_install() file delete failed: " + filename;
	} else {
		NetRequest NetRequest;
		NetResponse resp = getNetRequest(&NetRequest).pullFile(sUrl, filename, NULL, NULL,true,false);
		if (resp.isOK()) {
			StringW filenameW = convertToStringW(filename);
			LOG(INFO) + "Downloaded " + sUrl + " to " + filename;
			rho_wmsys_run_appW(filenameW.c_str(), L"");
		} else {
			LOG(ERROR) + "rho_sys_app_install() download failed: " + sUrl;
		}
	}
#else
    rho_wmsys_run_appW( convertToStringW(applicationUrl).c_str(), 0 );
#endif
}

static LONG _openRegAppPath( const rho::String& applicationName, CRegKey& oKey, StringW& strKeyPath)
{
    CFilePath oPath( applicationName );
    String strAppName = oPath.getFolderName();

#ifdef OS_WINDOWS_DESKTOP
    strKeyPath = L"SOFTWARE\\";
#else
    strKeyPath = L"Software\\Apps\\";
#endif

    strKeyPath += convertToStringW(strAppName);
    String_replace(strKeyPath, '/', '\\' );

#ifdef OS_WINDOWS_DESKTOP
    return oKey.Open(HKEY_CURRENT_USER, strKeyPath.c_str(), KEY_READ|KEY_WOW64_64KEY );
#else
    return oKey.Open(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), KEY_READ ); 
#endif
}

void CSystemImpl::isApplicationInstalled( const rho::String& applicationName, CMethodResult& oResult)
{
    bool bRet = false;
#ifdef OS_WINDOWS_DESKTOP
    CRegKey oKey;
    StringW strKeyPath;
    LONG res = _openRegAppPath(applicationName, oKey, strKeyPath);

	bRet = res == ERROR_SUCCESS;
#else
    CFilePath oPath( applicationName );
    StringW strAppName = convertToStringW(oPath.getFolderName());
    
    StringW strRequest = 
        L"<wap-provisioningdoc><characteristic type=\"UnInstall\">"
        L"<characteristic-query type=\"";
    strRequest += strAppName + L"\"/>"
        L"</characteristic></wap-provisioningdoc>"; 

#if defined( OS_WINCE ) && !defined( OS_PLATFORM_MOTCE )
    HRESULT hr         = E_FAIL;
    LPWSTR wszOutput   = NULL;
    hr = DMProcessConfigXML(strRequest.c_str(), CFGFLAG_PROCESS, &wszOutput);
    if (FAILED(hr) || !wszOutput )
        LOG(ERROR) + "DMProcessConfigXML failed: " + hr;
    else
    {
        StringW strResp = L"<characteristic type=\"";
        strResp += strAppName + L"\">";
        bRet = wcsstr(wszOutput, strResp.c_str()) != 0;
    }

    if ( wszOutput )
        free( wszOutput );
#endif
#endif

    oResult.set(bRet);
}

void CSystemImpl::applicationUninstall( const rho::String& applicationName, CMethodResult& oResult)
{
#ifdef OS_WINDOWS_DESKTOP
    CRegKey oKey;
    StringW strKeyPath;
    LONG res = _openRegAppPath(applicationName, oKey, strKeyPath);
    if ( res != ERROR_SUCCESS )
    {
        LOG(ERROR) + "Cannot open registry key: " + strKeyPath + "; Code:" + res;
    } else
    {
        TCHAR szBuf[MAX_PATH+1];
        ULONG nChars = MAX_PATH;

        res = oKey.QueryStringValue(L"Uninstaller", szBuf, &nChars);
        if ( res != ERROR_SUCCESS )
            LOG(ERROR) + "Cannot read registry key: Uninstaller; Code:" + res;
        else
        {
            StringW strFullPathW = szBuf;
			rho_wmsys_run_appW( strFullPathW.c_str(), L"" );
		}
	}
#else
    CFilePath oPath( applicationName );
    StringW strAppName = convertToStringW(oPath.getFolderName());
    
    StringW strRequest = 
        L"<wap-provisioningdoc><characteristic type=\"UnInstall\">"
        L"<characteristic type=\"";
    strRequest += strAppName + L"\">"
        L"<parm name=\"uninstall\" value=\"1\"/>"
        L"</characteristic>"
        L"</characteristic></wap-provisioningdoc>";

#if defined( OS_WINCE )&& !defined( OS_PLATFORM_MOTCE )
    HRESULT hr         = E_FAIL;
    LPWSTR wszOutput   = NULL;
    hr = DMProcessConfigXML(strRequest.c_str(), CFGFLAG_PROCESS, &wszOutput);
    if (FAILED(hr) || !wszOutput )
        LOG(ERROR) + "DMProcessConfigXML failed: " + hr;
    else
    {
    }

    if ( wszOutput )
        free( wszOutput );
#endif
#endif
}

void CSystemImpl::openUrl( const rho::String& url, CMethodResult& oResult)
{
    rho_wmsys_run_appW( convertToStringW(url).c_str(), 0 );
}

void CSystemImpl::runApplication( const rho::String& appName,  const rho::String& params,  bool blockingCall, rho::apiGenerator::CMethodResult& oResult)
{
    CRegKey oKey;
    StringW strKeyPath;
    LONG res = _openRegAppPath(appName, oKey, strKeyPath);

    if ( res != ERROR_SUCCESS )
    {
        LOG(ERROR) + "Cannot open registry key: " + strKeyPath + "; Code:" + res;
    } else
    {
        TCHAR szBuf[MAX_PATH+1];
        ULONG nChars = MAX_PATH;

        res = oKey.QueryStringValue(L"InstallDir", szBuf, &nChars);
        if ( res != ERROR_SUCCESS )
            LOG(ERROR) + "Cannot read registry key: InstallDir; Code:" + res;
        else
        {
            StringW strFullPath = szBuf;

            if ( strFullPath[strFullPath.length()-1] != '/' && strFullPath[strFullPath.length()-1] != '\\' )
                strFullPath += L"\\";

			StringW strBaseName;
            CFilePath oPath(appName);
            convertToStringW(oPath.getBaseName(), strBaseName);
            strFullPath += strBaseName;

            rho_wmsys_run_appW( strFullPath.c_str(), convertToStringW(params).c_str());
        }
    }
}

void CSystemImpl::setRegistrySetting( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
    bool bRetVal = false;
	bool bPersistent = false;
	rho::String szHive = "";
	rho::String szType = "";
	rho::String szSubkey = "";
	rho::String szSetting = "";
	rho::String szValue = "";
	rho::String comparitor;
	typedef std::map<rho::String, rho::String>::const_iterator it_type;
	for (it_type iterator = propertyMap.begin(); iterator != propertyMap.end(); iterator++)
	{
		comparitor.assign(iterator->first);
		std::transform(comparitor.begin(), comparitor.end(), comparitor.begin(), ::tolower);
		if (comparitor.compare("persistent") == 0)
			bPersistent = true;
		else if (comparitor.compare("hive") == 0)
			szHive = iterator->second;
		else if (comparitor.compare("type") == 0)
			szType = iterator->second;
		else if (comparitor.compare("key") == 0)
			szSubkey = iterator->second;
		else if (comparitor.compare("setting") == 0)
			szSetting = iterator->second;
		else if (comparitor.compare("value") == 0)
			szValue = iterator->second;
		else
			LOG(WARNING) + "Unrecognised parameter passed to setRegistrySetting: " + iterator->first;
	}

	bRetVal = WriteRegHandler(szHive, rho::common::convertToStringW(szSubkey).c_str(), 
		rho::common::convertToStringW(szSetting).c_str(), szType, 
		rho::common::convertToStringW(szValue).c_str(), bPersistent);

	oResult.set(bRetVal);

}

void CSystemImpl::getRegistrySetting( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
	rho::String szHive = "";
	rho::String szSubkey = "";
	rho::String szSetting = "";
	rho::String comparitor;
	typedef std::map<rho::String, rho::String>::const_iterator it_type;
	for (it_type iterator = propertyMap.begin(); iterator != propertyMap.end(); iterator++)
	{
		comparitor.assign(iterator->first);
		std::transform(comparitor.begin(), comparitor.end(), comparitor.begin(), ::tolower);
		if (comparitor.compare("hive") == 0)
			szHive = iterator->second;
		else if (comparitor.compare("key") == 0)
			szSubkey = iterator->second;
		else if (comparitor.compare("setting") == 0)
			szSetting = iterator->second;
		else
			LOG(WARNING) + "Unrecognised parameter passed to getRegistrySetting: " + iterator->first;
	}

	GetRegistrySetting(szHive, szSubkey, szSetting, oResult);

}

void CSystemImpl::deleteRegistrySetting( const rho::Hashtable<rho::String, rho::String>& propertyMap, rho::apiGenerator::CMethodResult& oResult)
{
	bool bRetVal = false;
	bool bPersistent = false;
	rho::String szHive = "";
	rho::String szSubkey = "";
	rho::String szSetting = "";
	rho::String comparitor;
	typedef std::map<rho::String, rho::String>::const_iterator it_type;
	for (it_type iterator = propertyMap.begin(); iterator != propertyMap.end(); iterator++)
	{
		comparitor.assign(iterator->first);
		std::transform(comparitor.begin(), comparitor.end(), comparitor.begin(), ::tolower);
		if (comparitor.compare("persistent") == 0)
			bPersistent = true;
		else if (comparitor.compare("hive") == 0)
			szHive = iterator->second;
		else if (comparitor.compare("key") == 0)
			szSubkey = iterator->second;
		else if (comparitor.compare("setting") == 0)
			szSetting = iterator->second;
		else
			LOG(WARNING) + "Unrecognised parameter passed to deleteRegistrySetting: " + iterator->first;
	}
	
		bRetVal = DeleteRegHandler(szHive, 
			rho::common::convertToStringW(szSubkey).c_str(),
			rho::common::convertToStringW(szSetting).c_str(), bPersistent, false);
	oResult.set(bRetVal);
}

void CSystemImpl::setWindowFrame( int x,  int y,  int width,  int height, CMethodResult& oResult)
{
#if defined(OS_WINDOWS_DESKTOP)
    rho_sys_set_window_frame( (int)x, (int)y, (int)width, (int)height);
#endif

}

void CSystemImpl::setWindowPosition( int x,  int y, CMethodResult& oResult)
{
#if defined(OS_WINDOWS_DESKTOP)
    rho_sys_set_window_position((int)x, (int)y);
#endif
}

void CSystemImpl::setWindowSize( int width,  int height, rho::apiGenerator::CMethodResult& oResult)
{
#if defined(OS_WINDOWS_DESKTOP)
    rho_sys_set_window_size((int)width, (int)height);
#endif
}

void CSystemImpl::getWebviewFramework(rho::apiGenerator::CMethodResult& oResult)
{
    String strRes = "";

#if defined(OS_WINDOWS_DESKTOP)
	strRes = rho_sys_win32_getWebviewFramework();
#elif defined(APP_BUILD_CAPABILITY_WEBKIT_BROWSER)
	strRes = "WEBKIT/MOTOROLA";
#else
	strRes = "IE";
    //TODO: get IE version for WM/CE
#endif

    oResult.set(strRes);
}

void CSystemImpl::bringToFront(rho::apiGenerator::CMethodResult& oResult)
{
    rho_sys_bring_to_front();
}

extern "C" void rho_sys_set_http_proxy_url(const char* url);
void CSystemImpl::set_http_proxy_url( const rho::String& proxyURI, rho::apiGenerator::CMethodResult& oResult)
{
    rho_sys_set_http_proxy_url( proxyURI.c_str() );
}

extern "C" void rho_sys_unset_http_proxy();
void CSystemImpl::unset_http_proxy(rho::apiGenerator::CMethodResult& oResult)
{
    rho_sys_unset_http_proxy();
}

extern "C" const char* rho_sys_get_http_proxy_url();
void CSystemImpl::getHttpProxyURI(CMethodResult& oResult)
{
    oResult.set(rho_sys_get_http_proxy_url());
}

void CSystemImpl::setHttpProxyURI( const rho::String& value, CMethodResult& oResult)
{
    if ( value.length() )
        rho_sys_set_http_proxy_url( value.c_str() );
    else
        rho_sys_unset_http_proxy();
}


void CSystemImpl::getHasCamera(CMethodResult& oResult)
{
#if defined( OS_WINDOWS_DESKTOP ) || defined( OS_PLATFORM_MOTCE )
    oResult.set(false);
#else
    oResult.set(true);
#endif

}

////////////////////////////////////////////////////////////////////////

class CSystemFactory: public CSystemFactoryBase
{
public:
    ~CSystemFactory(){}

    ISystemSingleton* createModuleSingleton(){ return new CSystemImpl(); }
};

}

extern "C" void Init_System()
{
    rho::CSystemFactory::setInstance( new rho::CSystemFactory() );
    rho::Init_System_API();
}
