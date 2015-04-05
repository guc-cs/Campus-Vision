/**---------------------------------------------------------------------
 * Subject: [Assignment2] Simple binary image morphology using OpenCV.
 * Author: Amr Tj. Wallas, 9th Semester CSEN
 * Id No: ****
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

/**
 * Function that represents a certain pixel-data in a given image.
 * @param [x,y]: The position of that pixel inside the image
 * @param img: A pointer to an image
 * @return char*: A pointer to the data of that pixel within the image data.
 */
unsigned char* I(int x, int y, IplImage* img) {
	return &((unsigned char*) (img->imageData + img->widthStep * y))[x];
}

/**
 * Data structure that represents a certain image along with the position of the
 * center of the dot in it as well as the radius of that dot.
 */
struct PointImage {
	IplImage *img;
	CvPoint pt;
	int r;
	PointImage(IplImage* image, CvPoint point, int radius) {
		img = image;
		pt = point;
		r = radius;
	}
};

/**
 * Accurately applies circle detection and filtering in order to detect the position
 * of the dot within an image then, it draws a red rectangle around it in the
 * original image.
 * @param img: pointer to the input image
 * @return PointImage: pointer to a newly created data structure holding the image,
 * center position of the dot within the image as well as it's radius.
 */
PointImage* getPoint(IplImage* img) {
	IplImage* clone = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	cvCvtColor(img, clone, CV_RGB2GRAY);
	cvThreshold(clone, clone, 127, 255, CV_THRESH_BINARY_INV);
	cvSmooth(clone, clone, CV_GAUSSIAN, 31, 31);
	cvMorphologyEx(clone, clone, NULL, NULL, CV_MOP_CLOSE, 6);
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* circles = cvHoughCircles(clone, storage, CV_HOUGH_GRADIENT, ((double) (clone->width * clone->height * 5)) / ((double) 964 * 726),
			50, 200, 100, 0, 35);
	float* p;
	PointImage* out;
	for (int i = circles->total; i > 0; i--) {
		p = (float*) cvGetSeqElem(circles, i);
		if (*I(cvRound(p[0]), cvRound(p[1]), clone) > 185)
			if (abs(cvRound(p[0]) - (clone->width / 2) * (clone->width / clone->height)) <= 100) {
				cvRectangle(img, cvPoint(cvRound(p[0] - p[2]), cvRound(p[1] - p[2])), cvPoint(cvRound(p[0] + p[2]), cvRound(p[1] + p[2])),
						CV_RGB(255,0,0), 3);
				out = new PointImage(clone, cvPoint(cvRound(p[0]), cvRound(p[1])), cvRound(p[2]));
				break;
			}
	}
	cvNamedWindow("Circle Detection", CV_WINDOW_AUTOSIZE);
	cvShowImage("Circle Detection", img);
	cvReleaseMemStorage(&storage);
	return out;
}

/**
 * Does some processing and scaling on both the input image and the template image
 * in order to improve template detection accuracy. This function also extracts and
 * draws the region of interest or the window that contains the room no. based on
 * the circle size and radius specified in the input PointImage data structure.
 * @param in: Datastructure holding input image, radius of circle, center of circle.
 * @param tpl: pointer to a loaded template image
 * @return IplImage: pointer to the extracted ROI as specified above.
 */
