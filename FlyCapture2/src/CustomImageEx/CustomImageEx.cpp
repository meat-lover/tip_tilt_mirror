
//=============================================================================
// Copyright © 2017 FLIR Integrated Imaging Solutions, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: CustomImageEx.cpp 316528 2017-02-22 00:03:53Z alin $
//=============================================================================

#include "stdafx.h"
#include <math.h> 
#include "FlyCapture2.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include"SerialPort.h"

#define _COLS  1280;
#define _ROWS  1024;

using namespace FlyCapture2;
using namespace std;

class Centroid {

};

void PrintBuildInfo()
{
	FC2Version fc2Version;
	Utilities::GetLibraryVersion(&fc2Version);

	ostringstream version;
	version << "FlyCapture2 library version: " << fc2Version.major << "."
		<< fc2Version.minor << "." << fc2Version.type << "."
		<< fc2Version.build;
	cout << version.str() << endl;

	ostringstream timeStamp;
	timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
	cout << timeStamp.str() << endl << endl;
}

void PrintCameraInfo(CameraInfo* pCamInfo)
{
	cout << endl;
	cout << "*** CAMERA INFORMATION ***" << endl;
	cout << "Serial number - " << pCamInfo->serialNumber << endl;
	cout << "Camera model - " << pCamInfo->modelName << endl;
	cout << "Camera vendor - " << pCamInfo->vendorName << endl;
	cout << "Sensor - " << pCamInfo->sensorInfo << endl;
	cout << "Resolution - " << pCamInfo->sensorResolution << endl;
	cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
	cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl
		<< endl;
}


void PrintFormat7Capabilities(Format7Info fmt7Info)
{
	cout << "Max image pixels: (" << fmt7Info.maxWidth << ", "
		<< fmt7Info.maxHeight << ")" << endl;
	cout << "Image Unit size: (" << fmt7Info.imageHStepSize << ", "
		<< fmt7Info.imageVStepSize << ")" << endl;
	cout << "Offset Unit size: (" << fmt7Info.offsetHStepSize << ", "
		<< fmt7Info.offsetVStepSize << ")" << endl;
	cout << "Pixel format bitfield: 0x" << fmt7Info.pixelFormatBitField << endl;
}

void PrintError(Error error) { error.PrintErrorTrace(); }


int* getCentroid(int** ImageData, int rows, int cols) {
	long long x = 0;
	long long y = 0;
	int colourSum = 1;
	static int result[2];
	int pixel_index = 0;
	// get centroid
	for (int xPos = 0; xPos < cols; xPos++) {
		for (int yPos = 0; yPos < rows; yPos++) {
			x = x + xPos * ImageData[xPos][yPos];
			y = y + yPos * ImageData[xPos][yPos];
			colourSum = colourSum + ImageData[xPos][yPos];
			pixel_index++;
		}
	}
	cout << "pixel_index = " << pixel_index << endl;

	result[0] = x / colourSum;
	result[1] = y / colourSum;

	return result;
}

int** imageThreshold(unsigned char* iimage, int threshold, int rows, int cols) {
	int pixel_index = 0;
	int border = 50;
	// Dynamically Allocate Memory for converting rawimage data array to 2D array of size cols x row in C++
	int** ImageData = new int* [cols];
	for (int i = 0; i < cols; i++) {
		ImageData[i] = new int[rows];
	}
	//assign values to allocated memory
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			if (int(iimage[pixel_index]) < threshold) {
				ImageData[i][j] = 0;
			}
			// To eliminate the noise at the image border
			else if ((i < border) || (i > cols - border) || (j < border) || (j > rows - border)) {
				ImageData[i][j] = 0;
			}
			else {
				ImageData[i][j] = int(iimage[pixel_index]);
			}
			pixel_index++;
		}
	}
	return ImageData;

}



const char* port = "\\\\.\\COM13";
SerialPort myPort(port);

void Serial_Transfer(int* pcentroid) {
	if (myPort.isConnected()) {
		cout << "Connection is Established" << endl;
	}
	else {
		cout << "error in connection" << endl;
	}
	if (myPort.isConnected()) {
		string command;
		command = to_string(*pcentroid) + ", " + to_string(*(pcentroid + 1));
		char* charArray = new char[command.size() + 1];

		command.copy(charArray, command.size() + 1);
		charArray[command.size()] = '\0';

		myPort.writeSerialPort(charArray, strlen(charArray));
		cout << "data TX = " << charArray << endl;
		myPort.readSerialPort(charArray, strlen(charArray));
		delete[] charArray;
	}
	else {
		cout << "Serial port is not connected" << endl;
	}
}

