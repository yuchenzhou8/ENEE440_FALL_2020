//hex2Testy01.c wmh 2020-11-15 : generate Testy command string from console input, check for validity, print out byte interpretastion for confirmation
//	input is restricted to only only 2-,4- or 8-digit numbers using hex characters 0-9,A-F
// 		good input: 	'01 02 03 4567 89ABCDEF'	output: ':0102036745EFCDAB895E'				(quotes added)
//		bad input:      '01 02 03 4567 9ABCDEF'		output:	'error: bad command arg 9ABCDEF'	(quotes added)
// side note: the format validation function 'hexcmd2bin() at the bottom of this file is also used on the 'H745

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEXBUFSIZE 100					//where hex commands are assembled
#define BINBUFSIZE (HEXBUFSIZE/2+1)		//where binary translations of commands are assembled
#define	CMDARGNUM 20					//maximum number of arguments for a command

typedef struct arg{
	uint32_t len;
	int val;
} arg_t;

//defined below
uint32_t write1hex(uint32_t, char *phex); 		//writes byte of val as two hex ASCII chars at phex, returns byte value
uint32_t write2LEhex(uint16_t val, char *phex); //writes two bytes of val little-endian as four hex ASCII chars at phex, return sum of bytes
uint32_t write4LEhex(uint32_t val, char *phex); //writes four bytes of val little-endian as eight hex ASCII chars at phex, return sum of bytes
int hexcmd2bin(char *hexcmd, uint8_t *bincmd);	//processes a text buffer containing a Testy command string into a binary command buffer; 

int main(int argc, char *argv[])	 // generate Testy command string from console input (see info at top), print it, then decode it
{
	uint8_t tst;
	uint32_t bytesum=0;
	uint8_t chksum=0;
	char badchar;
	char outbuf[HEXBUFSIZE];	//buffer where a hex command is assembled
	char *poutbuf=outbuf;
	int i,n;
	arg_t cmdarg[CMDARGNUM];	//array to hold command arguments before translation to hex
	uint8_t bincmd[BINBUFSIZE];	//buffer where a hex command is disassenmble for testing
	
	if(argc <3) { printf("error: not enough values"); exit(1); }  							//must have at least device and command values
	if((cmdarg[1].len=strlen(argv[1])) != 2) { printf("error: bad device num"); exit(2); }	//'device' field of a command must be two hex digits 
	if((cmdarg[2].len=strlen(argv[2])) != 2) { printf("error: bad command num"); exit(3); }	//'function' field of a command must be two hex digits
	for(i=3;i<argc;i++) {
		cmdarg[i].len = strlen(argv[i]);
		if( (cmdarg[i].len != 2) && (cmdarg[i].len != 4) && (cmdarg[i].len != 8) ) { printf("error: bad command arg %s",argv[i] ); exit(4); }	//command field must be two, four, aor eight hex digits  
	}
	//here with arguments checked for length
	
	*poutbuf++=':';
	
	for(i=1;i<argc;i++) {	//uses %X%c trick to identify bad values
		if(cmdarg[i].len == 2) {  
			if( (tst = sscanf(argv[i],"%02X%c",&cmdarg[i].val,&badchar)) != 1) { printf("error: bad hex: %s",argv[i] ); exit(5); }   
			bytesum += write1hex(cmdarg[i].val,poutbuf);	
			poutbuf +=2;
		}
		if(cmdarg[i].len == 4) { 
			if( (tst = sscanf(argv[i],"%04X%c",&cmdarg[i].val,&badchar)) != 1) { printf("error: bad hex: %s",argv[i] ); exit(6); }   
			bytesum += write2LEhex(cmdarg[i].val,poutbuf);	
			poutbuf +=4;
		}
		if(cmdarg[i].len == 8) { 
			if( (tst = sscanf(argv[i],"%08X%c",&cmdarg[i].val,&badchar)) != 1) { printf("error: bad hex: %s",argv[i] ); exit(7); }   
			bytesum += write4LEhex(cmdarg[i].val,poutbuf);	
			poutbuf +=8;
		}
	}

	//here with arguments validated and command string built except for checksum and newline
	chksum = 0xFF & (-(bytesum & 0xFF));
	write1hex(chksum,poutbuf); //chksum
	poutbuf +=2;
	*poutbuf++ ='\n';	//EOL
	*poutbuf++ ='\0';	// EOS

	
	printf("cmd string: %s",outbuf);	//command string for pasting 
	
	//here with Testy command in outbuf -- now decode it to binary to test hexcmd2bin() function to be used on the micro
	if( (n=hexcmd2bin(outbuf, bincmd)) < 0) { printf("bad hex command\n") ;	}		//convert the hex command string into binary command
	else { 
		for(i=0; i<n;i++) printf("%02X ",bincmd[i]);	//display bytes of the binary command
	}

	return 0;
}

