#include <jni.h>
#include "pch.h"
#include "com_huangzb_demo_PHash.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\core\mat.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2/imgcodecs/legacy/constants_c.h"
#include "func.cpp"
#include <opencv2\imgproc\types_c.h>
#include<list>
using namespace std;
using namespace cv;

class Pic
{
public:
	String id;   
	Mat picMat;  
};

int fingerprint(Mat src, Mat* hash);
String init(Mat src);
Mat MyJobject2Mat(JNIEnv* env, jobject jobj1);
JNIEXPORT jstring JNICALL Java_com_huangzb_demo_PHash_PHashMatch
(JNIEnv* env, jobject obj, jstring srcStr) {
	char const* str;
	str = env->GetStringUTFChars(srcStr, 0);
	if (str == NULL) {
		return NULL;
	}

	//std::cout << str << std::endl;
	//Mat src = imread("F:\\testpic\\chun.jpg", 0);
	//String outputpath = init(src);
	Mat src = imread(str, 0);
	String outputpath = init(src);
	//�ͷ���Դ
	env->ReleaseStringUTFChars(srcStr, str);

	// ����һ���ַ���
	char const* tmpstr = outputpath.c_str();
	jstring rtstr = env->NewStringUTF(tmpstr);
	return rtstr;
}

JNIEXPORT jstring JNICALL Java_com_huangzb_demo_PHash_PHashMatchMore
(JNIEnv* env, jobject obj, jobject jobj1, jobject jobjList) {
	//��ȡlist 
	jclass cls_arraylist = env->GetObjectClass(jobjList);
	//��ȡ����
	jmethodID arraylist_get = env->GetMethodID(cls_arraylist, "get", "(I)Ljava/lang/Object;");
	jmethodID arraylist_size = env->GetMethodID(cls_arraylist, "size", "()I");
	//��ȡ����
	jint len = env->CallIntMethod(jobjList, arraylist_size);
	Pic PicArray[100];
	for (int i = 0; i < len; i++) {
		jobject obj_user = env->CallObjectMethod(jobjList, arraylist_get, i);


	}

	String outputpath = init(MyJobject2Mat(env,jobj1));
	// ����һ���ַ���
	char const* tmpstr = outputpath.c_str();
	jstring rtstr = env->NewStringUTF(tmpstr);
	return rtstr;
}

Mat MyJobject2Mat(JNIEnv* env, jobject jobj1) {
	//���pic������
	jclass pic_cla = env->GetObjectClass(jobj1);
	if (pic_cla == NULL)
	{
		cout << "GetObjectClass failed \n";
	}
	//��ȡ���е�����
	jfieldID idFieldID = env->GetFieldID(pic_cla, "id", "Ljava/lang/String;"); //��õ�Student�������id 
	jfieldID picdataFieldID = env->GetFieldID(pic_cla, "picdata", "Ljava/lang/String;"); // �������ID

	//�����е�����ת����string
	//1.jstring��������ֵ
	jstring id = (jstring)env->GetObjectField(jobj1, idFieldID);  //�������ֵ
	jstring picdata = (jstring)env->GetObjectField(jobj1, picdataFieldID);//�������ֵ
	//2.const char*����ת����jstring
	const char* c_picdata = env->GetStringUTFChars(picdata, NULL);//ת���� char *
	const char* c_id = env->GetStringUTFChars(id, NULL);//ת���� char *
	//3.const char*ת����string����
	string str_picdata = c_picdata;
	string str_id = c_id;
	//4.�ͷ�����
	env->ReleaseStringUTFChars(picdata, c_picdata);
	env->ReleaseStringUTFChars(id, c_id);

	cout << "=======C++====" << endl;
	///*cout << " at Native age is :" << id << " # name is " << str_name << endl;*/
	///*const char* str = "C++String";*/
	//jstring rtstr = env->NewStringUTF(str);//env->NewStringUTF(c_name);

	//�����������ַ���ת����matͼ�񷽱����ͼ����
	Mat mymat = Base2Mat(str_picdata);
	Mat testpic;
	//��ʹ��base64ת����ͼ��ת���ɻҶ�ͼ����
	cvtColor(mymat, testpic, CV_RGB2GRAY);
	return testpic;
}
int fingerprint(Mat src, Mat* hash)
{
	cout << "Mat  src.type()=" << src.type() << "\n";
	resize(src, src, Size(32, 32));
	src.convertTo(src, CV_32FC1, 1.0/255.0);
	/*src.convertTo(src, CV_32F);*/
	cout << "src.type()="<<src.type() << "\n";
	Mat srcDCT;
	cout <<"src.type()"<< src.type() << endl;
	cout << "srcDCT.type()" << srcDCT.type() << endl;
	dct(src, srcDCT);
	cout << "dct(src, srcDCT);"  << "\n";
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
