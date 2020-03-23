#include <jni.h>
#include "pch.h"
#include "com_huangzb_demo_PHash.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\core\mat.hpp>
using namespace std;
using namespace cv;
int fingerprint(Mat src, Mat* hash);
String init(Mat src);
JNIEXPORT jstring JNICALL Java_com_huangzb_demo_PHash_PHashMatch
(JNIEnv* env, jobject obj, jstring srcStr) {
 char const* str;
str = env->GetStringUTFChars(srcStr, 0);
if(str == NULL) {
return NULL; 
}

std::cout << str << std::endl;
//Mat src = imread("F:\\testpic\\chun.jpg", 0);
//String outputpath = init(src);
Mat src = imread(str, 0);
String outputpath = init(src);
//释放资源
env->ReleaseStringUTFChars(srcStr, str);

// 返回一个字符串
char const* tmpstr = outputpath.c_str();
jstring rtstr = env->NewStringUTF(tmpstr);
return rtstr;
}
JNIEXPORT jstring JNICALL Java_com_huangzb_demo_PHash_PHashMatchMore
(JNIEnv* env, jobject obj, jobject jobj1, jobject jobj2) {

}
int fingerprint(Mat src, Mat* hash)
{
	resize(src, src, Size(32, 32));
	src.convertTo(src, CV_32F);
	Mat srcDCT;
	dct(src, srcDCT);
	srcDCT = abs(srcDCT);
	double sum = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			sum += srcDCT.at<float>(i, j);

	double average = sum / 64;
	Mat phashcode = Mat::zeros(Size(8, 8), CV_8U);
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			phashcode.at<char>(i, j) = srcDCT.at<float>(i, j) > average ? 1 : 0;

	*hash = phashcode.reshape(0, 1).clone();
	return 0;
}

String  init(Mat src) {

	if (src.empty())
	{
		cout << "the image is not exist" << endl;
		return "";
	}
	Mat srchash, dsthash;
	fingerprint(src, &srchash);
	int min = 100; int mini = 0;
	for (int i = 1; i <= 4; i++)
	{
		string path0 = "F:\\testpic\\chun";
		string number;
		stringstream ss;
		ss << i;
		ss >> number;
		string path = "F:\\testpic\\chun" + number + ".jpg";
		Mat dst = imread(path, 0);
		if (dst.empty())
		{
			cout << "the image is not exist" << endl;
			return "";
		}
		fingerprint(dst, &dsthash);
		int d = 0;
		for (int n = 0; n < srchash.size[1]; n++)
			if (srchash.at<uchar>(0, n) != dsthash.at<uchar>(0, n)) d++;

		cout << "chun" << i << " distance= " << d << "\n";
		if (d < min) {
			min = d;
			mini = i;
		}
		//personnel[i] = d;

	}
	string number;
	stringstream ss;
	ss << mini;
	ss >> number;
	String mypath = "F:\\testpic\\chun" + number + ".jpg";
	cout << mypath << endl;
	return mypath;
}