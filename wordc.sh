#!/bin/bash
#sets the SHELL environment to bash, in case it was not already set

if [[ -z "$1" || -z "$2" || -z "$3" ]];
	then
		echo Not enough file arguments.
		echo Terminating process. Please try again with 1 input and 2 outputs.
		exit 0
fi

#if [[ -z "$2" ]]; 	
#	then
#		echo There is no output countfile
#fi

#if [[ -z "$3" ]];	
#	then
#		echo There is no output runtimefile
#fi

#echo Terminating process. Please try again with 1 input and 2 output files.

#exit 0

BEGIN=$(date +%s%N)									#Start time of script

gawk '{for (i=1;i<=NF;i++) print tolower($i)}' $1 |					#goes through every character in first file of command line arguments and makes them lowercase
	sort | 										#sorts the file alphabetically
	sed -r 's/[^[:alnum:]]//g' | 							#removes all non-alphanumeric characters
	uniq -c |									#removes all repeated instances of any particular string
	gawk '{print $2 ", " $1}' > $2							#formats each line in the requested manner 
											#and all commands are piped and redirected into the second file from command line arguments

END=$(date +%s%N)									#End time of script
DIFF=$(($END-$BEGIN))

echo The shell script took "$DIFF" ns to execute for "$1" >> $3				#prints elapsed time to separate file

