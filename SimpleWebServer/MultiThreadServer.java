import java.io.*;
import java.net.*;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 * MultiThreadServer creates a server object that can be run by a thread
 * To create a server object, a connectionSocket, which is accepted by a listenSocket, should passed in as constructor argument
 * The default port is 8181
 * It can process two types request method: GET and HEAD
 * Other methods are regarded invalid and therefore 400 Bad Request is generated
 * If the header lines of the request message contain If-Modified-Since, it will check the date and time to decide whether to send the entity
 *
 */
public class MultiThreadServer implements Runnable {
    public static final int serverPort = 8181;      // Constant integer representing port number
    public static final int timeoutMax = 5000;      // Constant integer representing the maximum timeout in ms
    public PrintWriter logWriter = null;            // PrinterWriter to write server log
    Socket threadSocket = null;                     // The connection socket of this thread
    BufferedReader collectClient = null;            // The buffered reader for reading input stream from client
    DataOutputStream sendClient = null;             // The output stream to client
    String[] currentMessage = null;                 // An array of String storing the whole HTTP message
    int messageLineCount;                           // Number of lines in the request message
    String methodLine = null;                       // String to store the method line
    String ifHeaderLine = null;                     // String to store the If-Modified-Since header line
    StringTokenizer lineTokenizer = null;           // StringTokenizer to split the request or header lines
    Boolean isGet;                                  // Boolean value to determine if such method appears in the HTTP request message
    Boolean isHead;                                 // Ditto
    Boolean ifSince;                                // Ditto
    Boolean isValidPath;                            // Boolean value to determine if the request path is valid
    Boolean isValidVersion;                         // Boolean value to determine if the request version is valid
    Date clientModifiedTime;                        // Date to store the date time of If-Modified-Since
    SimpleDateFormat httpDateFormat;                // Simple date format for the Last-Modified and If-Modified-Since
    SimpleDateFormat rfc850DateFormat;              // Simple date format for the obsolete HTTP date format
    SimpleDateFormat asciiDateFormat;               // Simple date format for ASCII time format
    int dateFormat;                                 // int to store what date should be used
    Date acceptedTime;                              // Date to store the time and date the client connection is accepted


    /**
     * Constructor of the class MultiThreadServer
     * @param connectionSocket: the socket accepted from the listen socket
     */
    MultiThreadServer (Socket connectionSocket, Date acceptedTime) {
        this.threadSocket = connectionSocket;       // When a server thread is created, the connectionSocket is assigned to the threadSocket
        this.currentMessage = new String[100];      // Create an array of String to store the whole HTTP request message
        this.isGet = false;                         // Initialize boolean value to detect the request method
        this.isHead = false;
        this.ifSince = false;
        this.isValidPath = false;
        this.isValidVersion = false;
        messageLineCount = 0;                   // Count the number of the lines in the HTTP message
        dateFormat = -1;                        // Set the date format to -1 for debug
        this.httpDateFormat = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss z", Locale.US); // Create a HTTP time and date formatter mentioned in RFC 112
        this.httpDateFormat.setTimeZone(TimeZone.getTimeZone("GMT")); // Set timezone to GMT, as mentioned in RFC 1123
        this.rfc850DateFormat = new SimpleDateFormat("EEEEEEEEE, dd-MMM-yy HH:mm:ss z", Locale.US); // Create a HTTP time and date formatter mentioned in RFC 850
        this.rfc850DateFormat.setTimeZone(TimeZone.getTimeZone("GMT")); // Set timezone to GMT
        this.asciiDateFormat = new SimpleDateFormat("EEE MMM d HH:mm:ss yyyyy", Locale.US); // Create a ASCII time and date formatter
        this.asciiDateFormat.setTimeZone(TimeZone.getTimeZone("GMT")); // Set timezone to GMT

        this.acceptedTime = acceptedTime;
        try {
            // The buffered reader and output stream for transmission is also set up
            this.collectClient = new BufferedReader(new InputStreamReader(this.threadSocket.getInputStream()));
            this.sendClient = new DataOutputStream(this.threadSocket.getOutputStream());
            // The buffered write is made to write server log
            this.logWriter = new PrintWriter(new FileWriter("serverLog.txt", true));
        } catch (IOException e) {
            System.out.println("Buffered reader or output stream IOException. Exiting");
            System.exit(1);
        }
    }

