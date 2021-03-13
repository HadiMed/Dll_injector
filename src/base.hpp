// Retrive base adress of an executable using CreateRemoteThread to bypass ASLR 

DWORD getPID(std::wstring name) // Get pid by name of process 
{
	DWORD PID = -1;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			std::wstring binaryPath = entry.szExeFile;
			if (binaryPath.find(name) != std::wstring::npos)
			{
				PID = entry.th32ProcessID;

				break;
			}
		}
	}

	CloseHandle(snapshot);
	return PID;
}



DWORD get_base(std::wstring name)
{

	
	DWORD base;
	HMODULE k32 = GetModuleHandleA("kernel32.dll");


	LPVOID get_handle = GetProcAddress(k32, "GetModuleHandleA");

	DWORD pid = getPID(name);




	HANDLE process = OpenProcess( // Open a Handle to process to inject to . 
		PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
		FALSE,
		pid
	);




	if (process == INVALID_HANDLE_VALUE) {
		printf("Some error occured code error  : %d", GetLastError());
		exit(0);


	}


	HANDLE thread = CreateRemoteThread( // Create the remote thread
		process,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)get_handle,
		NULL,
		NULL,
		NULL

	);


	WaitForSingleObject(thread, INFINITE);

	GetExitCodeThread(thread, &base);


	CloseHandle(thread);

	

	return base;
}
