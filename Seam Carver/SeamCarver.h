/*
 * SeamCarver.h
 * 
 * Copyright 2016 Dell XPS <Dell XPS@DELXPS>
 * @author: Winston Moh Tangongho
 * 
 * 			CS 305 Assignment 3
 * 			Class to carve an image horizontally and vetically using
 * 					a memoization table. 
 * 
 */

#ifndef SEAMCARVER_H
#define SEAMCARVER_H

#include <iostream>
#include <cstdlib>
#include "CImg.h"
#include <cmath>
using namespace std;
using namespace cimg_library;

class SeamCarver
	{
		public:
				SeamCarver();					// constructors.
				SeamCarver(CImg<unsigned char>);// ""
				~SeamCarver();		// destructor.
				
				CImg<unsigned char>* getImage();//get image.
				void setImage(CImg<unsigned char>);	//set the image.
				
				int getHeight();	// get height of image.
				int getWidth();		// get width of image.
				
				CImg<unsigned char>* getEnergyImage();	// return energy image.
				
				int* findHorizontalSeam();		// get seam in horizontal direction.
				int* findVerticalSeam();		// get seam in vertical direction.
				
				void removeHorizontalSeam(int*);	// remove horizontal seam.
				void removeVerticalSeam(int*);		// remove vertical seam.
				
				void energyCalc();		// calculate image energies.
				double T(int,int);	//	Find vertical cost value.
				double Th(int,int);	// Find horizontal cost value.
				double min2(double,double);	// find min of 2 double values.
				double min3(double,double,double);	// find min of 3 double values.
				int indice(int, int);		// get indice from row and column.
				
				int getC2(double**, int, int, int, double);	//	get column.
				int getC3(double**, int, int, double);		// 	'	"
				int getR2(double**, int, int, int, double);	// get row.
				int getR3(double**, int, int, double);		//		"
				
		private:
				int canvasht, canvaswt;
				CImg <unsigned char> *canvas;	// make canvas a pointer variable.
				CImg <unsigned char> *energyImage;	// canvas for energy image.
				double **energy;			// array of energies.
				double **Table;			// cost table for vertical and horizontal seams.
				double *t;				// memoization table for vertical seam.
				double *th;				// memoization table for horizontal seam.
				int depth, spectrum;	// Parameters used in creating image.
				bool flag;				// flag for usable or unusable image.
	};

// Default constructor with unusable SeamCarver.
SeamCarver::SeamCarver()
	{
		flag = false;				// unusable image.
		depth = 1; spectrum = 3;	// To be used in image.
		canvasht = 0; 
		canvaswt = 0;
		canvas = NULL;
		Table = NULL;
		energy = NULL;
		t = NULL; th = NULL;
	}

// 1-parameter constructor.	
SeamCarver::SeamCarver(CImg<unsigned char> SC)
	{
		flag = true;				// usable image.
		depth = 1; spectrum = 3;	// To be used in image.
		canvasht = SC.height();
		canvaswt = SC.width();
		canvas = new CImg <unsigned char> (SC);	// create canvas.
		
		// Initialize 2D energy Array.
		energy = new double*[canvasht];
		for (int i=0; i<canvasht; i++)
			energy[i] = new double[canvaswt];
	
		//initialize cost tables.
		Table = new double*[canvasht];
		for (int i=0; i<canvasht; i++)
			Table[i] = new double[canvaswt];
		
		// initialize memoization tables.
		t = new double[canvasht*canvaswt];		// vertical seam.
		th = new double[canvasht*canvaswt];		// horizontal seam.
	}

// SeamCarver destructor.
SeamCarver::~SeamCarver()
	{
		delete[] energy;
		delete canvas;
		delete energyImage;
		delete[] Table;
		delete[] t;	
		delete[] th;
	}
	
// return current image.
CImg<unsigned char>* SeamCarver::getImage()
	{
		if (flag == 1)
		{
			if (canvaswt == 0 && canvasht == 0)
				return NULL;
			return canvas;
		}
		else
		{
			cout<<" No image created"<<endl;
			return NULL;
		}
	}

// set SeamCarver image.
void SeamCarver::setImage(CImg<unsigned char> im)
	{
		(*canvas) = im;
	}

// get canvasht of current image.
int SeamCarver::getHeight()
	{
		if (flag == 1)
		{
			return canvasht;
		}
		else
		{
			cout<<" No image created"<<endl;
			return 0;	
		}
	}

// get canvaswt of current image.
int SeamCarver::getWidth()
	{
		if (flag == 1)
		{
			return canvaswt;
		}
		else
		{
			cout<<" No image created"<<endl;
			return 0;
		}
	}

