//
// Created by Jack Yu on 21/10/2017.
//

#ifndef SWIFTPR_PLATERECOGNIZE_H
#define SWIFTPR_PLATERECOGNIZE_H
#include <opencv2/opencv.hpp>

typedef enum 
{ 
	BLUE, YELLOW, WHITE, GREEN, BLACK,UNKNOWN
}PlateColor ;//������ɫ
typedef enum  
{
	CHINESE,LETTER,LETTER_NUMS,INVALID
}CharType;//�����ַ�����
#ifndef __ALG_RECT__
#define __ALG_RECT__
typedef struct
{
	int x;
	int y;
	int width;
	int height;
}CRect;
#endif
typedef struct 
{
	char plateName[50];//������
	CRect plateRect;//��������
	PlateColor plateType;//������ɫ
	float confidence;//�������Ŷ�
}PlateInfo;

int LoadPlateNet();//���س�������
int FreePlateNet(int flag);//�ͷų�������
int PlateDetectandRecognize(cv::Mat image, const int segmentation_method, PlateInfo*  plateInfo, int flag);//����ʶ��
PlateInfo PlateRecognizeOnly(cv::Mat plateROI, const int segmentation_method, int flag);//���복������ͼ�񣬽���ʶ��


#endif //SWIFTPR_CNNRECOGNIZER_H
