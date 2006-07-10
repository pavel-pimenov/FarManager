#include "7z.h"
#include "Guid.h"
#include "commands.h"
#include "detectarchive.h"
#include <Collections.h>

const GUID FormatGUIDs[] = {
		CLSID_CFormat7z,
		CLSID_CArjHandler,
		CLSID_CCabHandler,
		CLSID_CChmHandler,
		CLSID_CDebHandler,
		CLSID_CIsoHandler,
		CLSID_CLzhHandler,
		CLSID_CNsisHandler,
		CLSID_CRarHandler,
		CLSID_CRpmHandler,
		CLSID_CSplitHandler,
		CLSID_CTarHandler,
		CLSID_CZipHandler,
		CLSID_CGZipHandler,
		CLSID_CZHandler,
		CLSID_CBZip2Handler,
		CLSID_CCpioHandler,
};

const unsigned char SevenZipSig[] = {'7' , 'z', 0xBC, 0xAF, 0x27, 0x1C};
const unsigned char ZipSig[]      = {0x50, 0x4B, 0x03, 0x04};
const unsigned char BZipSig[]     = {'B' , 'Z', 'h'};
const unsigned char GZipSig[]     = {0x1F, 0x8B};
const unsigned char ArjSig[]      = {0x60, 0xEA};
const unsigned char ZSig[]        = {0x1F, 0x9D};
const unsigned char RarSig[]      = {'R', 'a', 'r', '!'};
const unsigned char CabSig[]      = {'M', 'S', 'C', 'F'};
const unsigned char RpmSig[]      = {0xED, 0xAB, 0xEE, 0xDB};
const unsigned char DebSig[]      = {'!', '<', 'a', 'r', 'c', 'h', '>', 0x0A, 'd', 'e', 'b', 'i', 'a', 'n', '-', 'b', 'i', 'n', 'a', 'r', 'y'};
const unsigned char CpioSig[]     = {'0', '7', '0', '7', '0'}; //BUG BUG: �த� �� ᮢᥬ �筮
const unsigned char ChmSig[]      = {'I', 'T', 'S', 'F'};
const unsigned char NsisSig[]     = {0xEF, 0xBE, 0xAD, 0xDE, 0x4E, 0x75, 0x6C, 0x6C, 0x73, 0x6F, 0x66, 0x74, 0x49, 0x6E, 0x73, 0x74};
const unsigned char IsoSig[]      = {'C', 'D', '0', '0', '1', 0x1};

struct FormatInfo {
	const GUID *puid;
	const unsigned char *psig;
	int size;
	bool bPosZero;
	DETECTARCHIVE pDetectArchive;
};

const FormatInfo signs[] = {
	{&CLSID_CFormat7z,     (const unsigned char *)&SevenZipSig, 6, false, NULL},
	{&CLSID_CRarHandler,   (const unsigned char *)&RarSig,      4, false, IsRarHeader},
	{&CLSID_CZipHandler,   (const unsigned char *)&ZipSig,      4, false, IsZipHeader},
	{&CLSID_CRpmHandler,   (const unsigned char *)&RpmSig,      4, true,  NULL},
	{&CLSID_CDebHandler,   (const unsigned char *)&DebSig,     21, true,  NULL},
	{&CLSID_CCabHandler,   (const unsigned char *)&CabSig,      4, false, IsCabHeader},
	{&CLSID_CBZip2Handler, (const unsigned char *)&BZipSig,     3, true,  NULL},
	{&CLSID_CArjHandler,   (const unsigned char *)&ArjSig,      2, false, IsArjHeader},
	{&CLSID_CTarHandler,   NULL,                                0, true,  IsTarHeader},
	{&CLSID_CGZipHandler,  (const unsigned char *)&GZipSig,     2, true,  NULL},
	{&CLSID_CZHandler,     (const unsigned char *)&ZSig,        2, true,  NULL},
	{&CLSID_CLzhHandler,   NULL,                                0, false, IsLzhHeader},
	{&CLSID_CCpioHandler,  (const unsigned char *)&CpioSig,     5, true,  NULL},
	{&CLSID_CChmHandler,   (const unsigned char *)&ChmSig,      4, true,  NULL},
	{&CLSID_CNsisHandler,  (const unsigned char *)&NsisSig,    16, false, NULL},
	{&CLSID_CIsoHandler,   (const unsigned char *)&IsoSig,      6, false, NULL},
};

