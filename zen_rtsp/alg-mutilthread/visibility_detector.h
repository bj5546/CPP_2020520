#ifndef __VISIBILITY_DETECTOR_H__
#define __VISIBILITY_DETECTOR_H__
#include "m_arith.h"
//ͳ��ÿ��sobel��Ե������ȷ���ܼ��ȵ�λ��
Uint16 cal_edge_num(Uint8* img, int width, int height);
//����ÿ�жԱȶȣ����Աȶȴ���0.05ʱ��������λ��
Uint16 cal_visibility(Uint8* img, int calibration_point[][2], int width, int height);
//���������Աȶȣ����Աȶȴ���0.05ʱ��������λ��
Uint16 cal_region_visibility(Uint8* img, int calibration_point[][2], int width, int height);
Uint16 DayVisibilityDetection(Uint8* img, int calibration_point[][2], int width, int height);//�����ܼ��ȼ���
float NightVisibilityDetection(Uint8* img, int calibration_point[][2], int width, int height, float l1, float l2);//�����ܼ��ȼ���
#endif