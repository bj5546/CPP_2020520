#include "m_arith.h"
#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif
//���ý�ͨ�¼�����
bool CfgEventRegion(mEventInfo	EventDetectCfg, ALGCFGS *pCfgs, ALGPARAMS *pParams)
{
	int i = 0, j = 0, k = 0;
	int uEventRegionNum = 0;
	CPoint ptCorner[4];
	/*pParams->MaskIllegalParkImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 4;
	pParams->MaskOppositeDirDriveImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 5;
	pParams->MaskOffLineImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 6;
	pParams->MaskNoPersonAllowImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 7;
	pParams->MaskNonMotorAllowImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 8;
	pParams->MaskPersonFallImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 9;
	pParams->MaskDropImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 10;
	pParams->MaskTrafficAccidentImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 11;*/
	pParams->CurrBackImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 6;
	pParams->BufferBackImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 7;
	pParams->ForeImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 8;
	pParams->MaskOppositeDirDriveImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 9;
	pParams->MaskEventIDImage = (Uint8 *)pParams->CurrQueueImage + DETECTRECT_WIDTH_MAX * DETECTRECT_HEIGHT_MAX * 10;
	//�����ò����ӵ��
	pCfgs->bDetCongestion = FALSE;
	pCfgs->uCongestionThreshTime = 10;//����ӵ����ֵ10s
	//�����¼��������
	for(i = 0; i < EventDetectCfg.eventAreaNum; i++)
	{
		Uint16 areaNum = EventDetectCfg.eventArea[i].areaNum;//������
		mSelectType  eventType = EventDetectCfg.eventArea[i].eventType;//�¼�����
		for(j = 0; j < 4; j++)
		{
			ptCorner[j].x = EventDetectCfg.eventArea[i].realcoordinate[j].x;
			ptCorner[j].y = EventDetectCfg.eventArea[i].realcoordinate[j].y;
		}
		CorrectRegionPoint(ptCorner);//У����������
		for(j = 0; j < MAX_EVENT_TYPE; j++)
		{
			if(eventType.type & (1 << j))
			{
				pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].uRegionID = areaNum;//������
				memcpy((void*)pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].detRegion, (void*)ptCorner, 4 * sizeof(CPoint));
				pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].direction = EventDetectCfg.eventArea[i].direction;//������
				pCfgs->EventDetectCfg.ReportInterval[j + 1] = EventDetectCfg.eventArea[i].report[j];//�����һλ
				pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].eventType = (enum eventType)(j + 1);
		        printf("[%d,%d,%d,%d],%d\n",pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].detRegion[0].x,pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].detRegion[0].y,pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].detRegion[1].x,pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].detRegion[1].y,pCfgs->EventDetectCfg.EventRegion[uEventRegionNum].eventType);
				uEventRegionNum++;
			}
		}
	}
	pCfgs->EventDetectCfg.uEventRegionNum = uEventRegionNum;//�¼����������
	//�ж��Ƿ���ӵ��
	for(i = 0; i < uEventRegionNum; i++)
	{
		if(pCfgs->EventDetectCfg.EventRegion[i].eventType == CONGESTION)
		{
			pCfgs->bDetCongestion = TRUE;
			break;
		}
	}
	memset(pCfgs->uStatCongestionNum, 0, MAX_LANE * sizeof(Uint16));//ͳ��ӵ������
	printf("ReportInterval time REVERSE_DRIVE:%d,STOP_INVALID:%d,PERSON:%d,DRIVE_AWAY:%d,CONGESTION:%d,DROP:%d,PERSONFALL:%d,NONMOTORFALL:%d,NONMOTOR:%d,ACCIDENTTRAFFIC:%d,GREENWAYDROP:%d]\n",pCfgs->EventDetectCfg.ReportInterval[REVERSE_DRIVE],pCfgs->EventDetectCfg.ReportInterval[STOP_INVALID],pCfgs->EventDetectCfg.ReportInterval[NO_PEDESTRIANTION],pCfgs->EventDetectCfg.ReportInterval[DRIVE_AWAY],pCfgs->EventDetectCfg.ReportInterval[CONGESTION],pCfgs->EventDetectCfg.ReportInterval[DROP],pCfgs->EventDetectCfg.ReportInterval[PERSONFALL],pCfgs->EventDetectCfg.ReportInterval[NONMOTORFALL],pCfgs->EventDetectCfg.ReportInterval[NONMOTOR],pCfgs->EventDetectCfg.ReportInterval[ACCIDENTTRAFFIC],pCfgs->EventDetectCfg.ReportInterval[GREENWAYDROP]);
	//��ͨ�¼������Ϣ��ʼ��
	memset(pCfgs->event_targets, 0, MAX_TARGET_NUM * sizeof(CTarget));//��ͨ�¼��������Ŀ��
	pCfgs->event_target_id = 1;
	pCfgs->event_targets_size = 0;
	pCfgs->eventID = 1;//��ͨ�¼�ID��ʼ��Ϊ1
	pCfgs->uIllegalParkNum = 0;
	memset(pCfgs->IllegalParkBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//��ֹͣ��
	pCfgs->uIllegalParkTime = 0;//ǰһͣ��ʱ��
	pCfgs->uIllegalParkID = 0;//ǰһͣ���¼�ID
	pCfgs->uOppositeDirDriveNum = 0;
	memset(pCfgs->OppositeDirDriveBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//��ֹ����
	pCfgs->uOppositeDirDriveTime = 0;//ǰһ����ʱ��
	pCfgs->uOppositeDirDriveID = 0;//ǰһ�����¼�ID
	memset(pCfgs->direction, 0, MAX_REGION_NUM * sizeof(Uint16));//�������з���
    //memset(pCfgs->bCongestion, FALSE, MAX_LANE * sizeof(bool));//��������ӵ��
	pCfgs->uCongestionNum = 0;
	memset(pCfgs->CongestionBox, 0, MAX_LANE * sizeof(EVENTBOX));
	pCfgs->uOffLaneNum = 0;
	memset(pCfgs->OffLaneBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//ƫ�복��
	pCfgs->uOffLaneTime = 0;//ǰһʻ��ʱ��
	pCfgs->uOffLaneID = 0;//ǰһʻ���¼�ID
	pCfgs->uNoPersonAllowNum = 0;
	memset(pCfgs->NoPersonAllowBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//Υ������
	pCfgs->uPersonEventTime = 0;//ǰһ�����¼�ʱ��
	pCfgs->uCurrentPersonID = 0;//ǰһ�����¼�ID
	pCfgs->uNonMotorAllowNum = 0;
	memset(pCfgs->NonMotorAllowBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//Υ���ǻ�����
	pCfgs->uNonMotorEventTime = 0;//ǰһ�ǻ������¼�ʱ��
	pCfgs->uCurrentNonMotorID = 0;//ǰһ�ǻ������¼�ID
	pCfgs->uDropNum = 0;
	memset(pCfgs->DropBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//������
	pCfgs->uPersonFallNum = 0;//���˵�����
	memset(pCfgs->PersonFallBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//���˵���
	pCfgs->uPersonFallEventTime = 0;//ǰһ���˵����¼�ʱ��
	pCfgs->uCurrentPersonFallID = 0;//ǰһ���˵����¼�ID
	pCfgs->uNonMotorFallNum = 0;//�ǻ�����������
	memset(pCfgs->NonMotorFallBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//�ǻ���������
	pCfgs->uNonMotorFallEventTime = 0;//ǰһ�ǻ����������¼�ʱ��
	pCfgs->uCurrentNonMotorFallID = 0;//ǰһ�ǻ����������¼�ID
	pCfgs->uGreenwayDropNum = 0;//�̵�������
	memset(pCfgs->GreenwayDropBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//�̵��������
	pCfgs->uGreenwayDropEventTime = 0;//ǰһ�̵��������¼�ʱ��
	pCfgs->uCurrentGreenwayDropID = 0;//ǰһ�̵��������¼�ID
	pCfgs->uTrafficAccidentNum = 0;
	memset(pCfgs->TrafficAccidentBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));//��ͨ�¹�
	pCfgs->uTrafficAccidentTime = 0;//ǰһ��ͨ�¹�ʱ��
	pCfgs->uTrafficAccidentID = 0;//ǰһ��ͨ�¹�ID
	pCfgs->bMaskEventImage = FALSE;//��ͨ�¼���ģͼ��
	pCfgs->CurrCandidateROINum = 0;//������������
	memset(pCfgs->CurrCandidateROI, 0, 50 * sizeof(CRect));//���ڴ洢�������ѡ����
	memset(pCfgs->abandoned_targets, 0, 10 * sizeof(CTarget));//���ڴ洢������Ŀ��
	pCfgs->abandoned_targets_id = 1;
	pCfgs->abandoned_targets_size = 0;
	pCfgs->gThisFrameTime = 0;
	memset(pCfgs->EventInfo, 0 , MAX_EVENT_NUM * sizeof(EVENTINFO));//��ʼ���¼���Ϣ
	pCfgs->EventNum = 0;//�����¼�����Ϊ0  
	pCfgs->video_fps = 0;
	pCfgs->EventState = 0;
	pCfgs->HaveEvent = FALSE;
	pCfgs->first_update = 0;
	pCfgs->EventBeginTime = 0;
	pCfgs->EventEndTime = 0;
	//��·�¼������Ϣ��ʼ��
	memset(pCfgs->road_event_targets, 0, MAX_ROAD_TARGET_NUM * sizeof(CTarget));//��·�¼��������Ŀ��
	pCfgs->road_event_target_id = 1;
	pCfgs->road_event_targets_size = 0;
	return TRUE;

}
//pParams->MaskEventImage�Ĳ�ֵͬ�����ⲻͬ�Ľ�ͨ�¼�����
bool MaskEventImage(ALGCFGS *pCfgs, ALGPARAMS *pParams, int imgW, int imgH)
{
	Int32	i, j, k;
	CPoint	ptDetectCorner[4];
	CPoint pt;
	//����¼��������
	memset(pParams->MaskEventImage, 0, pCfgs->m_iWidth * pCfgs->m_iHeight * sizeof(Uint32));
	memset(pParams->MaskOppositeDirDriveImage, 0, pCfgs->m_iWidth * pCfgs->m_iHeight);
	for(i = 0; i < pCfgs->m_iHeight; i++)//�¼�����ID��ʼ��Ϊ255
	{
		for(j = 0; j < pCfgs->m_iWidth; j++)
		{
			*(pParams->MaskEventIDImage + i * pCfgs->m_iWidth + j) = 255;
		}
	}
	//�����¼��������
	for(i = 0; i < pCfgs->EventDetectCfg.uEventRegionNum; i++)
	{
		(void*)memcpy(ptDetectCorner, (void*)pCfgs->EventDetectCfg.EventRegion[i].detRegion, 4 * sizeof(CPoint));
		//resize ��640 * 480
		for(j = 0; j < 4; j++)
		{
			ptDetectCorner[j].x = ptDetectCorner[j].x * pCfgs->m_iWidth / imgW;
			ptDetectCorner[j].y = ptDetectCorner[j].y * pCfgs->m_iHeight / imgH;

		}
		EventType eventType = pCfgs->EventDetectCfg.EventRegion[i].eventType;
		if(eventType == REVERSE_DRIVE)//���������з���,��������
		{
			Uint8* p;
			for(j = 0; j < pCfgs->m_iHeight; j++)
			{
				p = pParams->MaskOppositeDirDriveImage + j * pCfgs->m_iWidth;
				for(k = 0; k < pCfgs->m_iWidth; k++)
				{
					pt.x = k;
					pt.y = j;
					if(isPointInRect(pt, ptDetectCorner[3], ptDetectCorner[0], ptDetectCorner[1], ptDetectCorner[2]))
					{
						if(pCfgs->EventDetectCfg.EventRegion[i].direction)//0 Ϊ���£�1Ϊ����
						{
							p[k] = 255;
						}
						else
						{
							p[k] = 128;
						}
					}
				}
			}
		}
		else //�����¼�
		{
			Uint32* p;
			int num = 0;
			printf("type = %d,[%d,%d,%d,%d]\n",eventType,ptDetectCorner[0].x,ptDetectCorner[0].y,ptDetectCorner[1].x,ptDetectCorner[1].y);
			for(j = 0; j < pCfgs->m_iHeight; j++)
			{
				p = pParams->MaskEventImage + j * pCfgs->m_iWidth;
				for(k = 0; k < pCfgs->m_iWidth; k++)
				{
					pt.x = k;
					pt.y = j;
					if(isPointInRect(pt, ptDetectCorner[3], ptDetectCorner[0], ptDetectCorner[1], ptDetectCorner[2]))
					{
						p[k] += (1 << eventType);//����Ӧ�¼�λ��Ϊ1
						num++;
					}
				}
			}
		}
		//�����¼�����ID
		Uint16 uRegionID = pCfgs->EventDetectCfg.EventRegion[i].uRegionID;
		for(j = 0; j < pCfgs->m_iHeight; j++)
		{
			Uint8* p = pParams->MaskEventIDImage + j * pCfgs->m_iWidth;
			for(k = 0; k < pCfgs->m_iWidth; k++)
			{
				pt.x = k;
				pt.y = j;
				if(isPointInRect(pt, ptDetectCorner[3], ptDetectCorner[0], ptDetectCorner[1], ptDetectCorner[2]))
				{
					p[k] = uRegionID;
				}
			}
		}

	}
	//������ģͼ��
	/*IplImage* mask = cvCreateImage(cvSize(pCfgs->m_iWidth, pCfgs->m_iHeight), IPL_DEPTH_8U, 1);
	memcpy(mask->imageData, pParams->MaskOppositeDirDriveImage, pCfgs->m_iWidth * pCfgs->m_iHeight);
	cvSaveImage("maskEvent1.jpg", mask, 0);
	cvReleaseImage(&mask);*/

	return	TRUE;

}
//�жϼ����Ƿ��ڼ�������ڣ��������м��
int RectInRegion0(unsigned char* maskImage, int width, int height, CRect rct, EventType type)
{
	int isInRegion = 0;
	int i = 0, j = 0;
	int num = 0;
	int val = 0;
	unsigned char* p;
	float ratio = 0;
	for(i = rct.y; i < (rct.y + rct.height); i++)
	{
		p = maskImage + i * width;
		for(j = rct.x; j < (rct.x + rct.width); j++)
		{
			int val0 = *(p + j);
			if(val0)
			{
				num++;
				val = *(p + j);
			}
		}
	}
	ratio = (float)num / (float)(rct.width * rct.height);
	if(ratio > 0.2)//������ֵ����������
	{
		if(type == REVERSE_DRIVE)
		{
			if(val == 128)//���뷽����Ϣ��128����directionΪ0��255����directionΪ1
			{
				isInRegion = 1;
			}
			else
			{
				isInRegion = 2;
			}
		}

	}
	return isInRegion;
}
//�жϼ����Ƿ��ڼ��������
int RectInRegion1(Uint32* maskImage, int width, int height, CRect rct, EventType type)
{
	int isInRegion = 0;
	int i = 0, j = 0;
	int num = 0;
	int val = 0;
	Uint32* p;
	float ratio = 0;
	for(i = rct.y; i < (rct.y + rct.height); i++)
	{
		p = maskImage + i * width;
		for(j = rct.x; j < (rct.x + rct.width); j++)
		{
			val = *(p + j);
			if(val & (1 << type))
			{
				num++;
			}
		}
	}
	ratio = (float)num / (float)(rct.width * rct.height);
	if(ratio > 0.2)//������ֵ����������
	{
		isInRegion = 1;
	}
	return isInRegion;
}
void calc_fore(unsigned char *current, unsigned char *back, unsigned char* mask, unsigned char *fore, int width, int height, ALGPARAMS *pParams)//������������ͼ�����ǰ�����
{
	int i, j;
	unsigned char* p1 = current;
	unsigned char* p2 = back;
	unsigned char* p3 = fore;
	unsigned char* mask1 = mask;
	Uint32* mask2 = pParams->MaskEventImage;//������������
	memset(fore, 0, width * height);
	for (i = 0; i < height; i++)
	{
		p1 = current + i * width;
		p2 = back + i * width;
		p3 = fore + i * width;
		mask1 = mask + i * width;
		mask2 = pParams->MaskEventImage + i * width;
		for (j = 0;j < width; j++)
		{
			if (p1[j] - p2[j] > 90 && mask1[j] == 0 && (mask2[j] & (1 << DROP)))
			{
				p3[j] = 255;//foreground
			}
		}
	}
}

void update_currentback(unsigned char *current, unsigned char *curr_back, int width, int height)//����curr����
{
	int i, j;
	unsigned char* p1 = current;
	unsigned char* p2 = curr_back;
	int val1, val2;
	for (i = 0; i < height; i++)
	{
		p1 = current + i * width;
		p2 = curr_back + i * width;
		for (j = 0; j < width; j++)
		{
			val1 = p1[j];
			val2 = p2[j];
			if (val1 >= val2)
			{
				if (val2 == 255)
				{
					val2 = 254;
				}
				p2[j] = val2 + 1;
			}else
			{
				if (val2 == 0)
				{
					val2 = 1;
				}
				p2[j] = val2 - 1;
			}
		}
	}
}

void update_bufferedback(unsigned char *curr_back, unsigned char *buf_back, unsigned char *abandon, int width, int height, ALGCFGS* pCfgs)//����buffer����
{
	int i, j;
	int leave_flag = 0;
	unsigned char* p1 = curr_back;
	unsigned char* p2 = buf_back;
	unsigned char* p3 = abandon;
	if (pCfgs->first_update == 0)
	{
		for ( i = 0; i < height; i++)
		{
			p1 = curr_back + i * width;
			p2 = buf_back + i * width;
			p3 = abandon + i * width;
			for (j = 0; j < width; j++)
			{
				if (abs(p1[j] - p2[j]) <= 50 )
				{
					p3[j] = 0;//background
				}else
				{
					p3[j] = p1[j];//foreground
					pCfgs->first_update = 1;
				}
			}
		}
		return;
	}

	//�����뿪�ж�
	for ( i = 0; i < height; i++)
	{
		p1 = curr_back + i * width;
		p3 = abandon + i * width;
		for (j = 0; j < width; j++)
		{
			if ( p3[j] != 0)
			{
				if (p3[j] != p1[j])
				{
					leave_flag = 1;	//������Ĥ��֮ǰ�����뵱ǰ�Ĳ�һ�£�1������δ�뿪��0�������뿪
				}
			}
		}
	}

	if(leave_flag == 0)	//�����뿪
	{
		memcpy(curr_back, buf_back, width * height);
		for ( i = 0; i < height; i++)
		{
			p1 = curr_back + i * width;
			p3 = abandon + i * width;
			for (j = 0; j < width; j++)
			{
				if (p3[j] != 0)
				{
					p3[j] = p1[j];
				}
			}
		}
	}else//����δ�뿪
	{
		for ( i = 0; i < height; i++)
		{
			p2 = buf_back + i * width;
			p3 = abandon + i * width;
			for (j = 0; j < width; j++)
			{
				if (p3[j] != 0)
				{
					p2[j] = p3[j];
				}
			}
		}
	}	

}
void DropDetect(ALGCFGS *pCfgs, ALGPARAMS *pParams, int event_idx, int width, int height)//��������
{
	int i, j;
	CRect rct[50];
	int rct_num = 0;
	int match_rct[50] = {0};
	IplImage* foreImg = cvCreateImage(cvSize(pCfgs->m_iWidth, pCfgs->m_iHeight), IPL_DEPTH_8U, 1);//ǰ��ͼ��
	IplImage* maskImg = cvCreateImage(cvSize(pCfgs->m_iWidth, pCfgs->m_iHeight), IPL_DEPTH_8U, 1);//�������ģͼ��
	unsigned char* fore = (unsigned char *)foreImg->imageData;//ǰ��ͼ��
	unsigned char* mask = (unsigned char *)maskImg->imageData;//�������ģͼ��
	memset(mask, 0, pCfgs->m_iWidth * pCfgs->m_iHeight);
	if (pCfgs->gThisFrameTime == 0)//��������
	{
		//��ʼ������ģ��
		memcpy((void *)pParams->CurrBackImage, (void *)pParams->CurrQueueImage, pCfgs->m_iWidth  * pCfgs->m_iHeight);
		memcpy((void *)pParams->BufferBackImage, (void *)pParams->CurrQueueImage, pCfgs->m_iWidth  * pCfgs->m_iHeight);
	}
	if(pCfgs->gThisFrameTime > 0)
	{
		//�õ�maskͼ��
		for(i = 0; i < pCfgs->classes; i++)
		{
			if(pCfgs->detClasses[i].classes_num == 0)
				continue;
			for(j = 0; j < pCfgs->detClasses[i].classes_num; j++)
			{
				CRect rct = pCfgs->detClasses[i].box[j];
				//�Կ��������
				rct.x = rct.x * pCfgs->m_iWidth / width;
				rct.width = rct.width * pCfgs->m_iWidth /width;
				rct.y = rct.y * pCfgs->m_iHeight / height;
				rct.height = rct.height * pCfgs->m_iHeight / height;
				rct.width = ((rct.x + rct.width) > (pCfgs->m_iWidth - 1))? (pCfgs->m_iWidth - 1 - rct.x) : rct.width;//��ֹԽ��
				rct.height = ((rct.y + rct.height) > (pCfgs->m_iHeight - 1))? (pCfgs->m_iHeight - 1 - rct.y) : rct.height;
				//����λ��������1
				for(int k = rct.y; k <= (rct.y + rct.height); k++)
				{
					memset(mask + k * pCfgs->m_iWidth + rct.x, 1, rct.width);
				}
			}
		}
		//����ǰ����Ĥ
		calc_fore(pParams->CurrBackImage, pParams->BufferBackImage, mask, fore, pCfgs->m_iWidth, pCfgs->m_iHeight, pParams);
		//���¸��ٱ���
		update_currentback(pParams->CurrQueueImage, pParams->CurrBackImage, pCfgs->m_iWidth, pCfgs->m_iHeight);
		if (pCfgs->gThisFrameTime % 20 == 0)
		{
			update_bufferedback(pParams->CurrBackImage, pParams->BufferBackImage, pParams->ForeImage, pCfgs->m_iWidth, pCfgs->m_iHeight, pCfgs);
		}
		//��ǰ��ͼ����и�ʴ�����Լ��ٸ���
		cvDilate(foreImg, foreImg, 0, 2);//����
		cvErode(foreImg, foreImg, 0, 2);//��ʴ
		//������Ӿ���
		CvSeq* pContour = NULL;
		CvMemStorage *pStorage = NULL;
		pStorage = cvCreateMemStorage(0);
		cvFindContours(foreImg, pStorage, &pContour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
		for (; pContour != NULL; pContour = pContour->h_next)   
        {   
			CvRect r = ((CvContour*)pContour)->rect;  
			int size =r.width * r.height;
			if(size > 50 && size < 10000)//�����������������Ϊ������������
			{
				rct[rct_num].x = r.x * width / pCfgs->m_iWidth;
				rct[rct_num].y = r.y * height / pCfgs->m_iHeight;
				rct[rct_num].width = r.width * width / pCfgs->m_iWidth;
				rct[rct_num].height = r.height * height / pCfgs->m_iHeight;
				rct_num++;
			}
        }
		if(pStorage)
		{
			cvReleaseMemStorage(&pStorage);   
			pStorage = NULL;  
		}
		for(i = 0; i < pCfgs->abandoned_targets_size; i++)
		{
			pCfgs->abandoned_targets[i].detected = FALSE;
		}
		if(pCfgs->CurrCandidateROINum == 0)//��һ֡û�к�ѡ����
		{
			memcpy(pCfgs->CurrCandidateROI, rct, rct_num * sizeof(CRect));
			pCfgs->CurrCandidateROINum = rct_num;
		}
		else//��һ֡�к�ѡ����,����ƥ��
		{
			for(j = 0; j < rct_num; j++)
			{
				match_rct[j] = 0;
				for(i = 0; i < pCfgs->CurrCandidateROINum; i++)
				{
					if(overlapRatio(pCfgs->CurrCandidateROI[i], rct[j]) > 50)//������ƥ�����50
					{
						match_rct[j] = 1;
						break;
					}
				}
				//���ٿ�ͼ������ƥ��
				if(match_rct[j] == 1)
				{
					int match_sucess = -1;
					if(pCfgs->abandoned_targets_size)
					{
						for(i =  0; i < pCfgs->abandoned_targets_size; i++)
						{
							if(overlapRatio(pCfgs->abandoned_targets[i].box, rct[j]) > 20)//������ƥ����
							{
								pCfgs->abandoned_targets[i].detected = TRUE;
								pCfgs->abandoned_targets[i].box = rct[j];
								match_sucess = 1;
							}
						}
					}
					if(match_sucess < 0 && pCfgs->abandoned_targets_size < 10)//û��ƥ���ϣ�����abandoned_targets��
					{
						CTarget nt; 
						Initialize_target(&nt);
						nt.box = rct[j];
						nt.detected = TRUE;
						memset(nt.event_continue_num, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ���¼�����֡��
						memset(nt.event_flag, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ���¼����
						memset(nt.cal_event, FALSE, MAX_EVENT_TYPE * sizeof(bool));//��ʼ�������¼�Ϊδ����
						memset(nt.sign_event, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ��Ϊδ��ǵ��¼�
						nt.target_id = (pCfgs->abandoned_targets_id > 5000)? 1 : pCfgs->abandoned_targets_id++;
						pCfgs->abandoned_targets[pCfgs->abandoned_targets_size] = nt;
						pCfgs->abandoned_targets_size++;
					}
				}
			}
		}
	}
	if(foreImg)
	{
		cvReleaseImage(&foreImg);
		foreImg = NULL;
	}
	if(maskImg)
	{
		cvReleaseImage(&maskImg);
		maskImg = NULL;
	}
	//�������������Ŀ��
	for(i = 0; i < pCfgs->abandoned_targets_size; i++)
	{
		//��⵽���������ٶ�
		if(pCfgs->abandoned_targets[i].detected)
		{
			pCfgs->abandoned_targets[i].lost_detected = 0;
			pCfgs->abandoned_targets[i].event_continue_num[event_idx]++;
		}
		else//δ��⵽
		{
			pCfgs->abandoned_targets[i].lost_detected++;
			pCfgs->abandoned_targets[i].box.x += pCfgs->abandoned_targets[i].vx;
			pCfgs->abandoned_targets[i].box.y += pCfgs->abandoned_targets[i].vy;
			pCfgs->abandoned_targets[i].event_continue_num[event_idx] = 0;
		}
		if(pCfgs->abandoned_targets[i].cal_event[event_idx] == FALSE && pCfgs->abandoned_targets[i].event_continue_num[event_idx] > 10)//����������뵽�¼���
		{
			pCfgs->abandoned_targets[i].cal_event[event_idx] = TRUE;
			pCfgs->abandoned_targets[i].event_flag[event_idx] = 1;
			/*pCfgs->EventInfo[pCfgs->EventNum].uEventID = pCfgs->abandoned_targets[i].target_id;
			pCfgs->EventInfo[pCfgs->EventNum].begin_time = pCfgs->gThisFrameTime;
			pCfgs->EventInfo[pCfgs->EventNum].type = DROP;
			pCfgs->EventInfo[pCfgs->EventNum].flag = 0;
			pCfgs->EventNum++;
			if(pCfgs->EventState == 0)//�¼���ʼʱ��
			{
				pCfgs->EventState = 1;
				pCfgs->EventBeginTime = pCfgs->gThisFrameTime;
			}*/
		}
		//printf("%d,[%d,%d,%d,%d]\n",pCfgs->abandoned_targets[i].detected, pCfgs->abandoned_targets[i].box.x,pCfgs->abandoned_targets[i].box.y,pCfgs->abandoned_targets[i].box.width,pCfgs->abandoned_targets[i].box.height);
		//��Ŀ������Ƶ����ʱ��̫����ʱ��û�м�⵽���뿪ͼ��ɾ��Ŀ��
		if(pCfgs->abandoned_targets[i].continue_num > 3000 || pCfgs->abandoned_targets[i].lost_detected > 20 ||((pCfgs->abandoned_targets[i].box.x < 10 || pCfgs->abandoned_targets[i].box.y < 10 || (pCfgs->abandoned_targets[i].box.x + pCfgs->abandoned_targets[i].box.width) > (width - 10) || (pCfgs->abandoned_targets[i].box.y + pCfgs->abandoned_targets[i].box.height) > (height - 10))&& pCfgs->abandoned_targets[i].lost_detected > 5))
		{
			/*for(j = 0; j < pCfgs->EventNum; j++)//���û�����ô��¼�����������
			{
				if(pCfgs->EventInfo[j].flag == 0)
				{
					if(pCfgs->EventInfo[j].uEventID == pCfgs->abandoned_targets[i].target_id)
					{
						//pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;
						pCfgs->EventInfo[j].flag = 1;
						break;
					}
				}
			}*/
			DeleteTarget(&pCfgs->abandoned_targets_size, &i, pCfgs->abandoned_targets);
			continue;
		}
		pCfgs->abandoned_targets[i].continue_num++;
	}
}
void IllegalParkDetect(ALGCFGS *pCfgs, int target_idx, int event_idx, int targetDisXY[][3], int width, int height)//��ֹͣ�����
{
	int j = 0;
	int disX = 0, disY = 0;
	int thr = 20;//��ֵ
	int num = 0;
	/*int continue_num = pCfgs->event_targets[target_idx].trajectory_num - 100;
	continue_num = (continue_num < 0)? 0 : continue_num;
	int dx = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].x - pCfgs->event_targets[target_idx].trajectory[continue_num].x;
	int dy = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].y - pCfgs->event_targets[target_idx].trajectory[continue_num].y;
	dx = (dx < 0)? -dx : dx;
	dy = (dy < 0)? -dy : dy;*/
	//�˶�����
	int dx = targetDisXY[target_idx][0];
	int dy = targetDisXY[target_idx][1];
	int id = targetDisXY[target_idx][2];//��¼�˶�ʱ������λ��

	//δ���Ϊͣ���¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		thr = pCfgs->event_targets[target_idx].box.height / 2;
		thr = (thr > 15)? 15 : thr;
		thr = (thr < 5)? 5 : thr;
		//if(dx < width / 30 && dy < height / 30 )//��ֹ
		if(dx < width / 30 && dy < thr && ((pCfgs->currTime - pCfgs->uStatFrameTime[id]) > 10 || id == 0))//10s֮�ϲ���ͣ��
		{
			//printf("id = %d,dx = [%d,%d,%d],[%d,%d],thr = %d\n", pCfgs->event_targets[target_idx].target_id, dx, dy,id, pCfgs->event_targets[target_idx].box.width, pCfgs->event_targets[target_idx].box.height, thr);
			//printf("dx = %d,dy =%d,time = %d,%f\n",dx, dy, id,pCfgs->currTime - pCfgs->uStatFrameTime[0][id]);
			/*if(pCfgs->event_targets[target_idx].detected)
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
			}*/
			if(pCfgs->event_targets[target_idx].lost_detected > 20)//��ʱ��û�м�⵽�����¼�������ֹ���
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
			}
			if(pCfgs->event_targets[target_idx].detected)
			{
				//printf("may be IllegalPark\n");
				num = 0;
				//�ж�Ŀ����Χ���޻������е�Ŀ��
				for(j = 0; j < pCfgs->event_targets_size; j++)
				{
					if(j == target_idx)
						continue;
					disX = MIN(pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width, pCfgs->event_targets[j].box.x + pCfgs->event_targets[j].box.width) - MAX(pCfgs->event_targets[target_idx].box.x, pCfgs->event_targets[j].box.x);
					disY = MIN(pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height, pCfgs->event_targets[j].box.y + pCfgs->event_targets[j].box.height) - MAX(pCfgs->event_targets[target_idx].box.y, pCfgs->event_targets[j].box.y);
					if(disX > -width / 6 && disY > -height / 6)//10
					{
						thr = pCfgs->event_targets[j].box.height / 4 * 3;
						thr = (thr > 100)? 100 : thr;
						thr = (thr < 10)? 10 : thr;
						//printf("[%d,%d,%d,%d], [%d,%d],thr = %d\n", pCfgs->event_targets[j].target_id, targetDisXY[j][0], targetDisXY[j][1], targetDisXY[j][2], pCfgs->event_targets[j].trajectory[0].y, pCfgs->event_targets[j].box.y + pCfgs->event_targets[j].box.height / 2,thr);
						if(targetDisXY[j][0] < width / 15 && targetDisXY[j][1] < thr)//�л������е�Ŀ��
						//if(targetDisXY[j][0] < width / 20 && targetDisXY[j][1] < height / 20)
						{
							num++;
						}
					}
					if(num > 0)
						break;
				}
				//���������ﵽ10֡���ϣ��ű�ͣ���¼�
				if(num < 1)
				{
					pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
				}
				else
				{
					pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
				}
				//pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
			}
			if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 10)//��ʼ����ͣ���¼�
			{
				printf("illegal park ok\n");
				if(pCfgs->event_targets[target_idx].box.y > 20)//��ֹ��죬ͼ�񶥲�����Ϊ��ͣ��
				{
					/*if(pCfgs->event_targets[target_idx].target_id == 0)
					pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
					pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
					pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
					//printf("illegal park,%d\n",pCfgs->event_targets[target_idx].target_id);
					/*pCfgs->EventInfo[pCfgs->EventNum].uEventID = pCfgs->event_targets[target_idx].target_id;
					pCfgs->EventInfo[pCfgs->EventNum].begin_time = pCfgs->gThisFrameTime;
					pCfgs->EventInfo[pCfgs->EventNum].type = STOP_INVALID;
					pCfgs->EventInfo[pCfgs->EventNum].flag = 0;
					pCfgs->EventNum++; 
					if(pCfgs->EventState == 0)//�¼���ʼʱ��
					{
					pCfgs->EventState = 1;
					pCfgs->EventBeginTime = pCfgs->gThisFrameTime - 120;
					}*/
				}

			}
		}
		else//�˶�
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] > 0 && dx < MIN(100, width / 10) && dy < MIN(pCfgs->event_targets[target_idx].box.height * 2, height / 10))//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�,��ֹ��죬��ͣ������ͣ��
	{
		//����ͣ���¼���
		if(pCfgs->uIllegalParkNum < MAX_EVENT_NUM && pCfgs->event_targets[target_idx].lost_detected < 10)
		{
			//�ж��Ƿ�Ϊ�³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].uNewEventFlag = 1;
			}
			else
				pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].uNewEventFlag = 0;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->IllegalParkBox[pCfgs->uIllegalParkNum].uEventType = STOP_INVALID;
			pCfgs->uIllegalParkNum++;
		}
	}
	else//���˶��󣬽���ͣ��
	{
		pCfgs->event_targets[target_idx].event_flag[event_idx] = 0;
		/*for(j = 0; j < pCfgs->EventNum; j++)//���û�����ô��¼�����������
		{
			if(pCfgs->EventInfo[j].flag == 0)
			{
				if(pCfgs->EventInfo[j].uEventID == pCfgs->event_targets[target_idx].target_id)
				{
					//pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;
					pCfgs->EventInfo[j].flag = 1;
					break;
				}
			}
		}*/
	}
}
void OppositeDirDriveDetect(ALGCFGS *pCfgs, int target_idx, int event_idx, int InRegionVal, int width, int height)//���м��
{
#if 1
	int i = 0, j = 0;
	int dis_x = 0;//������
	int dis_y = 0;
	int continue_num = 0;
	int startIdx = 0;
	int startIdy = 0;
	int dis_sum_x = 0;
	int dis_sum_y = 0;
	int inverse_type = 0;
	bool inverse_ok = FALSE;
	bool inverse_off = FALSE;
	if(pCfgs->event_targets[target_idx].trajectory_num < 5)//��������С��5֡�����м���
		return;
	if(pCfgs->event_targets[target_idx].trajectory_num % 5 == 2)//ÿ���5֡���о������
	{
		continue_num = pCfgs->event_targets[target_idx].trajectory_num - 1;
		i = continue_num / 5;
		dis_x = (pCfgs->event_targets[target_idx].trajectory[continue_num].x + pCfgs->event_targets[target_idx].trajectory[continue_num - 1].x - pCfgs->event_targets[target_idx].trajectory[continue_num - 5].x - pCfgs->event_targets[target_idx].trajectory[continue_num - 6].x) / 2;
	    dis_y = (pCfgs->event_targets[target_idx].trajectory[continue_num].y + pCfgs->event_targets[target_idx].trajectory[continue_num - 1].y - pCfgs->event_targets[target_idx].trajectory[continue_num - 5].y - pCfgs->event_targets[target_idx].trajectory[continue_num - 6].y) / 2;
		startIdx = pCfgs->event_targets[target_idx].statistic[0];
		startIdy = pCfgs->event_targets[target_idx].statistic[1];
		dis_sum_x = pCfgs->event_targets[target_idx].statistic[2];
		dis_sum_y = pCfgs->event_targets[target_idx].statistic[3];
		inverse_type = pCfgs->event_targets[target_idx].statistic[4];

		/*//����x�����˶�����
		if(startIdx < 0)//�жϳ����Ƿ����趨�ķ����෴������෴�Ϳ�ʼ���������ж�
		{
			if(dis_x < 0 && InRegionVal == 1)//����������
			{
				startIdx = i;
				dis_sum_x = dis_x;
			}
			if(dis_x > 0 && InRegionVal == 2)//����������
			{
				startIdx = i;
				dis_sum_x = dis_x;
			}
		}
		else if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)//��ʼ�����ж�
		{
			dis_sum_x += dis_x;
			if((dis_sum_x < MIN(-width / 10, -pCfgs->event_targets[target_idx].box.width) && InRegionVal == 1) || (dis_sum_x >= MAX(width / 10, pCfgs->event_targets[target_idx].box.width) && InRegionVal == 2))//�ﵽ��������
			{
				if(pCfgs->event_targets[target_idx].detected)
				{
					pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
					printf("x ok\n");
				}
			}
			else
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;//��������������������Ϊ0
				if((dis_sum_x >= 0 && InRegionVal == 1) || (dis_sum_x <= 0 && InRegionVal == 2))//�������з�����ȷ���������½��м���
				{
					startIdx = -1;
				}
			}
			if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] >= 2)//����7*2֡����������������Ϊ������
			{
				inverse_type = 1;
				inverse_ok = TRUE;
				dis_sum_x = 0;
				printf("x inverse ok\n");
			}

		}
		else if(pCfgs->event_targets[target_idx].event_flag[event_idx] > 0 && inverse_type == 1)//�Ѿ��ж����У��ж��Ƿ��������
		{
			if(startIdx < 1000)
			{
				if((dis_x > 0 && InRegionVal == 1) || (dis_x < 0 && InRegionVal == 2))//��ʻ������ȷ�����½���ͳ�ƣ��Ƿ��������
				{
					dis_sum_x = 0;
					startIdx = 1000;
				}
			}
			else
			{
				dis_sum_x += dis_x;
			}
			if((dis_sum_x >= width / 50 && InRegionVal == 1) || (dis_sum_x <= -width / 50 && InRegionVal == 2))
			{
				inverse_off = TRUE;
			}
		}*/

		//����y�����˶����м���
		if(startIdy < 0)//�жϳ����Ƿ����趨�ķ����෴������෴�Ϳ�ʼ���������ж�
		{
			if(dis_y < 0 && InRegionVal == 1)//����������
			{
				startIdy = i;
				dis_sum_y = dis_y;
			}
			if(dis_y > 0 && InRegionVal == 2)//����������
			{
				startIdy = i;
				dis_sum_y = dis_y;
			}
			//printf("idy =%d, %d, %d\n",startIdy,dis_sum_y,InRegionVal);
		}
		else if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)//��ʼ�����ж�
		{
			dis_sum_y += dis_y;
			if((dis_sum_y < MIN(-height / 6, -pCfgs->event_targets[target_idx].box.height) && InRegionVal == 1) || (dis_sum_y >= MAX(height / 6, pCfgs->event_targets[target_idx].box.height) && InRegionVal == 2))//�ﵽ��������
			{
				printf("inverse id = %d, dis_y = %d, dis_sum_y = %d\n", pCfgs->event_targets[target_idx].target_id, dis_y,dis_sum_y);
				//������ͼ���Ե���������¼�
				int top = pCfgs->event_targets[target_idx].box.y;
				int bottom = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
				if((bottom < (height - 10) && InRegionVal == 1) || (top > 10 && InRegionVal == 2))
				{
					if(pCfgs->event_targets[target_idx].detected)
					{
						pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
						//printf("y ok\n");
					}
				}
			}
			else 
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;//��������������������Ϊ0
				if((dis_sum_y >= 0 && InRegionVal == 1) || (dis_sum_y <= 0 && InRegionVal == 2))//�������з�����ȷ���������½��м���
				{
					startIdy = -1;
				}
			}
			if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] >= 2)//����7*2֡����������������Ϊ������
			{
				inverse_type = 2;
				inverse_ok = TRUE;
				dis_sum_y = 0;
				//printf("y inverse ok\n");
			}

		}
		else if(pCfgs->event_targets[target_idx].event_flag[event_idx] > 0 && inverse_type == 2)//�Ѿ��ж����У��ж��Ƿ��������
		{
			if(startIdy < 1000)
			{
				if((dis_y > 0 && InRegionVal == 1) || (dis_y < 0 && InRegionVal == 2))//��ʻ������ȷ�����½���ͳ�ƣ��Ƿ��������
				{
					dis_sum_y = 0;
					startIdy = 1000;
				}
			}
			else
			{
				dis_sum_y += dis_y;
			}
			if((dis_sum_y >= width / 50 && InRegionVal == 1) || (dis_sum_y <= -width / 50 && InRegionVal == 2))
			{
				inverse_off = TRUE;
			}
		}


		//���������ݱ�������
		pCfgs->event_targets[target_idx].statistic[0] = startIdx;
		pCfgs->event_targets[target_idx].statistic[1] = startIdy;
		pCfgs->event_targets[target_idx].statistic[2] = dis_sum_x;
		pCfgs->event_targets[target_idx].statistic[3] = dis_sum_y;
		pCfgs->event_targets[target_idx].statistic[4] = inverse_type;
		/*for(j = 0; j < pCfgs->event_targets[target_idx].trajectory_num;j++)
		{
			printf("%d ",pCfgs->event_targets[target_idx].trajectory[j].y);
		}
		printf("\n");
		printf("target_idx= %d,[%d,%d,%d,%d]\n",pCfgs->event_targets[target_idx].target_id,pCfgs->event_targets[target_idx].statistic[0], pCfgs->event_targets[target_idx].statistic[1],pCfgs->event_targets[target_idx].statistic[2],pCfgs->event_targets[target_idx].statistic[3]);*/
		
	}
	//��Ŀ��û�б����Ϊ�����¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		if(inverse_ok == TRUE)//����
		{
			printf("id = %d, OppositeDirDrive\n", pCfgs->event_targets[target_idx].target_id);
			/*if(pCfgs->event_targets[target_idx].target_id == 0)
			pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
			pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
			pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
			/*pCfgs->EventInfo[pCfgs->EventNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->EventInfo[pCfgs->EventNum].begin_time = pCfgs->gThisFrameTime;
			pCfgs->EventInfo[pCfgs->EventNum].type = REVERSE_DRIVE;
			pCfgs->EventInfo[pCfgs->EventNum].flag = 0;
			pCfgs->EventNum++;
			if(pCfgs->EventState == 0)//�¼���ʼʱ��
			{
				pCfgs->EventState = 1;
				pCfgs->EventBeginTime = pCfgs->gThisFrameTime - 10;

			}*/
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] > 0 && inverse_off == FALSE)//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//printf("uOppositeDirDriveNum ,%d\n",pCfgs->event_targets[target_idx].target_id);
		//����ͣ���¼���
		if(pCfgs->uOppositeDirDriveNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ�Ϊ�³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uNewEventFlag = 1;
			}
			else
				pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uNewEventFlag = 0;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uEventType = REVERSE_DRIVE;
			pCfgs->uOppositeDirDriveNum++;
		}
	}
	else//������������
	{
		pCfgs->event_targets[target_idx].event_flag[event_idx] = 0;
		/*for(j = 0; j < pCfgs->EventNum; j++)//���û�����ô��¼�����������
		{
			if(pCfgs->EventInfo[j].flag == 0)
			{
				if(pCfgs->EventInfo[j].uEventID == pCfgs->event_targets[target_idx].target_id)
				{
					//pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;
					pCfgs->EventInfo[j].flag = 1;
					break;
				}
			}
		}*/
	}