bool GetFormatCommand(const GUID guid, int nCommand, char *lpCommand)
{
	if ( IsEqualGUID(guid, CLSID_CFormat7z) )
		strcpy (lpCommand, p7Z[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CRarHandler) )
		strcpy (lpCommand, pRAR[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CZipHandler) )
		strcpy (lpCommand, pZIP[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CArjHandler) )
		strcpy (lpCommand, pARJ[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CTarHandler) )
		strcpy (lpCommand, pTAR[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CGZipHandler) )
		strcpy (lpCommand, pGZIP[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CBZip2Handler) )
		strcpy (lpCommand, pBZIP[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CZHandler) )
		strcpy (lpCommand, pZ[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CCabHandler) )
		strcpy (lpCommand, pCAB[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CLzhHandler) )
		strcpy (lpCommand, pLZH[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CCpioHandler) )
		strcpy (lpCommand, pCPIO[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CRpmHandler) )
		strcpy (lpCommand, pRPM[nCommand]);
	else if ( IsEqualGUID(guid, CLSID_CDebHandler) )
		strcpy (lpCommand, pDEB[nCommand]);
/*
	else if ( IsEqualGUID(guid, CLSID_CChmHandler) )
		strcpy (lpCommand, "");
	else if ( IsEqualGUID(guid, CLSID_CSplitHandler) )
		strcpy (lpCommand, "");
	else if ( IsEqualGUID(guid, CLSID_CNsisHandler) )
		strcpy (lpCommand, "");
	else if ( IsEqualGUID(guid, CLSID_CIsoHandler) )
		strcpy (lpCommand, "");
*/
	else
		return false;

	return true;
}

int FindFormats (const char *lpFileName, Collection <FormatPosition*> &formats)
{

	/*const FormatInfo *info = NULL;

	for (int i = 0; i < sizeof (signs)/sizeof (signs[0]); i++)
	{
		if ( IsEqualGUID (uid, *signs[i].puid) )
		{
			info = &signs[i];
			break;
		}
	}*/

	const GUID *pResult = NULL;

	HANDLE hFile = CreateFile (
			lpFileName,
			GENERIC_READ,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
			);

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		unsigned char *buffer = (unsigned char*)malloc (1 << 17);
		DWORD dwRead;

		if ( ReadFile (hFile, buffer, 1 << 17, &dwRead, NULL) )
		{
			for (int j = 0; j < sizeof(signs)/sizeof(signs[0]); j++)
			{
				const FormatInfo *info = &signs[j];

				if ( info->pDetectArchive)
				{
					int position = info->pDetectArchive(buffer, dwRead);
					if ( position != -1 )
					{
						FormatPosition *pos = new FormatPosition;

						pos->puid = info->puid;
						pos->position = position;

						formats.Add (pos);
					}
				}
				else
				{
					for (int i = 0; i < dwRead-info->size; i++)
					{
						if ( !memcmp (&buffer[i], info->psig, info->size) )
						{
							FormatPosition *pos = new FormatPosition;

							pos->puid = info->puid;
							pos->position = i;

							formats.Add (pos);
							break;
						}
						if (info->bPosZero)
							break;
					}
				}
			}

			if ( strstr(lpFileName, ".") && strlen(lpFileName) > 2 )
			{
				bool bMatch = false;
				int len = strlen(lpFileName);

				if ( lpFileName[len-2] == '0' && lpFileName[len-1] == '1' )
				{
					int i=len-3;
					for (; i>0; i--)
						if ( lpFileName[i] != '0' )
							break;
					if ( lpFileName[i] == '.' )
						bMatch = true;
				}
				else if ( (lpFileName[len-1] == 'A' || lpFileName[len-1] == 'a') &&
						  (lpFileName[len-2] == 'A' || lpFileName[len-2] == 'a') )
					bMatch = true;

				if ( bMatch )
				{
					FormatPosition *pos = new FormatPosition;

					pos->puid = &CLSID_CSplitHandler;
					pos->position = 0;

					formats.Add (pos);
				}
			}
		}

		free (buffer);

		CloseHandle (hFile);
	}

	return formats.GetCount();
}


