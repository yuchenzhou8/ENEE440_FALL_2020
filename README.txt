/*

README.txt

 	Author: Yuchen Zhou
	Date: 12/16/2020

*/

This folder contains the following contents:
	-- README.txt  
		A document that breifly lists and descries the contents of the .zip and direction
		Make sure you read Maintainer.pdf and Users_Manual.pdf before running the test scripts or running features manually using Realterm

	-- PC_code
		-- hex2Testy01
			contains an executable application that will generate a valid command for testy
			command line fields are parameters are described in the user manual
			please read readme_hex2Testy.txt before using h2T.exe

		-- testtermv04
			contains an executable application that will establish serial communication with the MCU then execute a written script provided by the user
			please read readme_testterm.pdf before runing testterm.exe

	-- test_scripts
		-- contains different test scripts that can be executable using testterm.exe
		-- for details about the test_script, please read the user manual
		-- testterm.exe
		-- h2t.exe

	-- Users_Manual
		-- Explains all the softwares and hardwares that are required for using testy
		-- Explains some setups must be done before 

	-- Testy Maintainer
		-- Explains all the initializations functions and device features of testy
		-- Explains how the processor works
		-- Reminders for some devices
		-- Nucleo Pin Map reference	

	-- project workspace

/*
 Not all functions are tested in the script, feel free use realterm to test them
 I think this project well designed, but I wish that we could have more time, espically the last three devices that we never touched except you breifly went over in class
 Make sure you press the reset button before running each script
*/			