//  MandelBrotSet.c
//  Program to draw up the Mandelbrot Set Fractal in a webserver at any coordinates and zoom level
//  Created by David McKinnon on 29/03/12.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define WIDTH 140
#define HEIGHT 40

void mandelBrot(void); 

int main(int argc, char *argv[]) {
    
    mandelBrot();
    
    return EXIT_SUCCESS;
}

void mandelBrot(void) {
    
    double cx, cy;
    double zx = 0, zy = 0, newzx;
    int iterations; // max 
    int row, col;
    //int pixels = HEIGHT * WIDTH;
    char graph[50000];
    strcpy(graph, "\n");
    char* inSet = "*";
    char* notSet = "_";
    char* endLine = "\n";
    //char* bmpFile = "424D5A0000000000000036000000280000000300000003000000010018000000";
    //char graph[70][20];
    
    for (row=HEIGHT; row>0; row --) {
        
        for (col=0; col<WIDTH; col ++) {
            
            cx = ((col/4.0) - 25)/10.0;
            cy = (row - HEIGHT/2)/20.0;
            zx = 0;
            zy = 0;
            iterations = 0; // max iterations = 256
            
            while ((zx*zx + zy*zy) < 4 && iterations < 257) {
                
                newzx = zx * zx - zy * zy + cx;
                zy = 2 * zx * zy + cy;
                zx = newzx; // must use a temp variable for zx else the previous value will not be used in the calculation for zy
                iterations ++;
            }
            if (zx*zx + zy*zy < 4) {
                
                //graph[row][col] = '#'; // at given coordinates c(x,y) there is a hash if within Set
                //printf("*"); 
                strcat(graph, inSet);
                // bmpFile = strcat(bmpFile, "FFFFFF"); // concatenate this onto the string
            } else {
                
                //graph[row][col] = ' ';  
                //printf(" ");
                strcat(graph, notSet);
                //bmpFile = strcat(bmpFile, "000000");
            }
            //printf("%f %f\n" zx, zy);
        }
        //printf("\n");
        strcat(graph, endLine);
        //bmpFile = strcat(bmpFile, "00000000");
    }
    
    //strcat(graph, inSet);
    printf("%s", graph);
}
