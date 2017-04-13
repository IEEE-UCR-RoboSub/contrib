
#include <opencv2/core/utility.hpp>
#include <opencv2/saliency.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;
using namespace saliency;

static const char* keys =
{ "{@video_name	| | video name}"
	"{@start_frame	|1| start frame}" };

static void help() {
	cout << "\n ./demo [optional]<video_name> [optional]<start_frame>" << endl;
}

Mat morphMap(Mat saliencyMap) {
	/* */
	erode(saliencyMap, saliencyMap, 
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(saliencyMap, saliencyMap, 
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	/* */
	dilate(saliencyMap, saliencyMap, 
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(saliencyMap, saliencyMap, 
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	/* */
	GaussianBlur(saliencyMap, saliencyMap, Size(9,9), 2, 2);

	return saliencyMap;
}

Mat addScaledMap(Mat saliencyMap, Ptr<Saliency> saliencyAlgorithm, 
		Mat image, int scale) {
	Mat scaledImage;
	Mat scaledMap;

	Size imageSize( image.size().width, image.size().height );
	Size scaledSize( image.size().width/scale, image.size().height/scale );
	resize( image, scaledImage, scaledSize );

	if( saliencyAlgorithm->computeSaliency( scaledImage, scaledMap ) ) {
		resize( scaledMap, scaledMap, imageSize );
		addWeighted( saliencyMap, (((float) scale-1)/(float) scale), 
				scaledMap, (1/(float) scale), 1, saliencyMap );
	}

	return morphMap(saliencyMap);
}

Mat get_saliencyMap(Mat image) {
	Mat scaledImage,
	    saliencyMap,
        scaledMap,
	    sumSaliencyMap;

	Ptr<Saliency> saliencyAlgorithm = Saliency::create( "FINE_GRAINED" );

    if( saliencyAlgorithm->computeSaliency( image, saliencyMap ) ) {

        saliencyMap.copyTo( sumSaliencyMap );

        int max = 10;
        for ( int i = 2 ; i < max ; i++ ) {
            addScaledMap(sumSaliencyMap, saliencyAlgorithm, image, i);
        }
    }

    return sumSaliencyMap;
}

vector<vector<Point>> get_contours(
        Mat src_img, 
        vector<vector<Point>> contours) {
    Mat canny_out;

    Canny(src_img, canny_out, 50, 100, 3);
    findContours(canny_out, contours, CV_RETR_LIST, 
            CV_CHAIN_APPROX_SIMPLE);

    return contours;
}

int main( int argc, char** argv ) {
	CommandLineParser parser( argc, argv, keys );

	String video_name = parser.get<String>( 0 );
	int start_frame = parser.get<int>( 1 );

	VideoCapture cap;
	if ( video_name.empty() ) {
		cap.open(0);
	} else {
		cap.open( video_name );
		cap.set( CAP_PROP_POS_FRAMES, start_frame );
	}

	if( !cap.isOpened() ) {
		help();
		cout << "***Could not initialize capturing...***\n";
		cout << "Current parameter's value: \n";
		parser.printMessage();
		return -1;
	}

	for ( ;; ) {
        Mat image;
        cap >> image;

        Mat saliency = get_saliencyMap(image);

        vector<vector<Point>> contours;
        contours = get_contours(saliency, contours);

        cout << "Contours: " << contours.size() << endl;

        RNG rng(12345);
        for (int i = 0; i < contours.size(); i++) {
            Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255),
                    rng.uniform(0,255));
            drawContours(image, contours, i, color, 2, 8);
        }

        imshow( "Original Image", image );
        //imshow( "Saliency Map", saliency );

		if (waitKey(30) == 27) {
			break;
		}
	}
}