bool SevenZipModule::Initialize (const char *lpFileName)
{
	bool bResult = false;

	m_hModule = LoadLibraryEx (
			lpFileName,
			NULL,
			LOAD_WITH_ALTERED_SEARCH_PATH
			);

	if ( m_hModule )
	{
		m_pfnCreateObject = (CREATEOBJECT)GetProcAddress (m_hModule, "CreateObject");
		m_pfnGetHandlerProperty = (GETHANDLERPROPERTY)GetProcAddress (m_hModule, "GetHandlerProperty");

		if ( m_pfnCreateObject &&
			 m_pfnGetHandlerProperty )
		{
			PROPVARIANT value;

			VariantInit ((VARIANTARG*)&value);

			m_pfnGetHandlerProperty (NArchive::kClassID, &value);

			if ( value.vt == VT_BSTR )
			{
				memcpy (&m_uid, value.bstrVal, sizeof (GUID));
				bResult = true;
			}

			VariantClear ((VARIANTARG*)&value);
		}
	}

	return bResult;
}


SevenZipModule::~SevenZipModule ()
{
	FreeLibrary (m_hModule);
}

bool SevenZipModule::HasSignature ()
{
	if ( IsEqualGUID (m_uid, CLSID_CSplitHandler) )
		return true;

	for (int i = 0; i < sizeof(signs)/sizeof(signs[0]); i++)
	{
    	if ( IsEqualGUID (m_uid, *(signs[i].puid)) )
    		return true;
	}

	return false;
}


void SevenZipModule::GetArchiveFormatInfo (ArchiveFormatInfo *pInfo)
{
	pInfo->dwFlags = AFF_SUPPORT_INTERNAL_EXTRACT|AFF_SUPPORT_INTERNAL_TEST;

	if ( IsEqualGUID (m_uid, CLSID_CFormat7z) )
	{
		pInfo->lpName = "7z archive [7z]";
		pInfo->lpDefaultExtention = "7z";
	}
	else

	if ( IsEqualGUID (m_uid, CLSID_CArjHandler) )
	{
		pInfo->lpName = "ARJ archive [7z]";
		pInfo->lpDefaultExtention = "arj";
	}
	else

	if ( IsEqualGUID (m_uid, CLSID_CBZip2Handler) )
	{
		pInfo->lpName = "BZip2 archive [7z]";
		pInfo->lpDefaultExtention = "bz2";
	}
	else

	if ( IsEqualGUID (m_uid, CLSID_CCabHandler) )
	{
		pInfo->lpName = "CAB archive [7z]";
		pInfo->lpDefaultExtention = "cab";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CChmHandler) )
	{
		pInfo->lpName = "CHM archive [7z]";
		pInfo->lpDefaultExtention = "chm";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CCpioHandler) )
	{
		pInfo->lpName = "Cpio archive [7z]";
		pInfo->lpDefaultExtention = "cpio";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CDebHandler) )
	{
		pInfo->lpName = "Debian archive [7z]";
		pInfo->lpDefaultExtention = "deb";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CGZipHandler) )
	{
		pInfo->lpName = "GZip archive [7z]";
		pInfo->lpDefaultExtention = "gz";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CIsoHandler) )
	{
		pInfo->lpName = "ISO archive [7z]";
		pInfo->lpDefaultExtention = "iso";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CLzhHandler) )
	{
		pInfo->lpName = "LZH archive [7z]";
		pInfo->lpDefaultExtention = "lzh";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CNsisHandler) )
	{
		pInfo->lpName = "NSIS archive [7z]";
		pInfo->lpDefaultExtention = "exe";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CRarHandler) )
	{
		pInfo->lpName = "RAR archive [7z]";
		pInfo->lpDefaultExtention = "rar";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CRpmHandler) )
	{
		pInfo->lpName = "RPM archive [7z]";
		pInfo->lpDefaultExtention = "rpm";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CSplitHandler) )
	{
		pInfo->lpName = "Split archive [7z]";
		pInfo->lpDefaultExtention = "001";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CTarHandler) )
	{
		pInfo->lpName = "TAR archive [7z]";
		pInfo->lpDefaultExtention = "tar";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CZHandler) )
	{
		pInfo->lpName = "Z archive [7z]";
		pInfo->lpDefaultExtention = "z";
	}
	else
	if ( IsEqualGUID (m_uid, CLSID_CZipHandler) )
	{
		pInfo->lpName = "ZIP archive [7z]";
		pInfo->lpDefaultExtention = "zip";
	}
}


SevenZipArchive::SevenZipArchive (SevenZipModule *pModule, const char *lpFileName)
{
	m_pArchive = NULL;
	m_pInFile = NULL;

	m_pModule = pModule;
	m_lpFileName = StrDuplicate (lpFileName);

	m_bPasswordDefined = false;
}

