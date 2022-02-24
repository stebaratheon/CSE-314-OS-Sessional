#!/bin/bash


#<----------- Task 1  2  3----------------->
#taking number of arguments

args=$#
outputDir="/home/dell/Desktop/L3T2/outputDir"

#if ther is no arguments,then tell him to provide at least one argumen as filename
if [ $args -eq 0 ];then
	echo "Please give at least one argument as input file name and try again!!"
	exit 1
elif [ $args -eq 1 ];then
	fileName=$1
elif [ $args -eq 2 ];then
	dirName=$1
	fileName=$2
else
	echo "Invalid number of arguments! Please provide directory name and file name and try again."
	exit 1
fi

#<------------- Task 4 ------------------->
n=0
if [ -f $fileName ];then
	while read line;
	do
		#echo "Line number $((n+1)): $line"
		n=$((n+1))
		arr[$n]=$line
		#echo $line
	done < $fileName
	#echo ${arr[*]}
else
	echo "Please provide a valid input file name and try again."
fi

#-----------------Task 5 , 6.5-------------------
p=0
s=0
idx=0
getFiles()
{	
	cd "$1"
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
			extension=${fileName_##*.}
#start --> detecting the extension free files
			if [[ $fileName_ =~ "." ]]; then
				s=0
			else
				extension=""
			fi
#end
			ext_exists=0
			for i in "${!arr[@]}";
			do
				if [[ ${arr[$i]} = $extension  ]];
				then
					ext_exists=1
				fi
			done
			if [ $ext_exists -eq 0 ];then
				extensions[$p]=$extension
				p=$((p+1))
				FILES[$idx]=$fileName_
				idx=$((idx+1))
				cp $fileName_ $outputDir
			fi
		fi
	done
	cd ../	
}
getFiles $dirName
#<-------------getExtension function---------->
getExtension()
{
	fileName_=$1
	extension=${fileName_##*.}
	if [[ $fileName_ =~ "." ]]; then
		s=0
	else
		extension=""
	fi
}

#<--------------------- task 6,7----------------------->
echo ${extensions[*]}
randomDir="others"
# traversing the array and create directories in outputDir
cd $outputDir
for i in ${!extensions[@]};
do
	if [[ ${extensions[$i]} = "" ]]; then
		p=0	
	else
		dirName_=${extensions[$i]}
		if [ -d $dirName_ ]; then
			p=0
		else
			mkdir $dirName_
		fi
	fi
done
#making the random directory
mkdir $randomDir
#treking back
cd ../
#moving the files into real directories
plus="/"
desc="_desc.txt"
m=0
cd $outputDir


for k in ${!FILES[@]};
do
	getExtension ${FILES[$k]}
	if [[ $extension = "" ]];then
		targetDir="$outputDir$plus$randomDir"
	else	
		targetDir="$outputDir$plus$extension"
	fi	
	mv $fileName_ $targetDir
	#echo "got path $targetDir"
	#now go to the directory and add the description
	cd $targetDir
	#--making the file if not exists	
	if [ -f $desc ];then
		m=0
	else
		touch $desc
	fi
	#------now append the path of the filename to it
	finalPath="$targetDir$plus$fileName_"
	#echo "Final path becomes $finalPath"
	echo $finalPath >> $desc
	cat $desc	
	cd ../
done
cd ../


#<---------------------- task 8----------------------->
touch "log.csv"
fillCSV()
{	
	cd "outputDir"
	for k in *;
	do
		if [ -d "$k" ]
		then
			if [ "$(ls -A $k)" ]
			then 
				cd $k
				count=$(ls | wc -l) #number of files in a directory
				count=$((count-1))
				cd ../
				cd ../
				echo "$k,$count" >> log.csv
				cd "outputDir"
			fi
		fi
	done
	cd ../	
}
fillCSV 

