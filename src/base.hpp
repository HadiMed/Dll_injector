// Retrive base adress of an executable using CreateRemoteThread to bypass ASLR 

DWORD getPID(std::wstring name) ;// Get pid by name of process 

DWORD get_base(std::wstring name) ; // get base address of a process knowing its name . 
