#ifndef __ATTRIBUTE_DETECT_H__
#define __ATTRIBUTE_DETECT_H__
#ifdef DETECT_PERSON_ATTRIBUTE
#include "m_arith.h"
#include <opencv2/opencv.hpp>
using namespace cv;
//��������ʶ��
void attri_init();//��ʼ��python,��ʼ��ȫ�ֱ���
#ifdef USE_PYTHON
void py_attri_init();//����python�ļ����м��
#else
void LoadAttriNet(int gpu_idx);//����caffe c++���м��
///////////////////////////////////////////////////////////////////////////����caffe c++���м��
//deploy_file ���������ļ�
//trained_file ��������ļ�
//gpu_idx gpu ID
//net_idx net ID
extern void LoadAttriNet(const char* deploy_file,
						 const char* trained_file,
						 int gpu_idx, int net_idx);//���ؼ������

//img ���ͼ��
//net_idx net ID
//result ����ֵ
extern void AttriDetect(unsigned char* imgdata, int w, int h, int net_idx, int* result);//�����������
#endif

bool HumanAttributeInit(ALGCFGS *pCfgs);//�������Գ�ʼ��
HumanAttribute HumanAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs);//��������ʶ��
void HumanAttributeDetect(ALGCFGS *pCfgs, IplImage* img);//�������Լ�����

//��������ʶ��
bool BicycleAttributeInit(ALGCFGS *pCfgs);//�������Գ�ʼ��
BicycleAttribute BicycleAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs);//��������ʶ��
void BicycleAttributeDetect(ALGCFGS *pCfgs, IplImage* img);//�������Լ�����
#endif

#endif