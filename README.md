This is a cluster chat server project based on muduo network library written in C++ language.If you want run it on your computer, you have to configure the following environment: 
1. linux operation system
2. cmake tool
3. JSON for Modern C++
4. boost and muduo network library
5. mysql
6. nginx
7. redis

If you have finished environmental configuration, you can run "chmod +x autobuild.sh && ./autobuild.sh" or change dictionary into build folder and operate according to compile.md on your terminal.

Or if you don't want to configure the project compilation environment, you just want to see the results of the code running, you can change dictionary into bin and run server and client executable programs through command "./ChatServer 127.0.0.1 6000" and "./ChatClient 127.0.0.1 8000" on different terminals.

The functions of each folder are described as follows:
bin: executable binary files
build: the compiled intermediate files
include: header files
src: source files
thirdparty: third party files that the project relies on