// energy calculations.
void SeamCarver::energyCalc()
	{
		if (flag == 1)
		{
			unsigned int r1,r2,r3,r4,g1,g2,g3,g4,b1,b2,b3,b4;	// variables for 4 sets of colors.
			// Get energies from each pixel. use form(column,row,0,colorNumber);
			for (int i=0; i<canvasht; i++)
			{
				for (int j=0; j<canvaswt; j++)
				{
					// get colors from left neighbor.
					if (j == 0)	// give it j=canvaswt-1.
					{
						r1 = (*canvas)(canvaswt-1,i,0,0);
						g1 = (*canvas)(canvaswt-1,i,0,1);
						b1 = (*canvas)(canvaswt-1,i,0,2);
					}
					else
					{
						r1 = (*canvas)(j-1,i,0,0);
						g1 = (*canvas)(j-1,i,0,1);
						b1 = (*canvas)(j-1,i,0,2);
					}			
					// get colors from right neighbor.
					if (j == canvaswt-1)	// give it j=0.
					{
						r2 = (*canvas)(0,i,0,0);
						g2 = (*canvas)(0,i,0,1);
						b2 = (*canvas)(0,i,0,2);
					}
					else 
					{
						r2 = (*canvas)(j+1,i,0,0);
						g2 = (*canvas)(j+1,i,0,1);
						b2 = (*canvas)(j+1,i,0,2);				
					}

					// get colors from neighbor above.
					if (i == 0)	// give it i=canvasht-1.
					{
						r3 = (*canvas)(j,canvasht-1,0,0);
						g3 = (*canvas)(j,canvasht-1,0,1);
						b3 = (*canvas)(j,canvasht-1,0,2);
					}
					else
					{
						r3 = (*canvas)(j,i-1,0,0);
						g3 = (*canvas)(j,i-1,0,1);
						b3 = (*canvas)(j,i-1,0,2);				
					}
					// get colors from neighbor below.
					if (i == canvasht-1)	// give it i=0.
					{
						r4 = (*canvas)(j,0,0,0);
						g4 = (*canvas)(j,0,0,1);
						b4 = (*canvas)(j,0,0,2);
					}
					else
					{
						r4 = (*canvas)(j,i+1,0,0);
						g4 = (*canvas)(j,i+1,0,1);
						b4 = (*canvas)(j,i+1,0,2);				
					}
					
					// Calculate dimensional energies and store in 2D-array.
					unsigned int x_dim = (r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2);
					unsigned int y_dim = (r3-r4)*(r3-r4) + (g3-g4)*(g3-g4) + (b3-b4)*(b3-b4);
					energy[i][j] = sqrt(double(x_dim + y_dim));
				}
			}	
		}
		else
		{
			cout<<" No image created"<<endl;
		}
	}	// end fxn.

// return energy image.
CImg<unsigned char>* SeamCarver::getEnergyImage()
	{
		if (flag == 1)
		{
			if (canvaswt == 0 || canvasht == 0)
			{
				exit(0);
			}
			energyImage = new CImg <unsigned char> (canvaswt,canvasht,1,3,0);	// create energy image.
			 energyCalc();		// call function to calculate energies.
			 unsigned int red,green,blue;
			 //Set pixel energies into energy image.
			 for (int i=0; i<canvasht; i++)
			 {
				 for (int j=0; j<canvaswt; j++)
				 {
					 red 	= (((unsigned int)energy[i][j]) & 0xff0000) >> 16;
					 green	= (((unsigned int)energy[i][j]) & 0x00ff00) >> 8;
					 blue	= (((unsigned int)energy[i][j]) & 0x0000ff);
					 (*energyImage)(j,i,0,0) = red;
					 (*energyImage)(j,i,0,1) = green;
					 (*energyImage)(j,i,0,2) = blue;
				 }
			 }
			 return energyImage;
		 }
		else
		{
			cout<<" No image created"<<endl;
			return NULL;
		}
	}// end function.

// Find min of 2 values
double SeamCarver::min2(double a, double b)
	{
		return (a < b ? a : b);
	}

// Find min of 3 values
double SeamCarver::min3(double a, double b, double c)
	{
		double min = min2(a,b);
		return (min < c ? min : c);
	}

// function to return indice of row and column.
int SeamCarver::indice(int row, int col)
	{
		if (row == 0) return col;
		return ((canvaswt * row) + col);
	}

