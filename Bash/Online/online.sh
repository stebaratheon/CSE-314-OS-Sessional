#!/bin/bash


#<----------- Task 1  2  3----------------->
#taking number of arguments

args=$1
outputDir="/home/dell/Desktop/L3T2/outputDir"
dirName=$1


#-----------------Task 5 , 6.5-------------------
p=0
s=0
idx=0
getFiles()
{	
	cd "$1"
	directory="$1"
	echo "------------------"
	echo "In directory $directory"
	echo "-------------------"
	ls -l
	for k in *;
	do
		if [ -d "$k" ]
		then
			if [ "$(ls -A $k)" ]
			then 
				getFiles "$k"
			fi
		elif [ -f "$k" ]
		then
			fileName_="$k"
			echo " invextigating file         =========  $fileName_" 
			if [[ -x "$fileName_" ]];then
				echo "$fileName_ is currently executable"
				echo "changing permission ...."
				chmod -x $fileName_
				if [[ -x "$fileName_" ]];then
					echo "error"
				else 
					echo "$fileName_ is not currently executable"	
				fi				
			else 
				echo "$fileName_ is currently not executable"
			fi  
		fi
	done
	cd ../	
}
getFiles $dirName










