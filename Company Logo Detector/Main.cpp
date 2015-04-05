/**---------------------------------------------------------------------
 * Subject: [Assignment3] Chamfer Matching using OpenCV.
 * Author: Amr Tj. Wallas, 9th Semester CSEN
 * Id No: *****
 * Tutorial: ****
 * Website: http://tjwallas.weebly.com
 * E-mail(s): eminem.virus2010@gmail.com
 * Platfrom: Linux i386, G++/GCC Compiler 4.4.5
 * Libraries/Frameworks: OpenCV 1.1, GTK 2.0
 * ---------------------------------------------------------------------
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>

//Macro to display a given image y in a new window with name x.
#define CV_DISPLAY(x,y) cvNamedWindow(x, CV_WINDOW_AUTOSIZE);cvShowImage(x, y);cvWaitKey(0);cvDestroyWindow(x)
//Macro to get the file names of templates given their indexes in a loaded templates array
#define TPL(x) (x == 0)? "mobinila.png": (x == 1)? "mobinile.png": "vodafone.png"

//The loaded input image.
IplImage* img;

/**
 * Calculates the chamfer matching score between a DT image and an Edge template
 * @param in: The Distance transform of an input edge-image.
 * @param tpl: The binary edge template image.
 * @return double: The sum of all distances in DT which lie under the 1's of the Edge template.
 */
double calcChamferScore(IplImage* in, IplImage* tpl) {
	//Resize if needed.
	if (in->width < tpl->width || in->height < tpl->height) {
		IplImage* resized = cvCloneImage(in);
		cvResize(tpl, resized);
		tpl = resized;
	}
	cvThreshold(tpl, tpl, 0, 1, CV_THRESH_BINARY); //No need for 255's ... make it 1 to facilitate multiplication.
	IplImage* res = cvCreateImage(cvSize(in->width - tpl->width + 1, in->height - tpl->height + 1), IPL_DEPTH_32F, 1);
	cvMatchTemplate(in, tpl, res, CV_TM_CCORR); //Correlation is the sum of multiplications.
	double min = 0;
	cvMinMaxLoc(res, &min, NULL, NULL, NULL);
	cvReleaseImage(&res); //Memory Cleanup
	return min;

}

/**
 * Calculates the distance transform of an input edge image and exhaustively chamfer-matches it with the given templates.
 * @param in: An input edge-image
 */
void chamferMatch(IplImage* in) {
	IplImage* dstTrans = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, in->nChannels);
	IplImage* dstTrans_8u = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_8U, in->nChannels);
	cvNot(in, in);
	cvDistTransform(in, dstTrans, CV_DIST_C);
	cvConvertScale(dstTrans, dstTrans_8u); //convert from 32F to 8U
	cvReleaseImage(&dstTrans);
	CV_DISPLAY("distance transform", dstTrans_8u);

	//Load the templates.
	IplImage* templates[] = { cvLoadImage("templates/mobinila.png", CV_LOAD_IMAGE_GRAYSCALE), cvLoadImage("templates/mobinile.png",
			CV_LOAD_IMAGE_GRAYSCALE), cvLoadImage("templates/vodafone.png", CV_LOAD_IMAGE_GRAYSCALE) };

	double minRes = 0xffffffff; //max double value.
	int minIndex = -1;
	double currRes = -1;

	//Calculate and get chamfer scores of each template vs the input image.
	for (int i = 0; i < 3; i++) {
		cvThreshold(templates[i], templates[i], 182, 255, CV_THRESH_BINARY);
		cvCanny(templates[i], templates[i], 0, 120); //Edge detection
		currRes = calcChamferScore(dstTrans_8u, templates[i]);
		if (currRes < minRes) {
			minRes = currRes;
			minIndex = i;
		}
		printf("Matching Score vs %s: %f\n", TPL(i), currRes);
	}

	//Print recorded best match. (The one with minimum summation value).
	printf("----------------------------------------------\n");
	printf("----> Best match is %s <----\n", TPL(minIndex));

	//Memory Cleanup.
	cvReleaseImage(&dstTrans_8u);
	for (int i = 2; i >= 0; i--) {
		cvReleaseImage(&templates[i]);
	}
}

/**
 * Calculates the Geometric distance between two points.
 * @param p1: The first (x,y) point
 * @param p2: The second (x,y) point
 * @return int: The geometric distance between p1, p2.
 */
int calcDistance(CvPoint p1, CvPoint p2) {
	return hypot((p1.x - p2.x), (p1.y - p2.y));
}

/**
 * Handles mouse input from the main window.
 * @param event: An event type
 * @param (x,y): Mouse coordinates when the event was fired.
 * @param flags: Other flags to indicate combo presses such as shift+click, .. etc)
 * @param param: Optional parameter to pass
 */
void handleMouse(int event, int x, int y, int flags, void* param) {
	static int clicks; //No. of clicks
	static CvPoint points[4]; //Points recorded.
	if (event == CV_EVENT_LBUTTONDOWN) {
		points[clicks] = cvPoint(x, y); //Save point
		clicks++;

		if (clicks == 4) {
			int width = calcDistance(points[0], points[1]); //Width of the new transformed image
			int height = calcDistance(points[2], points[1]); //height of the new transformed image

			//The user selected points
			CvPoint2D32f srcArr[] = { cvPoint2D32f(points[0].x, points[0].y), cvPoint2D32f(points[1].x, points[1].y), cvPoint2D32f(
					points[2].x, points[2].y), cvPoint2D32f(points[3].x, points[3].y) };

			//Destination of each point in the new intended image after perspective/homography transform.
			CvPoint2D32f dstArr[] = { cvPoint2D32f(0, 0), cvPoint2D32f(width, 0), cvPoint2D32f(width, height), cvPoint2D32f(0, height) };

			CvMat* h = cvCreateMat(3, 3, CV_32FC1);
			cvGetPerspectiveTransform(srcArr, dstArr, h); //Get perspective homography matrix.
			IplImage* imgnew = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);
			cvWarpPerspective(img, imgnew, h); //Apply detected homography matrix
			cvThreshold(imgnew, imgnew, 182, 255, CV_THRESH_BINARY); //I canz eliminate shadows?
			cvCanny(imgnew, imgnew, 50, 120); //Edge detection
			CV_DISPLAY("Feature detection", imgnew);
			chamferMatch(imgnew); //Apply distance transform and chamfer matching.

			//Memory Cleanup.
			cvReleaseImage(&imgnew);
			cvReleaseMat(&h);
		}
	}
}

/**
 * And Finally, the main Method.
 */
int main(int argc, char *argv[]) {
	char path[20];
	printf("Please enter path to input image:\n");
	gets(path); //Yeah, I know it's buffer overflow vulnerable
	img = cvLoadImage(path, CV_LOAD_IMAGE_GRAYSCALE);
	cvNamedWindow("Input Image", CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback("Input Image", handleMouse); //Register mouse handler.
	cvShowImage("Input Image", img);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&img); //Memory Cleanup.
	return 0;
}
