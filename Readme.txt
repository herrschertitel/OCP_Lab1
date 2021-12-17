lab1test
========
Набор тестовых программ для лабораторной № 1 по ОСП. Для сборки - make all.
//A set of test programs for laboratory No. 1 for OSB. To build - make all.

01. lab1test
------------
Программа для проверки заданной библиотеки на предмет её соответствия требованиям API плагинов.
//A program for checking a given library for its compliance with the plugins API requirements.

Пример запуска: //Launch example:
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./ -A -N --exe elf,pe32,coff ./file_test 

02. lab1tvhN3249
------------
Программа для вызова из плагина функции проверки файла. Дополнительно выводит информацию о плагине.
//The program for calling the file check function from the plugin. Additionally displays information about the plugin.

03. libtvhN3249.so
-------------
Пример плагина для лабораторной № 1. Позволять в указанном каталоге найти файлы, который в форматах elf,pe32, coff and a.out.
//An example of a plug-in for laboratory No. 1. Allow in the specified directory to find files in the formats elf, pe32, coff and a.out.

Поддерживаемые опции: //Supported options:
 --exe v
*Пример запуска:
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./ --exe pe32,coff  ./file_test 
 
04. libtvhN3249-2.so
-------------
найти все файлы, у которых размер меньше чем, заданный размер в котологе
//find all files with a size less than the specified size in the catalog

*Пример запуска:
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./ --size 1000 ./file_test 
 
05. Опции A O N
-------------
*Пример запуска:
   найти все файлы type elf или pe32 and size < 1000
   //find all files type elf or pe32 and size <1000
 $ LAB1DEBUG=1 ./lab1tvhN3249  -A --exe elf,pe32 --size 1000 ./file_test 
   найти все файлы type elf или pe32 or size < 1000
   //find all files type elf or pe32 or size <1000
 $ LAB1DEBUG=1 ./lab1tvhN3249  -0 --exe elf,pe32 --size 1000 ./file_test 
   найти все файлы type не elf или pe32 and size > 1000
   //find all files type not elf or pe32 and size> 1000
 $ LAB1DEBUG=1 ./lab1tvhN3249  -A -N --exe elf,pe32 --size 1000 ./file_test 

06. опция P
------------
сначала я переместил все файлы .so в другую папку (можно любую потому что я использую рекурсивный поиск)
и потом если без опции -P программа нельзя компилировать потому что в каталоге (test1 - содержит ./main) щас нет файлы .so
потом с помощью -P программа находил файлы .so и подключить к ними и выводить на консоль путь к файлам .so
 например переместить все файлы ".so" в каталоге "file_test"
 //first I moved all the .so files to another folder (you can use any one because I use recursive search)
//and then if without the -P option the program cannot be compiled because there are no .so files in the directory (test1 - contains ./main)
//then using -P the program found the .so files and connect to them and display the path to the .so files to the console
//for example move all ".so" files in the "file_test" directory
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./file_test -A --exe elf,pe32 --size 1000 ./file_test 
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./file_test -O --exe elf,pe32 --size 1000 ./file_test 
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./file_test -A -N --exe elf,pe32 --size 1000 ./file_test 
 $ LAB1DEBUG=1 ./lab1tvhN3249 -P ./file_test -O -N --exe elf,pe32 --size 1000 ./file_test 

	./ это значит мы начинает с каталога, в котором содержит ./main
	//./ means we start with the directory that contains ./main

07. опция h
------------
$ LAB1DEBUG=1 ./lab1tvhN3249 -h

08 опция v
------------
$ LAB1DEBUG=1 ./lab1tvhN3249 -v