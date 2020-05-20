#include "visibility_detector.h"
int getBrightness1 = 0;
int getBrightness2 = 0;
Uint16 cal_edge_num(Uint8* img, int width, int height)
{
	int i = 0, j = 0;
	int val = 0, zero_num = 0, visibility_pos = 0;
	Uint8* p;
	int stat_edge_num[DETECTRECT_HEIGHT_MAX] = { 0 };
	Uint8* out =  (Uint8*) malloc(width * height * sizeof(Uint8));
	SobelCalculate(img, out, 80, width, height);//sobel��Ե���
	//ͳ��ÿ�б�Եͼ��ĸ���
	memset(stat_edge_num, 0, DETECTRECT_HEIGHT_MAX * sizeof(int));
	for(i = 0; i < height; i++)
	{
		p = out + i * width;
		for(j = 0; j < width; j++)
		{
			if(*(p + j)) stat_edge_num[i]++;
		}
	}
	//��ͳ��������о�ֵ�˲�,�����Ա�Ե�������������ܼ���λ��
	for(i = height - 10; i >= 0; i--)
	{
		val = (stat_edge_num[i - 1] + stat_edge_num[i] + stat_edge_num[i + 1]) / 3;
		if(val <= 0)
			zero_num++;
		else
			zero_num = 0;
		if(zero_num > 20)
			break;
	}
	visibility_pos = i + zero_num;
	if(out)
	{
		free(out);
		out =NULL;
	}
	return visibility_pos;

}
//����ֵ
Uint8 median(Uint8* arr, int num)//ȡ�м�ֵ
{
	int i = 0, j = 0;
	Uint8 temp;
	Uint8 array_temp[DETECTRECT_HEIGHT_MAX] = { 0 };
	memcpy(array_temp, arr, num * sizeof(Uint8));
	for(i = 0; i <= num / 2; i++)
	{
		for(j = i + 1; j < num; j++)
		{
			if(array_temp[i] > array_temp[j])
			{
				temp = array_temp[i];
				array_temp[i] = array_temp[j];
				array_temp[j] = temp;
			}
		}
	}
	return array_temp[num / 2];
}
int d1[4] = {-1, 1, 0, 0};//left right top bottom
int d2[4] = {0, 0, -1, 1};
//���ݶԱȶ�0.05��Ϊ�ܼ���λ��
Uint16 cal_visibility(Uint8* img, int calibration_point[][2], int width, int height)
{
	int i = 0, j = 0, k = 0;
	float start_point, end_point;
	float k02, b02, k13, b13;
	Uint8* p;
	int val = 0, max_value = 0, max_dis = 0;
	float vis = 0, max_vis = 0;
	int num = 0;
	//��궨����ֱ��б�ʺͽؾ�
	if(calibration_point[0][1] != calibration_point[2][1])
	{
		k02 = 1.0 * (calibration_point[0][1] - calibration_point[2][1]) / (calibration_point[0][0] - calibration_point[2][0]);
		b02 = calibration_point[0][1] - k02 * calibration_point[0][0];
	}
	else
	{
		k02 = 1e+6;
	}
	if(calibration_point[1][1] != calibration_point[3][1])
	{
		k13 = 1.0 * (calibration_point[1][1] - calibration_point[3][1]) / (calibration_point[1][0] - calibration_point[3][0]);
		b13 = calibration_point[1][1] - k13 * calibration_point[1][0];
	}
	else
	{
		k13 = 1e+6;
	}
	//����ÿ������ܼ���
	for(i = height - 2; i >= 1; i--)
	{
		start_point = (i - b02) / (k02 + 0.0000001);//���Ҷ˵�
		end_point = (i - b13) / (k13 + 0.0000001);
		num = 0;
		max_vis = 0;
		for(j = max(1, (int)start_point); j < min((int)end_point, width - 1); j++)
		{
			p = img + i * width + j;
			max_value = *p;
			max_dis = 0;
			for(k = 0; k < 4; k++)
			{
				val = *(p + d1[k] + d2[k] * width);
				max_value = (val > max_value)? val : max_value;
				max_dis = (max_dis < abs(*p - val))? abs(*p - val) : max_dis;

			}
			vis = (float)max_dis / (float)max_value;

			if(max_vis < vis)
				max_vis = vis;
			num++;
		}
		vis = vis / num;
		if(max_vis < 0.05)//�����ܼ���λ��
		{
			return i;
		}

	}
	return 0;
}
//����ֶ����������Ҷ�ֵ����С�Ҷ�ֵ��ƽ���Ҷȼ����ܼ���λ��
Uint16 cal_region_visibility(Uint8* img, int calibration_point[][2], int width, int height)
{
	int i = 0, j = 0, k = 0;
	float start_point, end_point;
	float k02, b02, k13, b13;
	Uint8* p;
	float mean_val = 0, max_val = 0, min_val = 0;
	float vis = 0, max_vis = 0;
	int num = 0;
	//��궨����ֱ��б�ʺͽؾ�
	if(calibration_point[0][1] != calibration_point[2][1])
	{
		k02 = 1.0 * (calibration_point[0][1] - calibration_point[2][1]) / (calibration_point[0][0] - calibration_point[2][0]);
		b02 = calibration_point[0][1] - k02 * calibration_point[0][0];
	}
	else
	{
		k02 = 1e+6;
	}
	if(calibration_point[1][1] != calibration_point[3][1])
	{
		k13 = 1.0 * (calibration_point[1][1] - calibration_point[3][1]) / (calibration_point[1][0] - calibration_point[3][0]);
		b13 = calibration_point[1][1] - k13 * calibration_point[1][0];
	}
	else
	{
		k13 = 1e+6;
	}
	//�ֶ�����
	for(i = height - 1; i >= 20/*0*/; i -= 10)
	{
		mean_val = 0;
		max_val = 0;
		min_val = 255;
		num = 0;
		for(k = 0; k < 10; k++)
		{
			start_point = (i - k - b02) / (k02 + 0.0000001);//���Ҷ˵�
			end_point = (i - k - b13) / (k13 + 0.0000001);
			for(j = max(1, (int)start_point); j < min((int)end_point, width - 1); j++)
			{
				p = img + (i - k) * width + j;
				mean_val += *p;
				max_val = (max_val < *p)? *p : max_val;
				min_val = (min_val > *p)? *p : min_val;
				num++;
			}
		}
		mean_val = mean_val /(num + 1);
		vis = (max_val - mean_val) / mean_val;
		max_vis = vis;
		vis = (mean_val - min_val) / mean_val;
		max_vis = (max_vis < vis)? vis : max_vis;
		if(max_vis < 0.05)
			return (i + 5);
	}
	return 0;
}
//ͨ������������õ�·����
void RegionGrowing(Uint8* img, Uint8* dst_img, int width, int height)
{
	int i = 0, j = 0;
	Uint8* p;
	CPoint pt, ptGrowing, ptTemp;//��ʼ�����ӵ�
	int nSrcValue = 0, nCurrValue = 0, nGrowValue = 0, nEdgeValue = 0, nTempValue = 0;
	int num = 0;
	std::vector<CPoint> growPtVector;//������ջ
	//��ͼ�����sobel��Ե���
	Uint8* edge_img =  (Uint8*) malloc(width * height * sizeof(Uint8));
	SobelCalculate(img, edge_img, 80, width, height);//sobel��Ե���
	memset(dst_img, 0, width * height * sizeof(Uint8));//��ʼ���������
	nSrcValue = median(img + (height - 20) * width + 20, width - 40);
	for(i = 0; i < width; i++)
	{
		p = img + (height - 20) * width + i;
		if(abs(*p - nSrcValue) < 20)
		{
			pt.x = i;
			pt.y = height - 20;
			growPtVector.push_back(pt);//���ӵ�ѹ��ջ
			*(dst_img + pt.y * width + pt.x) = 255;//���������
		}
	}
	int DIR[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
	int thr[3] = {4, 6, 4};
	//��������
	while(!growPtVector.empty())
	{
		pt = growPtVector.back();//ȡ��һ������
		growPtVector.pop_back();
		nSrcValue = *(img + pt.y * width + pt.x);
		//�������ڵĵ��������
		for(i = 0; i < 8; i++)
		{
			ptGrowing.x = pt.x + DIR[i][0];
			ptGrowing.y = pt.y + DIR[i][1];
			//����Ƿ��Ǳ�Ե��
			if(ptGrowing.x < 0 || ptGrowing.x >= width || ptGrowing.y < 0 || ptGrowing.y >= height)
				continue;
			nGrowValue = *(dst_img + ptGrowing.y * width + ptGrowing.x);
			nEdgeValue = *(edge_img + ptGrowing.y * width + ptGrowing.x);
			nCurrValue = *(img + ptGrowing.y * width + ptGrowing.x);
			if(nGrowValue == 0 && nEdgeValue == 0)//��ǵ㻹û������,���Ҳ��Ǳ�Ե��
			{
				num = 1;
				for(j = 0; j < 3; j++)
				{
					ptTemp.x = ptGrowing.x + DIR[j][0];
					ptTemp.y = ptGrowing.y + DIR[j][1];
					if(ptTemp.x < 0 || ptTemp.x >= width || ptTemp.y < 0 || ptTemp.y >= height)
						continue;
					nTempValue = *(img + ptTemp.y * width + ptTemp.x);
					if(abs(nTempValue - nCurrValue) > thr[j])
					{
						num = 0;
						break;
					}
				}
				if(num == 1 && abs(nCurrValue - nSrcValue) < 0.8 * (pt.y - ptGrowing.y) * thr[0])//������������������
				{
					*(dst_img + ptGrowing.y * width + ptGrowing.x) = 255;
					growPtVector.push_back(ptGrowing);//����һ��������ѹջ
				}

			}
		}

	}
	if(edge_img)
	{
		free(edge_img);
		edge_img = NULL;
	}
}
//��ÿ����󳤶�
void getVerticalBandwidth(Uint8* segment_img, int bandwidth[][2], int width, int height)
{
	int i = 0, j = 0;
	int begin_col = 0, end_col = 0, dis = 0, temp = 0;
	for(i = 0; i < height; i++)
	{
		begin_col = 0;
		end_col = 0;
		temp = 0;
		dis = 0;
		for(j = 0; j < width; j++)
		{
			if(*(segment_img + i * width + j) == 255)
			{
				temp++;
			}
			else
			{
				if(dis < temp)
				{
					end_col = j - 1;
					begin_col = j - temp;
					dis = temp;
				}
				temp = 0;
			}
		}
		if(begin_col == 0 && end_col == 0)
		{
			end_col = width - 1;
			dis = width - 1;
		}
		bandwidth[i][0] = begin_col;
		bandwidth[i][1] = end_col;
	}
}
//�������ȵĶ����ݶ�Ϊ·������յ�
Uint16 getIntensity(Uint8* img, int bandwidth[][2], int width, int height)
{
	int i = 0, j = 0;
	int len = 0, deriy = 0, min_val = 9999, location = 0;
	int luminance[DETECTRECT_HEIGHT_MAX] = { 0 };
	//��ÿ��·�����ȵ���ֵ
	for(i = 0; i < height; i++)
	{
		len = bandwidth[i][1] - bandwidth[i][0] + 1;
		luminance[i] = median(img + i * width + bandwidth[i][0], len);
	}
	//�����ȵĶ����ݶ�,���õ��յ�
	for(i = 1; i < height - 50; i++)
	{
		deriy = (luminance[i + 1] - luminance[i - 1]) / 2;
		if(deriy < min_val)
		{
			location = i;
			min_val = deriy;
		}		
	}
	return location;

}
//�������ȵĶ����ݶ�Ϊ·������յ�
int getIntensity0(Uint8* segment_img, Uint8* img, int width, int height)
{
	int i = 0, j = 0;
	int len = 0, deriy = 0, min_val = 9999, val = 0, location = 0;
	int luminance[DETECTRECT_HEIGHT_MAX] = { 0 };
	Uint8* grayval = (Uint8*)malloc(width * sizeof(Uint8));
	for(i = height - 1; i >= 0; i--)
	{
		len = 0;
		for(j = 0; j < width; j++)
		{
			if(*(segment_img + i * width + j) == 255)
			{
				*(segment_img + i * width + j) = *(img + i * width + j);
				grayval[len++] = *(img + i * width + j);
			}
		}
		if(len == 0)
			luminance[i] = median(img + i * width + 50, width - 50);
		else
		{
			luminance[i] = median(grayval, len);
			if(val == 0)
			{
				val = luminance[i];
			}
		}
	}
	//�����ȵĶ����ݶ�,���õ��յ�
	for(i = 1; i < height - 50; i++)
	{
		deriy = (luminance[i + 1] - luminance[i - 1]) / 2;
		if(deriy < min_val)
		{
			location = i;
			min_val = deriy;
		}	
	}
	if(abs(min_val) <= val / 10)//С����ֵ������Ϊ���ǹյ�
		location = 0;
	if(grayval)
	{
		free(grayval);
		grayval = NULL;
	}
	return location;
}
Uint16 DayVisibilityDetection(Uint8* img, int calibration_point[][2], int width, int height)
{
	Uint16 loc = 0;//�ܼ�����ͼ���е�λ��
	Uint8* out = (Uint8*)malloc(width * height * sizeof(Uint8));
	Uint8* growImg = (Uint8*)malloc(width * height * sizeof(Uint8));
	cal_region_visibility(img, calibration_point, width, height);//���ݻҶȶԱȶȽ����ܼ��ȼ���
	cal_visibility(img, calibration_point, width, height);//�������ȶԱȶ�0.05ȷ���ܼ���λ��
	AvgFilter(img, out, width, height);//��ֵ�˲�
	RegionGrowing(out, growImg, width, height);//��������
	int bandwidth[DETECTRECT_HEIGHT_MAX][2] = { 0 };
	getVerticalBandwidth(growImg, bandwidth, width, height);//�õ�ÿ�����bandwidth
	int line_y = getIntensity(out, bandwidth, width, height);//����·��յ�
   if(out)
   {
	   free(out);
	   out = NULL;
   }
   if(growImg)
   {
	   free(growImg);
	   growImg = NULL;
   }

}
//l1,l2����λ�õĳ��ƣ�I1,I2���Ƶ�����ֵ
float getNightVisibility(float l1, float l2, Uint8 I1, Uint8 I2)
{
	if(abs(l1 - l2) < 0.1)//����������̫Сʱ���ܼ���Ϊ0
		return 0;
	float vis = 0;//�ܼ���
	float coff = 0.0000000001;//����ϵ��
	coff = log((float)I1) - log((float)I2) + 2 * log(l1) - 2 * log(l2);
	coff = coff / (l2 - l1);
	vis = 3.912 / coff; 
	return vis;
}
//�ж��趨λ���Ƿ��г���
int detectHasLight(Uint8* img, Uint8* dst_img, int width, int height, int l1, int l2)
{
	int location = -1;
	//��ͼ�����Ԥ����ȥ��������������ɢ��Ӱ��
	//��ֵ�˲�
	AvgFilter(img, dst_img, width, height);
	//��ֵ��
	location = Threshold(dst_img, dst_img, width, height, 180, l1, l2);//��ֵ180
	return location;
}

//����l���봦�ĳ��ƹ�Դ������
Uint8 calLightBrightness(Uint8* img, int width, int height, int l)
{
	int loc0 = max(0, l - 50);
	int loc1 = min(height - 1, l + 50);
	int lightLoc[20][5];
	int left = 0, right = 0, top = 0, bottom = 0, area = 0;
	int left1 = 0, right1 = 0, top1 = 0, bottom1 = 0, area1 = 0;
	int dwArrayCurrentSize = 0, dwProcessedSize = 0, dwArraySize = 0;
	Uint8* MidImage = (Uint8*)malloc(width * height * sizeof(Uint8));
	memset(MidImage, 0, width * height * sizeof(Uint8));
	CPoint* m_pPointArray = (CPoint*)malloc(200 * width * sizeof(CPoint));
	int sum = 0, num = 0;
	int getLightNum = 0;
	int getLight = 0;
	//�Դ����������ͨ�����
	for(int row = loc0; row < loc1; row++)
	{
		for(int col = 0; col < width; col++)
		{
			if(*(img + row * width + col) && *(MidImage + row * width + col) == 0)
			{
				m_pPointArray[0].x = col;
				m_pPointArray[0].y = row;
				left = col;
				right = col;
				top = row;
				bottom = row;

				dwArrayCurrentSize = 1;
				dwProcessedSize = 0;
				sum = 0;
				num = 0;
				do
				{
					dwArraySize = dwArrayCurrentSize;
					for(int l = dwProcessedSize; l < dwArraySize; l++)
					{
						if(left > m_pPointArray[l].x)
						{
							left = m_pPointArray[l].x;
						}
						else if(right < m_pPointArray[l].x)
						{
							right = m_pPointArray[l].x;
						}
						if(top > m_pPointArray[l].y)
						{
							top = m_pPointArray[l].y;
						}
						else if(bottom < m_pPointArray[l].y)
						{
							bottom = m_pPointArray[l].y;
						}
						for(int t = -1; t <= 1; t++)
						{
							for(int s = -1; s <= 1; s++)
							{
								int k = m_pPointArray[l].y + t;
								int j = m_pPointArray[l].x + s;
								if((k >= 0) && (k < height) && (j >= 0) && (j < width))
								{
									if(*(img + k * width + j) && *(MidImage + k * width + j) == 0)
									{
										sum += *(img + k * width + j);
										num++;
										*(MidImage + k * width + j) = 1;
										m_pPointArray[dwArrayCurrentSize].x = j;
										m_pPointArray[dwArrayCurrentSize].y = k;
										dwArrayCurrentSize++;							
									}
								}
							}
						}
					}
					dwProcessedSize = dwArraySize;
				}while(dwArrayCurrentSize > dwArraySize);
				int w = right - left + 1;
				int h = bottom - top + 1;
				sum = (num > 0) ? sum / num : 0;
				if(w * h > 50 &&  w / h < 3 && h / w < 3 && (double)dwArraySize / (double)(w * h) > 0.3)//�õ��˳�������
				{
					lightLoc[getLightNum][0] = left;
					lightLoc[getLightNum][1] = right;
					lightLoc[getLightNum][2] = top;
					lightLoc[getLightNum][3] = bottom;
					lightLoc[getLightNum][4] = sum;
					getLightNum++;
				}
			}
		}
	}
	for(int i = 0; i < getLightNum - 1; i++)
	{
		int is_match = 0;
		left = lightLoc[i][0];
		right = lightLoc[i][1];
		top = lightLoc[i][2];
		bottom = lightLoc[i][3];
		area = (right - left) * (bottom - top);
		for(int j = i + 1; j < getLightNum; j++)
		{
			left1 = lightLoc[j][0];
			right1 = lightLoc[j][1];
			top1 = lightLoc[j][2];
			bottom1 = lightLoc[j][3];
			area1 = (right1 - left1) * (bottom1 - top1);
			if(MAX(left, left1) - MIN(right, right1) > 10 && MAX(left, left1) - MIN(right, right1) < 100 && MAX(top, top1) < MIN(bottom, bottom1)) 
			{
				if((float)area / (float)area1 < 3 && (float)area1 / (float)area < 3)
				{
					getLight = 1;
					sum = lightLoc[j][4];
					break;
				}

			}
		}
		if(getLight == 1)
			break;
	}
	if(MidImage)
	{
		free(MidImage);
		MidImage = NULL;
	}
	if(m_pPointArray)
	{
		free(m_pPointArray);
		m_pPointArray = NULL;
	}
	if(getLight == 1)
		return sum;
	else
		return 0;
}
float NightVisibilityDetection(Uint8* img, int calibration_point[][2], int width, int height, float l1, float l2)
{
	int loc1 = height - 300, loc2 = height - 100;
	Uint8* dst_img = (Uint8*)malloc(width * height * sizeof(Uint8));
	int loc = detectHasLight(img, dst_img, width, height, l1, l2);
	int getLight = 0;
	float vis = 0;
	if((loc == 1 || loc == 3) && getBrightness1 == 0 && getLight == 0)//λ��1���г���, ����û�еõ�1���ĳ�������
	{
		getBrightness1 = calLightBrightness(dst_img, width, height, l1);
		if(getBrightness1)//�õ����Ƶ�����
			getLight = 1;
	}
	if((loc == 2 || loc == 3) && getBrightness2 == 0 && getLight == 0)//λ��2���г��ƣ�����û�еõ�2���ĳ�������
		getBrightness2 = calLightBrightness(dst_img, width, height, l2);
	if(getBrightness1 && getBrightness2)//�õ�������λ�õĳ�������
	{
		vis = getNightVisibility(l1, l2, getBrightness1, getBrightness2);
		getBrightness1 = 0;
		getBrightness2 = 0;
	}
	if(dst_img)
	{
		free(dst_img);
		dst_img = NULL;
	}
	return vis;

}