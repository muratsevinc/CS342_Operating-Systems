My own intercepting shell program that imitates normal shell in linux.

*Project Summary:*
A new shell program called isp implemented in this project. Basic linux commands can be run by isp such as: <br/>
- *cp file1.txt file2.txt* <br/>
- *ps aux | sort*
- *ls -al | sort*

*To use:*

	- Run the Makefile
	- Start the produced application isp by writing ./isp
	- Your shell is ready! You will see lines starting with "isp$" you can then use basic linux commands i.e. cp, mv, mkdir, ls etc.

*Used Methodoligies:*

	- Theads (main process keeps track of cli where child processes actually execute the command(s))
	- I/O re-direction (file descriptor manipulation for inter process communication)


*Project Report:*
There is a "Project Report.pdf" file which examines the performance of the shell program when I/O re-direction size changed.