    /**
     * run() is the function a thread runs when the thread is started
     * It receives input from the socket input stream and stores them as String
     * The Strings are then checked to find out the request method and options
     * Corresponding methods in the class are then run
     */
    public void run () {
        String tempMessage = "";
        int tempChar = 0;
        try {
            // When the thread is running, we keep reading the lines from the stream
            while ((tempChar = collectClient.read()) != -1 && collectClient.ready()) {
                tempMessage += (char) tempChar;
                if (tempMessage.endsWith("\r\n")) {
                    currentMessage[messageLineCount] = tempMessage;
                    messageLineCount++;
                    tempMessage = "";
                }
            }
        } catch (IOException e) {
            System.out.println("Input stream IOException. Exiting");
            System.exit(1);
        }

        // After fetching the entire HTTP request message, we need to find out the request method
        // It will be either GET or HEAD
        // We also need to determine if the message contains If-Last-Modified header
        // The corresponding lines are saved in the variables
        for (int i=0; i<messageLineCount; i++) {
            // Use StringTokenizer to split the string
            lineTokenizer = new StringTokenizer(currentMessage[i]);
            if (lineTokenizer.hasMoreElements()) {
                switch (lineTokenizer.nextToken()) {
                    case "GET":
                        isGet = true;
                        methodLine = currentMessage[i];
                        break;
                    case "HEAD":
                        isHead = true;
                        methodLine = currentMessage[i];
                        break;
                    case "If-Modified-Since:":
                        ifSince = true;
                        ifHeaderLine = currentMessage[i];
                        break;
                }
            }
        }
        // Check if the method line is a valid one (containing only a request method, path and the protocol version
        if (methodLine != null) {
            lineTokenizer = new StringTokenizer(methodLine);
            if (lineTokenizer.hasMoreElements()) {
                lineTokenizer.nextToken(); // skip the GET/HEAD request type
                // if it is a path, it should start with "/"
                if (lineTokenizer.hasMoreElements()) {
                    if (lineTokenizer.nextToken().startsWith("/")) isValidPath = true;
                    else isValidPath = false;
                    // if should exist a HTTP version of 1.0 or 1.1
                    if (lineTokenizer.hasMoreElements()) {
                        String requestVersion = lineTokenizer.nextToken();
                        if (requestVersion.equals("HTTP/1.0") || requestVersion.equals("HTTP/1.1"))
                            isValidVersion = true;
                        else isValidVersion = false;
                    }
                }
            }
        }



        // Check what date is used, by using exception
        if (ifHeaderLine != null) {
            try {
                httpDateFormat.parse(ifHeaderLine.substring(19));
                dateFormat = 0;
            } catch (ParseException e1) {
                try {
                    rfc850DateFormat.parse(ifHeaderLine.substring(19));
                    dateFormat = 1;
                } catch (ParseException e2){
                        try {
                            asciiDateFormat.parse(ifHeaderLine.substring(19));
                            dateFormat = 2;
                        } catch (ParseException e3) {
                            // If neither the date format is used, send a bad request
                            isHead = false;
                            isGet = false;
                        }
                    }
                }
        }

        // We can now parse the format now
        try {
            switch (dateFormat) {
                case 0:
                    clientModifiedTime = httpDateFormat.parse(ifHeaderLine.substring(19));
                    break;
                case 1:
                    clientModifiedTime = rfc850DateFormat.parse(ifHeaderLine.substring(19));
                    break;
                case 2:
                    clientModifiedTime = asciiDateFormat.parse(ifHeaderLine.substring(19));
                    break;
            }
        } catch (ParseException ignored) {}


        // We then determine which method to use, and whether we need to reply with Last-Modified:
        try {
            // If it is GET and not HEAD, and the message is complete and valid, we use getMethod with ifSince as parameter
            if (isGet && !isHead && isValidVersion && isValidPath) getMethod(ifSince);
            // Otherwise, if it HEAD and not GET, we use headMethod without ifSince, since HEAD method will never transmit the entity
            else if (isHead && !isGet && isValidVersion && isValidPath) headMethod(ifSince);
            // Otherwise, if it is neither, we generate a 400 Bad Request message
            else badRequest();
        } catch (Exception e) {
            System.out.println("HTTP Response Exception. Exiting.");
            System.exit(1);
        }

        // Close the PrintWriter for the server log and the server socket after completing the response
        try {
            logWriter.close();
            threadSocket.close();
        } catch (IOException e) {
            System.out.println("IOException when closing socket. Exiting.");
            System.exit(1);
        }
    }

