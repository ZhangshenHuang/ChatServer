This is a cluster chat server project based on muduo network library written in C++ language.If you want run it on your computer, you have to configure the following environment: 
1. linux operation system
2. cmake and g++ tools
3. JSON for Modern C++
4. boost and muduo network libraries
5. mysql
6. nginx
7. redis

If you have finished environmental configuration, you can run "chmod +x autobuild.sh && ./autobuild.sh" or change dictionary into build folder and operate according to compile.md on your terminal. Then you can change dictionary into bin and run "ChatServer" executable programs through command "./ChatServer 127.0.0.1 6000" and "ChatClient" executable programs through command "./ChatClient 127.0.0.1 8000" to log in on your personal computer.

The functions of each folder are described as follows:
1. bin: executable binary files
2. build: the compiled intermediate files
3. include: header files
4. src: source files
5. thirdparty: part of third party files that the project relies on