IplImage* preProcess(PointImage* in, IplImage* tpl) {
	static bool flag = false;
	IplImage* img = in->img;
	CvPoint pt = in->pt;
	int r = in->r;
	cvThreshold(img, img, 150, 255, CV_THRESH_BINARY);
	int rect_x_pos = cvRound((double) pt.x + 76.0 * (double) r / 22);
	int rect_y_pos = cvRound((double) pt.y + 24.0 * (double) r / 22);
	int window_width = cvRound(130.0 * (double) r / 22);
	int window_height = cvRound(200.0 * (double) r / 22);
	CvRect rect = cvRect(rect_x_pos, rect_y_pos - window_height, window_width * 3 + 20, window_height);
	cvSetImageROI(img, rect);
	IplImage* imgnew = cvCreateImage(cvSize(rect.width * 1.2, rect.height * 1.2), img->depth, img->nChannels);
	cvResize(img, imgnew);
	IplImage* tplnew = cvCreateImage(cvSize(rect.width / 3, rect.height), img->depth, img->nChannels);
	cvResize(tpl, tplnew);
	*tpl = *tplnew;
	if (!flag) {
		cvNamedWindow("Region of interest", CV_WINDOW_AUTOSIZE);
		cvShowImage("Region of interest", imgnew);
		cvMoveWindow("Region of interest", 300, 20);
		cvWaitKey(0);
		cvDestroyAllWindows();
		flag = true;
	}
	return imgnew;
}

/**
 * Returns matching ratio by analyzing a result image from cvMatchTemplate(..) using
 * CV_TM_SQDIFF_NORMED. Typically this matching method returns min value pixels in case
 * of perfect match (Zero = perfect match, 1 = total mis-match)
 * @param in: pointer to the result from cvMatchTemplate(..)
 * @return double: matching ratio.
 */
double calcMatchingPercent(IplImage* in) {
	double min = 0;
	cvMinMaxLoc(in, &min, NULL, NULL, NULL);
	return min;
}

/**
 * Estimates the room number digits by smartly analyzing an input ROI or window
 * and applying template matching to sub-windows representing the digits' images
 * respectively in order to find the best match.
 * @param in: Datastructure containing input image, circle radius and center.
 */
void calcRoom(PointImage* in) {
	char path[20]; //temp to hold path to template images
	IplImage* tpl, *tempRes, *img = cvCreateImage(cvSize(0, 0), 8, 1);
	IplImage templates[10]; //array of loaded template images
	int room[3], matchIndex = -1; //array holding room detected digits
	CvRect rect;
	double maxMatch = 1.5, currMatch = 1.5; //their max range is typically 1.0.

	//Load the templates
	for (int k = 0; k < 10; k++) {
		sprintf(path, "%s%d%s", "templates/", k, ".png");
		templates[k] = *cvLoadImage(path, CV_LOAD_IMAGE_GRAYSCALE);
	}

	//find best match for every sub-window (the three subimages from ROI)
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i <= 9; i++) {
			tpl = &templates[i];
			img = preProcess(in, tpl);
			rect = cvRect(j * cvRound(img->width / 3), 0, cvRound(img->width / 3), img->height);
			cvSetImageROI(img, rect);
			tempRes = cvCreateImage(cvSize(rect.width - tpl->width + 1, rect.height - tpl->height + 1), IPL_DEPTH_32F, 1);
			cvMatchTemplate(img, tpl, tempRes, CV_TM_SQDIFF_NORMED);
			currMatch = calcMatchingPercent(tempRes);
			if (currMatch < maxMatch) {
				maxMatch = currMatch;
				matchIndex = i;
			}
			cvReleaseImage(&tempRes);
		}
		//Assign current detected digit.
		room[j] = matchIndex;
		//Reset temporary vars.
		currMatch = 1.5;
		maxMatch = 1.5;
		matchIndex = -1;
	}
	//print the room no.
	room[1] = (room[1] == (CV_TM_CCOEFF_NORMED | 6)) ? 1 : room[1];
	printf("Room no: %d %d %d\n", room[0], room[1], room[2]);

	//Memory cleanup.
	cvReleaseImage(&img);
	cvReleaseImage(&in->img);
	for (int l = 0; l < 10; l++) {
		cvReleaseImage((IplImage**) &templates[l]);
	}
}

/**
 * And Finally, the main method.
 */
int main(void) {
	char path[20];
	printf("Please enter an input image path: \n");
	gets(path); //Yeah, I know its buffer overflow vulnerable.
	IplImage *img = cvLoadImage(path, CV_LOAD_IMAGE_UNCHANGED);
	PointImage* out = getPoint(img);
	calcRoom(out);
	delete out;
	cvReleaseImage(&img);
	return 0;
}
