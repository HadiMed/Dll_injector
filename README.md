# Dll_injector
simple tool that uses Windows APIs to trick a process to load dll to memory 



# Windows APIs used :



<b> OpenProcess </b> : get a handle for a process in memory knowing its PID . 
<b> VirtualAllocEx</b> : Allocate memory in a process knowing its handle . 
<b> WriteProcessMemory </b> : Write to a memory in a process knowing its handle . 
<b> LoadLibraryW</b> : load dll into a process (This is the function that our process need to execute ) . 
<b> CreateRemoteThread</b> : Create thread in another process knowing its handle . 

# How it works ? 


first it search for the pid of the process given in arguments and it open a handle to the process to write in its memory the path of the dll , then it create a remote thread  calling the function loadlibraryW to import the dll .
for the part : <b> bypassing ASLR </b> 
(this works because kernel32 would have the same base address in all process and the same offsets for all the functions ) so loadlibrary will have the same address in our process (Inject.exe) and in the process we want to inject to . 

# Usage : 
  <font size="+2"> <b> Inject.exe process_name Fullpathofdll.dll </b> </font>