// Find cost Table values using memoization for vertical seam.
// x is row and y is column
double SeamCarver::T(int x, int y)
	{	 
		int n = indice(x,y);	// get indice from row and column.
		if (t[n] != -1.0)		// if not equal to -1 then value was already calculated.
			return t[n];		// reurn that value!
		else if (y+1 == canvaswt)
			t[n] = min2(T(x-1,y), T(x-1,y-1));
		else if (y-1 < 0)
			t[n] = min2(T(x-1,y), T(x-1,y+1));
		else 
			t[n] = min3(T(x-1,y-1), T(x-1,y), T(x-1, y+1));
		return t[n];	
	}
	
// Find cost Table values using memoization for horizontal seam.
// x is row and y is column.
double SeamCarver::Th(int x, int y)
	{	 
		int n = indice(x,y);	// get indice from row and column.
		if (th[n] != -1.0)		// if not equal to -1 then value was already calculated.
			return th[n];		// return that value.
		else if (x+1 == canvasht)
			th[n] = min2(Th(x,y-1), Th(x-1,y-1));
		else if (x-1 < 0)
			th[n] = min2(Th(x+1,y-1), Th(x,y-1));
		else 
			th[n] = min3(Th(x,y-1), Th(x-1,y-1), Th(x+1,y-1));
		return th[n];	
	}

// get column with 2 neighbors.
int SeamCarver::getC2(double **a, int r, int c1, int c2, double x)
	{
		if (a[r][c1] == x)	return c1;
		return c2;
	}

// get column with 3 neighbors.
int SeamCarver::getC3(double **a, int r, int c, double x)
	{
		if (a[r][c] == x)	return c;
		else if (a[r][c-1] == x) return c-1;
		return c+1;
	}

// get row with 2 neighbors.
int SeamCarver::getR2(double **a, int c, int r1, int r2, double x)
	{
		if (a[r1][c] == x)	return r1;
		return r2;
	}

// get row with 3 neighbors.
int SeamCarver::getR3(double **a, int c, int r, double x)
	{
		if (a[r][c] == x)	return r;
		else if (a[r-1][c] == x) return r-1;
		return r+1;
	}

// Return array of indices for optimal seam in Horizontal direction.
int* SeamCarver::findHorizontalSeam()
	{
		if (flag == 1)
		{
			energyCalc();		// call function to calculate energies.
			// Fill memoization table;
			for (int i=0; i<canvasht*canvaswt; i++)	// fill it with the value -1.0
				th[i] = -1.0;
			// Fill first column.
			for (int i=0; i<canvasht; i++)
			{
				int n = indice(i,0);
				th[n] = energy[i][0];
				Table[i][0] = energy[i][0];
			}
			// Fill cost Table.
			for (int i=0; i<canvasht; i++)	// *don't forget to fill the first row. (i starts from 0)
			{
				for (int j=1; j<canvaswt; j++)	// (j starts from 1.)
				{
					Table[i][j] = energy[i][j] + Th(i,j);
				}
			}	
			// Find optimal path.
			int cols = canvaswt;
			int rows = canvasht;
			int *b = new int[cols];	// array for columns.
			for (int j=0; j<cols; j++)
				{
					b[j] = -1;
				}
			
			double mini = Table[0][cols-1];	// initial min. value.
			//b[cols-1] = 0;
			for (int i=1; i<rows; i++)
			{
				if (Table[i][cols-1] < mini)
				{
					mini = Table[i][cols-1];		// get min. value on right-most column.
					b[cols-1] = i;			// get row of min. value.
				}
			}
			int k=cols-1;	// first item in array.
			int i = b[k];
			for (int j=cols-1; j>0; j--)
			{
				if (i+1 == rows) 
				{
					double mini = min2(Table[i][j-1], Table[i-1][j-1]);
					i = getR2(Table,j-1,i,i-1,mini);
					b[k-1] = i;		
				}
				else if (i == 0)
				{
					double mini = min2(Table[i][j-1], Table[i+1][j-1]);
					i = getR2(Table,j-1,i,i+1,mini);
					b[k-1] = i;
				}
				else
				{
					double mini = min3(Table[i-1][j-1], Table[i][j-1], Table[i+1][j-1]);
					i = getR3(Table,j-1,i,mini);
					b[k-1] = i;
				}
				k--;		// decrement size in array.
			}
			return b;		// return array of indices.
		}
		else
		{
			cout<<" No image created"<<endl;
			return NULL;
		}
	}

