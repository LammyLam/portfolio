This is a project for COMP2322

*********************************************
***** HOW TO COMPILE AND RUN THE SERVER *****
*********************************************

1.  Place the .java file and your server file (like .html or .jpg) in a your preferred directory (say D:\MyServer).
2.  Open command prompt, change the directory (cd) the preferred directory) (e.g. cd D:\Myserver).
3.  Compile the .java file by typing "javac MultiThreadServer.java". It is recommended to use Java 8 for compilation.
4.  A MultiThreadServer.class file should appear in the preferred directory.
5.  Run the .class file via the command "java MultiThreadServer".
6.  When you see "Server main running" in the command prompt, the server is up and running. 
7.  You can connect to the server via IPv4 Address + :8181. For example, a server address will be 192.168.0.223:8181
8.  "Listen socket established" will appear subsequently to annouce the creation of listen socket.
9.  When the server is running and a client attempts to connect, "Connection Accepted from IP:" will be shown.
10.  The log is printed in the command prompt and is stored in the serverLog.txt, which is in the preferred directory
11. To terminate the server, simply exit from the command prompt.


************************************
***** FORMAT OF THE SERVER LOG *****
************************************

       Client IP             Client Accepted Time  -->      Server Responded Time      Server Response     Request File Name
e.g. 192.168.0.125   Sat, 20 Mar 2021 04:53:28 GMT --> Sat, 20 Mar 2021 04:53:28 GMT  304 Not Modified     helloworld.jpg   