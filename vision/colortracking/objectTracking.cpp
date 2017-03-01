#include <iostream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

	cout << "Starting" << endl;

	/* capture video from webcam */
	VideoCapture cap(0);

	if (!cap.isOpened()) { // if not success, exit program
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	/* create a window called "Control" */
	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	/* table used to divide image colors */
	int divideWidth = 0;
	stringstream s;
	s << argv[1];
	s >> divideWidth;
	if (!s || !divideWidth) {
		cout << "error";
	}
	uchar table[256];
	for (int i = 0; i < 256; ++i) {
		table[i] = (uchar)(divideWidth * (i/divideWidth));
	}
	Mat lookUpTable(1, 256, CV_8U);
	uchar * p = lookUpTable.data;
	for (int i = 0; i < 256; ++i) {
		p[i] = table[i];
	}
	cout << "lookUpTable created successfully" << endl;

	/* Orange 109-119
	   Red 115 - 179 */
	int iLowH = 112;
	int iHighH = 119;

	int iLowS = 115; 
	int iHighS = 255;

	int iLowV = 75;
	int iHighV = 255;

	/* create trackbars in "Control" window */
	createTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 255)
	createTrackbar("HighH", "Control", &iHighH, 255);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	int iLastX = -1; 
	int iLastY = -1;

	/* Capture a temporary image from the camera */
	Mat imgTmp;
	cap.read(imgTmp); 

	/* Create a black image with the size as the camera output */
	Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 

	while (true) {
		Mat imgOriginal;

		//bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		/* read a new frame from the video */
		if (!(cap.read(imgOriginal))) { //if not success, break loop
		     cout << "Cannot read a frame from video stream" << endl;
		     break;
		}
		
		Mat imgDiv;
		LUT(imgOriginal, lookUpTable, imgDiv);
		imshow("Divided Image", imgDiv);

		Mat imgSharp;
		GaussianBlur(imgOriginal, imgSharp, Size(0,0), 3);
		addWeighted(imgOriginal, 1.5, imgSharp, -0.5, 0, imgSharp);
		imshow("Sharpened Image", imgSharp);

		Mat imgHSV;

		/* convert the captured frame from RGB to HSV */
		cvtColor(imgOriginal, imgHSV, COLOR_RGB2HSV); 
		 
		Mat imgThresholded;

		/* threshold the image */
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), 
				Scalar(iHighH, iHighS, iHighV), imgThresholded);
		      
		/* morphological opening (removes small objects from the foreground) */
		erode(imgThresholded, imgThresholded, 
				getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, 
				getStructuringElement(MORPH_ELLIPSE, Size(5, 5))); 

		/* morphological closing (removes small holes from the foreground) */
		dilate(imgThresholded, imgThresholded, 
				getStructuringElement(MORPH_ELLIPSE, Size(5, 5))); 
		erode(imgThresholded, imgThresholded, 
				getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		/* Gaussian blur to better define circles */
		GaussianBlur(imgThresholded, imgThresholded, Size(9, 9), 2, 2);

		/* show the thresholded image */
		imshow("Thresholded Image", imgThresholded);

		/* show the original image */
		imgOriginal = imgOriginal + imgLines;
		imshow("Original", imgOriginal);

		/* detect circles */
		//cout << "Detecting circles" << endl;
		vector<Vec3f> circles;
		HoughCircles(imgThresholded, circles, CV_HOUGH_GRADIENT, 1, 
				imgThresholded.rows/8, 100, 20, 0, 0);
		if (circles.size() != 0) {
			Point center(round(circles[0][0]));
			cout << "Circle: " << center.x - (imgThresholded.size().width/2)
				<< endl;
		}

		/* Calculate the moments of the thresholded image */
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		/* if the area <= 10000, I consider that the there are no object 
		   in the image and it's because of the noise, the area is not zero */
		if (dArea > 10000) {
			/* calculate the position */
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;        

			/* position from center */
			//cout << "Moment: " << posX - (imgThresholded.size().width / 2) << endl;
		
			/*
			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0) {
				//Draw a red line from the previous point to the current point
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), 
					Scalar(0,0,255), 2);
			}
			*/

			iLastX = posX;
			iLastY = posY;
		} 

		/* show the thresholded image */
		//imshow("Thresholded Image", imgThresholded);

		/* show the original image */
		//imgOriginal = imgOriginal + imgLines;
		//imshow("Original", imgOriginal);

		/* wait for 'esc' key press for 30ms */
		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
			break; 
		}
	}

   return 0;
}
