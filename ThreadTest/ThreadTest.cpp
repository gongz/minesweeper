// ThreadTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int Data_1=0, Data_2=0;
	HANDLE Handle_1 = 0;	
	HANDLE Array_Of_Thread_Handles[3];

	Handle_1 = CreateThread(NULL,0, Thread_1, &Data_1, 0, NULL);
	cout <<"***************************" << endl;
	cout <<"**    Win7 Minesweeper   **" << endl;
	cout <<"**Gong Zhang, Jan 04 2012**" << endl;
	cout <<"***************************" << endl;
	if(Handle_1 == NULL)
	{
		ExitProcess(Data_1);
		//cout << "Nothing Created" << endl;
	}
	else
	{
		//cout << "Handle1 Created"<<endl;
	}
	Array_Of_Thread_Handles[0] = Handle_1;
	WaitForMultipleObjects(1, Array_Of_Thread_Handles, TRUE, INFINITE);
	if (CloseHandle(Handle_1))
	{
		//cout <<"Handle1 closed" << endl;
	}
	
	return 0;
}

//Get the processID wanted
DWORD GetProcessIdByName(DWORD processId, TCHAR* sProcessNameWanted)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	//require information type, ?, processid
	HANDLE hProcess = OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );
	DWORD baseAddr=0;
	MODULEINFO miModInfo = { 0 };
	if(NULL!=hProcess)
	{
		HMODULE hMod;
		DWORD cbReturned;
		if(EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbReturned))
		{
			GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
 		}
		if(_tcscmp(szProcessName,sProcessNameWanted) == 0)
		{
			GetModuleInformation(hProcess,hMod, &miModInfo, sizeof(miModInfo));
			//_tprintf( TEXT("%s  (PID: %u) baseAddr: 0x%X\n"), szProcessName, processId,miModInfo.lpBaseOfDll);
			CloseHandle(hProcess);
			return processId;
		}		
		CloseHandle(hProcess);
		return 0;
	}
	return 0;
}
//Use ProcessID to control the specific process 
void GetProcessBaseAddr(DWORD processId)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	//require information type, ?, processid
	HANDLE hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, processId);
	DWORD baseAddr=0;
	
	HMODULE lphModule[BUF_SIZE];
	
	MODULEINFO miModInfo = { 0 };
	MEMORY_BASIC_INFORMATION sMemoryInfo = { 0 };

	unsigned int i =0;
	LPVOID lpBuffer[700]; //max mine is 668
	DWORD cReturned =0;
	
	if(NULL!=hProcess)
	{
		//HMODULE hMod;
		DWORD cbReturned;
		cReturned = 0;	
		
		if(EnumProcessModules(hProcess, lphModule, sizeof(lphModule), &cbReturned))
		{
			//for(i=0; i<cbReturned/4; i++)
			for(i=0; i<1; i++)
			{
				GetModuleBaseName(hProcess, lphModule[i], szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
				GetModuleInformation(hProcess, lphModule[i], &miModInfo, sizeof(miModInfo));
				//_tprintf( TEXT("%s  (PID: %u) EntryPoint: 0x%8X\n"), szProcessName, processId, miModInfo.EntryPoint);
				_tprintf( TEXT("baseAddr1: 0x%8X\n"),(DWORD)miModInfo.lpBaseOfDll);
				{
						DWORD base = (DWORD)miModInfo.lpBaseOfDll+0x868B4;
						bool isMine[30][30];
						//_tprintf( TEXT("baseAddr: 0x%8X\n"),base);					
						//first pointer
						ReadProcessMemory(hProcess,(LPCVOID)(base), lpBuffer, 4, &cReturned);
						base = (DWORD)lpBuffer[0]; 
						//_tprintf( TEXT("ESI+OFFSET: %8X \n"), base);
						//second pointer 
						ReadProcessMemory(hProcess,(LPCVOID)((DWORD)(base+0x10)), lpBuffer, 4, &cReturned);
						base = (DWORD)lpBuffer[0]; //HERE IS ESI 
						//_tprintf( TEXT("%8X \n"), base);
						//NUMBER OF mines rows cols
						ReadProcessMemory(hProcess,(LPCVOID)((DWORD)(base+0x4)), lpBuffer, 12, &cReturned);
						int mines = (DWORD)lpBuffer[0], rows = (DWORD)lpBuffer[1], cols = (DWORD)lpBuffer[2]; 
						_tprintf( TEXT("Number of mines  :%2d \n"), mines);
						_tprintf( TEXT("Number of rows   :%2d \n"), rows);
						_tprintf( TEXT("Number of columns:%2d \n"), cols);

						DWORD mineBase;
						//mov ebx,dword ptr ds:[esi+44]
						ReadProcessMemory(hProcess,(LPCVOID)((DWORD)(base+0x44)), lpBuffer, 4, &cReturned);
						mineBase = (DWORD)lpBuffer[0];//ebx
						//mov ebx,dword ptr ds:[ebx+C] 
						ReadProcessMemory(hProcess,(LPCVOID)((DWORD)(mineBase+0x0C)), lpBuffer, 4, &cReturned);
						mineBase = (DWORD)lpBuffer[0];//ebx
						_tprintf(TEXT("%8X\n"), (DWORD)(lpBuffer[0]));
						//mov edx,dword ptr ds:[ebx+edx*4]

						int ptr = 0, c = 0, edx = 0;
						for(edx=0 ; edx < cols ; edx++)
						{
							//each colum
							ReadProcessMemory ( hProcess ,(LPCVOID)(mineBase+4*edx) , lpBuffer ,4 ,NULL);							
							ReadProcessMemory ( hProcess ,(LPCVOID)((DWORD)lpBuffer[0]+0xC) , lpBuffer ,4 ,NULL);	
							for ( c=0 ;c<cols ;c++ )
								ReadProcessMemory ( hProcess ,(LPCVOID)((DWORD)lpBuffer[0]+c) ,(LPVOID)&isMine[edx][c] ,1 ,&cReturned);
						}
						int r = 0;
						_tprintf(TEXT("  "));
						for ( r = 0 ; r < cols ; r++ )
						{
							_tprintf( TEXT("%2d"), r + 1);
						}
						_tprintf(TEXT("\n"));
						for ( r=0 ; r< rows ;r++ )
						{
							_tprintf(TEXT("%2d"), r + 1);
							for ( c=0 ;c< cols ;c++ )
							{
								if(isMine[c][r]) _tprintf(TEXT(" *"));
								else _tprintf(TEXT(" -"));
							}
							_tprintf(TEXT("\n"));
						}
				}
			}
 		}
	}
	else 
	{
		cout << "Failed to open the Process" << endl;
	}
	CloseHandle(hProcess);
}
//Open a thread to get the processID and control the process
DWORD WINAPI Thread_1 (LPVOID lpParam)
{
	DWORD pProcessIds[1024],cbReturned,cProcesses;
	DWORD pProcessIdWanted = 0;
	TCHAR sProcessName[MAX_PATH] = TEXT("minesweeper.exe");

	unsigned int i;	
	if(!EnumProcesses(pProcessIds,sizeof(pProcessIds),&cbReturned))
	{
		return 1;
	}
	//Number of Processes returned from Enum from Byte to #
	cProcesses = cbReturned / sizeof(DWORD);

	for(i=0;i<cProcesses;i++)
	{
		if(pProcessIds[i] != 0)
		{
			pProcessIdWanted = GetProcessIdByName(pProcessIds[i], sProcessName);
			if(pProcessIdWanted != 0) break;
		}
	}
	pProcessIdWanted ? cout<< pProcessIdWanted <<endl : cout<<"Process Not Found" << endl;
	GetProcessBaseAddr(pProcessIdWanted);	
	return 0;
}

