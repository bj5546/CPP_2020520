#include "m_arith.h"
//�״������Ƶ��������ں�
//objRadar:�״���Ŀ�� 
//objRadarNum �״���Ŀ����
//objInfo ��Ƶ���״��ںϺ��Ŀ��
//objNum ��Ƶ���״��ںϺ��Ŀ����
void associate_radar_and_video(ALGCFGS *pCfgs, mRadarRTObj* objRadar, int objRadarNum, OBJECTINFO* objVehicleInfo, Uint16* objVehicleNum, OBJECTINFO* objPersonInfo, Uint16* objPersonNum)
{
	if(objRadarNum <= 0)//û���״���Ŀ��,�򷵻�
		return;
	int i = 0, j = 0, k = 0;
	int actual_x = 0, actual_y = 0;//�״����Ƶ���������෴
	int offset_x = 0, offset_y = 0; //x��y�������״���������ϵ��ƫ��
	//�״����Ƶ��ƥ���ϣ������״�ļ����Ϣ
	//��Ƶ������Ŀ�꣬�״�û�У�������Ƶ���м���Ŀ����ٶȡ�λ�á�Ŀ�곤�ȿ��
	//�״������Ŀ�꣬��Ƶû�У���Ҫ����ͼ��λ��
	OBJECTINFO* objVehicleVideo = pCfgs->ResultMsg.uResultInfo.udetBox;
	int objVehicleVideoNum = pCfgs->ResultMsg.uResultInfo.udetNum;
	OBJECTINFO* objPersonVideo = pCfgs->ResultMsg.uResultInfo.udetPersonBox;
	int objPersonVideoNum = pCfgs->ResultMsg.uResultInfo.udetPersonNum;
	int objVideoNum = objVehicleVideoNum + objPersonVideoNum;
	Uint16 vehicleNum = 0, personNum = 0;
	int match_video[200] = { -1 };
	int match_radar[200] = { -1 };
	float dis_obj[200][200] = { 1000000 };//���ڼ�����Ƶ���״�Ŀ��֮��ľ���
	for(i = 0; i < 200; i++)
	{
		for(j = 0; j < 200; j++)
			dis_obj[i][j] = 1000000;
	}
	memset(match_video, -1, 200 * sizeof(int));
	memset(match_radar, -1, 200 * sizeof(int));
	printf("detect vehicle obj = %d, person obj =%d\n", objVehicleVideoNum, objPersonVideoNum);
	for(i = 0; i < objRadarNum; i++)
	{
		//���״�Ŀ��ӳ�䵽ͼ��
		mRadarRTObj objR = objRadar[i];
		actual_x = int((objRadar[i].y_Point + offset_x - pCfgs->actual_origin[0]) * pCfgs->mapping_ratio[0]);
		actual_y = int((objRadar[i].x_Point + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		printf("i = %d, radar = [ %f, %f, %f, %f, %f], [%d,%d]\n", i, objRadar[i].x_Point, objRadar[i].y_Point, objRadar[i].Speed_x, objRadar[i].Speed_y, objRadar[i].Obj_Len, actual_x,actual_y);
		if(actual_x >= 2000 || actual_y >= 2000 || actual_x < 0 || actual_y < 0)//����ʵ�����굽ͼ���ӳ�䷶Χ
			continue;
		int img_x = pCfgs->actual_image[actual_y][actual_x][0];
		int img_y = pCfgs->actual_image[actual_y][actual_x][1];
		printf("i = %d img point = [%d,%d]\n", i, img_x, img_y);
		if(img_x < 0 || img_y < 0)//������Ƶͼ��Χ
			continue;
		//����ƵĿ�������
		for(j = 0; j < objVehicleVideoNum; j++)
		{
			dis_obj[i][j] = abs(objVehicleVideo[j].x + objVehicleVideo[j].w / 2 - img_x) + abs(objVehicleVideo[j].y + objVehicleVideo[j].h / 2 - img_y);
		}
		for(j = 0; j < objPersonVideoNum; j++)
		{
			dis_obj[i][j + objVehicleVideoNum] = abs(objPersonVideo[j].x + objPersonVideo[j].w / 2 - img_x) + abs(objPersonVideo[j].y + objPersonVideo[j].h / 2 - img_y);
		}
	}
	//�ж���ƵĿ����״�Ŀ���Ƿ�ƥ��
	for(j = 0; j < objVideoNum; j++)
	{
		float min_dis = 1000000;
		int idx = -1;
		for(i = 0; i < objRadarNum; i++)
		{
			//�ҵ�û��ƥ����������ƵĿ��
			if(dis_obj[i][j] < min_dis && match_radar[i] < 0)
			{
				min_dis = dis_obj[i][j];
				idx = i;

			}
		}
		if(idx >= 0 && min_dis < 100)//100Ϊ��ֵ��ƥ��ɹ�
		{
			match_radar[idx] = j;
			match_video[j] = idx;
			if(j < objVehicleVideoNum)
			{
				objVehicleInfo[vehicleNum] = objVehicleVideo[j];
				objVehicleInfo[vehicleNum].speed_Vx = (int)(objRadar[idx].Speed_y); 
				objVehicleInfo[vehicleNum].speed = (int)(objRadar[idx].Speed_x);
				objVehicleInfo[vehicleNum].length = (int)(objRadar[idx].Obj_Len + 0.5);
				for(i = 0; i < pCfgs->detTargets_size; i++)
				{
					if(pCfgs->detTargets[i].target_id == objVehicleInfo[vehicleNum].id)
					{
						pCfgs->detTargets[i].vx = objVehicleInfo[vehicleNum].speed_Vx;
						pCfgs->detTargets[i].vy = objVehicleInfo[vehicleNum].speed;
						pCfgs->detTargets[i].radar_speed = TRUE;
						break;
					}
				}
				printf("join vehicle dis = %f,%f, %d, speed = [%f,%d],length = [%f, %d]\n", dis_obj[idx][j], min_dis, idx, objRadar[idx].Speed_x, objVehicleInfo[vehicleNum].speed, objRadar[idx].Obj_Len,objVehicleInfo[vehicleNum].length);
				vehicleNum++;

			}
			else
			{
				objPersonInfo[personNum] = objPersonVideo[j - objVehicleVideoNum];
				objPersonInfo[personNum].speed_Vx = (int)(objRadar[idx].Speed_y);
				objPersonInfo[personNum].speed = (int)(objRadar[idx].Speed_x);  
				objPersonInfo[personNum].length = (int)(objRadar[idx].Obj_Len + 0.5);
				for(i = 0; i < pCfgs->objPerson_size; i++)
				{
					if(pCfgs->objPerson[i].target_id == objPersonInfo[personNum].id)
					{
						pCfgs->objPerson[i].vx = objPersonInfo[personNum].speed_Vx;
						pCfgs->objPerson[i].vy = objPersonInfo[personNum].speed;
						pCfgs->objPerson[i].radar_speed = TRUE;
						break;
					}
				}
				printf("join person dis = %f,%f, %d, speed = [%f,%d],length = [%f, %d]\n", dis_obj[idx][j], min_dis, idx, objRadar[idx].Speed_x, objPersonInfo[personNum].speed, objRadar[idx].Obj_Len,objPersonInfo[personNum].length);
				personNum++;

			}
		}
		if(match_video[j] < 0)//��ƵĿ��û��ƥ����
		{
			if(j < objVehicleVideoNum)
			{
				objVehicleInfo[vehicleNum++] = objVehicleVideo[j];
			}
			else
			{
				objPersonInfo[personNum++] = objPersonVideo[j - objVehicleVideoNum];
			}
		}
	}

	for(i = 0; i < objRadarNum; i++)//�״�Ŀ��û��ƥ����
	{
		if(match_radar[i] >= 0)
			continue;
		//���״�Ŀ��ӳ�䵽ͼ��
		mRadarRTObj objR = objRadar[i];
		if(abs(objRadar[i].Speed_x) < 10)//�״�Ŀ���ٶ�̫С����Ϊ�����
			continue;
		int top = 0, bottom = 0;
		actual_x = int((objRadar[i].y_Point + offset_x - pCfgs->actual_origin[0]) * pCfgs->mapping_ratio[0]);
		actual_y = int((objRadar[i].x_Point + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		top = int((objRadar[i].x_Point - objRadar[i].Obj_Len / 2 + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		bottom = int((objRadar[i].x_Point + objRadar[i].Obj_Len / 2 + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		if(actual_x >= 2000 || actual_y >= 2000 || actual_x < 0 || actual_y < 0)//����ʵ�����굽ͼ���ӳ�䷶Χ
			continue;
		int x = pCfgs->actual_image[actual_y][actual_x][0];
		int y = pCfgs->actual_image[actual_y][actual_x][1];
		int h = abs(pCfgs->actual_image[top][actual_x][1] - pCfgs->actual_image[bottom][actual_x][1]);
		if(x < 0 || y < 0)//������Ƶͼ��Χ
			continue;
		objVehicleInfo[vehicleNum].x = x;
		objVehicleInfo[vehicleNum].y = y;
		objVehicleInfo[vehicleNum].w = h * 2 / 3;
		objVehicleInfo[vehicleNum].h = h;
		objVehicleInfo[vehicleNum].distance[0] = objRadar[i].y_Point + offset_x;
		objVehicleInfo[vehicleNum].distance[1] = objRadar[i].x_Point + offset_y;
		if(objRadar[i].Obj_Len < 5)//car
		{
			objVehicleInfo[vehicleNum].label = 2;
		}
		else if(objRadar[i].Obj_Len < 12)//truck
		{
			objVehicleInfo[vehicleNum].label  = 3;
		}
		else//bus
		{
			objVehicleInfo[vehicleNum].label = 1;

		}
		objVehicleInfo[vehicleNum].speed_Vx = (int)(objRadar[i].Speed_y);
		objVehicleInfo[vehicleNum].speed = (int)(objRadar[i].Speed_x); 
		objVehicleInfo[vehicleNum].length = (int)(objRadar[i].Obj_Len + 0.5);
		vehicleNum++;

	}
	*objVehicleNum = vehicleNum;
	*objPersonNum = personNum;

}
//�״������Ƶ��������ں�
//objRadar:�״���Ŀ�� 
//objRadarNum �״���Ŀ����
//objInfo ��Ƶ���״��ںϺ��Ŀ��
//objNum ��Ƶ���״��ںϺ��Ŀ����
void associate_radar_and_video(ALGCFGS *pCfgs, ALGPARAMS *pParams, mRadarRTObj* objRadar, int objRadarNum, OBJECTINFO* objVehicleInfo, Uint16* objVehicleNum)//����������״��ں�
{
	if(objRadarNum <= 0)//û���״���Ŀ��,�򷵻�
		return;
	int i = 0, j = 0, k = 0;
	int actual_x = 0, actual_y = 0;//�״����Ƶ���������෴
	int offset_x = 0, offset_y = 0; //x��y�������״���������ϵ��ƫ��
	//�״����Ƶ��ƥ���ϣ������״�ļ����Ϣ
	//��Ƶ������Ŀ�꣬�״�û�У�������Ƶ���м���Ŀ����ٶȡ�λ�á�Ŀ�곤�ȿ��
	//�״������Ŀ�꣬��Ƶû�У���Ҫ����ͼ��λ��
	OBJECTINFO* objVehicleVideo = pCfgs->ResultMsg.uResultInfo.udetBox;
	int objVehicleVideoNum = pCfgs->ResultMsg.uResultInfo.udetNum;
	Uint16 vehicleNum = 0;
	int match_video[200] = { -1 };
	int match_radar[200] = { -1 };
	float dis_obj[200][200] = { 1000000 };//���ڼ�����Ƶ���״�Ŀ��֮��ľ���
	for(i = 0; i < 200; i++)
	{
		for(j = 0; j < 200; j++)
			dis_obj[i][j] = 1000000;
	}
	memset(match_video, -1, 200 * sizeof(int));
	memset(match_radar, -1, 200 * sizeof(int));
	printf("detect vehicle obj = %d\n", objVehicleVideoNum);
	for(i = 0; i < objRadarNum; i++)
	{
		//���״�Ŀ��ӳ�䵽ͼ��
		mRadarRTObj objR = objRadar[i];
		actual_x = int((objRadar[i].y_Point + offset_x - pCfgs->actual_origin[0]) * pCfgs->mapping_ratio[0]);
		actual_y = int((objRadar[i].x_Point + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		printf("i = %d, radar = [ %f, %f, %f, %f, %f], [%d,%d]\n", i, objRadar[i].x_Point, objRadar[i].y_Point, objRadar[i].Speed_x, objRadar[i].Speed_y, objRadar[i].Obj_Len, actual_x,actual_y);
		if(actual_x >= 2000 || actual_y >= 2000 || actual_x < 0 || actual_y < 0)//����ʵ�����굽ͼ���ӳ�䷶Χ
			continue;
		int img_x = pCfgs->actual_image[actual_y][actual_x][0];
		int img_y = pCfgs->actual_image[actual_y][actual_x][1];
		printf("i = %d img point = [%d,%d]\n", i, img_x, img_y);
		if(img_x < 0 || img_y < 0)//������Ƶͼ��Χ
			continue;
		//����ƵĿ�������
		for(j = 0; j < objVehicleVideoNum; j++)
		{
			dis_obj[i][j] = abs(objVehicleVideo[j].x + objVehicleVideo[j].w / 2 - img_x) + abs(objVehicleVideo[j].y + objVehicleVideo[j].h / 2 - img_y);
		}
	}
	//�ж���ƵĿ����״�Ŀ���Ƿ�ƥ��
	/*for(i = 0; i < objRadarNum; i++)
	{
		float min_dis = 1000000;
		int idx = -1;
		for(j = 0; j < objVehicleVideoNum; j++)
		{
			//�ҵ�û��ƥ����������ƵĿ��
			if(dis_obj[i][j] < min_dis && match_video[j] < 0)
			{
				min_dis = dis_obj[i][j];
				idx = j;

			}
		}
		if(idx >= 0 && min_dis < 100)//100Ϊ��ֵ��ƥ��ɹ�
		{
			match_radar[i] = idx;
			match_video[idx] = i;
			if(idx < objVehicleVideoNum)
			{
				objVehicleInfo[vehicleNum] = objVehicleVideo[idx];
				objVehicleInfo[vehicleNum].speed_Vx =(int)(objRadar[i].Speed_y);
				objVehicleInfo[vehicleNum].speed = (int)(objRadar[i].Speed_x); 
				objVehicleInfo[vehicleNum].length = (int)(objRadar[i].Obj_Len + 0.5);
				for(j = 0; j < pCfgs->detTargets_size; j++)
				{
					if(pCfgs->detTargets[j].target_id == objVehicleInfo[vehicleNum].id)
					{
						pCfgs->detTargets[j].vx = objVehicleInfo[vehicleNum].speed_Vx;
						pCfgs->detTargets[j].vy = objVehicleInfo[vehicleNum].speed;
						pCfgs->detTargets[i].radar_speed = TRUE;
						break;
					}
				}
				printf("join vehicle dis = %f,%f, %d, speed = [%f,%d],length = [%f, %d]\n", dis_obj[i][idx], min_dis, idx, objRadar[i].Speed_x, objVehicleInfo[vehicleNum].speed, objRadar[i].Obj_Len,objVehicleInfo[vehicleNum].length);
				vehicleNum++;

			}
		}
	}*/

	//�ж���ƵĿ����״�Ŀ���Ƿ�ƥ��
	for(j = 0; j < objVehicleVideoNum; j++)
	{
		float min_dis = 1000000;
		int idx = -1;
		for(i = 0; i < objRadarNum; i++)
		{
			//�ҵ�û��ƥ����������ƵĿ��
			if(dis_obj[i][j] < min_dis && match_radar[i] < 0)
			{
				min_dis = dis_obj[i][j];
				idx = i;

			}
		}
		if(idx >= 0 && min_dis < 100)//100Ϊ��ֵ��ƥ��ɹ�
		{
			match_radar[idx] = j;
			match_video[j] = idx;
			if(idx < objRadarNum)
			{
				objVehicleInfo[vehicleNum] = objVehicleVideo[j];
				objVehicleInfo[vehicleNum].speed_Vx = (int)(objRadar[idx].Speed_y);
				objVehicleInfo[vehicleNum].speed = (int)(objRadar[idx].Speed_x); 
				objVehicleInfo[vehicleNum].length = (int)(objRadar[idx].Obj_Len + 0.5);
				objVehicleInfo[vehicleNum].laneid = 2;//�״�����
				printf("join vehicle dis = %f,%f, %d, speed = [%f,%d],length = [%f, %d]\n", dis_obj[idx][j], min_dis, idx, objRadar[idx].Speed_x, objVehicleInfo[vehicleNum].speed, objRadar[idx].Obj_Len,objVehicleInfo[vehicleNum].length);

			}
		}
		if(match_video[j] < 0)//��ƵĿ��û��ƥ����
		{
			objVehicleVideo[j].laneid = 1;//��Ƶ�����
			objVehicleInfo[vehicleNum] = objVehicleVideo[j];
		}
		for(i = 0; i < pCfgs->detTargets_size; i++)
		{
			if(pCfgs->detTargets[i].target_id == objVehicleInfo[vehicleNum].id)
			{
				//�Ա���һ֡�͵�ǰ֡���ٶȲ��
				if(((pCfgs->detTargets[i].vy - objVehicleInfo[vehicleNum].speed) > 10) && abs(objVehicleInfo[vehicleNum].speed < 5) && (objVehicleInfo[vehicleNum].laneid == 2))//�����״�����ٶ�Ϊ0
				{
					objVehicleInfo[vehicleNum].speed_Vx = pCfgs->detTargets[i].vx;
					objVehicleInfo[vehicleNum].speed = pCfgs->detTargets[i].vy;//������һ֡���ٶ�
				}
			    pCfgs->detTargets[i].vx = objVehicleInfo[vehicleNum].speed_Vx;
				pCfgs->detTargets[i].vy = objVehicleInfo[vehicleNum].speed;//���浱ǰ֡���ٶ�
				//pCfgs->detTargets[i].radar_speed = TRUE;
				break;
			}
		}
		vehicleNum++;
	}

	/*for(i = 0; i < objRadarNum; i++)//�״�Ŀ��û��ƥ����
	{
		if(match_radar[i] >= 0)
			continue;
		//���״�Ŀ��ӳ�䵽ͼ��
		mRadarRTObj objR = objRadar[i];
		if(abs(objRadar[i].Speed_x) < 10 || abs(objRadar[i].Speed_y) < 10)//�״�Ŀ���ٶ�̫С����Ϊ�����
			continue;
		int top = 0, bottom = 0;
		actual_x = int((objRadar[i].y_Point + offset_x - pCfgs->actual_origin[0]) * pCfgs->mapping_ratio[0]);
		actual_y = int((objRadar[i].x_Point + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		top = int((objRadar[i].x_Point - objRadar[i].Obj_Len / 2 + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		bottom = int((objRadar[i].x_Point + objRadar[i].Obj_Len / 2 + offset_y - pCfgs->actual_origin[1]) * pCfgs->mapping_ratio[1]);
		if(actual_x >= 2000 || actual_y >= 2000 || actual_x < 0 || actual_y < 0)//����ʵ�����굽ͼ���ӳ�䷶Χ
			continue;
		int x = pCfgs->actual_image[actual_y][actual_x][0];
		int y = pCfgs->actual_image[actual_y][actual_x][1];
		int h = abs(pCfgs->actual_image[top][actual_x][1] - pCfgs->actual_image[bottom][actual_x][1]);
		if(x < 0 || y < 0)//������Ƶͼ��Χ
			continue;
		//�ж��Ƿ����ڼ�⳵����
		int overlapNum[MAX_LANE] = {-1};
		int max_value = -1, idx = -1;
		CRect rct;
		rct.x = x;
		rct.y = y;
		rct.width = h * 2 / 3;
		rct.height = h;
		for( j = 0; j < pCfgs->LaneAmount; j++)//�����복���ֵཻ
		{
			overlapNum[j] = RectInRegion(pParams->MaskLaneImage, pCfgs, pCfgs->img_width, pCfgs->img_height, rct, j);
			if(overlapNum[j] > max_value)
			{
				max_value = overlapNum[j];
				idx = j;
			}
		}
		if(max_value < 10)
			continue;
		objVehicleInfo[vehicleNum].x = x;
		objVehicleInfo[vehicleNum].y = y;
		objVehicleInfo[vehicleNum].w = h * 2 / 3;
		objVehicleInfo[vehicleNum].h = h;
		objVehicleInfo[vehicleNum].distance[0] = objRadar[i].y_Point + offset_x;
		objVehicleInfo[vehicleNum].distance[1] = objRadar[i].x_Point + offset_y;
		if(objRadar[i].Obj_Len < 5)//car
		{
			objVehicleInfo[vehicleNum].label = 2;
		}
		else if(objRadar[i].Obj_Len < 12)//truck
		{
			objVehicleInfo[vehicleNum].label  = 3;
		}
		else//bus
		{
			objVehicleInfo[vehicleNum].label = 1;

		}
		//objVehicleInfo[vehicleNum].laneid = idx;
		objVehicleInfo[vehicleNum].laneid = 2;//�״�����
		objVehicleInfo[vehicleNum].speed_Vx = (int)(objRadar[i].Speed_y);
		objVehicleInfo[vehicleNum].speed = (int)(objRadar[i].Speed_x); 
		objVehicleInfo[vehicleNum].length = (int)(objRadar[i].Obj_Len + 0.5);
		vehicleNum++;

	}*/
	*objVehicleNum = vehicleNum;

}