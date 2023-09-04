#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<mpi.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;
int width, height;

int* inputImage(int* w, int* h, System::String^ imagePath) //put the size of image in w & h
{
	int* input;


	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height*BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i*BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	width = BM.Width;
	height = BM.Height;
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//Output//Tests//100000" + index + ".png");
	cout << "result Image Saved " << index << endl;
}


int main()
{
	MPI_Init(NULL, NULL);
	MPI_Barrier(MPI_COMM_WORLD);
	int ImageWidth = 4, ImageHeight = 4, sizeOfImage =256;

	int start_s, stop_s, TotalTime = 0;

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (int s = 1; s <= 1; s++)
	{
		System::String^ imagePath;
		std::string img;
		img = "..//Data//Input//10N.png";
		//img = "..//Data//Input//5N//5N"+to_string(s)+".png";
		imagePath = marshal_as<System::String^>(img);
		int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);

		int* pixel_intenisties = new int[256]{ 0 };
		double* probability = new double[256];
		double* comProbability = new double[256]{ 0 };
		int* floorComProbability = new int[256];

		start_s = clock();
		//step #1
		for (int i = 0; i < width * height; i++)
			pixel_intenisties[imageData[i]]++;

		//step #2
		for (int i = 0; i < sizeOfImage; i++)
			probability[i] = (double)pixel_intenisties[i] / (double)(width * height);

		//step #3
		double sum = 0;
		for (int i = 0; i < sizeOfImage; i++)
		{
			sum += probability[i];
			comProbability[i] = sum;
		}

		//step #4
		for (int i = 0; i < sizeOfImage; i++)
			floorComProbability[i] = floor(comProbability[i] * 256);

		//step #5
		for (int i = 0; i < width * height; i++)
			imageData[i] = floorComProbability[imageData[i]];


		if (rank == 0)
		{
			createImage(imageData, ImageWidth, ImageHeight, s);
		}

		free(imageData);
	}
	MPI_Finalize();
	cout << "____________________________________________" << endl;
	cout << endl << ImageWidth << " " << ImageHeight << endl;
	stop_s = clock();

	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;
	return 0;

}

	






