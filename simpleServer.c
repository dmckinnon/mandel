/*
 *  simpleServer.c
 *  1917 lab 5
 *
 *  Created by Richard Buckland on 28/01/11.
 *  Copyright 2011 Licensed under Creative Commons SA-BY-NC 3.0. 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

int waitForConnection (int serverSocket);
int makeServerSocket (int portno);
void serveBMP (int socket);
void drawMandelBrot(unsigned char* bmpFile);

#define WIDTH 512
#define HEIGHT 512 // Height and width of MandelBrot drawing
#define MAX_ITERATIONS 512
#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 7191
#define NUMBER_OF_PAGES_TO_SERVE 10
// after serving this many pages the server will halt
 

int main (int argc, char *argv[]) {
      
   printf ("************************************\n");
   printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
   printf ("Serving poetry since 2011\n");   
   
   int serverSocket = makeServerSocket (DEFAULT_PORT);   
   printf ("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
   printf ("************************************\n");
   
   char request[REQUEST_BUFFER_SIZE];
   
   int numberServed = 0;
   while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {
      
      printf ("*** So far served %d pages ***\n", numberServed);
      
      int connectionSocket = waitForConnection (serverSocket);
      // wait for a request to be sent from a web browser, open a new
      // connection for this conversation
      
      // read the first line of the request sent by the browser  
      int bytesRead;
      bytesRead = read (connectionSocket, request, (sizeof request)-1);
      assert (bytesRead >= 0); 
      // were we able to read any data from the connection?
            
      // print entire request to the console 
      printf (" *** Received http request ***\n %s\n", request);
      
      //send the browser a simple html page using http
      printf (" *** Sending http response ***\n");
      serveBMP(connectionSocket);
      
      // close the connection after sending the page- keep aust beautiful
      close(connectionSocket);
      
      numberServed++;
   } 
   
   // close the server connection after we are done- keep aust beautiful
   printf ("** shutting down the server **\n");
   close (serverSocket);
   
   return EXIT_SUCCESS; 
}

void serveBMP (int socket) {
    
    char* message;
    
    // first send the http response header
    
    // (if you write strings one after another like this on separate
    // lines the c compiler kindly joins them togther for you into
    // one long string)
    message = "HTTP/1.0 200 OK\r\n"
    "Content-Type: image/bmp\r\n"
    "\r\n";
    printf ("about to send=> %s\n", message);
    write (socket, message, strlen (message));
    
    
    // now send the BMP
    unsigned char bmp[512*512*3 + 0x36] = {
        0x42,0x4D,0x36,0x00,0x0C,0x00,0x00,0x00,0x00,
        0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,
        0x00,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x01,
        0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    
    /*int i;
    for(i=54; i<(512*512*3 + 0x36); i+=3) {
        
        bmp[i] = 0xFF;
        bmp[i+1] = 0x00;
        bmp[i+2] = 0xFF;
    }*/
    
    //drawMandelBrot(bmp);
    
    double cx, cy;
    long double zx = 0, zy = 0, newzx;
    int iterations; 
    int row, col;
    
    int bmpIndex = 54; // starting index of image in the bmp for hex
    
    for (row=HEIGHT; row>0; row --) {
        
        for (col=0; col<WIDTH; col ++) {
            
            cx = (col - WIDTH/2)/256.0;
            cy = (row - HEIGHT/2)/256.0;
            zx = 0;
            zy = 0;
            iterations = 0; 
            
            while ((zx*zx + zy*zy) < 4 && iterations < MAX_ITERATIONS) {
                
                newzx = zx * zx - zy * zy + cx;
                zy = 2 * zx * zy + cy;
                zx = newzx; // must use a temp variable for zx else the previous value will not be used in the calculation for zy
                iterations ++;
            }
            if (zx*zx + zy*zy < 4) {
                
                bmp[bmpIndex] = 0xFF; // Next three bytes in hex file are black
                bmp[bmpIndex+1] = 0x00;
                bmp[bmpIndex+2] = 0x00;
                //printf("%d", bmpFile[bmpIndex]);
                //printf("%d", bmpFile[bmpIndex+1]);
                //printf("%d", bmpFile[bmpIndex+2]);
                //strcat(graph, inSet);
                //strcat(bmpFile, "FFFFFF");
            } else {
                
                bmp[bmpIndex] = 0xFF; // Next three bytes in hex file are white
                bmp[bmpIndex+1] = 0xFF;
                bmp[bmpIndex+2] = 0xFF;
                //printf("%d", bmpFile[bmpIndex]);
                //strcat(graph, notInSet);
                //strcat(bmpFile, "000000");
            }
            
            /* colouring things fancy
             if (iterations == MAX_ITERATIONS) {
             
             pixelColour = "FFFFFF";
             } else {
             
             pixelColour = "%c", char;
             } 
             
             strcat(bmpFile, pixelColour);
             */
        }
        
        bmpIndex += 3;
        //strcat(graph, endLine);
        //strcat(bmpFile, "00000000");
    }

    
    write (socket, bmp, sizeof(bmp));
}


