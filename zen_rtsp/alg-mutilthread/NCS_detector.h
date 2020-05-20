#ifndef __NCS_DETECTOR_H__
#define __NCS_DETECTOR_H__
#include <sys/time.h>
#include "DSPARMProto.h"
#include "m_arith.h"
#include "Python.h"
#include <numpy/arrayobject.h>
#include <opencv2/opencv.hpp>
using namespace cv;
////////////////////////////////////////////////////////////
void py_init();//����python,��NCS���м��
void py_free();//�ͷ�python�ڴ�
int get_ncs_id();//���������NCS id
void free_ncs_id(int NCS_ID);//�ͷ������NCS id
extern int NCSArithDetect(Mat BGRImage, ALGCFGS* pCfgs, int* rst);

#endif