    /**
     * getMethod generates corresponding message to request method GET
     * @param ifSince: whether If-Modified-Since is in the header lines
     */
    void getMethod (Boolean ifSince) throws Exception {
        lineTokenizer = new StringTokenizer(methodLine);
        lineTokenizer.nextToken(); // Skip the GET token
        String requestPath = lineTokenizer.nextToken(); // Get the file name
        if (requestPath.startsWith("/")) requestPath = requestPath.substring(1); // Remove the forward slash if necessary
        File requestFile = new File(requestPath); // Create a File object with the given file path
        // If the file exists, we determine whether is contains ifSince
        if (requestFile.exists()) {
            // Placing the file in a input stream
            FileInputStream requestStream = new FileInputStream(requestPath);
            int fileLength = (int) requestFile.length();
            byte[] fileInBytes = new byte[fileLength];
            requestStream.read(fileInBytes);
            // If If-Modified-Since is present in the header lines, we check whether the file is updated
            if (ifSince) {
                Date serverModifiedTime = httpDateFormat.parse(httpDateFormat.format(requestFile.lastModified()));

                // Then we compare the server time and the client time
                // If the client time is before the server time, it is not updated
                // The file should be retransmitted and 200 is sent
                if (clientModifiedTime.before(serverModifiedTime)) {

                    // Write the status lines and Last-Modified line
                    sendClient.writeBytes("HTTP/1.0 200 OK\r\n");
                    sendClient.writeBytes("Last-Modified: " + httpDateFormat.format(requestFile.lastModified()) + "\r\n");
                    sendClient.writeBytes("\r\n");
                    sendClient.write(fileInBytes, 0, fileLength);
                    // Write to the log after GET operation in both console and log file
                    System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                            httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
                    logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                            httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
                }

                // Otherwise, if it is updated, no file needs to be retransmitted and 304 is sent
                else {
                    sendClient.writeBytes("HTTP/1.0 304 Not Modified\r\n");
                    sendClient.writeBytes("Last-Modified: " + httpDateFormat.format(requestFile.lastModified()) + "\r\n");
                    sendClient.writeBytes("\r\n");
                    // Write to the log after GET operation in both console and log file
                    System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                            httpDateFormat.format(new Date(System.currentTimeMillis())), "304 Not Modified", requestFile.getName());
                    logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                            httpDateFormat.format(new Date(System.currentTimeMillis())), "304 Not Modified", requestFile.getName());
                }
            }