// start the server listening on the specified port number
int makeServerSocket (int portNumber) { 
   
   // create socket
   int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
   assert (serverSocket >= 0);   
   // error opening socket
   
   // bind socket to listening port
   struct sockaddr_in serverAddress;
   bzero ((char *) &serverAddress, sizeof (serverAddress));
   
   serverAddress.sin_family      = AF_INET;
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   serverAddress.sin_port        = htons (portNumber);
   
   // let the server start immediately after a previous shutdown
   int optionValue = 1;
   setsockopt (
      serverSocket,
      SOL_SOCKET,
      SO_REUSEADDR,
      &optionValue, 
      sizeof(int)
   );

   int bindSuccess = 
      bind (
         serverSocket, 
         (struct sockaddr *) &serverAddress,
         sizeof (serverAddress)
      );
   
   assert (bindSuccess >= 0);
   // if this assert fails wait a short while to let the operating 
   // system clear the port before trying again
   
   return serverSocket;
}

// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
int waitForConnection (int serverSocket) {
   // listen for a connection
   const int serverMaxBacklog = 10;
   listen (serverSocket, serverMaxBacklog);
   
   // accept the connection
   struct sockaddr_in clientAddress;
   socklen_t clientLen = sizeof (clientAddress);
   int connectionSocket = 
      accept (
         serverSocket, 
         (struct sockaddr *) &clientAddress, 
         &clientLen
      );
   
   assert (connectionSocket >= 0);
   // error on accept
   
   return (connectionSocket);
}

void drawMandelBrot(unsigned char* bmpFile) {
    
    double cx, cy;
    long double zx = 0, zy = 0, newzx;
    int iterations; 
    int row, col;
    
    int bmpIndex = 54; // starting index of image in the bmp for hex
    
    //char graph[10000]; // massive array just so memory cannot be exceeded
    //char bmpFile[1000000];
    //strcpy(graph, "<br>");
    //strcpy(bmpFile, "424D5A0000000000000036000000280000000300000003000000010018000000000024000000130B0000130B000000000000000000000000"); // <-- concat all required hex stuff
    //char* inSet = "*";
    //char* notInSet = "_";
    //char* endLine = "<br>";
    //char* pixelColour = "FFFFFF";
    
    for (row=HEIGHT; row>0; row --) {
        
        for (col=0; col<WIDTH; col ++) {
            
            cx = (col - WIDTH/2)/256.0;
            cy = (row - HEIGHT/2)/256.0;
            zx = 0;
            zy = 0;
            iterations = 0; 
            
            while ((zx*zx + zy*zy) < 4 && iterations < MAX_ITERATIONS) {
                
                newzx = zx * zx - zy * zy + cx;
                zy = 2 * zx * zy + cy;
                zx = newzx; // must use a temp variable for zx else the previous value will not be used in the calculation for zy
                iterations ++;
            }
            if (zx*zx + zy*zy < 4) {
                
                bmpFile[bmpIndex] = 0xFF; // Next three bytes in hex file are black
                bmpFile[bmpIndex+1] = 0x00;
                bmpFile[bmpIndex+2] = 0x00;
                //printf("%d", bmpFile[bmpIndex]);
                //printf("%d", bmpFile[bmpIndex+1]);
                //printf("%d", bmpFile[bmpIndex+2]);
                //strcat(graph, inSet);
                //strcat(bmpFile, "FFFFFF");
            } else {
                
                bmpFile[bmpIndex] = 0xFF; // Next three bytes in hex file are white
                bmpFile[bmpIndex+1] = 0xFF;
                bmpFile[bmpIndex+2] = 0xFF;
                //printf("%d", bmpFile[bmpIndex]);
                //strcat(graph, notInSet);
                //strcat(bmpFile, "000000");
            }
            
            /* colouring things fancy
            if (iterations == MAX_ITERATIONS) {
            
                pixelColour = "FFFFFF";
            } else {
                
                pixelColour = "%c", char;
            } 
             
            strcat(bmpFile, pixelColour);
             */
        }
        
        bmpIndex += 3;
        //strcat(graph, endLine);
        //strcat(bmpFile, "00000000");
    }
    
    //write (socket, bmpFile, sizeof(bmpFile));
}
