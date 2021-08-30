hex2Testy01.c wmh 2020-11-15 : checks console input for validity, generates Testy command string from input, prints out byte-wise interpretation for confirmation

usage: h2T <list of hex bytes, halfwords, or words of the command separated by spaces
	input is restricted to only only 2-,4- or 8-digit numbers using hex characters 0-9,A-F
 		good input: 	'01 02 03 4567 89ABCDEF'	output: ':0102036745EFCDAB895E'				(quotes added)
		bad input:      '01 02 03 4567 9ABCDEF'		output:	'error: bad command arg 9ABCDEF'	(quotes added)

side note: the format validation function 'hexcmd2bin() at the bottom of this file is also used on the 'H745

executable: h2T.exe was compiled with MINGW-W64 gcc; source code should also compile on Linux or Mac. 