SevenZipArchive::~SevenZipArchive ()
{
	if ( m_bPasswordDefined )
		free (m_lpPassword);

	StrFree (m_lpFileName);
}

bool __stdcall SevenZipArchive::pOpenArchive (
		int nOpMode,
		ARCHIVECALLBACK pfnCallback
		)
{
	m_pfnCallback = pfnCallback;

  	m_pInFile = new CInFile;

  	if ( m_pInFile->Open (m_lpFileName) )
  	{
		HRESULT hr = m_pModule->m_pfnCreateObject (
  				&m_pModule->m_uid,
  				&IID_IInArchive,
  				(void**)&m_pArchive
  				);

		if ( SUCCEEDED (hr) )
  		{
  			unsigned __int64 max = 1 << 17;

			CArchiveOpenCallback *pCallback = new CArchiveOpenCallback (this);

			hr = m_pArchive->Open (m_pInFile, &max, pCallback);

			if ( SUCCEEDED (hr) )
  			{
  				m_pArchive->GetNumberOfItems((unsigned int*)&m_nItemsNumber);

  				if ( m_nItemsNumber )
  				{
  					m_nItemsNumber--;

  					delete pCallback;
  					return true;
  				}

  				m_pArchive->Close();
  			}

			delete pCallback;

  			//if we get here, there is an error

  			m_pArchive->Release ();
  		}

  		m_pInFile->Release ();
  	}

	return false;
}

void __stdcall SevenZipArchive::pCloseArchive ()
{
	if ( m_pArchive )
	{
		m_pArchive->Close ();
		m_pArchive->Release ();
		m_pArchive = NULL;
	}

	if ( m_pInFile )
	{
		m_pInFile->Release (); //???
		m_pInFile = NULL;
	}
}

unsigned __int64 VariantToInt64 (PROPVARIANT *value)
{
	switch ( value->vt )
	{
		case VT_UI1:
			return value->bVal;
		case VT_UI2:
			return value->uiVal;
		case VT_UI4:
			return value->ulVal;
		case VT_UI8:
			return (unsigned __int64)value->uhVal.QuadPart;
		default:
			return 0;
	}
}


int __stdcall SevenZipArchive::pGetArchiveItem (
		ArchiveItemInfo *pItem
		)
{
	if ( m_nItemsNumber == -1)
		return E_EOF;

	int nResult = E_BROKEN;

	PROPVARIANT value;

	VariantInit ((VARIANTARG*)&value);

	if ( SUCCEEDED (m_pArchive->GetProperty (
			m_nItemsNumber,
			kpidPath,
			&value
			)) )
	{
		if ( value.vt == VT_BSTR )
			WideCharToMultiByte (CP_OEMCP, 0, value.bstrVal, -1, pItem->pi.FindData.cFileName, sizeof (pItem->pi.FindData.cFileName), NULL, NULL);
		else
		{
			strcpy (pItem->pi.FindData.cFileName, FSF.PointToName (m_lpFileName));
			CutTo (pItem->pi.FindData.cFileName, '.', true);
		}

		VariantClear ((VARIANTARG*)&value);
		VariantInit ((VARIANTARG*)&value);

		if ( SUCCEEDED (m_pArchive->GetProperty (
				m_nItemsNumber,
				kpidAttributes,
				&value
				)) )
		{
			if ( value.vt == VT_UI4 )
		        pItem->pi.FindData.dwFileAttributes = value.ulVal;
		}

		if ( SUCCEEDED (m_pArchive->GetProperty (
				m_nItemsNumber,
				kpidIsFolder,
				&value
				)) )
		{
			if ( value.vt == VT_BOOL )
			{
				if ( value.boolVal == VARIANT_TRUE )
					pItem->pi.FindData.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
			}
		}

		if ( SUCCEEDED (m_pArchive->GetProperty(
				m_nItemsNumber,
				kpidSize,
				&value
				)) )
		{
			unsigned __int64 size = VariantToInt64 (&value);

			pItem->pi.FindData.nFileSizeLow = (DWORD)size;
			pItem->pi.FindData.nFileSizeHigh = (DWORD)(size >> 32);
		}

		if ( SUCCEEDED (m_pArchive->GetProperty(
				m_nItemsNumber,
				kpidPackedSize,
				&value
				)) )
		{
			unsigned __int64 size = VariantToInt64 (&value);

			pItem->pi.PackSize = (DWORD)size;
			pItem->pi.PackSizeHigh = (DWORD)(size >> 32);
		}


		if ( SUCCEEDED (m_pArchive->GetProperty(
				m_nItemsNumber,
				kpidCreationTime,
				&value
				)) )
			memcpy (&pItem->pi.FindData.ftCreationTime, &value.filetime, sizeof (FILETIME));

		if ( SUCCEEDED (m_pArchive->GetProperty(
				m_nItemsNumber,
				kpidLastAccessTime,
				&value
				)) )
			memcpy (&pItem->pi.FindData.ftLastAccessTime, &value.filetime, sizeof (FILETIME));

		if ( SUCCEEDED (m_pArchive->GetProperty(
				m_nItemsNumber,
				kpidLastWriteTime,
				&value
				)) )
			memcpy (&pItem->pi.FindData.ftLastWriteTime, &value.filetime, sizeof (FILETIME));


		pItem->pi.UserData = m_nItemsNumber;

		nResult = E_SUCCESS;
	}

	m_nItemsNumber--;

	return nResult;
}


