// dllmain.cpp : Defines the entry point for the DLL application.
#include "..\Header Files\stdafx.h"
#include "..\Header Files\CommonCore.h"
//using namespace NativeLogging;

#include <string>


#define WINAPI __stdcall

int InitComLibrary();

extern "C" BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID /*lpReserved*/
	)
{
	int returnValue = -1;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{


							   char szNativeDllVe1r[20] = { 0 };
							   MARVIS_Auth_GetVersion(szNativeDllVe1r);

							   std::string strDllVer = szNativeDllVe1r;

							   returnValue = CommInitLibrary();
							   if (MARVIS_AUTH_E_SUCCESS != returnValue)
							   {
								   //LOG_ERROR("One or more of the dependencies failed to load.");
								   //::MessageBox(NULL, "One or more of the dependencies failed to load.", "CommInitLibrary", MB_OK);
								   return FALSE;
							   }
							   else
							   {
								   char szCommLibVer[20] = { 0 };

								   CommGetLibVersion(szCommLibVer);
								   //CommSetLogProperties(LEVEL_LOG_ERROR,"Comm.log");
								   //std::string strComVersion();
								   //LOG_INFO("CommInitLibrary succeeded.");
							   }
							   CommEnableLogs(COMM_LEVEL_LOG_OFF, NULL);

							   MARVIS_Auth_EnableLogs(MARVIS_AUTH_LOG_LEVEL::MARVIS_AUTH_LOG_LEVEL_OFF, NULL);

							   AutoDetection();
	}
		break;
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;
	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		MARVIS_Auth_UninitDevice();
		CommUninitLibrary();
		//LOG_ALWAYS("*~*~*~*~*~*~*~*~*~*~*~*~*~ MIDFinger_Auth_ Uninitialized *~*~*~*~*~*~*~*~*~*~*~*~*~");
		//LOG_CLOSE();
		break;
	}
	return TRUE;    // Successful DLL_PROCESS_ATTACH.
}

