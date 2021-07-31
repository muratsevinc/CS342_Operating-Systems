**My own intercepting shell program that imitates normal shell in linux.**

*To use:*

	- Run the Makefile
	- Start the produced application isp by writing ./isp
	- Your shell is ready! You will see lines starting with "isp$" you can then use basic linux commands i.e. cp, mv, mkdir, ls etc.

*Used Methodoligies:*

	- Theads (main process keeps track of cli where child processes actually execute the command(s))
	- I/O re-direction (file descriptor manipulation for inter process communication)
 

*Project Report:*
	<br />   There is a "Project Report.pdf" file which examines the performance of the shell program when I/O re-direction size changed.