#include <iostream>

#include <cstdio>

#include <stdlib.h>

#include <opencv2/opencv.hpp>

#include <opencv2/core.hpp>

#include <opencv2/imgcodecs.hpp>

#include <opencv2/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>

using namespace std;



cv::Mat masking(cv::Mat);//return masking image

void fill(cv::Mat);

int * find(cv::Mat);//return left,right,top,bottom

cv::Mat cutting(cv::Mat, int *);//return cuting image

void histogram(cv::Mat);

void checkBalance(cv::Mat);


int main(int argc, char* argv[]) {

	string name = "Test3.jpg";

	cv::Mat image = cv::imread("C:/Users/opo/Desktop/image2/" + name, cv::IMREAD_COLOR);//원본 이미지

	cv::Mat original = cv::imread("C:/Users/opo/Desktop/image2/" + name, cv::IMREAD_COLOR);//원본 이미지

	int row = image.rows;//세로

	int col = image.cols;//가로



	cv::Mat black = masking(image);//흑백 이미지

	cv::Mat rough = masking(image);



	int * index = find(black);//빡세게 잡은거

	int * index_rough = find(rough);//러프하게 잡은거



	cout << "*** case B ***" << endl;

	cout << "left : " << index[0] - index_rough[0] << ", right : " << index_rough[1] - index[1] << endl;

	cout << "top : " << index[2] - index_rough[2] << ", bottom : " << index_rough[3] - index[3] << endl;





	cv::Mat capture = cutting(image, index);//잘린 이미지

	cv::Mat cuttingBlack = cutting(black, index);//Balance를 확인하기위해 black의 이미지 사이즈 cutting

	checkBalance(cuttingBlack);//cutting한 black으로 Balance확인

	fill(rough);



	vector<vector<cv::Point>> contours;

	cv::findContours(rough, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int cmin = 100;  // 최소 외곽선 길이

	int cmax = 1000; // 최대 외곽선 길이

	vector<vector<cv::Point>>::const_iterator itc = contours.begin();

	while (itc != contours.end()) {

		if (itc->size() < cmin || itc->size() > cmax)

			itc = contours.erase(itc);

		else

			++itc;

	}

	cv::drawContours(original, contours, -1, cv::Scalar(255, 255, 255), 2);



	histogram(capture);//색조 판단

	cv::imshow("original", image);

	cv::imshow("masking", black);

	cv::imshow("rough", rough);

	cv::imshow("slice", capture);



	cv::waitKey(0);

	return 0;

}



cv::Mat masking(cv::Mat image) {
	cv::Mat black;
	
	cv::Mat gray_image;
	
	medianBlur(image, image, 7);
	
	cv::cvtColor(image, gray_image, CV_BGR2GRAY); // 흑백영상으로 변환
	
	
	// get a adaptive threshold image
	cv::adaptiveThreshold(gray_image, black, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 401, 60);

	return black;//흑백으로 마스킹된 이미지 반환

}



void fill(cv::Mat black) {

	int row = black.rows;

	int col = black.cols;

	int read = -1;

	int start = 0;

	int end = 0;



	for (int i = 0; i < row; i++) {

		int x = col - 1;

		while (x >= 0) {

			read = black.at<uchar>(i, x);

			if (read == 255) {

				end = x;

				break;

			}

			x--;

		}

		x = 0;

		while (x < col) {

			read = black.at<uchar>(i, x);

			if (read == 255) {

				start = x;

				break;

			}

			x++;

		}

		for (int j = start; j < end; j++)

			black.at<uchar>(i, j) = 255;

		start = 0;

		end = 0;

	}



	for (int i = 0; i < col; i++) {

		int x = row - 1;

		while (x >= 0) {

			read = black.at<uchar>(x, i);

			if (read == 255) {

				end = x;

				break;

			}

			x--;

		}

		x = 0;

		while (x < row) {

			read = black.at<uchar>(x, i);

			if (read == 255) {

				start = x;

				break;

			}

			x++;

		}

		for (int j = start; j < end; j++)

			black.at<uchar>(j, i) = 255;

		start = 0;

		end = 0;

	}

}



int * find(cv::Mat black) {

	int row = black.rows;

	int col = black.cols;

	int left = 0, right = 0, top = 0, bottom = 0;

	int read = -1;

	bool flag = false;

	for (int i = 0; i < row; i++) {//find top

		for (int j = 0; j < col; j++) {

			read = black.at<uchar>(i, j);

			if (read == 0) {

				top = i;

				read = -1;

				flag = true;

				break;

			}

		}

		if (flag) {

			flag = false;

			break;

		}

	}

	for (int i = row - 1; i >= 0; i--) {//find bottom

		for (int j = 0; j < col; j++) {

			read = black.at<uchar>(i, j);

			if (read == 0) {

				bottom = i;

				read = -1;

				flag = true;

				break;

			}

		}

		if (flag) {

			flag = false;

			break;

		}

	}

	for (int i = 0; i < col; i++) {//find left

		for (int j = 0; j < row; j++) {

			read = black.at<uchar>(j, i);

			if (read == 0) {

				left = i;

				read = -1;

				flag = true;

				break;

			}

		}

		if (flag) {

			flag = false;

			break;

		}

	}

	for (int i = col - 1; i >= 0; i--) {//find right

		for (int j = 0; j < row; j++) {

			read = black.at<uchar>(j, i);

			if (read == 0) {

				right = i;

				read = -1;

				flag = true;

				break;

			}

		}

		if (flag) {

			flag = false;

			break;

		}

	}

	cout << "left" << left << "right" << right << endl;

	cout << "top" << top << "bottom" << bottom << endl;

	int * index = (int *)malloc(sizeof(int) * 4);

	index[0] = left;

	index[1] = right;

	index[2] = top;

	index[3] = bottom;

	return index;

}

cv::Mat cutting(cv::Mat image, int * index) {//마스킹된 이미지를 기반으로 원본 이미지를 자름

	int left = index[0];

	int right = index[1];

	int top = index[2];

	int bottom = index[3];



	cv::Mat capture = image;

	if (!(left == 0 || right == 0 || top == 0 || bottom == 0))//예외 처리

		capture = image(cv::Range(top, bottom), cv::Range(left, right));



	return capture;//잘린 이미지 리턴

}

void ImageFilter() {

}

void histogram(cv::Mat capture) {

	cv::Mat dst;

	cv::Mat bgr[3];

	cv::Mat hist; //Histogram 계산값 저장

	int channel[] = { 0,1,2 };

	int histSize = 255; //Histogram 가로값의 수

	int count = 0;

	float range[] = { 0,255.0 };

	const float * ranges = range;

	int hist_w = 512; int hist_h = 400;

	int number_bins = 255;

	int bin_w = cvRound((double)hist_w / number_bins);

	unsigned row2 = capture.rows; unsigned col2 = capture.cols; //자른 사진의 크기 저장



	cvtColor(capture, dst, CV_HSV2BGR); //Color 변경

	calcHist(&dst, 3, channel, cv::Mat(), hist, 1, &histSize, &ranges, true, false); //Histogram 계산

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());



	for (int i = 1; i < number_bins; i++) {	//Histogram 선 그리기

		line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(hist.at<float>(i - 1))), cv::Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))), cv::Scalar(0, 255, 0), 2, 8, 0);

	}



	for (int i = 0; i < histSize; i++) { //색의 다양성 검출

		if (hist.at<float>(i) > 229) {

			count++;

		}

	}



	printf("카운트 수 : %d\n", count);



	if (count > 10) {

		printf("다양한 색조를 보입니다.");

	}

	else {

		printf("다양한 색조를 보이지 않습니다.");

	}



	cv::namedWindow("Histogram", CV_WINDOW_AUTOSIZE);

	cv::imshow("HSV2BGR", dst);

	cv::imshow("Histogram", histImage);

}

