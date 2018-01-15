/*
 * Project.cpp
 * 
 * Copyright 2016 Dell XPS <Dell XPS@DELXPS>
 * @author: Winston Moh Tangongho
 * CS 305 Assignment 3.
 * 
 */


#include <iostream>
#include <cstdlib>

#include "SeamCarver.h"
#include "CImg.h"

using namespace std;
using namespace cimg_library;

string getUserInput(string prompt)
   {
      cout<<prompt;
      string t="";
      getline(cin,t);
      return t;
   }

void errorMsg(string msg, bool term)
   {
      cout<<msg<<endl;
      if (term)
         exit(0);
   }

void dump1DArray(string msg, int *a, int n)
   {
      if (msg.length()>0)
         cout<<msg<<endl;

      for (int i=0;i<n;i++)
         cout<<a[i]<<' ';
      cout<<endl;
   }

//======================================================================


int main(int argc, char *argv[])
   {

      unsigned char fuscia[3] = {255,0,255};
      unsigned char red[3] = {255,0,0};

      string imagename = getUserInput("Image file: ");
      CImg<unsigned char> image(imagename.c_str());

      if (!image)
         errorMsg("File not found...",true);

      SeamCarver sc(image);

      CImg<unsigned char> *wi = sc.getImage();
      CImg<unsigned char> *wei = sc.getEnergyImage();

      int *vseam = NULL;
      int *hseam = NULL;

      wi = sc.getImage();
      CImgDisplay originaldisplay(image, "Original Image"), energydisplay((*wei),"Energy Image"), carveddisplay((*wi),"Carved Image");
      
      originaldisplay.move(50,50);        // put it somewhere in the NW corner of the screen
      energydisplay.move(50+image.width()+50,50); // put it next to the original...maybe move the carved image, too

      while (!originaldisplay.is_closed() && !energydisplay.is_closed() && !carveddisplay.is_closed())
         {
            originaldisplay.wait();
            if (originaldisplay.button()&1 && originaldisplay.mouse_y() >= 0 && originaldisplay.mouse_x() >= 0) // left button
               {
                  if (vseam !=NULL)
                     delete[] vseam;
                  vseam = sc.findVerticalSeam();
                  //dump1DArray("Here's the vert seam",vseam,sc.getHeight());       // icyc
                  delete wei;
                  wei = sc.getEnergyImage();
                  for (int y=0;y<sc.getHeight();y++)
                     (*wei).draw_point(vseam[y],y,fuscia);
                  energydisplay.resize(*wei);
                  energydisplay.display(*wei);
                  sc.removeVerticalSeam(vseam);
                  delete wi;	
                  wi = sc.getImage();
                  cout<<"Displaying new image width: "<<wi->width()<<"  height: "<<wi->height()<<endl;  // icyc
                  carveddisplay.resize(*wi);
                  carveddisplay.display(*wi);
               }

            else if (originaldisplay.button()&2 && originaldisplay.mouse_y() >= 0 && originaldisplay.mouse_x() >= 0) // right button
               {
                  if (hseam!=NULL)
                     delete[] hseam; 
                  hseam = sc.findHorizontalSeam();
                  //dump1DArray("Here's the HORIZONTAL seam",hseam,sc.getWidth());    // icyc
                  delete wei;
                  wei = sc.getEnergyImage();
                  for (int x=0;x<sc.getWidth();x++)
                     (*wei).draw_point(x,hseam[x],red);
                  energydisplay.resize(*wei);
                  energydisplay.display(*wei);               
                  sc.removeHorizontalSeam(hseam);
                  delete wi;	
                  wi = sc.getImage();
                  cout<<"Displaying new image width: "<<wi->width()<<"  height: "<<wi->height()<<endl;   // icyc
                  carveddisplay.resize(*wi);
                  carveddisplay.display(*wi);
               }


            originaldisplay.flush();                    // individual clicks...no buffering
         }
   }
