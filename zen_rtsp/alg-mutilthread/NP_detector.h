#ifndef __NP_DETECTOR_H__
#define __NP_DETECTOR_H__
#include "m_arith.h"
Uint16 NPDetector(Mat img, NonMotorInfo* NPDetectInfo, ALGCFGS *pCfgs);//���зǻ��������Ա���
Uint16 AnalysisNPDetect(ALGCFGS* pCfgs, NonMotorInfo* NPDetectInfo);//���������
bool detect_riderNum(CRect nonMotorBox, CRect* riderBox, int boxNum);//�����������
bool detect_helmet(CRect nonMotorBox, CRect helmetBox);//�Ƿ��ñ
#endif