            // If there is no If-Modified-Since header line, just transmit the file like the normal GET method
            // And 200 is sent in this case
            else {
                sendClient.writeBytes("HTTP/1.0 200 OK\r\n");
                sendClient.writeBytes("Last-Modified: " + httpDateFormat.format(requestFile.lastModified()) + "\r\n");
                sendClient.writeBytes("\r\n");
                sendClient.write(fileInBytes, 0, fileLength);
                // Write to the log after GET operation in both console and log file

                System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                        httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
                logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                        httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
            }
        }
        // Otherwise, if file is not found, we send 404
        else {
            sendClient.writeBytes("HTTP/1.0 404 File Not Found\r\n");
            // Write to the log after GET operation in both console and log file
            System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                    httpDateFormat.format(new Date(System.currentTimeMillis())), "404 File Not Found", requestFile.getName());
            logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                    httpDateFormat.format(new Date(System.currentTimeMillis())), "404 File Not Found", requestFile.getName());
        }
    }

    /**
     * headMethod is similar to getMethod(ifSincce)
     * Tt does not take ifSince into account and always generate a response without entity
     */

    void headMethod (Boolean ifSince) throws Exception{
        lineTokenizer = new StringTokenizer(methodLine);
        lineTokenizer.nextToken(); // Skip the HEAD token
        String requestPath = lineTokenizer.nextToken(); // Get the file name
        if (requestPath.startsWith("/")) requestPath = requestPath.substring(1); // Remove the forward slash if necessary
        File requestFile = new File(requestPath); // Create a File object with the given file path
        // In HEAD case, since no file needs to be transmitted, no fileInputStream is needed

        // If the file exists, we send the either 200 or 304 status line
        if (requestFile.exists()) {
            if (ifSince) {
                    Date serverModifiedTime = httpDateFormat.parse(httpDateFormat.format(requestFile.lastModified()));

                    // Then we compare the server time and the client time
                    // If the client time is before the server time, it is not updated
                    // Since it is HEAD, no file needs to be transmitted

                    // If it is not updated, send 200
                    if (clientModifiedTime.before(serverModifiedTime)) {
                        sendClient.writeBytes("HTTP/1.0 200 OK\r\n");
                        sendClient.writeBytes("Last-Modified: " + httpDateFormat.format(requestFile.lastModified()) + "\r\n");
                        sendClient.writeBytes("\r\n");
                        System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                                httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
                        logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                                httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
                    }

                    // Otherwise, if it is updated, send 304
                    else {
                        sendClient.writeBytes("HTTP/1.0 304 Not Modified\r\n");
                        sendClient.writeBytes("Last-Modified: " + httpDateFormat.format(requestFile.lastModified()) + "\r\n");
                        sendClient.writeBytes("\r\n");
                        System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                                httpDateFormat.format(new Date(System.currentTimeMillis())), "304 Not Modified", requestFile.getName());
                        logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                                httpDateFormat.format(new Date(System.currentTimeMillis())), "304 Not Modified", requestFile.getName());
                    }
            }

            // If there is no If-Modified-Since header, just reply as normal HEAD
            else {
                sendClient.writeBytes("HTTP/1.0 200 OK\r\n");
                sendClient.writeBytes("Last-Modified: " + httpDateFormat.format(requestFile.lastModified()) + "\r\n");
                sendClient.writeBytes("\r\n");
                System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                        httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
                logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                        httpDateFormat.format(new Date(System.currentTimeMillis())), "200 OK", requestFile.getName());
            }
        }
        // Otherwise, if file is not found, we send 404
        else {
            sendClient.writeBytes("HTTP/1.0 404 File Not Found\r\n");
            System.out.printf("%-15s %-29s --> %-30s %-20s %-35s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                    httpDateFormat.format(new Date(System.currentTimeMillis())), "404 File Not Found", requestFile.getName());
            logWriter.printf("%-15s %-29s --> %-30s %-20s %-35s  \n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                    httpDateFormat.format(new Date(System.currentTimeMillis())), "404 File Not Found", requestFile.getName());
        }

    }

    /**
     * badRequest generates 400 Bad Request when methods other than GET or HEAD are encountered
     */
    void badRequest () throws IOException {
        sendClient.writeBytes("HTTP/1.0 400 Bad Request\r\n");
        sendClient.writeBytes("\r\n");

        System.out.printf("%-15s %-29s --> %-30s %-20s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                httpDateFormat.format(new Date(System.currentTimeMillis())), "400 Bad Request");
        logWriter.printf("%-15s %-29s --> %-30s %-20s\n", threadSocket.getInetAddress().toString().substring(1), httpDateFormat.format(acceptedTime),
                httpDateFormat.format(new Date(System.currentTimeMillis())), "400 Bad Request");

    }


/////////////////////////////////// Driver Program ///////////////////////////////////
    public static void main (String[] args) {
        System.out.println("Server main running");
        // First, we create a welcome (listen) socket and a connection socket
        // connectionSocket here is only a temporary variable for passing (it is NOT the socket in the thread)
        ServerSocket listenSocket = null;
        Socket connectionSocket = null;
        try {
            listenSocket = new ServerSocket(serverPort);
            System.out.println("Listen socket established");
        } catch (IOException e) {
            System.out.println("Listen socket IOException. Exiting");
            System.exit(1);
        }

        // Then, we run a infinite loop to keep accepting incoming connection from client
        // When the client connection is accepted, the socket is stored in connectionSocket
        while (true) {
            // Accepting the incoming connection
            try {
                connectionSocket = listenSocket.accept();
                System.out.println("Connection accepted from IP: " + connectionSocket.getInetAddress().toString().substring(1));
            } catch (IOException e) {
                System.out.println("Listen socket IOException. Exiting");
                System.exit(1);
            }

            // Now we pass the socket to construct a server thread
            // Then, create and start a thread
            // Lastly, run() is performed (by start()) to perform server operation
            (new Thread(new MultiThreadServer(connectionSocket, new Date(System.currentTimeMillis())))).start();
        }
    }
}