// --- helper functions
uint32_t write4LEhex(uint32_t val, char *phex) //writes four bytes of val little-endian as eight hex ASCII chars at phex
// and returns sum of individual bytes of the value

{
	uint32_t tmp;
	uint32_t bytesum=0;
	tmp= val%256;	//least-significant byte (byte0) of val
	val= val/256;
	bytesum += tmp;
	sprintf(&phex[0],"%02X",tmp);
	tmp= val%256;	//byte1
	val= val/256;
	bytesum += tmp;
	sprintf(&phex[2],"%02X",tmp);
	tmp= val%256;	//byte2
	val= val/256;
	bytesum += tmp;
	sprintf(&phex[4],"%02X",tmp);
	tmp= val%256;	//most-significant byte (byte3) of val
	bytesum += tmp;
	sprintf(&phex[6],"%02X",tmp);
	return bytesum;
}

uint32_t write2LEhex(uint16_t val, char *phex) //writes two bytes of val little-endian as four hex ASCII chars at phex
// and returns sum of individual bytes of the value
{
	uint16_t tmp;
	uint32_t bytesum=0;
	tmp= val%256;	//least-significant byte (byte0) of val
	val= val/256;
	bytesum += tmp;
	sprintf(&phex[0],"%02X",tmp);
	tmp= val%256;	//most-significant byte (byte1) of val
	bytesum += tmp;
	sprintf(&phex[2],"%02X",tmp);
	return bytesum;
}

uint32_t write1hex(uint32_t val, char *phex) //writes byte of val as two hex ASCII chars at phex
// and returns byte value

{
	uint8_t tmp;
	uint32_t bytesum=0;
	tmp= val%256;	//we shouldn't need to do this but is might rescue us if user non-uint8_t argument for val
	bytesum += tmp;
	sprintf(&phex[0],"%02X",tmp);
	return bytesum;
}

int hex2bin(char HASCII)   // translate uppercase hex ASCII to bin, return value 0-15 or -1 if fail
{   
	if('0'<=HASCII && '9'>=HASCII) return(HASCII - '0');   
	if('A'<=HASCII && 'F'>=HASCII) return(HASCII - 'A' + 10);  
	return -1;   
}   

int hexcmd2bin(char *hexcmd, uint8_t *bincmd)	//processes a text buffer containing a Testy-format command 'string'  into a binary command buffer; 
//	returns #of bytes in bincmd or -1 if format violation
{
	int i;
	uint8_t lonybble,hinybble,byte;
	uint8_t chk=0;
	if(hexcmd[0] != ':' ) return -1;	//bad format -- no start symbol
	for(i=1;i<HEXBUFSIZE;) {
		if( (hexcmd[i]=='\n')| (hexcmd[i]=='\r')) break;//we're at the end of a command
		//here if parsing should continue
		if( ((hinybble=hex2bin(hexcmd[i]))>=0) && ((lonybble=hex2bin(hexcmd[i+1]))>=0) ) byte=16*hinybble+lonybble; //successful hex byte to bin conversion  
		else return -2;					//bad hex byte
		//here when the two characters of a hex byte are successfully converted into a value in 'byte'
		*bincmd++ = byte;				//add byte to the binary cmd string
		chk += byte;					// and to the checksum (overflows are expected and desired)
		i += 2;							//  then advance to convert the next byte
	}
	//here iff the text string lies between ':' and '\n' or '\r' and consists exclusively of hex bytes !!TODO what about too long? 
	if ( chk != 0 ) return -3;			//bad checksum	
	return i/2-1;						//number of bytes in bindcmd, not including checksum
}
		