void FreeDynamicArray(int** dArray)
{
	delete[] * dArray;
	delete[] dArray;
}
/*
int* get_SCH_Slope(int** ImageData, int cols, int rows) {
	int** AIO = new int* [12];
	int spot_dex = 1;
	for (int xPos = 0; xPos < cols; xPos++) {
		for (int yPos = 0; yPos < rows; yPos++) {
			if (ImageData[xPos][yPos] - ImageData[xPos][yPos] > 0) {
				sub_spots[spot_dex] =
			}
			
			pixel_index++;
		}
	}
}
*/
int main(int /*argc*/, char** /*argv*/)
{
	PrintBuildInfo();
	const Mode k_fmt7Mode = MODE_0;
	const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_MONO8;
	const int k_numImages = 10;

	Error error;

	// Since this application saves images in the current folder
	// we must ensure that we have permission to write to this folder.
	// If we do not have permission, fail right away.
	FILE* tempFile = fopen("test.txt", "w+");
	if (tempFile == NULL)
	{
		cout << "Failed to create file in current folder.  Please check "
			"permissions."
			<< endl;
		return -1;
	}
	fclose(tempFile);
	remove("test.txt");

	BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	cout << "Number of cameras detected: " << numCameras << endl;

	if (numCameras < 1)
	{
		cout << "Insufficient number of cameras... exiting" << endl;
		return -1;
	}

	PGRGuid guid;
	error = busMgr.GetCameraFromIndex(0, &guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Connect to a camera
	Camera cam;
	error = cam.Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Get the camera information
	CameraInfo camInfo;
	error = cam.GetCameraInfo(&camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	PrintCameraInfo(&camInfo);

	// Query for available Format 7 modes
	Format7Info fmt7Info;
	bool supported;
	fmt7Info.mode = k_fmt7Mode;
	error = cam.GetFormat7Info(&fmt7Info, &supported);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	PrintFormat7Capabilities(fmt7Info);

	if ((k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0)
	{
		// Pixel format not supported!
		cout << "Pixel format is not supported" << endl;
		return -1;
	}

	Format7ImageSettings fmt7ImageSettings;
	fmt7ImageSettings.mode = k_fmt7Mode;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	fmt7ImageSettings.width = _COLS; // ROI setting may need to fit with binning
	fmt7ImageSettings.height = _ROWS; // also need to consider frame rate at the current ROI
	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	error = cam.ValidateFormat7Settings(
		&fmt7ImageSettings, &valid, &fmt7PacketInfo);
	if (error != PGRERROR_OK)
	{
		cout << "why this is invalid" << endl;
		PrintError(error);
		return -1;
	}

	if (!valid)
	{
		// Settings are not valid
		cout << "Format7 settings are not valid" << endl;
		return -1;
	}

	// Set the settings to the camera
	error = cam.SetFormat7Configuration(
		&fmt7ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	cout << "press any key to continue " << endl;
	cin.get();

	// Start capturing images
	error = cam.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	/*
	// Retrieve frame rate property
	Property frmRate;
	frmRate.type = FRAME_RATE;
	error = cam.GetProperty(&frmRate);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	cout << "Frame rate is " << fixed << setprecision(2) << frmRate.absValue
		<< " fps" << endl;

	cout << "Grabbing " << k_numImages << " images" << endl;
	*/
	Image rawImage;
	
	// get reference
	error = cam.RetrieveBuffer(&rawImage);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
	}
	PixelFormat pixFormat;
	unsigned int rows, cols, stride;
	rawImage.GetDimensions(&rows, &cols, &stride, &pixFormat);
	cout << "rows: = " << rows << endl;
	cout << "cols: = " << cols << endl;
	cout << "pixFormat: - " << pixFormat << endl;

	unsigned char* ref_image = rawImage.GetData();
	int** ref_data = imageThreshold(ref_image, 180, rows, cols);
	int* ref_pCentroid = getCentroid(ref_data, rows, cols);
	int centroid_ref[2] = { *ref_pCentroid, *(ref_pCentroid + 1) };
	
	cout << "...........get centroid reference: " << *ref_pCentroid << ", " << *(ref_pCentroid + 1) << "..........." << endl;
	cout << "press any key to continue" << endl;

	int v_control[2];
	int count = 0;
	Image raw_image;
	while(1){
		// Retrieve an image
		error = cam.RetrieveBuffer(&raw_image);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			continue;
		}
		count++;
		cout << "check count: " << count << endl;

		// In 8 bits per pixel modes, the first byte represents the pixel at [row 0, column 0],
		// the second byte at[row 0, column 1], and so on.The top left corner of the image data represents row 0, column 0.
		// Eg. assumming image is 320 x 256, char* data = rawImage.Getdata(). length(data) = 320 x256 = 81920
		// data[0] = Row 0, Column 0 
		// data[1] = Row 0, Column 1
		// data[320] = Row 1, Column 0 
		// data[321] = Row 1, Column 1 
		unsigned char* cvrt_iimage = raw_image.GetData();
		int** imagedata = imageThreshold(cvrt_iimage, 180, rows, cols);
		int* pCentroid = getCentroid(imagedata, rows, cols);
		cout << "The centroid of image= [" << *pCentroid << ", " << *(pCentroid + 1) << "]" << endl;
		//caluacute displacement
		v_control[0] = 0.18 * (centroid_ref[0] - *pCentroid);
		v_control[1] = 0.18 * (centroid_ref[1] - *(pCentroid + 1));
		int* control_transfer = v_control;
		//cout << "see the control voltage scale Vx= " << v_control[0] << ", Vy= " << v_control[1] << endl;
		Serial_Transfer(control_transfer);
		cout << "Press any key to take next frame" << endl;
		char exit = cin.get();
		if (exit == 'X' || exit == 'x') {
			break;
		}
	}
	cout << "Finished grabbing images" << endl;
	// Stop capturing images
	error = cam.StopCapture();
	if (error != PGRERROR_OK)
	{
		cout << "errors when stop capturing" << endl;
		PrintError(error);
		return -1;
	}

	// Disconnect the camera
	error = cam.Disconnect();
	if (error != PGRERROR_OK)
	{
		cout << "errors for camera disconnection" << endl;
		PrintError(error);
		return -1;
	}

	cout << "Done! Press Enter to exit..." << endl;
	cin.ignore();

	return 0;
}
