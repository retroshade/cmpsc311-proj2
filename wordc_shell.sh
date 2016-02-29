#!/bin/bash

BEGIN=$(date +%s%N)									#Start time of script

gawk '{for (i=1;i<=NF;i++) print tolower($i)}' $1 |					#goes through every character in first file of command line arguments and makes them lowercase
	sort | 										#sorts the file alphabetically
	sed -r 's/[^[:alnum:]]//g' | 							#removes all non-alphanumeric characters
	sort |
	uniq -c > $2									#removes all repeated instances of any particular string 
											#and all commands are piped and redirected into the second file from command line arguments
	

#gawk '{printf "%s, %d", $2}' $2 > outtest3

END=$(date +%s%N)									#End time of script

printf 'The shell script took %d ns to execute for %s\n' $(($END - $BEGIN)) $1 >> $3	#prints elapsed time to separate file