bool __stdcall SevenZipArchive::pTest (
		PluginPanelItem *pItems,
		int nItemsNumber
		)
{
	return true;
}

/*
int GetIndex (IInArchive *pArchive, const char *lpFileName)
{
	unsigned int dwIndex = 0;
	PROPVARIANT value;
	char szArcFileName[NM];

	pArchive->GetNumberOfItems((unsigned int*)&dwIndex);

	for (int i = 0; i < dwIndex; i++)
	{
		VariantInit ((VARIANTARG*)&value);

		if ( SUCCEEDED (pArchive->GetProperty (
				i,
				kpidPath,
				&value
				)) )
		{
			WideCharToMultiByte (CP_OEMCP, 0, value.bstrVal, -1, szArcFileName, sizeof (szArcFileName), NULL, NULL);

            if ( !FSF.LStricmp (
            		lpFileName,
            		szArcFileName
            		) )
			{
				VariantClear ((VARIANTARG*)&value);
				return i;
			}
		}

		VariantClear ((VARIANTARG*)&value);
	}

	return -1;
}
*/

int __cdecl compare(const void *p1, const void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	if ( i1 > i2 )
		return 1;

	if ( i1 == i2 )
		return 2;

	if ( i1 < i2 )
		return -1;
}


bool __stdcall SevenZipArchive::pExtract (
		PluginPanelItem *pItems,
		int nItemsNumber,
		const char *lpDestPath,
		const char *lpCurrentFolder
		)
{
	bool bResult = false;
	unsigned int *indices = (unsigned int*)malloc (nItemsNumber*sizeof (unsigned int));
	ArchiveItem *items = (ArchiveItem*)malloc (nItemsNumber*sizeof (ArchiveItem));

	for (int i = 0; i < nItemsNumber; i++)
	{
		indices[i] = pItems[i].UserData; //GetIndex (m_pArchive, pItems[i].FindData.cFileName);

		items[i].nIndex = indices[i];
		items[i].pItem = &pItems[i];
	}

	FSF.qsort (indices, nItemsNumber, 4, compare);

	//unsigned int nItems = 0;
	//m_pArchive->GetNumberOfItems((unsigned int*)&nItems);

	CArchiveExtractCallback *pCallback = new CArchiveExtractCallback (this, items, nItemsNumber, lpDestPath, lpCurrentFolder);

	if ( SUCCEEDED (m_pArchive->Extract(
			indices,
			(unsigned int)nItemsNumber,
			0,
			pCallback
			)) )
		bResult = true;

	delete pCallback;
	free (indices);
	free (items);

	return bResult;
}


void SevenZipArchive::SetPassword (const char *lpPassword, int nLength)
{
	if ( m_bPasswordDefined )
		free (m_lpPassword);

	m_nPasswordLength = nLength;
	m_lpPassword = (char *)malloc (nLength+1);

	memcpy (m_lpPassword, lpPassword, nLength);

	m_bPasswordDefined = true;
}

int SevenZipArchive::GetPasswordLength ()
{
	return m_bPasswordDefined?m_nPasswordLength:-1;
}

void SevenZipArchive::GetPassword (char *lpBuffer)
{
	if ( m_bPasswordDefined )
		memcpy (lpBuffer, m_lpPassword, m_nPasswordLength);
}