// Return array of indices for optimal seam in Vertical direction.	
int* SeamCarver::findVerticalSeam()
	{
		if (flag == 1)
		{
			energyCalc();		// call function to calculate energies.
			// Fill memoization table;
			for (int i=0; i<canvasht*canvaswt; i++)	// fill it with the value -1.0
				t[i] = -1.0;
			// Fill first row in cost table and memoization table.
			for (int j=0; j<canvaswt; j++)
			{
				t[j] = energy[0][j];
				Table[0][j] = energy[0][j];
			}
			// Fill cost Table.
			for (int i=1; i<canvasht; i++)
			{
				for (int j=0; j<canvaswt; j++)
				{
					Table[i][j] = energy[i][j] + T(i,j);
				}
			}	
			
			// Find optimal path.
			int rows = canvasht;
			int cols = canvaswt;
			int *b = new int[rows];	// array for columns.
			for (int i=0; i<rows; i++)
				{
					b[i] = -1;
				}
			
			double mini = Table[rows-1][0];	// initial min. value.
			//b[rows-1] = 0;
			for (int j=1; j<cols; j++)
			{
				if (Table[rows-1][j] < mini )
				{
					mini = Table[rows-1][j];		// get min. value on bottom row.
					b[rows-1] = j;			// get column of min. value.
				}
			}
			int k=rows-1;	// first item in array.
			int j = b[k];

			for (int i=rows-1; i>0; i--)
			{
				if (j+1 == cols) 
				{
					double mini = min2(Table[i-1][j], Table[i-1][j-1]);
					j = getC2(Table,i-1,j,j-1,mini);
					b[k-1] = j;		
				}
				else if (j-1 == -1)
				{
					double mini = min2(Table[i-1][j], Table[i-1][j+1]);
					j = getC2(Table,i-1,j,j+1,mini);
					b[k-1] = j;
				}
				else
				{
					double mini = min3(Table[i-1][j], Table[i-1][j-1], Table[i-1][j+1]);
					j = getC3(Table,i-1,j,mini);
					b[k-1] = j;
				}
				k--;		// decrement size in array.
			}
			return b;		// return array of indices.	
		}
		else
		{
			cout<<" No image created"<<endl;
			return NULL;
		}
	}

// Remove pixels along Horizontal seam.	
void SeamCarver::removeHorizontalSeam(int *hs)
	{
		if (flag == 1)
		{
			CImg<unsigned char> *t = new CImg<unsigned char>(canvaswt,canvasht-1,depth,spectrum);
			// set pixels in new image;
			 for (int i=0; i<canvasht; i++)
			 {
				 for (int j=0; j<canvaswt; j++)
				 {
					 if (i < hs[j])
					 {
						(*t)(j,i,0,0) = (*canvas)(j,i,0,0);
						(*t)(j,i,0,1) = (*canvas)(j,i,0,1);
						(*t)(j,i,0,2) = (*canvas)(j,i,0,2);
					 }
					else if (i > hs[j])
					{
						(*t)(j,i-1,0,0) = (*canvas)(j,i,0,0);
						(*t)(j,i-1,0,1) = (*canvas)(j,i,0,1);
						(*t)(j,i-1,0,2) = (*canvas)(j,i,0,2);
					}
				 }
			 }
			 //delete canvas;		// do this in main program.
			 canvas = t;
			 canvasht = (*canvas).height();
			 canvaswt = (*canvas).width();
		 }
		else
		{
			cout<<" No image created"<<endl;
		}
	}
	
// Remove pixels along vertical seam.
void SeamCarver::removeVerticalSeam(int *vs)
	{
		if (flag == 1)
		{
			CImg<unsigned char> *t = new CImg<unsigned char>(canvaswt-1,canvasht,depth,spectrum);
			// set pixels in new image;
			 for (int i=0; i<canvasht; i++)
			 {
				 for (int j=0; j<canvaswt; j++)
				 {
					 if (j < vs[i])
					 {
						(*t)(j,i,0,0) = (*canvas)(j,i,0,0);
						(*t)(j,i,0,1) = (*canvas)(j,i,0,1);
						(*t)(j,i,0,2) = (*canvas)(j,i,0,2);
					 }
					else if (j > vs[i])
					{
						(*t)(j-1,i,0,0) = (*canvas)(j,i,0,0);
						(*t)(j-1,i,0,1) = (*canvas)(j,i,0,1);
						(*t)(j-1,i,0,2) = (*canvas)(j,i,0,2);	
					}
				}
			 }
			 //delete canvas;		// do this in main program.
			 canvas = t;
			 canvasht = (*canvas).height();
			 canvaswt = (*canvas).width();	
		 }
		else
		{
			cout<<" No image created"<<endl;
		}
	}

#endif