void checkBalance(cv::Mat cutblack) {
	
	int halfRow = cutblack.rows / 2;
	
	int halfCol = cutblack.cols / 2;
	
	int countk = 0;
	
	int read = -1, Preread = -1, Postread = -1;
	
	int size = cutblack.rows*cutblack.cols;
	
	int* savei = new int[size];
	
	int* savej = new int[size];
	
	for (int j = 1; j < cutblack.rows-1; j++) {
	
		for (int i = 1; i < cutblack.cols-1; i++) {
		
			read = cutblack.at<uchar>(j, i);
			Preread = cutblack.at<uchar>(j, i-1);
			Postread = cutblack.at<uchar>(j, i+1);
			if ((read == 0 && Preread !=0) ||(read == 0 && Postread != 0)) {
			
				savej[countk] = j;
				
				savei[countk++] = i;
			
			}
		
		}
	
	}
	
	/*for (int i = 0; i < size; i++) {
	
		if (savej[i] != 0) {

			cout << i << "번 째" << " 중심으로 부터 거리는 : " << abs((halfRow - savej[i]))*abs((halfRow - savej[i])) + abs((halfCol - savei[i]))*abs((halfCol - savei[i])) << endl;
	
		}

	}*/
	
	delete[] savei;
	
	delete[] savej;
	
	//return;*/

}