#else
	int j = 0;
	//�õ��˶�����
	int continue_num = pCfgs->event_targets[target_idx].trajectory_num - 200;//��ֹ�������к��������֡��Ϊ200
	continue_num = (continue_num < 0)? 0 : continue_num;
	int dx = 0;
	int dy = 0;
	if(pCfgs->event_targets[target_idx].trajectory_num == 1)
	{
		dx = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].x - pCfgs->event_targets[target_idx].trajectory[continue_num].x;
		dy = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].y - pCfgs->event_targets[target_idx].trajectory[continue_num].y;
	}
	else
	{
		dx = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].x + pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 2].x - pCfgs->event_targets[target_idx].trajectory[continue_num + 1].x - pCfgs->event_targets[target_idx].trajectory[continue_num].x ;
		dy = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].y + pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 2].y - pCfgs->event_targets[target_idx].trajectory[continue_num + 1].y - pCfgs->event_targets[target_idx].trajectory[continue_num].y;
		dx = dx / 2;
		dy = dy / 2;
	}
	//��Ŀ��û�б����Ϊ�����¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		if((dy > height / 8 && InRegionVal == 2) || (dy < -height / 8 && InRegionVal == 1))//�������У���������Ϊ���У��������У�������������
		{
			//printf("may be OppositeDirDrive\n");
			if(pCfgs->event_targets[target_idx].detected)
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
			}
			if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 10 && pCfgs->event_targets[target_idx].detected)//�����Ŀ��ﵽһ��֡��ʱ������Ϊ���¼�
			{
				//printf("OppositeDirDrive\n");
				/*if(pCfgs->event_targets[target_idx].target_id == 0)
				pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
				pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
				pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
				/*pCfgs->EventInfo[pCfgs->EventNum].uEventID = pCfgs->event_targets[target_idx].target_id;
				pCfgs->EventInfo[pCfgs->EventNum].begin_time = pCfgs->gThisFrameTime;
				pCfgs->EventInfo[pCfgs->EventNum].type = REVERSE_DRIVE;
				pCfgs->EventInfo[pCfgs->EventNum].flag = 0;
				pCfgs->EventNum++;
				if(pCfgs->EventState == 0)//�¼���ʼʱ��
				{
					pCfgs->EventState = 1;
					pCfgs->EventBeginTime = pCfgs->gThisFrameTime - 10;

				}*/
			}
		}
		else//����������������Ϊ0
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] > 0 && ((dy > height / 50 && InRegionVal == 2) || (dy < -height / 50 && InRegionVal == 1)))//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//printf("uOppositeDirDriveNum ,%d\n",pCfgs->event_targets[target_idx].target_id);
		//���������¼���
		if(pCfgs->uOppositeDirDriveNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ�Ϊ�³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uNewEventFlag = 1;
			}
			else
				pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uNewEventFlag = 0;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->OppositeDirDriveBox[pCfgs->uOppositeDirDriveNum].uEventType = REVERSE_DRIVE;
			pCfgs->uOppositeDirDriveNum++;
		}
	}
	else//������������
	{
		pCfgs->event_targets[target_idx].event_flag[event_idx] = 0;
		/*for(j = 0; j < pCfgs->EventNum; j++)//���û�����ô��¼�����������
		{
			if(pCfgs->EventInfo[j].flag == 0)
			{
				if(pCfgs->EventInfo[j].uEventID == pCfgs->event_targets[target_idx].target_id)
				{
					//pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;
					pCfgs->EventInfo[j].flag = 1;
					break;
				}
			}
		}*/
	}
