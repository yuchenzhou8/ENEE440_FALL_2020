//04testterm115200.c wmh 2020-11-18 : minor changes
//03testterm115200.c wmh 2020-04-13 : changes some nomenclature and messages
// compiles on Mingw-64 with 'gcc -Wall 04testterm115200.c -o testterm.exe'
// 
// see 'readme_testerm.txt' for usage

	#include <Windows.h>
	#include <stdio.h>
	#include <string.h>
	
	#define lpBUFSIZE 600
	#define rxBUFSIZE 600
	
//----------------------------	
//functions from https://codeload.github.com/waynix/SPinGW/zip/master	

/**
	\brief Read data from the serial port
	\param hSerial		File HANDLE to the serial port
	\param buffer		pointer to the area where the read data will be written
	\param buffersize	maximal size of the buffer area
	\return				amount of data that was read
	*/
DWORD readFromSerialPort(HANDLE hSerial, char * buffer, int buffersize)
{
    DWORD dwBytesRead = 0;
    if(!ReadFile(hSerial, buffer, buffersize, &dwBytesRead, NULL)){ 
	//ReadFile stuff at https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx 
        perror("readFromSerialPort"); exit(2);
    }
    return dwBytesRead;
}

/**
	\brief write data to the serial port
	\param hSerial	File HANDLE to the serial port
	\param buffer	pointer to the area where the read data will be read
	\param length	amount of data to be read
	\return			amount of data that was written
	*/
DWORD writeToSerialPort(HANDLE hSerial, char * data, int length)
{
	DWORD dwBytesRead = 0;
	if(!WriteFile(hSerial, data, length, &dwBytesRead, NULL)){
		perror("writeFromSerialPort"); exit(2);
	}
	return dwBytesRead;
}

//-----------------------	
int main(int argc, char *argv[]) //arguments are COM port ident, e.g. 'COM18' and script name, e.g. 'test_script.txt'
{
	
	HANDLE hComm;                          	// Handle to the Serial port
	BOOL   Status;
	char   lpBuffer[lpBUFSIZE]; //Holds script comments and command. Enough to buffer for one 512 byte 'sector' being sent 
	//!!TODO : make sure rx buffer is big enough. 												
	char   rxBuffer[rxBUFSIZE]; //buffer for reply.  Enough to buffer for one 512 byte 'sector' being received
	char scriptname[40]; 
	char ComPortName[40]= "\\\\.\\";	
	int i,len,rxlen;
	DCB dcbSerialParams = { 0 };	
	COMMTIMEOUTS timeouts = { 0 };
	
	if(argc < 2) { // not enough arguments
		printf("Program wants arguments 'COMn' (Windows name for serial connection) and '<scriptname>' e.g.,\n");
		printf("\t$>testterm COM18 test_script.txt\n");
		printf("Please try again\n\n");
		return 0;
	}
	
	strcat(ComPortName,argv[1]);
	strcpy(scriptname,argv[2]);

	printf("Starting testterm.exe\n");  
	printf("See '_readme_testterm.txt' and 'test_script_demo.txt' for more info\n");
	printf("Attempting connection to target on %s; testing with %s",ComPortName,scriptname);
	
	FILE *fp=fopen(scriptname,"rb");
	//test script ok?
	if(fp== NULL) {perror("fopen"); exit(1);} 

	
	//----------------------------------- Opening the Serial Port --------------------------------------------

	hComm = CreateFile( ComPortName,                       // Name of the Port to be Opened
						GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
						0,                                 // No Sharing, ports cant be shared
						NULL,                              // No Security
						OPEN_EXISTING,                     // Open existing port only
						0,                                 // Non Overlapped I/O
						NULL);                             // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE) { printf("\n\nERROR - Port %s can't be opened", ComPortName); exit(2);}
	else printf("\nserial port %s is ready", ComPortName);

	// Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(hComm, &dcbSerialParams);      	//get current settings
	if (Status == FALSE) { printf("\nERROR --  GetCommState() failed"); exit(3); } 

	//set COM parameters
	dcbSerialParams.BaudRate = CBR_115200;    // Setting BaudRate = 115200
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 
	if (Status == FALSE) { printf("\nERROR -- SetCommState() failure"); exit(4); } 
	else { //display the contents of the DCB Structure
			printf("\nserial parameter settings successful");
			printf("\n       Baudrate = %d", (int)dcbSerialParams.BaudRate);
			printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
			printf("\n       StopBits = %d", dcbSerialParams.StopBits);
			printf("\n       Parity   = %d", dcbSerialParams.Parity);
	}
	
	//set timeouts
	timeouts.ReadIntervalTimeout         = 50;
	timeouts.ReadTotalTimeoutConstant    = 50;
	timeouts.ReadTotalTimeoutMultiplier  = 10;
	timeouts.WriteTotalTimeoutConstant   = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (SetCommTimeouts(hComm, &timeouts) == FALSE){printf("\nERROR -- SetCommTimeouts() failure"); exit(4); } 
	else printf("\nSerial timeout settings successful");

	
	//display user instructions and start interpreting test script
	printf("\n\nScript information messages starting with '#'  will display one line at a time");
	printf("\nHit 'Enter' to advance to each next message or command operation");
	printf("\nIMPORTANT: in order to avoid disrupting operation of testterm you must wait for a");
	printf("\n'bytes received' message before proceeding with the next step in the script.");
	printf("\n");
	printf("\nBeginning test script '%s'\n",scriptname);	
	
	while( fgets(lpBuffer,200,fp) != NULL ) { //we haven't finished reading the input file
		if(lpBuffer[0] == '#') { //this is a comment line so print it to console
			strtok(lpBuffer,"\n"); 	//strip the newline
			printf("%s",lpBuffer);	// then display the script message
			getchar();				//   then block until user hits 'Enter' (which adds a newline)
		}
		else { //this is a command so send it
			strtok(lpBuffer," \t\n"); 								//clean off the end of the script's string, just in case
			strcat(lpBuffer,"\n");									//  add a newline back 
			len=strlen(lpBuffer);									//  need length for writeToSerialPort()
			len= writeToSerialPort(hComm,lpBuffer,len);				//send the command string 
			printf("\t%i bytes sent.\n\ttxmsg= %s",len-1,lpBuffer);	//message brings its own newline
			
			//get the command reply 
			rxlen= readFromSerialPort(hComm, rxBuffer, 600);		//this blocks until timeout
			printf("\n\t%i bytes received.\n\trxmsg= ",rxlen);		//kludge due to lack of buffer cleanout between messages
			for(i=0;i<rxlen;i++) printf("%c",rxBuffer[i]);			//message brings its own newline
			printf("\n");											// in case of no message or no newline
		}
	}
	return 0;
}