#endif
}

void OffLaneDetect(ALGCFGS *pCfgs, int target_idx, int event_idx)//����ʻ����
{
	//��Ŀ��û�б����Ϊʻ���¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		if(pCfgs->event_targets[target_idx].detected)
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
		}
		if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 10 && pCfgs->event_targets[target_idx].detected)//Ŀ����ʻ�����򣬼��ﵽһ��֡��
		{
			//printf("OffLane\n");
			/*if(pCfgs->event_targets[target_idx].target_id == 0)
			pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
			pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
			pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] > 0)//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//����ʻ���¼���
		if(pCfgs->uOffLaneNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ����³����¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->OffLaneBox[pCfgs->uOffLaneNum].uNewEventFlag = 1;
			}
			else
				pCfgs->OffLaneBox[pCfgs->uOffLaneNum].uNewEventFlag = 0;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->OffLaneBox[pCfgs->uOffLaneNum].uEventType = DRIVE_AWAY;
			pCfgs->uOffLaneNum++;
		}
	}
}
void NoPersonAllowDetect(ALGCFGS *pCfgs, int target_idx, int event_idx, int width, int height)//��ֹ���˼��
{
	//�Ѿ����Ϊ�ǻ������¼����򲻽��������¼��ж�
	if(pCfgs->event_targets[target_idx].cal_event[NONMOTOR])
		return;
	//δ���Ϊ�����¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		if(pCfgs->event_targets[target_idx].continue_num < 20 && (pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height) > height - 20)
			return;
		//�������Ŀ��ͷǻ�����Ŀ���ཻ������Ϊ�Ƿǻ������ϵ�����
		for(int i = 0; i < pCfgs->event_targets_size; i++)
		{
			if(strcmp(pCfgs->event_targets[i].names, "motorbike") != 0 && strcmp(pCfgs->event_targets[i].names, "bicycle") != 0)//ֻ��ǻ��������бȽ�
				continue;
			if(overlapRatio(pCfgs->event_targets[i].box, pCfgs->event_targets[target_idx].box) > 5)//����ཻ���˳��������к����ж�
				return;
		}
		if(pCfgs->event_targets[target_idx].detected && pCfgs->event_targets[target_idx].prob >= 0.5)//δ��ֹ��죬�����Ŷ���Ϊ0.5
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;//�����¼��ĳ���֡��
		}
		if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 15 && pCfgs->event_targets[target_idx].detected)//����һ��֡������Ϊ��ֹ���ˣ���ֹ�������
		{
			//printf("no person allow ok\n");
			/*if(pCfgs->event_targets[target_idx].target_id == 0)
			pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
			pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
			pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
			/*pCfgs->EventInfo[pCfgs->EventNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->EventInfo[pCfgs->EventNum].begin_time = pCfgs->gThisFrameTime;
			pCfgs->EventInfo[pCfgs->EventNum].type = NO_PEDESTRIANTION;
			pCfgs->EventInfo[pCfgs->EventNum].flag = 0;
			pCfgs->EventNum++;
			if(pCfgs->EventState == 0)//�¼���ʼʱ��
			{
				pCfgs->EventState = 1;
				pCfgs->EventBeginTime = pCfgs->gThisFrameTime;
			}*/
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx])//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//���������¼���
		if(pCfgs->uNoPersonAllowNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ�Ϊ�³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].uNewEventFlag = 1;
			}
			else
				pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].uNewEventFlag = 0;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->NoPersonAllowBox[pCfgs->uNoPersonAllowNum].uEventType = NO_PEDESTRIANTION;
			pCfgs->uNoPersonAllowNum++;
		}
	}
}
void NonMotorAllowDetect(ALGCFGS *pCfgs, int target_idx, int event_idx)//��ֹ�ǻ��������
{
	//�Ѿ����Ϊ�ǻ������¼����򲻽��������¼��ж�
	if(pCfgs->event_targets[target_idx].cal_event[NO_PEDESTRIANTION])
		return;
	//δ���Ϊ�ǻ������¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		//�������Ŀ��ͷǻ�����Ŀ���ཻ�����Ѿ���⵽�����¼����򲻽��зǻ������¼��ж�
		for(int i = 0; i < pCfgs->event_targets_size; i++)
		{
			if(strcmp(pCfgs->event_targets[i].names, "person") != 0)//ֻ�����˽��бȽ�
				continue;
			if(pCfgs->event_targets[i].cal_event[NO_PEDESTRIANTION] == FALSE)
				continue;
			if(overlapRatio(pCfgs->event_targets[i].box, pCfgs->event_targets[target_idx].box) > 0)//����ཻ���˳��������к����ж�
				return;
		}
		if(pCfgs->event_targets[target_idx].detected && pCfgs->event_targets[target_idx].prob >= 0.5)//���
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;//�����¼��ĳ���֡��
		}
		if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 10 && pCfgs->event_targets[target_idx].detected)//����һ��֡������Ϊ��ֹ�ǻ���������ֹ�ǻ��������
		{
			/*if(pCfgs->event_targets[target_idx].target_id == 0)
			pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
			pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
			pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx])//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//����ǻ������¼���
		if(pCfgs->uNonMotorAllowNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ����³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].uNewEventFlag = 1;
			}
			else
				pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].uNewEventFlag = 0;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->NonMotorAllowBox[pCfgs->uNonMotorAllowNum].uEventType = NONMOTOR;
			pCfgs->uNonMotorAllowNum++;
		}
	}
}
void PersonFallDetect(ALGCFGS *pCfgs, int target_idx, int event_idx, int width, int height)//���˵������
{
	int j = 0;
	//δ���Ϊ���˵����¼�
	float ratio = (float) pCfgs->event_targets[target_idx].box.height / (float) pCfgs->event_targets[target_idx].box.width;//��ǰ֡�ĳ����  
	//printf("height and width =%f\n",ratio);
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] == 0)
	{
		bool inDetRegion = FALSE;//�����ж������Ƿ��ڼ�������ڲ�
		if(pCfgs->event_targets[target_idx].box.x > 10 && pCfgs->event_targets[target_idx].box.y > 10 && (pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width) < width - 10 && (pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height) < height - 10)//�ڼ�������ڲ�
			inDetRegion = TRUE;
		if(inDetRegion)
		{  
			float ratioMax = 0;
			int personStand = 0;
			for(j = pCfgs->event_targets[target_idx].trajectory_num - 1; j  >= 0; j--)
			{
				float ratio0 = (float) pCfgs->event_targets[target_idx].trajectory[j].height / pCfgs->event_targets[target_idx].trajectory[j].width;
				ratioMax = (ratioMax < ratio0)? ratio0 : ratioMax;
				if(ratio0 > 2.5) //����վ��
					personStand++;
				else if(ratio0 < 2)
					personStand = 0;
				if(personStand > 5)
					break;
			} 
			if(personStand > 5 || ratio < 0.8)//����վ������5֡
			{
				//printf("detected =%d,%f,%d\n",pCfgs->event_targets[target_idx].detected,pCfgs->event_targets[target_idx].prob,pCfgs->event_targets[target_idx].event_continue_num[event_idx]);
				if(pCfgs->event_targets[target_idx].detected && pCfgs->event_targets[target_idx].prob >= 0.5 && ((ratioMax / ratio > 2 && ratio < 1.2)|| ratio < 0.8))//δ��ֹ��죬�����Ŷ���Ϊ0.5
				{
					pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;//�����¼��ĳ���֡��
				}

				if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 20 && pCfgs->event_targets[target_idx].detected)//����һ��֡������Ϊ���˵���
				{
					if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
					{
						/*if(pCfgs->event_targets[target_idx].target_id == 0)
							pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
						pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
					}
					pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
				}
			}
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx])//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//printf("person down \n");
		//�������˵����¼���
		if(pCfgs->uPersonFallNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ����³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->PersonFallBox[pCfgs->uPersonFallNum].uNewEventFlag = 1;
				//printf("new person fall event\n");
			}
			else
				pCfgs->PersonFallBox[pCfgs->uPersonFallNum].uNewEventFlag = 0;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->PersonFallBox[pCfgs->uPersonFallNum].uEventType = PERSONFALL;
			pCfgs->uPersonFallNum++;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] && ratio > 2)//�������˵���
	{
		pCfgs->event_targets[target_idx].event_flag[event_idx] = 0;
		pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
	}
}
void NonMotorFallDetect(ALGCFGS *pCfgs, int target_idx, int event_idx, int width, int height)//�ǻ���������
{
	int j = 0;
	//δ���Ϊ�ǻ������¼�
	float ratio = (float) pCfgs->event_targets[target_idx].box.height / (float) pCfgs->event_targets[target_idx].box.width;//��ǰ֡�ĳ����  
	//printf("height and width =%f\n",ratio);
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] == 0)
	{
		bool inDetRegion = FALSE;//�����жϷǻ������Ƿ��ڼ�������ڲ�
		if(pCfgs->event_targets[target_idx].box.x > 10 && pCfgs->event_targets[target_idx].box.y > 10 && (pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width) < width - 10 && (pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height) < height - 10)//�ڼ�������ڲ�
			inDetRegion = TRUE;
		if(inDetRegion)
		{  
			float ratioMax = 0;
			int NonMontorStand = 0;
			for(j = pCfgs->event_targets[target_idx].trajectory_num - 1; j  >= 0; j--)
			{
				float ratio0 = (float) pCfgs->event_targets[target_idx].trajectory[j].height / pCfgs->event_targets[target_idx].trajectory[j].width;
				ratioMax = (ratioMax < ratio0)? ratio0 : ratioMax;
				if(ratio0 > 2.5) //�ǻ�����վ��
					NonMontorStand++;
				else if(ratio0 < 2)
					NonMontorStand = 0;
				if(NonMontorStand > 5)
					break;
			} 
			if(NonMontorStand > 5 || ratio < 0.8)//�ǻ�����վ������5֡
			{
				//printf("detected =%d,%f,%d\n",pCfgs->event_targets[target_idx].detected,pCfgs->event_targets[target_idx].prob,pCfgs->event_targets[target_idx].event_continue_num[event_idx]);
				if(pCfgs->event_targets[target_idx].detected && pCfgs->event_targets[target_idx].prob >= 0.5 && ((ratioMax / ratio > 2 && ratio < 1.2)|| ratio < 0.8))//δ��ֹ��죬�����Ŷ���Ϊ0.5
				{
					pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;//�����¼��ĳ���֡��
				}

				if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 20 && pCfgs->event_targets[target_idx].detected)//����һ��֡������Ϊ�ǻ���������
				{
					if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
					{
						/*if(pCfgs->event_targets[target_idx].target_id == 0)
							pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
						pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
					}
					pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
				}
			}
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx])//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//printf("person down \n");
		//����ǻ����������¼���
		if(pCfgs->uNonMotorFallNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ����³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].uNewEventFlag = 1;
			}
			else
				pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].uNewEventFlag = 0;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->NonMotorFallBox[pCfgs->uNonMotorFallNum].uEventType = NONMOTORFALL;
			pCfgs->uNonMotorFallNum++;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] && ratio > 2)//�������˵���
	{
		pCfgs->event_targets[target_idx].event_flag[event_idx] = 0;
		pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
	}
}
void GreenwayDropDetect(ALGCFGS *pCfgs, int target_idx, int event_idx)//ֽ����ƿ��
{
	//δ���Ϊ�̵�������
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		if(pCfgs->event_targets[target_idx].detected && pCfgs->event_targets[target_idx].prob >= 0.5)//���
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;//�����¼��ĳ���֡��
		}
		if(pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 10 && pCfgs->event_targets[target_idx].detected)//����һ��֡������Ϊ�������ֹ���
		{
			/*if(pCfgs->event_targets[target_idx].target_id == 0)
			pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
			pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
			pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx])//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		//�����̵��������¼���
		if(pCfgs->uGreenwayDropNum < MAX_EVENT_NUM)
		{
			//�ж��Ƿ����³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].uNewEventFlag = 1;
			}
			else
				pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].uNewEventFlag = 0;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->GreenwayDropBox[pCfgs->uGreenwayDropNum].uEventType = GREENWAYDROP;
			pCfgs->uGreenwayDropNum++;
		}
	}
}
void TrafficAccidentDetect(ALGCFGS *pCfgs, int target_idx, int event_idx, int targetDisXY[][3], int width, int height)//��ͨ�¹ʼ��
{
	int i = 0, j = 0, k = 0;
	int thr = 20;//��ֵ
	int disX = 0, disY = 0;
	int num = 0;
	/*int continue_num = pCfgs->event_targets[target_idx].trajectory_num - 100;
	continue_num = (continue_num < 0)? 0 : continue_num;
	int dx = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].x - pCfgs->event_targets[target_idx].trajectory[continue_num].x;
	int dy = pCfgs->event_targets[target_idx].trajectory[pCfgs->event_targets[target_idx].trajectory_num - 1].y - pCfgs->event_targets[target_idx].trajectory[continue_num].y;
	dx = (dx < 0)? -dx : dx;
	dy = (dy < 0)? -dy : dy;*/
	int dx = targetDisXY[target_idx][0];
	int dy = targetDisXY[target_idx][1];
	int idx = target_idx;
	//δ���Ϊ�¹��¼�
	if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		thr = pCfgs->event_targets[target_idx].box.height / 2;
		thr = (thr > 200)? 200 : thr;
		thr = (thr < 5)? 5 : thr;
		if(dx < width / 20 && dy < thr )//������Ծ�ֹ����Χ�г���Ծ�ֹ�����Ҹ������ˣ�����Ϊ�ǽ�ͨ�¼�
		{
			/*if(pCfgs->event_targets[target_idx].detected)
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
			}*/
			if(pCfgs->event_targets[target_idx].lost_detected > 20)//��ʱ��û�м�⵽�����¼�������ֹ���
			{
				pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
			}
			if(/*pCfgs->event_targets[target_idx].event_continue_num[event_idx] > 20 &&*/ pCfgs->event_targets[target_idx].detected)//ͣ��֡���ﵽ20֡������Ϊ��ͣ��
			{
				bool hasPerson = FALSE;
				//�жϸ��ٿ���Χ�Ƿ�����Ծ�ֹ��Ŀ��
				for(j = 0; j < pCfgs->event_targets_size; j++)
				{
					if( j == target_idx || pCfgs->event_targets[j].cal_event[event_idx] || pCfgs->event_targets[j].continue_num < 50 || pCfgs->event_targets[j].lost_detected)
						continue;
					disX = MIN(pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width,pCfgs->event_targets[j].box.x + pCfgs->event_targets[j].box.width)- MAX(pCfgs->event_targets[target_idx].box.x, pCfgs->event_targets[j].box.x);
					disY = MIN(pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height,pCfgs->event_targets[j].box.y + pCfgs->event_targets[j].box.height)- MAX(pCfgs->event_targets[target_idx].box.y, pCfgs->event_targets[j].box.y);
					//if(disX > -width / 10 && disY > -height / 10)
					if(disX >= 0 && disY >= 0)//�������ཻ
					{
						thr = pCfgs->event_targets[j].box.height / 2;
						thr = (thr > 200)? 200 : thr;
						thr = (thr < 5)? 5 : thr;
						if(targetDisXY[j][0] < width / 20 && targetDisXY[j][1] < thr)
						{
							num++;
						}
					}
					if(num > 0)
					{
						/*if(pCfgs->event_targets[j].box.width > pCfgs->event_targets[target_idx].box.width || pCfgs->event_targets[j].box.height > pCfgs->event_targets[target_idx].box.height)
							idx = target_idx;
						else
							idx = j;*/
						//��չ����
						CRect rct;
						rct.x = MIN(pCfgs->event_targets[target_idx].box.x, pCfgs->event_targets[j].box.x) - 30;
						rct.y = MIN(pCfgs->event_targets[target_idx].box.y, pCfgs->event_targets[j].box.y) - 30;
						rct.width = MAX(pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width,pCfgs->event_targets[j].box.x + pCfgs->event_targets[j].box.width) - MIN(pCfgs->event_targets[target_idx].box.x, pCfgs->event_targets[j].box.x) + 60;
						rct.height = MAX(pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height,pCfgs->event_targets[j].box.y + pCfgs->event_targets[j].box.height) - MIN(pCfgs->event_targets[target_idx].box.y, pCfgs->event_targets[j].box.y) + 60;
						//�жϸ��ٿ���Χ�Ƿ�������
						for(i = 0; i < pCfgs->classes; i++)
						{
							if(strcmp(pCfgs->detClasses[i].names, "person") != 0)
								continue;
							for( k = 0; k < pCfgs->detClasses[i].classes_num; k++)
							{
								if(pCfgs->detClasses[i].box[k].width * 2  > pCfgs->detClasses[i].box[k].height)//������˳���
									continue;
								if(overlapRatio(pCfgs->detClasses[i].box[k], rct) > 2)
								{
									hasPerson = TRUE;
									break;
								}
							}
							if(hasPerson)
								break;
						}
						break;
					}
				}
				//�������ˣ�������ΧҲ����Ծ�ֹ��Ŀ��
				if(hasPerson && num)
				{
					pCfgs->event_targets[target_idx].event_continue_num[event_idx]++;
				}
				if(pCfgs->event_targets[target_idx].cal_event[event_idx] == FALSE && pCfgs->event_targets[target_idx].event_continue_num[event_idx]> 10)//��ʼ�����¹��¼�
				{
					/*if(pCfgs->event_targets[target_idx].target_id == 0)
						pCfgs->event_targets[target_idx].target_id = pCfgs->eventID++;//��ÿ���¼�һ��ID*/
					pCfgs->event_targets[target_idx].cal_event[event_idx] = TRUE;
					pCfgs->event_targets[target_idx].event_flag[event_idx] = 1;
				}
			}
		}
		else//�����˶�
		{
			pCfgs->event_targets[target_idx].event_continue_num[event_idx] = 0;
		}
	}
	if(pCfgs->event_targets[target_idx].event_flag[event_idx] == 1 && dx < width / 10 && dy < height / 5)//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�,��ֹ��죬�����뿪
	{
		/*for(k = 0; k < pCfgs->uTrafficAccidentNum; k++)
		{
			if(pCfgs->TrafficAccidentBox[k].uEventID == pCfgs->event_targets[target_idx].target_id)
			{
				break;
			}
		}*/
		if(pCfgs->uTrafficAccidentNum < MAX_EVENT_NUM /*&& k == pCfgs->uTrafficAccidentNum*/)//֮ǰû�д�ID�¼���
		{
			//����Χ��ֹ��Ŀ����Ϊ�Ѽ�⵽��ͨ�¹ʣ���ֹ��
			for(j = 0; j < pCfgs->event_targets_size; j++)
			{
				if( j == target_idx)
					continue;
				if(overlapRatio(pCfgs->event_targets[j].box,pCfgs->event_targets[target_idx].box) > 0)
				{
					thr = pCfgs->event_targets[j].box.height;
					thr = (thr > 200)? 200 : thr;
					thr = (thr < 5)? 5 : thr;
					if(targetDisXY[j][0] < width / 10 && targetDisXY[j][1] < thr)
					{
						pCfgs->event_targets[j].event_flag[event_idx] = pCfgs->event_targets[target_idx].target_id;//���Ϊ�����¹ʵ�ID
						pCfgs->event_targets[j].cal_event[event_idx] = TRUE;
						//pCfgs->event_targets[j].target_id = pCfgs->event_targets[target_idx].target_id;//����Ŀ���ID����Ϊ��Ӧ��ͨ�¹ʵ�ID

					}
				}
			}

			//�ж��Ƿ�Ϊ�³��ֵ��¼�
			if(pCfgs->event_targets[target_idx].sign_event[event_idx] == 0)
			{
				pCfgs->event_targets[target_idx].sign_event[event_idx] = 1;
				pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].uNewEventFlag = 1;
			}
			else
				pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].uNewEventFlag = 0;

			//���潻ͨ�¹��¼���
			int minX = pCfgs->event_targets[target_idx].box.x;
			int maxX = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			int minY = pCfgs->event_targets[target_idx].box.y;
			int maxY = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].uRegionID = pCfgs->event_targets[target_idx].region_idx;//�¼�����ID
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].uEventID = pCfgs->event_targets[target_idx].target_id;
			/*pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[0].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[0].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[1].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[1].y = pCfgs->event_targets[target_idx].box.y;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[2].x = pCfgs->event_targets[target_idx].box.x + pCfgs->event_targets[target_idx].box.width;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[2].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[3].x = pCfgs->event_targets[target_idx].box.x;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[3].y = pCfgs->event_targets[target_idx].box.y + pCfgs->event_targets[target_idx].box.height;*/
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[0].x = minX;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[0].y = minY;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[1].x = maxX;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[1].y = minY;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[2].x = maxX;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[2].y = maxY;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[3].x = minX;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].EventBox[3].y = maxY;
			pCfgs->TrafficAccidentBox[pCfgs->uTrafficAccidentNum].uEventType = ACCIDENTTRAFFIC;
			pCfgs->uTrafficAccidentNum++;
		}
	}
	else//���˶��󣬽����¹�
	{
		pCfgs->event_targets[target_idx].event_flag[event_idx] = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////��ͨ�¼����
void TrafficEventAnalysis(ALGCFGS *pCfgs, ALGPARAMS *pParams, int width, int height)
{	
	int i = 0, j = 0, k = 0, l = 0;
	int left = 0, right = 0, top = 0, bottom = 0;
	//int match_object[MAX_TARGET_NUM] = { -1 };
	//int match_rect[MAX_TARGET_NUM] = { -1 };
	int match_object[MAX_CLASSES][MAX_DETECTION_NUM];
	int match_rect[MAX_TARGET_NUM][3];
	int match_success = -1;
	bool isInRegion = FALSE;
	CRect targetRect[MAX_TARGET_NUM];
	int  targetInRegion[MAX_TARGET_NUM][MAX_EVENT_TYPE]={ 0 };
	int targetDisXY[MAX_TARGET_NUM][3]={ 0 };
	bool isCycle = FALSE;
	//��ʼ����ͨ�¼������
	pCfgs->uIllegalParkNum = 0;//Υ��ͣ��
	memset(pCfgs->IllegalParkBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uOppositeDirDriveNum = 0;//����
	memset(pCfgs->OppositeDirDriveBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uOffLaneNum = 0;//ʻ��
	memset(pCfgs->OffLaneBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uNoPersonAllowNum = 0;//��ֹ����
	memset(pCfgs->NoPersonAllowBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uDropNum = 0;//������
	memset(pCfgs->DropBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uNonMotorAllowNum = 0;//��ֹ�ǻ�����
	memset(pCfgs->NonMotorAllowBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uPersonFallNum = 0;//���˵���
	memset(pCfgs->PersonFallBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uNonMotorFallNum = 0;//�ǻ���������
	memset(pCfgs->NonMotorFallBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uGreenwayDropNum = 0;//�̵�������
	memset(pCfgs->GreenwayDropBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uTrafficAccidentNum = 0;//��ͨ�¹�
	memset(pCfgs->TrafficAccidentBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	//����Ŀ��Ϊδ��⵽
	for( i = 0; i < pCfgs->event_targets_size; i++)
	{
		pCfgs->event_targets[i].detected = FALSE;
	}
	//�����Ŀ������ƥ��
	match_object_rect1(pCfgs, pCfgs->event_targets, pCfgs->event_targets_size, match_object, match_rect, 10);
	//��������
	for( i = 0; i < pCfgs->classes; i++)
	{
		//������Ҫ�����
		if(strcmp(pCfgs->detClasses[i].names, "person") != 0 && strcmp(pCfgs->detClasses[i].names, "bicycle") != 0 && strcmp(pCfgs->detClasses[i].names, "motorbike") != 0 && strcmp(pCfgs->detClasses[i].names, "car") != 0 && strcmp(pCfgs->detClasses[i].names, "bus") != 0 && strcmp(pCfgs->detClasses[i].names, "truck") != 0 && strcmp(pCfgs->detClasses[i].names,"cup") != 0 && strcmp(pCfgs->detClasses[i].names,"bottle") != 0)
			continue;
		if(pCfgs->detClasses[i].classes_num)
		{
			//match_object_rect(pCfgs->event_targets, pCfgs->event_targets_size, pCfgs->detClasses, i, match_object, match_rect, 10);

			for( j = 0; j < pCfgs->detClasses[i].classes_num; j++)
			{
				//���Ŷȵ���0.5������
				//if(pCfgs->detClasses[i].prob[j] < 0.5)
				//	continue;
				/*if(strcmp(pCfgs->detClasses[i].names, "person") == 0)//�ж��Ƿ���ǻ���������
				{
					isCycle = FALSE;
					for( k = 0; k < pCfgs->classes; k++)
					{
						if(strcmp(pCfgs->detClasses[k].names, "bicycle") != 0 && strcmp(pCfgs->detClasses[k].names, "motorbike") != 0)//�ǻ�����
							continue;
						for(l = 0; l < pCfgs->detClasses[k].classes_num; l++)
						{
							if(overlapRatio(pCfgs->detClasses[i].box[j], pCfgs->detClasses[k].box[l]) > 5)//���ڷǻ������ϵ�����
							{
								isCycle = TRUE;
								break;
							
							}
						}
						if(isCycle == TRUE)
							break;
					}
				}
				if(isCycle == TRUE)//���ڷǻ������ϵ���
					continue;*/
				//�����������ڼ��������
				left = MAX(0, pCfgs->detClasses[i].box[j].x * pCfgs->m_iWidth / width);
				right = MIN((pCfgs->detClasses[i].box[j].x + pCfgs->detClasses[i].box[j].width) * pCfgs->m_iWidth / width, pCfgs->m_iWidth - 1);
				top = MAX(0, pCfgs->detClasses[i].box[j].y * pCfgs->m_iHeight / height);
				bottom = MIN((pCfgs->detClasses[i].box[j].y + pCfgs->detClasses[i].box[j].height) * pCfgs->m_iHeight / height, pCfgs->m_iHeight - 1);
				CRect rct;
				rct.x = left;
				rct.y = top;
				rct.width = right - left + 1;
				rct.height = bottom - top + 1;
				isInRegion = FALSE;//��ʼ��Ϊδ���¼��������
				int inRegion[MAX_EVENT_TYPE] = { 0 };
				if(strcmp(pCfgs->detClasses[i].names, "person") == 0)//����
				{
					inRegion[NO_PEDESTRIANTION] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, NO_PEDESTRIANTION);//��ֹ����
					inRegion[PERSONFALL] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, PERSONFALL);//���˵���

				}
				else if(strcmp(pCfgs->detClasses[i].names, "bicycle") == 0 || strcmp(pCfgs->detClasses[i].names, "motorbike") == 0)//�ǻ�����
				{
					inRegion[NONMOTOR] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, NONMOTOR);//���зǻ�����
					inRegion[NONMOTORFALL] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, NONMOTORFALL);//�ǻ���������
				}
				else if(strcmp(pCfgs->detClasses[i].names, "cup") == 0 || strcmp(pCfgs->detClasses[i].names, "bottle") == 0)//ֽ����ƿ��
				{
					inRegion[GREENWAYDROP] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, GREENWAYDROP);//ֽ����ƿ��
				}
				else
				{ 
					inRegion[STOP_INVALID] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, STOP_INVALID);//��ֹͣ��
					inRegion[REVERSE_DRIVE] = RectInRegion0(pParams->MaskOppositeDirDriveImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, REVERSE_DRIVE);//��ֹ����
					inRegion[DRIVE_AWAY] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, DRIVE_AWAY);//ʻ��
					inRegion[ACCIDENTTRAFFIC] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, ACCIDENTTRAFFIC);//׷β
				}
				for(k = 0; k < MAX_EVENT_TYPE; k++)
				{
					if(inRegion[k])
					{
						isInRegion = TRUE;//���¼��������
						break;
					}
				}
				//if(isInRegion)//���¼���������ڽ��и���
				{
					match_success = -1;
					for( k = 0; k < pCfgs->event_targets_size; k++)
					{
						/*if(match_object[j] == k && match_rect[k] == j)
						{
							//float ratio = (float)(pCfgs->event_targets[k].box.width * pCfgs->event_targets[k].box.height)/(float)(pCfgs->detClasses[i].box[j].width * pCfgs->detClasses[i].box[j].height);
							//if(ratio > 0.5 && ratio < 1.5)
								match_success = 1;
							break;
						}*/
						if(match_object[i][j] == k && match_rect[k][0] == i && match_rect[k][1] == j)
						{
							//float ratio = (float)(pCfgs->event_targets[k].box.width * pCfgs->event_targets[k].box.height)/(float)(pCfgs->detClasses[i].box[j].width * pCfgs->detClasses[i].box[j].height);
							//if(ratio > 0.5 && ratio < 1.5)
							match_success = 1;
							break;
						}
						if(match_object[i][j] == k && (match_rect[k][0] != i || match_rect[k][1] != j))//��������ƥ��һ��Ŀ���û��ƥ���ϣ���������Ŀ��
						{
							match_success = 0;
							break;
						}
					}
					if(match_success < 0)//����û���ҵ�ƥ���Ŀ���
					{
						//Ħ�г������г��ϵ����˲��ܵ�������
						for( k = 0; k < pCfgs->event_targets_size; k++)
						{
							//����Ϊ���ˣ�Ŀ���Ϊ������������������ཻ����Ϊ�ǻ������ϵ����ˣ��������µ�Ŀ��
							/*if(strcmp(pCfgs->detClasses[i].names, "person") == 0 && (strcmp(pCfgs->event_targets[k].names, "motorbike") == 0 || strcmp(pCfgs->event_targets[k].names, "bicycle") == 0))//����
							{
								if(overlapRatio(pCfgs->detClasses[i].box[j], pCfgs->event_targets[k].box) > 10 && match_rect[k][0] < 0 && match_rect[k][1] < 0)//���ڷǻ������ϵ�����
								{
									//printf("person match nonmotor %d\n", pCfgs->event_targets[k].target_id);
									match_success = 0;
									pCfgs->event_targets[k].box = pCfgs->detClasses[i].box[j];
									pCfgs->event_targets[k].prob = pCfgs->detClasses[i].prob[j];
									pCfgs->event_targets[k].detected = TRUE;
									break;
								}
							}*/
							//����Ϊ�ǻ�������Ŀ���Ϊ���ˣ�����������ཻ���ü������Ŀ���
							if((strcmp(pCfgs->detClasses[i].names, "motorbike") == 0 || strcmp(pCfgs->detClasses[i].names, "bicycle") == 0) && strcmp(pCfgs->event_targets[k].names, "person") == 0)
							{
								if(overlapRatio(pCfgs->detClasses[i].box[j], pCfgs->event_targets[k].box) > 5 && match_rect[k][0] < 0 && match_rect[k][1] < 0)//������Ŀ�����Ϊ�ǻ�����Ŀ��
								{
									//printf("nonmotor match person %d\n", pCfgs->event_targets[k].target_id);
									match_success = 1;
									break;
								}

							}
						}

					}
					if(match_success > 0)//���ٵ�,���¼���
					{
						//���Ŀ���Ѿ�����Ϊͣ���¼���Ӧ�����ƥ��ȸߣ�����Ϊ��⵽����ֹĿ��Ư�ƣ���Ϊ�˶�Ŀ�꣬�����ٳ���ͣ���¼�
						if(pCfgs->event_targets[k].cal_event[STOP_INVALID])
						{
							if(overlapRatio(pCfgs->detClasses[i].box[j], pCfgs->event_targets[k].box) < 60)
							{
								continue;
							}
						}
						pCfgs->event_targets[k].box = pCfgs->detClasses[i].box[j];
						pCfgs->event_targets[k].prob = pCfgs->detClasses[i].prob[j];
						pCfgs->event_targets[k].class_id = pCfgs->detClasses[i].class_id;
						strcpy(pCfgs->event_targets[k].names, pCfgs->detClasses[i].names);
						pCfgs->event_targets[k].detected = TRUE;
					}
					else if(isInRegion && match_success < 0 && pCfgs->event_targets_size < MAX_TARGET_NUM)//�¼������ڣ�δ���ٵ��������µ�Ŀ��
					{
						if(strcmp(pCfgs->detClasses[i].names, "person") == 0 && pCfgs->detClasses[i].prob[j] < 0.5)
							continue;						
						CTarget nt; 
						Initialize_target(&nt);//��ʼ���¼�Ŀ��
						nt.box = pCfgs->detClasses[i].box[j];
						nt.class_id = pCfgs->detClasses[i].class_id;
						nt.prob = pCfgs->detClasses[i].prob[j];
						nt.detected = TRUE;
						nt.target_id = pCfgs->event_target_id++;
						nt.start_time = pCfgs->currTime;//Ŀ�꿪ʼʱ��
						nt.region_idx = pCfgs->EventDetectCfg.EventRegion[0].uRegionID;//��ʼ��0����ID
						if(pCfgs->event_target_id > 5000)
							pCfgs->event_target_id = 1;
						strcpy(nt.names, pCfgs->detClasses[i].names);
						memset(nt.event_continue_num, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ���¼�����֡��
						memset(nt.event_flag, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ���¼����
						memset(nt.cal_event, FALSE, MAX_EVENT_TYPE * sizeof(bool));//��ʼ�������¼�Ϊδ����
						memset(nt.sign_event, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ��Ϊδ��ǵ��¼�
						memset(nt.statistic, -1, 5 * sizeof(int));//����ͳ���˶����
						pCfgs->event_targets[pCfgs->event_targets_size] = nt;
						pCfgs->event_targets_size++;
					}
				}
			}
		}
	}
	//�ϲ��ཻ������ֵ�ķǻ�����Ŀ�������Ŀ��
	for(i = 0; i < pCfgs->event_targets_size; i++)
	{
		int del_target = 0;
		if(strcmp(pCfgs->event_targets[i].names, "person") != 0)//������Ŀ��Ƚ�
			continue;
		if(pCfgs->event_targets[i].cal_event[NO_PEDESTRIANTION])//���Ŀ���Ѿ����Ϊ�����¼������ϲ�Ŀ��
			continue;
		for(j = 0; j < pCfgs->event_targets_size; j++)
		{
			if(strcmp(pCfgs->event_targets[j].names, "motorbike") != 0 && strcmp(pCfgs->event_targets[j].names, "bicycle") != 0)
				continue;
			if(overlapRatio(pCfgs->event_targets[i].box, pCfgs->event_targets[j].box) > 10)//�ǻ�����������Ŀ���غϱȽϴ�,��Ϊ�Ƿǻ�����
			{
				CRect rct;
				bool detected = pCfgs->event_targets[j].detected;
				float prob = pCfgs->event_targets[j].prob;
				if(pCfgs->event_targets[i].detected && pCfgs->event_targets[j].detected)//Ŀ��򶼼�⵽���ϲ���
				{
					rct.x = MIN(pCfgs->event_targets[i].box.x, pCfgs->event_targets[j].box.x);
					rct.y = MIN(pCfgs->event_targets[i].box.y, pCfgs->event_targets[j].box.y);
					rct.width = MAX(pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width - rct.x, pCfgs->event_targets[j].box.x + pCfgs->event_targets[j].box.width - rct.x);
					rct.height = MAX(pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height - rct.y, pCfgs->event_targets[j].box.y + pCfgs->event_targets[j].box.height - rct.y);
				}
				else if(pCfgs->event_targets[i].detected)//�������˿�
				{
					rct = pCfgs->event_targets[i].box;
					prob = pCfgs->event_targets[i].prob;
					detected = pCfgs->event_targets[i].detected;
				}
				else
					rct = pCfgs->event_targets[j].box;//���÷ǻ�������
				pCfgs->event_targets[j].box = rct;
				pCfgs->event_targets[j].prob = prob;
				pCfgs->event_targets[j].detected = detected;
				del_target = 1;
				break;
			}

		}
		if(del_target == 1)//ɾ������Ŀ��
			DeleteTarget(&pCfgs->event_targets_size, &i, pCfgs->event_targets);

	}
	//���ص���ķǻ��������кϲ�
	for(i = 0; i < pCfgs->event_targets_size; i++)
	{
		int del_target = 0;
		if(strcmp(pCfgs->event_targets[i].names, "motorbike") != 0 && strcmp(pCfgs->event_targets[i].names, "bicycle") != 0)//�ǻ�����Ŀ��
			continue;
		if(pCfgs->event_targets[i].cal_event[NONMOTOR])//���Ŀ���Ѿ����Ϊ�ǻ������¼������ϲ�Ŀ��
			continue;
		for(j = 0; j < pCfgs->event_targets_size; j++)
		{
			if(strcmp(pCfgs->event_targets[j].names, "motorbike") != 0 && strcmp(pCfgs->event_targets[j].names, "bicycle") != 0)//�ǻ�����Ŀ��
				continue;
			if(i == j)//��ͬĿ��
				continue;
			if(overlapRatio(pCfgs->event_targets[i].box, pCfgs->event_targets[j].box) > 10)//�ǻ�����Ŀ���غϱȽϴ�,��Ϊ�Ƿǻ�����
			{
				CRect rct;
				bool detected = pCfgs->event_targets[j].detected;
				float prob = pCfgs->event_targets[j].prob;
				if(pCfgs->event_targets[i].detected && pCfgs->event_targets[j].detected)//Ŀ��򶼼�⵽���ϲ���
				{
					rct.x = MIN(pCfgs->event_targets[i].box.x, pCfgs->event_targets[j].box.x);
					rct.y = MIN(pCfgs->event_targets[i].box.y, pCfgs->event_targets[j].box.y);
					rct.width = MAX(pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width - rct.x, pCfgs->event_targets[j].box.x + pCfgs->event_targets[j].box.width - rct.x);
					rct.height = MAX(pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height - rct.y, pCfgs->event_targets[j].box.y + pCfgs->event_targets[j].box.height - rct.y);
				}
				else if(pCfgs->event_targets[i].detected)
				{
					rct = pCfgs->event_targets[i].box;
					prob = pCfgs->event_targets[i].prob;
					detected = pCfgs->event_targets[i].detected;
				}
				else
					rct = pCfgs->event_targets[j].box;
				pCfgs->event_targets[j].box = rct;
				pCfgs->event_targets[j].prob = prob;
				pCfgs->event_targets[j].detected = detected;
				del_target = 1;
				break;
			}

		}
		if(del_target == 1)//ɾ������Ŀ��
			DeleteTarget(&pCfgs->event_targets_size, &i, pCfgs->event_targets);
	}
	//����Ŀ��
	for(i = 0; i < pCfgs->event_targets_size; i++)
	{

		//�켣��С��3000��ֱ�ӱ��棬����3000��ȥ���ɵ�
		if(pCfgs->event_targets[i].trajectory_num < 3000)
		{

			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num].x = pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width / 2;
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num].y = pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height / 2;
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num].width = pCfgs->event_targets[i].box.width;
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num].height = pCfgs->event_targets[i].box.height;
			pCfgs->event_targets[i].trajectory_num++;
		}
		else
		{
			for(j = 0; j < pCfgs->event_targets[i].trajectory_num - 1; j++)
			{
				pCfgs->event_targets[i].trajectory[j] = pCfgs->event_targets[i].trajectory[j + 1];
			}
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num - 1].x = pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width / 2;
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num - 1].y = pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height / 2;
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num - 1].width = pCfgs->event_targets[i].box.width;
			pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num - 1].height = pCfgs->event_targets[i].box.height;
		}
		//��⵽���������ٶ�
		if(pCfgs->event_targets[i].detected)
		{
			/*if(pCfgs->event_targets[i].lost_detected == 0)//ǰһ֡Ҳ�м�⵽�������ٶ�
			{
			get_speed(&pCfgs->event_targets[i]);
			}*/
			pCfgs->event_targets[i].lost_detected = 0;
			/*for(j = 0; j < pCfgs->EventNum; j++)
			{
				if(pCfgs->EventInfo[j].uEventID == pCfgs->event_targets[i].target_id)
				{
					pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;//��¼�������һ֡
					if(pCfgs->EventEndTime < pCfgs->gThisFrameTime)
						pCfgs->EventEndTime = pCfgs->gThisFrameTime;
				}
			}*/
		}
		else//δ��⵽
		{
			pCfgs->event_targets[i].lost_detected++;
			pCfgs->event_targets[i].box.x += pCfgs->event_targets[i].vx;
			pCfgs->event_targets[i].box.y += pCfgs->event_targets[i].vy;
		}

		//�ж�Ŀ���Ƿ��ڼ��������
		left = MAX(0, pCfgs->event_targets[i].box.x * pCfgs->m_iWidth / width);
		right = MIN((pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width)* pCfgs->m_iWidth / width, pCfgs->m_iWidth - 1);
		top = MAX(0, pCfgs->event_targets[i].box.y * pCfgs->m_iHeight / height);
		bottom = MIN((pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height) * pCfgs->m_iHeight / height, pCfgs->m_iHeight - 1);
		CRect rct;
		rct.x = left;
		rct.y = top;
		rct.width = right - left + 1;
		rct.height = bottom - top + 1;
		isInRegion = FALSE;
		if(strcmp(pCfgs->event_targets[i].names, "person") == 0)//����
		{
			targetInRegion[i][NO_PEDESTRIANTION] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, NO_PEDESTRIANTION);
			targetInRegion[i][PERSONFALL] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, PERSONFALL);
		}
		else if(strcmp(pCfgs->event_targets[i].names, "bicycle") == 0 || strcmp(pCfgs->event_targets[i].names, "motorbike") == 0)//�ǻ�����
		{
			targetInRegion[i][NONMOTOR] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, NONMOTOR);
			targetInRegion[i][NONMOTORFALL] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, NONMOTORFALL);
		}
		else if(strcmp(pCfgs->event_targets[i].names, "cup") == 0 || strcmp(pCfgs->event_targets[i].names, "bottle") == 0)//ֽ����ƿ��
		{
			targetInRegion[i][GREENWAYDROP] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, GREENWAYDROP);//ֽ����ƿ��
		}
		else//������
		{
			targetInRegion[i][STOP_INVALID] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, STOP_INVALID);//Υͣ
			targetInRegion[i][REVERSE_DRIVE] = RectInRegion0(pParams->MaskOppositeDirDriveImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, REVERSE_DRIVE);//����
			targetInRegion[i][DRIVE_AWAY] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, DRIVE_AWAY);//ʻ��
			targetInRegion[i][ACCIDENTTRAFFIC] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, ACCIDENTTRAFFIC);//׷β
		}
		//�ж�Ŀ�������ĸ��¼�����
		for(k = 0; k < pCfgs->EventDetectCfg.uEventRegionNum; k++)
		{
			int inRegionRatio = RectInRegion(pParams->MaskEventIDImage, pCfgs, width, height, pCfgs->event_targets[i].box, pCfgs->EventDetectCfg.EventRegion[k].uRegionID);
			if(inRegionRatio > 10)
			{
				pCfgs->event_targets[i].region_idx = pCfgs->EventDetectCfg.EventRegion[k].uRegionID;
				break;
			}
		}
		for(k = 0; k < MAX_EVENT_TYPE; k++)
		{
			if(targetInRegion[i][k])
			{
				isInRegion = TRUE;//���¼��������
				break;
			}
		}
		//ȥ�����ڼ�������Ŀ��
		if(isInRegion)
		{
			;
		}
		else if(pCfgs->event_targets[i].continue_num > 5)//Ŀ�겻���¼������ڣ�����Ŀ���ڼ�������ڴ���5֡����
		{
			/*for(j = 0; j < pCfgs->EventNum; j++)//���û�����ô��¼�����������
			{
				if(pCfgs->EventInfo[j].flag == 0)
				{
					if(pCfgs->EventInfo[j].uEventID == pCfgs->event_targets[i].target_id)
					{
						//pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;
						pCfgs->EventInfo[j].flag = 1;
						break;
					}
				}
			}*/
			//if(pCfgs->event_targets[i].target_id > 0)
				//printf("delete 1\n");
			//DeleteTarget(&pCfgs->event_targets_size, &i, pCfgs->event_targets);
			//continue;
		}
		//��Ŀ������Ƶ����ʱ��̫����ʱ��û�м�⵽���뿪ͼ��ɾ��Ŀ��
		if(pCfgs->event_targets[i].continue_num > 5000 || (pCfgs->event_targets[i].lost_detected > 20 && (pCfgs->event_targets[i].event_flag[STOP_INVALID] == 0 && pCfgs->event_targets[i].event_flag[ACCIDENTTRAFFIC] == 0)) || (pCfgs->event_targets[i].lost_detected > 200 && (pCfgs->event_targets[i].event_flag[STOP_INVALID] || pCfgs->event_targets[i].event_flag[ACCIDENTTRAFFIC]))||((pCfgs->event_targets[i].box.x < 10 || pCfgs->event_targets[i].box.y < 10 || (pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width) > (width - 10) || (pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height) > (height - 10))&& pCfgs->event_targets[i].lost_detected > 0))
		{
			/*for(j = 0; j < pCfgs->EventNum; j++)//���û�����ô��¼�����������
			{
				if(pCfgs->EventInfo[j].flag == 0)
				{
					if(pCfgs->EventInfo[j].uEventID == pCfgs->event_targets[i].target_id)
					{
						//pCfgs->EventInfo[j].end_time = pCfgs->gThisFrameTime;
						pCfgs->EventInfo[j].flag = 1;
						break;
					}
				}
			}*/
			//if(pCfgs->event_targets[i].target_id > 0)
			//	printf("delete 2,%d,lost_detected =%d,flag = %d\n",pCfgs->event_targets[i].continue_num,pCfgs->event_targets[i].lost_detected, pCfgs->event_targets[i].event_flag[ACCIDENTTRAFFIC]);
			DeleteTarget(&pCfgs->event_targets_size, &i, pCfgs->event_targets);
			continue;

		}
		//��Ŀ��򱣴������������¼����
		targetRect[i] = pCfgs->event_targets[i].box;
		//����Ŀ����˶�������������10s������˶����
		int continue_num = pCfgs->event_targets[i].trajectory_num - 150;
		for(j = 0; j < 150; j++)
		{
			continue_num = pCfgs->event_targets[i].trajectory_num - 1 - j;
			if(pCfgs->currTime - pCfgs->uStatFrameTime[149 - j] > 10 || continue_num <= 0)//10s���
			{
				break;
			}
		}
		int dx = pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width / 2 - pCfgs->event_targets[i].trajectory[continue_num].x;
		int dy = pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height / 2 - pCfgs->event_targets[i].trajectory[continue_num].y;
		dx = (dx < 0)? -dx : dx;
		dy = (dy < 0)? -dy : dy;
		targetDisXY[i][0] = dx;
		targetDisXY[i][1] = dy;
		targetDisXY[i][2] = ((149 - j) < 0) ? 0 : (149 - j);//��¼10s�����֡��λ��
		pCfgs->event_targets[i].continue_num++;
	}
	//����Ŀ���¼�����
	for(i = 0;i < pCfgs->event_targets_size; i++)
	{

		isInRegion = FALSE;
		for(k = 0; k < MAX_EVENT_TYPE; k++)
		{
			if(targetInRegion[i][k])
			{
				isInRegion = TRUE;//���¼��������
			}
			if(targetInRegion[i][k] == 0)
			{
				pCfgs->event_targets[i].event_continue_num[k] = 0;
				pCfgs->event_targets[i].event_flag[k] = 0;//���¼��뿪������󣬲������¼�����¼�����ֹһ��Ŀ�����ͬ���¼����
			}
		}
		//�ڼ�������Ŀ��
		if(isInRegion)
		{
			//�жϽ�ͨ�¼�
			if(targetInRegion[i][STOP_INVALID] && pCfgs->event_targets[i].continue_num > 100)//Ŀ���ڽ�ֹͣ������
			{
				IllegalParkDetect(pCfgs, i, STOP_INVALID, targetDisXY, width, height);//��ֹͣ�����
			}
			if(targetInRegion[i][REVERSE_DRIVE])//Ŀ���ڽ�ֹ��������
			{
				OppositeDirDriveDetect(pCfgs, i, REVERSE_DRIVE, targetInRegion[i][REVERSE_DRIVE], width, height);
			}
			if(targetInRegion[i][DRIVE_AWAY])//ʻ��
			{
				OffLaneDetect(pCfgs, i, DRIVE_AWAY);
			}
			if(targetInRegion[i][NO_PEDESTRIANTION])//����
			{
				NoPersonAllowDetect(pCfgs, i, NO_PEDESTRIANTION, width, height);//��ֹ���˼��
			}
			if(targetInRegion[i][NONMOTOR])//�ǻ�����
			{
				NonMotorAllowDetect(pCfgs, i, NONMOTOR);//��ֹ�ǻ��������
			}
			if(targetInRegion[i][PERSONFALL])//���˵���
			{
				PersonFallDetect(pCfgs, i, PERSONFALL, width, height);//���˵������
			}
			if(targetInRegion[i][NONMOTORFALL])//�ǻ���������
			{
				NonMotorFallDetect(pCfgs, i, NONMOTORFALL, width, height);//�ǻ������������
			}
			if(targetInRegion[i][GREENWAYDROP])//ֽ����ƿ��
			{
				GreenwayDropDetect(pCfgs, i, GREENWAYDROP);////ֽ����ƿ��
			}
			if(targetInRegion[i][ACCIDENTTRAFFIC] && pCfgs->event_targets[i].continue_num > 100)//Ŀ�����¹ʼ��������
			{
				TrafficAccidentDetect(pCfgs, i, ACCIDENTTRAFFIC, targetDisXY, width, height);
			}

		}

	}
	//�ж��Ƿ������������
	/*bool detDrop = FALSE;
	for(i = 0; i < pCfgs->EventDetectCfg.uEventRegionNum; i++)
	{
		if(pCfgs->EventDetectCfg.EventRegion[i].eventType == DROP)
		{
			detDrop = TRUE;
			break;
		}
	}
	if(detDrop)//���м��
	{
		DropDetect(pCfgs, pParams, DROP, width, height);
	}*/
	//printf("event person = %d, person fall = %d, nonmotor = %d,greendrop = %d\n",pCfgs->uPersonNum,pCfgs->uPersonFallNum,pCfgs->uNonMotorFallNum,pCfgs->uGreenwayDropNum);
}
void EventDetectProc(ALGCFGS *pCfgs, ALGPARAMS *pParams, int width, int height)
{
	int i = 0, j = 0;
	int eventID = 0, newID = 0, new_event_flag = 0;

	//���ý�ͨ�¼���ģͼ��
	if(pCfgs->bMaskEventImage == FALSE)
	{
		MaskEventImage(pCfgs, pParams, width, height);
		pCfgs->bMaskEventImage = TRUE;
	}
	
	//�Խ�ͨ�¼����з���
	TrafficEventAnalysis(pCfgs, pParams, width, height);

	//�Ե�·�¼����з���
	TrafficRoadAnalysis(pCfgs, pParams, width, height);

	memset((void *)&pCfgs->ResultMsg.uResultInfo.eventData, 0, sizeof(pCfgs->ResultMsg.uResultInfo.eventData));//��ʼ��
	//����ͣ���¼�
	if(pCfgs->uIllegalParkNum)//��ͣ���¼�
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool IllegalParkEvent = FALSE;
		for(i = 0; i < pCfgs->uIllegalParkNum; i++)
		{
			if(pCfgs->uIllegalParkID == pCfgs->IllegalParkBox[i].uEventID)
			{
				IllegalParkEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->IllegalParkBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ�ͣ���¼�
		if(IllegalParkEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uIllegalParkTime) > (pCfgs->EventDetectCfg.ReportInterval[STOP_INVALID] * 60) || pCfgs->uIllegalParkID == 0)//�����µ�ID,����ǰ��ͣ���¼����7����֮��
			{
				eventID = newID;
				IllegalParkEvent = TRUE;
				pCfgs->uIllegalParkID = pCfgs->IllegalParkBox[eventID].uEventID;//��������
				pCfgs->uIllegalParkTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
				printf("STOP_INVALID new flag\n");
			}
		}
		else//֮ǰ�Ѿ����ֵ�ͣ���¼�
		{
			new_event_flag = 0;
		}
		//ֻ��һ��ͣ���¼�
		if(IllegalParkEvent)
		{
			int IsIllegalPark = TRUE;
			//�ж�ͣ���Ƿ���ӵ��������
			for(i = 0; i < pCfgs->LaneAmount; i++)
			{
				if(pCfgs->bCongestion[i])
				{
					int sum = 0;
					int num = 0;
					for(int k = pCfgs->IllegalParkBox[eventID].EventBox[0].x; k <= pCfgs->IllegalParkBox[eventID].EventBox[1].x; k++)
					{
						for(int l = pCfgs->IllegalParkBox[eventID].EventBox[0].y; l <= pCfgs->IllegalParkBox[eventID].EventBox[3].y; l++)
						{
							CPoint pt;
							pt.x = k;
							pt.y = l;
							if(isPointInRect(pt, pCfgs->CongestionBox[i].EventBox[3], pCfgs->CongestionBox[i].EventBox[0], pCfgs->CongestionBox[i].EventBox[1], pCfgs->CongestionBox[i].EventBox[2]))
								sum++;
							num++;
						}
					}
					//��ӵ��������
					if((float)sum / (float)(num) > 0.8)
						IsIllegalPark = FALSE;
				}
			}
			if(IsIllegalPark == TRUE)
			{
				if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
					pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->IllegalParkBox[eventID].uRegionID;//�¼�����ID
					if(new_event_flag == 1)
					{
						pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
					}
					memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, pCfgs->IllegalParkBox[eventID].EventBox, 4 * sizeof(CPoint));
					pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = STOP_INVALID;
					pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
				}
			}
			else
			{
				printf("STOP_INVALID in congestion region\n");
			}
		}
	}

	//���������¼�
	/*for(i = 0; i < pCfgs->uOppositeDirDriveNum; i++)
	{
		if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
		{
			pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = pCfgs->OppositeDirDriveBox[i].uNewEventFlag;
			if(pCfgs->OppositeDirDriveBox[i].uNewEventFlag == 1)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
			}
			memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, pCfgs->OppositeDirDriveBox[i].EventBox, 4 * sizeof(CPoint));
			pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = REVERSE_DRIVE;
			pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
		}
	}*/
	if(pCfgs->uOppositeDirDriveNum)//�������¼�
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool OppositeDirDriveEvent = FALSE;
		for(i = 0; i < pCfgs->uOppositeDirDriveNum; i++)
		{
			//һֱ���������¼�
			if(pCfgs->uOppositeDirDriveID == pCfgs->OppositeDirDriveBox[i].uEventID)
			{
				OppositeDirDriveEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->OppositeDirDriveBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ������¼�
		if(OppositeDirDriveEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uOppositeDirDriveTime) > (pCfgs->EventDetectCfg.ReportInterval[REVERSE_DRIVE] * 60) || pCfgs->uOppositeDirDriveID == 0)//�����µ�ID,ǰ�����г���3000
			{
				eventID = newID;
				OppositeDirDriveEvent = TRUE;
				pCfgs->uOppositeDirDriveID = pCfgs->OppositeDirDriveBox[eventID].uEventID;//��������
				pCfgs->uOppositeDirDriveTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(OppositeDirDriveEvent == TRUE)
		{
			//ֻ��һ�������¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->OppositeDirDriveBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->OppositeDirDriveBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = REVERSE_DRIVE;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}

	//����ʻ���¼�
	/*for(i = 0; i < pCfgs->uOffLaneNum; i++)
	{
		if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
		{
			pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = pCfgs->OffLaneBox[i].uNewEventFlag;
			pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->OppositeDirDriveBox[eventID].uRegionID;//�¼�����ID
			if(pCfgs->OffLaneBox[i].uNewEventFlag == 1)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
			}
			memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, pCfgs->OffLaneBox[i].EventBox, 4 * sizeof(CPoint));
			pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = DRIVE_AWAY;
            pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
		}
	}*/
	if(pCfgs->uOffLaneNum)//��ʻ���¼�
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool OffLaneEvent = FALSE;
		for(i = 0; i < pCfgs->uOffLaneNum; i++)
		{
			//һֱ����ʻ���¼�
			if(pCfgs->uOffLaneID == pCfgs->OffLaneBox[i].uEventID)
			{
				OffLaneEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->OffLaneBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ�ʻ���¼�
		if(OffLaneEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uOffLaneTime) > (pCfgs->EventDetectCfg.ReportInterval[DRIVE_AWAY] * 60) || pCfgs->uOffLaneID == 0)//�����µ�ID,ǰ��ʻ�볬��3000
			{
				eventID = newID;
				OffLaneEvent = TRUE;
				pCfgs->uOffLaneID = pCfgs->OffLaneBox[eventID].uEventID;//��������
				pCfgs->uOffLaneTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(OffLaneEvent == TRUE)
		{
			//ֻ��һ��ʻ���¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->OffLaneBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->OffLaneBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = DRIVE_AWAY;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}
	//���������¼�
	if(pCfgs->uNoPersonAllowNum)//�������¼�
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool PersonEvent = FALSE;
		for(i = 0; i < pCfgs->uNoPersonAllowNum; i++)
		{
			//һֱ���ڵ������¼�
			if(pCfgs->uCurrentPersonID == pCfgs->NoPersonAllowBox[i].uEventID)
			{
				PersonEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->NoPersonAllowBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ������¼�
		if(PersonEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uPersonEventTime) > MAX(5, pCfgs->EventDetectCfg.ReportInterval[NO_PEDESTRIANTION] * 60) || pCfgs->uCurrentPersonID == 0)//�����µ�ID,����ǰ�������¼��������5s
			{
				eventID = newID;
				PersonEvent = TRUE;
				pCfgs->uCurrentPersonID = pCfgs->NoPersonAllowBox[eventID].uEventID;//��������
				pCfgs->uPersonEventTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(PersonEvent == TRUE)
		{
			//ֻ��һ�������¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->NoPersonAllowBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
					printf("person new flag, time = %f,%f,%d\n", pCfgs->currTime,pCfgs->uPersonEventTime,pCfgs->EventDetectCfg.ReportInterval[NO_PEDESTRIANTION]);
				}
				memcpy(detectRegion.detRegion, pCfgs->NoPersonAllowBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = NO_PEDESTRIANTION;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}

	//�������˵���
	if(pCfgs->uPersonFallNum)//�����˵���
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool PersonFallEvent = FALSE;
		for(i = 0; i < pCfgs->uPersonFallNum; i++)
		{
			//һֱ���ڵ����˵���
			if(pCfgs->uCurrentPersonFallID == pCfgs->PersonFallBox[i].uEventID)
			{
				PersonFallEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->PersonFallBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ����˵����¼�
		if(PersonFallEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uPersonFallEventTime) > MAX(5, pCfgs->EventDetectCfg.ReportInterval[PERSONFALL] * 60) || pCfgs->uCurrentPersonFallID == 0)//�����µ�ID,ǰ�����˵�������5s
			{
				eventID = newID;
				PersonFallEvent = TRUE;
				pCfgs->uCurrentPersonFallID = pCfgs->PersonFallBox[eventID].uEventID;//��������
				pCfgs->uPersonFallEventTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(PersonFallEvent == TRUE)
		{
			//ֻ��һ�����˵����¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->PersonFallBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				//printf("person fall new flag = [%d,%d]\n",pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID,pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag);
				memcpy(detectRegion.detRegion, pCfgs->PersonFallBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = PERSONFALL;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}

	//�����ǻ������¼�
	if(pCfgs->uNonMotorAllowNum)//�зǻ������¼�
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool NonMotorEvent = FALSE;
		for(i = 0; i < pCfgs->uNonMotorAllowNum; i++)
		{
			//һֱ���ڵķǻ������¼�
			if(pCfgs->uCurrentNonMotorID == pCfgs->NonMotorAllowBox[i].uEventID)
			{
				NonMotorEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->NonMotorAllowBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵķǻ������¼�
		if(NonMotorEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uNonMotorEventTime) > MAX(5, pCfgs->EventDetectCfg.ReportInterval[NONMOTOR] * 60) || pCfgs->uCurrentNonMotorID == 0)//�����µ�ID,����ǰ��ǻ������¼��������5s
			{
				eventID = newID;
				NonMotorEvent = TRUE;
				pCfgs->uCurrentNonMotorID = pCfgs->NonMotorAllowBox[eventID].uEventID;//��������
				pCfgs->uNonMotorEventTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(NonMotorEvent == TRUE)
		{
			//ֻ��һ���ǻ�����
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->NonMotorAllowBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->NonMotorAllowBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = NONMOTOR;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}

	//�����ǻ����������¼�
	if(pCfgs->uNonMotorFallNum)//�зǻ���������
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool NonMotorFallEvent = FALSE;
		for(i = 0; i < pCfgs->uNonMotorFallNum; i++)
		{
			//һֱ���ڵķǻ���������
			if(pCfgs->uCurrentNonMotorFallID == pCfgs->NonMotorFallBox[i].uEventID)
			{
				NonMotorFallEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->NonMotorFallBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵķǻ����������¼�
		if(NonMotorFallEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uNonMotorFallEventTime) > MAX(5, pCfgs->EventDetectCfg.ReportInterval[NONMOTORFALL] * 60) || pCfgs->uCurrentNonMotorFallID == 0)//�����µ�ID,ǰ��ǻ�������������5s
			{
				NonMotorFallEvent = TRUE;
				pCfgs->uCurrentNonMotorFallID = pCfgs->NonMotorFallBox[eventID].uEventID;//��������
				pCfgs->uNonMotorFallEventTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(NonMotorFallEvent == TRUE)
		{
			//ֻ��һ���ǻ����������¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->NonMotorFallBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->NonMotorFallBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = NONMOTORFALL;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}

	//�����̵��������¼�
	if(pCfgs->uGreenwayDropNum)//���̵�������
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool GreenwayDropEvent = FALSE;
		for(i = 0; i < pCfgs->uGreenwayDropNum; i++)
		{
			//һֱ�����̵�������
			if(pCfgs->uCurrentGreenwayDropID == pCfgs->GreenwayDropBox[i].uEventID)
			{
				GreenwayDropEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->GreenwayDropBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ��̵�������
		if(GreenwayDropEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uGreenwayDropEventTime) > (pCfgs->EventDetectCfg.ReportInterval[GREENWAYDROP] * 60) || pCfgs->uCurrentGreenwayDropID == 0)//�����µ�ID,ǰ���̵������ﳬ��3000
			{
				GreenwayDropEvent = TRUE;
				pCfgs->uCurrentGreenwayDropID = pCfgs->GreenwayDropBox[eventID].uEventID;//��������
				pCfgs->uGreenwayDropEventTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(GreenwayDropEvent == TRUE)
		{
			//ֻ��һ���̵��������¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->GreenwayDropBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->GreenwayDropBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = GREENWAYDROP;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}

	//������ͨ�¹��¼�
	if(pCfgs->uTrafficAccidentNum)//�н�ͨ�¹�
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool TrafficAccidentEvent = FALSE;
		for(i = 0; i < pCfgs->uTrafficAccidentNum; i++)
		{
			//һֱ�н�ͨ�¹�
			if(pCfgs->uTrafficAccidentID == pCfgs->TrafficAccidentBox[i].uEventID)
			{
				TrafficAccidentEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->TrafficAccidentBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵĽ�ͨ�¹�
		if(TrafficAccidentEvent == FALSE)
		{
			if((pCfgs->currTime - pCfgs->uTrafficAccidentTime) > (pCfgs->EventDetectCfg.ReportInterval[ACCIDENTTRAFFIC] * 60) || pCfgs->uTrafficAccidentID == 0)//�����µ�ID,ǰ��ͨ�¹ʳ���3000
			{
				eventID = newID;
				TrafficAccidentEvent = TRUE;
				pCfgs->uTrafficAccidentID = pCfgs->TrafficAccidentBox[eventID].uEventID;//��������
				pCfgs->uTrafficAccidentTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
				printf("ACCIDENTTRAFFIC new flag\n");
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(TrafficAccidentEvent == TRUE)
		{
			//ֻ��һ����ͨ�¹��¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->TrafficAccidentBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->TrafficAccidentBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = ACCIDENTTRAFFIC;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}
	printf("new event = %d, event_num = %d\n", pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag, pCfgs->ResultMsg.uResultInfo.eventData.uEventNum);
	//////////////////////////////////////////////////////������·�¼�
	//��·��ˮ
	if(pCfgs->uRoadWaterNum)//�е�·��ˮ
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool RoadWaterEvent = FALSE;
		for(i = 0; i < pCfgs->uRoadWaterNum; i++)
		{
			//һֱ���ڻ�ˮ�¼�
			if(pCfgs->uRoadWaterID == pCfgs->RoadWaterBox[i].uEventID)
			{
				RoadWaterEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->RoadWaterBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵĻ�ˮ�¼�
		if(RoadWaterEvent == FALSE)
		{
			{
				eventID = newID;
				RoadWaterEvent = TRUE;
				pCfgs->uRoadWaterID = pCfgs->RoadWaterBox[eventID].uEventID;//��������
				pCfgs->uRoadWaterTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(RoadWaterEvent == TRUE)
		{
			//ֻ��һ����ˮ�¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->RoadWaterBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->RoadWaterBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = ROADWATER;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}
	//��·����
	if(pCfgs->uRoadHollowNum)//�е�·����
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool RoadHollowrEvent = FALSE;
		for(i = 0; i < pCfgs->uRoadHollowNum; i++)
		{
			//һֱ���ڿ����¼�
			if(pCfgs->uRoadHollowID == pCfgs->RoadHollowBox[i].uEventID)
			{
				RoadHollowrEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->RoadHollowBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵĿ����¼�
		if(RoadHollowrEvent == FALSE)
		{
			{
				eventID = newID;
				RoadHollowrEvent = TRUE;
				pCfgs->uRoadHollowID = pCfgs->RoadHollowBox[eventID].uEventID;//��������
				pCfgs->uRoadHollowTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(RoadHollowrEvent == TRUE)
		{
			//ֻ��һ�������¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->RoadHollowBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->RoadHollowBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = ROADHOLLOW;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}
	//��·����
	if(pCfgs->uRoadDamageNum)//�е�·����
	{
		eventID = 0, newID = 0;
		new_event_flag = 0;
		bool RoadDamagerEvent = FALSE;
		for(i = 0; i < pCfgs->uRoadDamageNum; i++)
		{
			//һֱ���������¼�
			if(pCfgs->uRoadDamageID == pCfgs->RoadDamageBox[i].uEventID)
			{
				RoadDamagerEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->RoadDamageBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ������¼�
		if(RoadDamagerEvent == FALSE)
		{
			{
				eventID = newID;
				RoadDamagerEvent = TRUE;
				pCfgs->uRoadDamageID = pCfgs->RoadDamageBox[eventID].uEventID;//��������
				pCfgs->uRoadDamageTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(RoadDamagerEvent == TRUE)
		{
			//ֻ��һ�������¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->RoadDamageBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->RoadDamageBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = ROADDAMAGE;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}
	//��·�ѷ�
	if(pCfgs->uRoadCrackNum)//�е�·�ѷ�
	{
		eventID = 0, eventID = 0;
		new_event_flag = 0;
		bool RoadCrackEvent = FALSE;
		for(i = 0; i < pCfgs->uRoadCrackNum; i++)
		{
			//һֱ�����ѷ��¼�
			if(pCfgs->uRoadCrackID == pCfgs->RoadCrackBox[i].uEventID)
			{
				RoadCrackEvent = TRUE;
				eventID = i;
			}
			if(pCfgs->RoadCrackBox[i].uNewEventFlag == 1)
			{
				new_event_flag = 1;
				newID = i;
			}
		}
		//�³��ֵ��ѷ��¼�
		if(RoadCrackEvent == FALSE)
		{
			{
				eventID = newID;
				RoadCrackEvent = TRUE;
				pCfgs->uRoadCrackID = pCfgs->RoadCrackBox[eventID].uEventID;//��������
				pCfgs->uRoadCrackTime = pCfgs->currTime;
				//new_event_flag = 1;//���¼�
			}
		}
		else
		{
			new_event_flag = 0;
		}
		if(RoadCrackEvent == TRUE)
		{
			//ֻ��һ���ѷ��¼�
			DETECTREGION detectRegion;
			if(pCfgs->ResultMsg.uResultInfo.eventData.uEventNum < MAX_EVENT_NUM)
			{
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventID = new_event_flag;
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uRegionID = pCfgs->RoadCrackBox[eventID].uRegionID;//�¼�����ID
				if(new_event_flag == 1)
				{
					pCfgs->ResultMsg.uResultInfo.eventData.uNewEventFlag = 1;
				}
				memcpy(detectRegion.detRegion, pCfgs->RoadCrackBox[eventID].EventBox, 4 * sizeof(CPoint));
				memcpy(pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].EventBox, detectRegion.detRegion, 4 * sizeof(CPoint));
				pCfgs->ResultMsg.uResultInfo.eventData.EventBox[pCfgs->ResultMsg.uResultInfo.eventData.uEventNum].uEventType = ROADCRACK;
				pCfgs->ResultMsg.uResultInfo.eventData.uEventNum++;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////�Ե�·�¼����д���
//����ͬ��������Ŀ���кϲ�
void post_process_box_road(ALGCFGS* pCfgs, Uint16 ratio_threshold)
{
	Uint16 i = 0, j = 0, k = 0, l = 0;
	Uint16 dis_x = 0, dis_y = 0;
	CRect r0, r1, r2;
	for(i = 0; i < pCfgs->classes; i++)//��������
	{
		if(pCfgs->detClasses[i].classes_num == 0)
			continue;
		if(strcmp(pCfgs->detClasses[i].names, "js") != 0 && strcmp(pCfgs->detClasses[i].names, "kw") != 0 && strcmp(pCfgs->detClasses[i].names, "ps") != 0 && strcmp(pCfgs->detClasses[i].names, "lf") != 0)//������𲻽��д���
			continue;
		for(j = 0; j < pCfgs->detClasses[i].classes_num; j++)
		{
			r0 = pCfgs->detClasses[i].box[j];
			for(k = j + 1; k < pCfgs->detClasses[i].classes_num; k++)
			{
				//������֮�����
				r1 = pCfgs->detClasses[i].box[k];
				dis_x = MIN(r0.x + r0.width, r1.x + r1.width) - MAX(r0.x, r1.x);
				dis_y = MIN(r0.y + r0.height, r1.y + r1.height) - MAX(r0.y, r1.y);
				if(dis_x  > -ratio_threshold && dis_y > -ratio_threshold)//����С����ֵ���кϲ�
				{
					r2.x = (r0.x < r1.x)? r0.x : r1.x;//ȡ��Сֵ
					r2.y = (r0.y < r1.y)? r0.y : r1.y;
					r2.width = ((r0.x + r0.width) > (r1.x + r1.width))? (r0.x + r0.width - r2.x) : (r1.x + r1.width - r2.x);
					r2.height = ((r0.y + r0.height) > (r1.y + r1.height))? (r0.y + r0.height - r2.y) : (r1.y + r1.height - r2.y);
					r0 = r2;
					for( l = k + 1; l < pCfgs->detClasses[i].classes_num; l++)
					{
						pCfgs->detClasses[i].box[l - 1] = pCfgs->detClasses[i].box[l];
						pCfgs->detClasses[i].prob[l - 1] = pCfgs->detClasses[i].prob[l];
					}
					pCfgs->detClasses[i].classes_num = pCfgs->detClasses[i].classes_num - 1;
					k--;
				}
			}
			pCfgs->detClasses[i].box[j] = r0;
		}
	}
}
int match_object_rect2(CTarget* targets, int targets_size, CDetBox* detClasses, int class_id, int* match_object, int* match_rect, int thresh)
{

	if(targets_size < 1 || detClasses[class_id].classes_num < 1)//û��Ŀ�����򣬷���
	{
		return -1;
	}
	int i = 0, j = 0;
	int dis_min = 1e+9, idx_max = 0;
	CRect r1,r2;
	//��ʼ��ƥ��ֵΪ-1
	memset(match_object, -1, MAX_ROAD_TARGET_NUM * sizeof(int));
	memset(match_rect, -1, MAX_ROAD_TARGET_NUM * sizeof(int));

	for(j = 0; j < detClasses[class_id].classes_num; j++)//ƥ��Ŀ��
	{
		r1 = detClasses[class_id].box[j];
		match_object[j] = -1;
		idx_max = -1;
		dis_min = 1e+9;
		for(i = 0; i < targets_size; i++)
		{
			r2 = targets[i].box;
			if(class_id == targets[i].class_id)
			{
				//��������������ľ���
				int dis = 0;
				dis = (r1.x + r1.width /2 - r2.x - r2.width / 2) * (r1.x + r1.width /2 - r2.x - r2.width / 2) + (r1.y + r1.height / 2 - r2.y - r2.height / 2) * (r1.y + r1.height / 2 - r2.y - r2.height / 2);
				dis =sqrt(dis);
				if(dis < dis_min)//�õ���С����
				{
					dis_min = dis;
					idx_max = i;
				}
			}
		}
		if(dis_min < thresh)//��С����С����ֵ
		{
			match_object[j] = idx_max;
		}
	}

	for(j = 0; j < targets_size; j++)//ƥ���
	{
		match_rect[j] = -1;
		idx_max = -1;
		dis_min = 1e+9;
		r1 = targets[j].box;
		for(i = 0; i < detClasses[class_id].classes_num; i++)
		{
			if(class_id == targets[j].class_id)
			{
				r2 = detClasses[class_id].box[i];
				//��������������ľ���
				int dis = 0;
				dis = (r1.x + r1.width /2 - r2.x - r2.width / 2) * (r1.x + r1.width /2 - r2.x - r2.width / 2) + (r1.y + r1.height / 2 - r2.y - r2.height / 2) * (r1.y + r1.height / 2 - r2.y - r2.height / 2);
				dis =sqrt(dis);
				if(dis < dis_min)//�õ���С����
				{
					dis_min = dis;
					idx_max = i;
				}
			}
		}
		if(dis_min < thresh)//��С����С����ֵ
		{
			match_rect[j] = idx_max;
		}
	}

	return 1;

}
void RoadEventDetect(ALGCFGS *pCfgs, int target_idx, int event_idx)//��·��ˮ���
{
	CPoint pt[4];
	//��Ŀ��û�б����Ϊ��·�¼�
	if(pCfgs->road_event_targets[target_idx].cal_event[event_idx] == FALSE)
	{
		if(pCfgs->road_event_targets[target_idx].detected)
		{
			pCfgs->road_event_targets[target_idx].event_continue_num[event_idx]++;
		}
		if(pCfgs->road_event_targets[target_idx].event_continue_num[event_idx] > 5 && pCfgs->road_event_targets[target_idx].detected)//Ŀ���ڼ�������ڣ����ﵽһ��֡��
		{
			pCfgs->road_event_targets[target_idx].cal_event[event_idx] = TRUE;
			pCfgs->road_event_targets[target_idx].event_flag[event_idx] = 1;
		}
	}
	if(pCfgs->road_event_targets[target_idx].event_flag[event_idx] > 0)//�Ѿ���Ǵ����¼������¼�һֱ����ʱ�����¼�
	{
		pt[0].x = pCfgs->road_event_targets[target_idx].box.x;
		pt[0].y = pCfgs->road_event_targets[target_idx].box.y;
		pt[1].x = pCfgs->road_event_targets[target_idx].box.x + pCfgs->road_event_targets[target_idx].box.width;
		pt[1].y = pCfgs->road_event_targets[target_idx].box.y;
		pt[2].x = pCfgs->road_event_targets[target_idx].box.x + pCfgs->road_event_targets[target_idx].box.width;
		pt[2].y = pCfgs->road_event_targets[target_idx].box.y + pCfgs->road_event_targets[target_idx].box.height;
		pt[3].x = pCfgs->road_event_targets[target_idx].box.x;
		pt[3].y = pCfgs->road_event_targets[target_idx].box.y + pCfgs->road_event_targets[target_idx].box.height;
		if(event_idx == ROADWATER)//�����·��ˮ��
		{
			if(pCfgs->uRoadWaterNum < MAX_EVENT_NUM)
			{
				printf("road water event\n");
				//�ж��Ƿ����³����¼�
				if(pCfgs->road_event_targets[target_idx].sign_event[event_idx] == 0)
				{
					pCfgs->road_event_targets[target_idx].sign_event[event_idx] = 1;
					pCfgs->RoadWaterBox[pCfgs->uRoadWaterNum].uNewEventFlag = 1;
				}
				else
					pCfgs->RoadWaterBox[pCfgs->uRoadWaterNum].uNewEventFlag = 0;
				pCfgs->RoadWaterBox[pCfgs->uRoadWaterNum].uEventID = pCfgs->road_event_targets[target_idx].target_id;
				memcpy(pCfgs->RoadWaterBox[pCfgs->uRoadWaterNum].EventBox, pt, 4 * sizeof(CPoint));
				pCfgs->RoadWaterBox[pCfgs->uRoadWaterNum].uEventType = ROADWATER;
				pCfgs->uRoadWaterNum++;
			}

		}
		if(event_idx == ROADHOLLOW)//�����·���ݿ�
		{
			if(pCfgs->uRoadHollowNum < MAX_EVENT_NUM)
			{
				printf("road hollow event\n");
				//�ж��Ƿ����³����¼�
				if(pCfgs->road_event_targets[target_idx].sign_event[event_idx] == 0)
				{
					pCfgs->road_event_targets[target_idx].sign_event[event_idx] = 1;
					pCfgs->RoadHollowBox[pCfgs->uRoadHollowNum].uNewEventFlag = 1;
				}
				else
					pCfgs->RoadHollowBox[pCfgs->uRoadHollowNum].uNewEventFlag = 0;
				pCfgs->RoadHollowBox[pCfgs->uRoadHollowNum].uEventID = pCfgs->road_event_targets[target_idx].target_id;
				memcpy(pCfgs->RoadHollowBox[pCfgs->uRoadHollowNum].EventBox, pt, 4 * sizeof(CPoint));
				pCfgs->RoadHollowBox[pCfgs->uRoadHollowNum].uEventType = ROADHOLLOW;
				pCfgs->uRoadHollowNum++;
			}

		}
		if(event_idx == ROADDAMAGE)//�����·�����
		{
			if(pCfgs->uRoadDamageNum < MAX_EVENT_NUM)
			{
				printf("road damage event\n");
				//�ж��Ƿ����³����¼�
				if(pCfgs->road_event_targets[target_idx].sign_event[event_idx] == 0)
				{
					pCfgs->road_event_targets[target_idx].sign_event[event_idx] = 1;
					pCfgs->RoadDamageBox[pCfgs->uRoadDamageNum].uNewEventFlag = 1;
				}
				else
					pCfgs->RoadDamageBox[pCfgs->uRoadDamageNum].uNewEventFlag = 0;
				pCfgs->RoadDamageBox[pCfgs->uRoadDamageNum].uEventID = pCfgs->road_event_targets[target_idx].target_id;
				memcpy(pCfgs->RoadDamageBox[pCfgs->uRoadDamageNum].EventBox, pt, 4 * sizeof(CPoint));
				pCfgs->RoadDamageBox[pCfgs->uRoadDamageNum].uEventType = ROADDAMAGE;
				pCfgs->uRoadDamageNum++;
			}

		}
		if(event_idx == ROADCRACK)//�����·�ѷ��
		{
			if(pCfgs->uRoadCrackNum < MAX_EVENT_NUM)
			{
				printf("road crack event\n");
				//�ж��Ƿ����³����¼�
				if(pCfgs->road_event_targets[target_idx].sign_event[event_idx] == 0)
				{
					pCfgs->road_event_targets[target_idx].sign_event[event_idx] = 1;
					pCfgs->RoadCrackBox[pCfgs->uRoadCrackNum].uNewEventFlag = 1;
				}
				else
					pCfgs->RoadCrackBox[pCfgs->uRoadCrackNum].uNewEventFlag = 0;
				pCfgs->RoadCrackBox[pCfgs->uRoadCrackNum].uEventID = pCfgs->road_event_targets[target_idx].target_id;
				memcpy(pCfgs->RoadCrackBox[pCfgs->uRoadCrackNum].EventBox, pt, 4 * sizeof(CPoint));
				pCfgs->RoadCrackBox[pCfgs->uRoadCrackNum].uEventType = ROADCRACK;
				pCfgs->uRoadCrackNum++;
			}

		}


	}
}
//����·�¼�ʱ�������������ͨ�¼�������Ŀ����event_targets,�������⣬������event_targets
void TrafficRoadAnalysis(ALGCFGS *pCfgs, ALGPARAMS *pParams, int width, int height)
{	
	int i = 0, j = 0, k = 0, l = 0;
	int left = 0, right = 0, top = 0, bottom = 0;
	int match_object[MAX_ROAD_TARGET_NUM];
	int match_rect[MAX_ROAD_TARGET_NUM];
	int match_success = -1;
	bool isInRegion = FALSE;
	CRect targetRect[MAX_ROAD_TARGET_NUM];
	int targetDisXY[MAX_ROAD_TARGET_NUM][2]={ 0 };
	//��ʼ����·�¼������
	pCfgs->uRoadWaterNum = 0;//��·��ˮ
	memset(pCfgs->RoadWaterBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uRoadHollowNum = 0;//��·����
	memset(pCfgs->RoadHollowBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uRoadDamageNum = 0;//��·����
	memset(pCfgs->RoadDamageBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	pCfgs->uRoadCrackNum = 0;//��·�ѷ�
	memset(pCfgs->RoadCrackBox, 0, MAX_EVENT_NUM * sizeof(EVENTBOX));
	//����Ŀ��Ϊδ��⵽
	for( i = 0; i < pCfgs->road_event_targets_size; i++)
	{
		pCfgs->road_event_targets[i].detected = FALSE;
	}
	//�Լ�����д�������ͬ�����ý��ĺϲ�
	post_process_box_road(pCfgs, 50);
	//��������
	for( i = 0; i < pCfgs->classes; i++)
	{
		//������Ҫ�����
		if(strcmp(pCfgs->detClasses[i].names, "js") != 0 && strcmp(pCfgs->detClasses[i].names, "kw") != 0 && strcmp(pCfgs->detClasses[i].names, "ps") != 0 && strcmp(pCfgs->detClasses[i].names, "lf") != 0)
			continue;
		if(pCfgs->detClasses[i].classes_num)
		{
			match_object_rect2(pCfgs->road_event_targets, pCfgs->road_event_targets_size, pCfgs->detClasses, i, match_object, match_rect, 300);

			for( j = 0; j < pCfgs->detClasses[i].classes_num; j++)
			{
				//�����������ڼ��������
				left = MAX(0, pCfgs->detClasses[i].box[j].x * pCfgs->m_iWidth / width);
				right = MIN((pCfgs->detClasses[i].box[j].x + pCfgs->detClasses[i].box[j].width) * pCfgs->m_iWidth / width, pCfgs->m_iWidth - 1);
				top = MAX(0, pCfgs->detClasses[i].box[j].y * pCfgs->m_iHeight / height);
				bottom = MIN((pCfgs->detClasses[i].box[j].y + pCfgs->detClasses[i].box[j].height) * pCfgs->m_iHeight / height, pCfgs->m_iHeight - 1);
				CRect rct;
				rct.x = left;
				rct.y = top;
				rct.width = right - left + 1;
				rct.height = bottom - top + 1;
				isInRegion = FALSE;//��ʼ��Ϊδ���¼��������
				int inRegion[MAX_EVENT_TYPE] = { 0 };
				if(strcmp(pCfgs->detClasses[i].names, "js") == 0)//��·����
				{
					inRegion[ROADWATER] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, ROADWATER);
				}
				if(strcmp(pCfgs->detClasses[i].names, "kw") == 0)//��·����
				{
					inRegion[ROADHOLLOW] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, ROADHOLLOW);
				}
				if(strcmp(pCfgs->detClasses[i].names, "ps") == 0)//��·����
				{
					inRegion[ROADDAMAGE] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, ROADDAMAGE);
				}
				if(strcmp(pCfgs->detClasses[i].names, "lf") == 0)//��·�ѷ�
				{
					inRegion[ROADCRACK] = RectInRegion1(pParams->MaskEventImage, pCfgs->m_iWidth, pCfgs->m_iHeight, rct, ROADCRACK);
				}
				for(k = 0; k < MAX_EVENT_TYPE; k++)
				{
					if(inRegion[k])
					{
						isInRegion = TRUE;//���¼��������
						break;
					}
				}
				//if(isInRegion)//���¼���������ڽ��и���
				{
					match_success = -1;
					for( k = 0; k < pCfgs->road_event_targets_size; k++)
					{
						if(match_object[j] == k && match_rect[k] == j)
						{
							match_success = 1;
							break;
						}

					}
					if(match_success > 0)//���ٵ�,���¼���
					{
						pCfgs->road_event_targets[k].box = pCfgs->detClasses[i].box[j];
						pCfgs->road_event_targets[k].prob = pCfgs->detClasses[i].prob[j];
						pCfgs->road_event_targets[k].class_id = pCfgs->detClasses[i].class_id;
						strcpy(pCfgs->road_event_targets[k].names, pCfgs->detClasses[i].names);
						pCfgs->road_event_targets[k].detected = TRUE;
					}
					else if(isInRegion && match_success < 0 && pCfgs->road_event_targets_size < MAX_ROAD_TARGET_NUM)//�¼������ڣ�δ���ٵ��������µ�Ŀ��
					{	
						CTarget nt; 
						Initialize_target(&nt);
						nt.box = pCfgs->detClasses[i].box[j];
						nt.class_id = pCfgs->detClasses[i].class_id;
						nt.prob = pCfgs->detClasses[i].prob[j];
						nt.detected = TRUE;
						nt.target_id = pCfgs->road_event_target_id++;
						nt.start_time = pCfgs->currTime;//Ŀ�꿪ʼʱ��
						nt.region_idx = pCfgs->EventDetectCfg.EventRegion[0].uRegionID;//��ʼ��0����ID
						if(pCfgs->road_event_target_id > 5000)
							pCfgs->road_event_target_id = 1;
						strcpy(nt.names, pCfgs->detClasses[i].names);
						memset(nt.event_continue_num, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ���¼�����֡��
						memset(nt.event_flag, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ���¼����
						memset(nt.cal_event, FALSE, MAX_EVENT_TYPE * sizeof(bool));//��ʼ�������¼�Ϊδ����
						memset(nt.sign_event, 0, MAX_EVENT_TYPE * sizeof(int));//��ʼ��Ϊδ��ǵ��¼�
						memset(nt.statistic, -1, 5 * sizeof(int));//����ͳ���˶����
						pCfgs->road_event_targets[pCfgs->road_event_targets_size] = nt;
						pCfgs->road_event_targets_size++;
					}
				}
			}
		}
	}
	//����Ŀ��
	for(i = 0; i < pCfgs->road_event_targets_size; i++)
	{

		//�켣��С��3000��ֱ�ӱ��棬����3000��ȥ���ɵ�
		if(pCfgs->road_event_targets[i].trajectory_num < 3000)
		{

			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num].x = pCfgs->road_event_targets[i].box.x + pCfgs->road_event_targets[i].box.width / 2;
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num].y = pCfgs->road_event_targets[i].box.y + pCfgs->road_event_targets[i].box.height / 2;
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num].width = pCfgs->road_event_targets[i].box.width;
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num].height = pCfgs->road_event_targets[i].box.height;
			pCfgs->road_event_targets[i].trajectory_num++;
		}
		else
		{
			for(j = 0; j < pCfgs->road_event_targets[i].trajectory_num - 1; j++)
			{
				pCfgs->road_event_targets[i].trajectory[j] = pCfgs->road_event_targets[i].trajectory[j + 1];
			}
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num - 1].x = pCfgs->road_event_targets[i].box.x + pCfgs->road_event_targets[i].box.width / 2;
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num - 1].y = pCfgs->road_event_targets[i].box.y + pCfgs->road_event_targets[i].box.height / 2;
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num - 1].width = pCfgs->road_event_targets[i].box.width;
			pCfgs->road_event_targets[i].trajectory[pCfgs->road_event_targets[i].trajectory_num - 1].height = pCfgs->road_event_targets[i].box.height;
		}
		//��⵽���������ٶ�
		if(pCfgs->road_event_targets[i].detected)
		{
			pCfgs->road_event_targets[i].lost_detected = 0;
		}
		else//δ��⵽
		{
			pCfgs->road_event_targets[i].lost_detected++;
			pCfgs->road_event_targets[i].box.x += pCfgs->road_event_targets[i].vx;
			pCfgs->road_event_targets[i].box.y += pCfgs->road_event_targets[i].vy;
		}
		//��Ŀ������Ƶ����ʱ��̫����ʱ��û�м�⵽���뿪ͼ��ɾ��Ŀ��
		if(pCfgs->road_event_targets[i].continue_num > 5000 || pCfgs->road_event_targets[i].lost_detected > 5 ||((pCfgs->road_event_targets[i].box.x < 5 || pCfgs->road_event_targets[i].box.y < 5 || (pCfgs->road_event_targets[i].box.x + pCfgs->road_event_targets[i].box.width) > (width - 5) || (pCfgs->road_event_targets[i].box.y + pCfgs->road_event_targets[i].box.height) > (height - 5))&& pCfgs->road_event_targets[i].lost_detected > 2))
		{
			DeleteTarget(&pCfgs->road_event_targets_size, &i, pCfgs->road_event_targets);
			continue;

		}
		//�ж�Ŀ�������ĸ��¼�����
		for(k = 0; k < pCfgs->EventDetectCfg.uEventRegionNum; k++)
		{
			int inRegionRatio = RectInRegion(pParams->MaskEventIDImage, pCfgs, width, height, pCfgs->road_event_targets[i].box, pCfgs->EventDetectCfg.EventRegion[k].uRegionID);
			if(inRegionRatio > 10)
			{
				pCfgs->road_event_targets[i].region_idx = pCfgs->EventDetectCfg.EventRegion[k].uRegionID;
				break;
			}
		}
		//������·�¼�
		if(strcmp(pCfgs->road_event_targets[i].names, "js") == 0)//��·����
		{
			RoadEventDetect(pCfgs, i, ROADWATER);
		}
		if(strcmp(pCfgs->road_event_targets[i].names, "kw") == 0)//��·����
		{
			RoadEventDetect(pCfgs, i, ROADHOLLOW);
		}
		if(strcmp(pCfgs->road_event_targets[i].names, "ps") == 0)//��·����
		{
			RoadEventDetect(pCfgs, i, ROADDAMAGE);
		}
		if(strcmp(pCfgs->road_event_targets[i].names, "lf") == 0)//��·�ѷ�
		{
			RoadEventDetect(pCfgs, i, ROADCRACK);
		}
		//��Ŀ��򱣴������������¼����
		targetRect[i] = pCfgs->road_event_targets[i].box;
		//����Ŀ����˶����
		int continue_num = pCfgs->road_event_targets[i].trajectory_num - 100;
		continue_num = (continue_num < 0)? 0 : continue_num;
		int dx = pCfgs->road_event_targets[i].box.x + pCfgs->road_event_targets[i].box.width / 2 - pCfgs->road_event_targets[i].trajectory[continue_num].x;
		int dy = pCfgs->road_event_targets[i].box.y + pCfgs->road_event_targets[i].box.height / 2 - pCfgs->road_event_targets[i].trajectory[continue_num].y;
		dx = (dx < 0)? -dx : dx;
		dy = (dy < 0)? -dy : dy;
		targetDisXY[i][0] = dx;
		targetDisXY[i][1] = dy;
		pCfgs->road_event_targets[i].continue_num++;
	}
}
bool ArithInitEvent(ALGCFGS *pCfgs, mEventInfo	EventDetectCfg, ALGPARAMS *pParams)
{
	bool bInit = FALSE;
#ifdef DETECT_GPU
	//�����������
	if(pCfgs->net_params->net == NULL)
	{
		LoadNetParams(pCfgs->net_params, 0);
	}
#endif
	//��ͨ�¼������ʼ��
	CfgEventRegion(EventDetectCfg, pCfgs, pParams);
	return bInit;
}
Uint16 ArithProcEvent(IplImage* img, ALGCFGS *pCfgs, ALGPARAMS *pParams, char* videoName, char* resultfile)
{
	Int32 i, j;
	int result[6 * MAX_DETECTION_NUM] = { 0 };
	int nboxes = 0;
	unsigned char* pInFrameBuf;
	if(img->width <= 0 || img->height <= 0)
	{
		printf("img cannot be zero!\n");
		return 0;
	}
	//�������ݴ�С
	pCfgs->m_iHeight = (img->height > FULL_ROWS)? FULL_ROWS : img->height;
	pCfgs->m_iWidth = (img->width > FULL_COLS)? FULL_COLS : img->width;

	//gbrתyuv420
	IplImage* imgBGR = cvCreateImage(cvSize(pCfgs->m_iWidth, pCfgs->m_iHeight), IPL_DEPTH_8U, 3);
	IplImage* imgYUV = cvCreateImage(cvSize(pCfgs->m_iWidth, pCfgs->m_iHeight * 3 / 2), IPL_DEPTH_8U, 1);
	if(img->width != pCfgs->m_iWidth || img->height != pCfgs->m_iHeight)
	{
		cvResize(img, imgBGR, CV_INTER_LINEAR);
	}
	else
	{
		cvCopy(img, imgBGR, NULL); 
	}
	cvCvtColor(imgBGR, imgYUV, CV_BGR2YUV_I420);
	pInFrameBuf = (unsigned char *)imgYUV->imageData;
	memcpy((void *)pParams->CurrQueueImage, (void *)pInFrameBuf, pCfgs->m_iWidth * pCfgs->m_iHeight);//�Ҷ�ͼ��
	//printf("process,%d,%d\n",pCfgs->m_iHeight,pCfgs->m_iWidth);
	//���ý�ͨ�¼���ģͼ��
	if(pCfgs->bMaskEventImage == FALSE)
	{
		printf("mask event image\n");
		MaskEventImage(pCfgs, pParams, img->width, img->height);
		pCfgs->bMaskEventImage = TRUE;
	}
	//�õ�ϵͳʱ��
	gettimeofday(&pCfgs->time_end, NULL);
	if(pCfgs->gThisFrameTime == 0)
		pCfgs->currTime = 0;
	else
		pCfgs->currTime += (pCfgs->time_end.tv_sec - pCfgs->time_start.tv_sec) + (pCfgs->time_end.tv_usec - pCfgs->time_start.tv_usec)/1000000.0;
	gettimeofday(&pCfgs->time_start, NULL);
	//���
#ifdef DETECT_GPU
	if(pCfgs->net_params->net)
	{
		nboxes = YoloArithDetect(img, pCfgs->net_params, result);//yolo���
	}
#endif
	printf("frame = %d,nboxes = %d\n",pCfgs->gThisFrameTime, nboxes);
	//���������
	ProcessDetectBox(pCfgs, result, nboxes);
	//�Խ�ͨ�¼����з���
	TrafficEventAnalysis(pCfgs, pParams, img->width, img->height);
	pCfgs->gThisFrameTime++;
	if(pCfgs->EventNum == 0 && pCfgs->EventState == 1)//�¼�������д�¼����ļ���
	{
		//pCfgs->EventEndTime = pCfgs->gThisFrameTime;
		pCfgs->EventState = 0;
		FILE* fp = fopen(resultfile,"a");
		if(fp == NULL)
		{
			perror("fail to read");
			exit (1) ;
		}
		if(fp)
		{
			fputs("{\"file\":\"",fp);
			fprintf(fp,"%s",videoName);
			if(pCfgs->eventType  == NO_PEDESTRIANTION)
				fputs("\",\"catalog\":\"people\",\"event\":true,\"btime\" : ",fp);
			if(pCfgs->eventType  == STOP_INVALID)
				fputs("\",\"catalog\":\"stop\",\"event\":true,\"btime\" : ",fp);
			if(pCfgs->eventType  == REVERSE_DRIVE)
				fputs("\",\"catalog\":\"wrongway\",\"event\":true,\"btime\" : ",fp);

			if(pCfgs->eventType  == DROP)
				fputs("\",\"catalog\":\"drop\",\"event\":true,\"btime\" : ",fp);
			fprintf(fp,"%d",pCfgs->EventBeginTime/ pCfgs->video_fps); 
			fputs(" ,\"etime\" :",fp);
			fprintf(fp,"%d",pCfgs->EventEndTime/ pCfgs->video_fps);
			fputs(" }\n",fp);
		}
		fclose(fp);
	}

	if(pCfgs->EventNum)//�����¼�
	{
		pCfgs->HaveEvent = TRUE;
		for( i = 0; i < pCfgs->EventNum; i++)
		{
			if(pCfgs->EventInfo[i].flag == 1)//д���ݵ��ļ���
			{
				//ɾ����Ӧ���¼�
				for(j = i + 1; j < pCfgs->EventNum; j++)
				{
					pCfgs->EventInfo[j - 1] = pCfgs->EventInfo[j];
				}
				pCfgs->EventNum = pCfgs->EventNum - 1;
			}
		}

	}
	if(imgBGR)
	{
		cvReleaseImage(&imgBGR);
		imgBGR = NULL;
	}
	if(imgYUV)
	{
		cvReleaseImage(&imgYUV);
		imgYUV = NULL;
	}
	return 1;
}
