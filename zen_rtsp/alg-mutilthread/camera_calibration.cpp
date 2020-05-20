#include "m_arith.h"
//lhx,20150608
/*void camera_calibration(float actual_point[][2], float img_point[][2], float mapping_matrix[], int calibration_num, ALGCFGS *pCfgs)
{
	int sub_calibration_num = 4;
	int subsection_num = (calibration_num - sub_calibration_num) / 2 + 1;
	int num = 0;
	float tx = 0, ty = 0,tu = 0, tv = 0;
	float s1 = 0, s2 = 0;
	float T1[9] = { 0 };
	float T2[9] = { 0 };
	int i, j;

	float normalization_actual_x[4];
	float normalization_actual_y[4];
	float normalization_img_x[4];
	float normalization_img_y[4];
	float A[2 * 4 * 9] = { 0 };
	float v[9 * 9] = { 0 };
	float d[9] = { 0 };
	float L[9] = { 0 };
	float T1_inv[9] = { 0 };
	float temp[9] = { 0 };
	int overlap_row1, overlap_row2, flag = 0;
	//??????
	for(num = 0; num < subsection_num; num++)
	{
		//???
		tx = ty = tu = tv = 0;
		s1 = s2 = 0;		
		for(i = 0; i < sub_calibration_num; i++)
		{
			j = i + num * 2;
			tx = tx + actual_point[j][0];
			ty = ty + actual_point[j][1];
			tu = tu + img_point[j][0];
			tv = tv + img_point[j][1];
		}
		tx = tx / sub_calibration_num;
		ty = ty / sub_calibration_num;
		tu = tu / sub_calibration_num;
		tv = tv / sub_calibration_num;
		for(i = 0; i < sub_calibration_num; i++)
		{
			j = i + num * 2;
			s1 = s1 + sqrt((img_point[j][0] - tu) * (img_point[j][0] - tu) + (img_point[j][1] - tv) * (img_point[j][1] - tv));
			s2 = s2 + sqrt((actual_point[j][0] - tx) * (actual_point[j][0] - tx) + (actual_point[j][1] - ty) * (actual_point[j][1] - ty));
		}
		s1 = (fabs(s1) < 1e-6)? 0 : sqrt(2.0) / s1;
		s2 = (fabs(s2) < 1e-6)? 0 : sqrt(2.0) / s2;
		T1[1] = T1[3] = T1[6] = T1[7] = 0;
		T1[0] = T1[4] = s1;
		T1[2] = -s1 * tu;
		T1[5] = -s1 * tv;
		T1[8] = T2[8] = 1;
		T2[1] = T2[3] = T2[6] = T2[7] = 0;
		T2[0] = T2[4] = s2;
		T2[2] = -s2 * tx;
		T2[5] = -s1 * ty;
		//?????	
		for(i = 0; i < sub_calibration_num; i++)
		{
			j = i + num * 2;
			normalization_img_x[i] = T1[0] * img_point[j][0] + T1[1] * img_point[j][1] + T1[2];
			normalization_img_y[i] = T1[3] * img_point[j][0] + T1[4] * img_point[j][1] + T1[5];
			normalization_actual_x[i] = T2[0] * actual_point[j][0] + T2[1]*actual_point[j][1] + T2[2];
			normalization_actual_y[i] = T2[3] * actual_point[j][0] + T2[4]*actual_point[j][1] + T2[5];

		}
		//????,?????

		for(i = 0; i < 2 * 4 * 9; i++)
		{
			A[i] = 0.0;
		}
		for(i = 0; i < sub_calibration_num; i++)
		{
			A[(2 * i) * 9 + 0] = normalization_actual_x[i];
			A[(2 * i) * 9 + 1] = normalization_actual_y[i];
			A[(2 * i) * 9 + 2] = 1;
			A[(2 * i) * 9 + 6] = -1 * normalization_img_x[i] * normalization_actual_x[i];
			A[(2 * i) * 9 + 7] = -1 * normalization_img_x[i] * normalization_actual_y[i];
			A[(2 * i) * 9 + 8] = -1 * normalization_img_x[i];
			A[(2 * i + 1) * 9 + 3] = normalization_actual_x[i];
			A[(2 * i + 1) * 9 + 4]=normalization_actual_y[i];
			A[(2 * i + 1) * 9 + 5] = 1;
			A[(2 * i + 1) * 9 + 6] = -1 * normalization_img_y[i] * normalization_actual_x[i];
			A[(2 * i + 1) * 9 + 7] = -1 * normalization_img_y[i] * normalization_actual_y[i];
			A[(2 * i + 1) * 9 + 8] = -1 * normalization_img_y[i];

		}
		for(i = 0; i < 9 * 9; i++)
		{
			v[i] = 0.0;
		}
		for(i = 0; i < 9;i++)
		{
			d[i] = 0.0;
		}
		//svd??
		svd( A, 2 * sub_calibration_num, 9, d, v );
		//v???????????????
		//	float L[9] = { 0 };
		for( i = 0; i < 9; i++ )
			L[i] = v[9 * i + 8];
		//???????????inv(T1)*L*T2;
		//		float T1_inv[9] = { 0 };
		for(i = 0; i < 9; i++)
		{
			T1_inv[i] = 0.0;
		}
		matrix_inverse(T1, T1_inv);

		for(i = 0; i < 9; i++)
		{
			temp[i] = 0.0;

		}


		matrix_mult(temp, T1_inv, L, 3,3,3 );
		matrix_mult(mapping_matrix, temp, T2, 3, 3, 3);
		flag = 0;
		if(num == 0)
		{
			flag = 1;
			overlap_row1 = 0;
			overlap_row2 = 0;
		}
		else
		{
			if(num == subsection_num - 1)
				flag = 2;
			overlap_row1 = (img_point[num * 2][1] + img_point[num * 2 + 1][1]) / 2;
			overlap_row2 = (img_point[num * 2 + sub_calibration_num - 4][1] + img_point[num * 2 + sub_calibration_num - 3][1]) / 2;
		}
		img_to_actual(mapping_matrix, (img_point[num * 2][1] + img_point[num * 2 + 1][1]) / 2, (img_point[num * 2 + sub_calibration_num - 2][1] + img_point[num * 2 + sub_calibration_num - 1][1]) / 2,overlap_row1, overlap_row2, flag, pCfgs);
	}

}*/
//�Ե��������
void sort_point(int array[][2], int length)
{
	int i = 0, j = 0;
	int temp[2];
	for(i = 0; i < length - 1; i++)
	{
		for(j = i + 1; j < length; j++)
		{
			if(array[i][1] > array[j][1])
			{
				temp[0] = array[i][0];
				temp[1] = array[i][1];
				array[i][0] = array[j][0];
				array[i][1] = array[j][1];
				array[j][0] = temp[0];
				array[j][1] = temp[1];
			}
		}
	}
	if(array[0][0] > array[1][0])
	{
		temp[0] = array[0][0];
		temp[1] = array[0][1];
		array[0][0] = array[1][0];
		array[0][1] = array[1][1];
		array[1][0] = temp[0];
		array[1][1] = temp[1];
	}
	if(array[2][0] > array[3][0])
	{
		temp[0] = array[2][0];
		temp[1] = array[2][1];
		array[2][0] = array[3][0];
		array[2][1] = array[3][1];
		array[3][0] = temp[0];
		array[3][1] = temp[1];
	}

}
//�����x�����Ͻ��б궨
static void camera_calibration_x(int base_line[][2], float* base_length, int calibration_point[][2], float near_point_length, int laneNum, ALGCFGS *pCfgs, int imgW, int imgH)
{
	int i = 0, j = 0;
	//�궨����ֱ��0����1����3����2����0
	float k13 = 0, b13 = 0;
	float k20 = 0, b20 = 0;
	float pt_x = 0, pt_y = 0, pt1_x = 0, pt1_y = 0, pt2_x = 0, pt2_y = 0;
	float base_length_x = base_length[1];//x�����ϵĻ�׼��ʵ�ʾ���
	float dis = 0, dis_x = base_length_x, dis_y = 0;
	int center_x = 0;
	float center_dis = 0;
	int origin_x = (base_line[2][0] < base_line[3][0])? base_line[2][0] : base_line[3][0];//���ʵ������ϵxԭ��
	origin_x = (origin_x < 0)? 0 : ((origin_x < imgW)? origin_x : (imgW - 1));
	int max_calibration_height = MIN(MAX_IMAGE_HEIGHT, imgH);//���궨�߶�
	int max_calibration_width = MIN(MAX_IMAGE_WIDTH, imgW);//���궨���
	//��궨����ֱ��б�ʺͽؾ�
	if(calibration_point[1][0] != calibration_point[3][0])
	{
		k13 = 1.0 * (calibration_point[1][1] - calibration_point[3][1]) / (calibration_point[1][0] - calibration_point[3][0]);
		b13 = calibration_point[1][1] - k13 * calibration_point[1][0];
	}
	else
	{
		k13 = 1e+6;
	}
	if(calibration_point[2][0] != calibration_point[0][0])
	{
		k20 = 1.0 * (calibration_point[2][1] - calibration_point[0][1]) / (calibration_point[2][0] - calibration_point[0][0]);
		b20 = calibration_point[2][1] - k20 * calibration_point[2][0];
	}
	else
	{
		k20 = 1e+6;
	}
	//��x�����׼���Ͻ��б궨
	if(k20 > 1e+5)//��ֱ��
	{
		pt1_x = calibration_point[0][0];
	}
	else
	{
		pt1_x = (base_line[2][1] - b20) / (k20 + 0.0000001);
	}
	if(k13 > 1e+5)//��ֱ��
	{
		pt2_x = calibration_point[1][0];
	}
	else
	{
		pt2_x = (base_line[2][1] - b13) / (k13 + 0.0000001);
	}
	dis_x = base_length_x * abs(pt1_x - pt2_x) / (abs(base_line[2][0] - base_line[3][0])+ 0.0000001);//��׼���ϱ궨��������Ӧ��ʵ�ʾ���
	//ÿ�б궨�������ĵ㵽���x����ľ������
	for(i = max_calibration_height - 1; i >= 0; i--)
	{
		
		if(k20 > 1e+5)//��ֱ��
		{
			pt1_x = calibration_point[0][0];
		}
		else
		{
			pt1_x = (i * pCfgs->scale_y - b20) / (k20 + 0.0000001);
		}
		if(k13 > 1e+5)//��ֱ��
		{
			pt2_x = calibration_point[1][0];
		}
		else
		{
			pt2_x = (i * pCfgs->scale_y - b13) / (k13 + 0.0000001);
		}
		dis = dis_x / (abs(pt1_x - pt2_x) + 0.0000001);//ÿ�����ص�����ʵ�ʾ���
		dis = dis * pCfgs->scale_x;
		center_x = (pt1_x + pt2_x) / (2 * pCfgs->scale_x);//�궨��������xֵ
		center_x = (center_x < 0)? 0 : ((center_x < max_calibration_width)? center_x : max_calibration_width - 1);
		if(i == (max_calibration_height - 1))//ͼ�����¶�
		{
			origin_x = (int)((float)origin_x / pCfgs->scale_x);
			pCfgs->image_actual[i][origin_x][0] = 0;
			for(j = origin_x - 1; j >= 0; j--)
			{
				pCfgs->image_actual[i][j][0] = pCfgs->image_actual[i][j + 1][0] + dis;
			}
			for(j = origin_x + 1; j < max_calibration_width; j++)
			{
				pCfgs->image_actual[i][j][0] = pCfgs->image_actual[i][j - 1][0] - dis;
			}
			center_dis = pCfgs->image_actual[i][center_x][0];//���ĵ�x�ľ���
		}
		else//ͼ��������
		{
			pCfgs->image_actual[i][center_x][0] = center_dis;//���ĵ�x�ľ���
			for(j = center_x + 1; j < max_calibration_width; j++)
			{
				pCfgs->image_actual[i][j][0] = pCfgs->image_actual[i][j - 1][0] - dis;
			}
			for(j = center_x - 1; j >= 0; j--)
			{
				pCfgs->image_actual[i][j][0] = pCfgs->image_actual[i][j + 1][0] + dis;
			}
		}

	}
}
//�����y������б궨
static void camera_calibration_y(int base_line[][2], float* base_length, int calibration_point[][2], float near_point_length, int laneNum, ALGCFGS *pCfgs, int imgW, int imgH)
{
	int i = 0, j = 0, k = 0;
	//�궨����ֱ��0����1����3����2����0
	float k01 = 0, b01 = 0;
	float k13 = 0, b13 = 0;
	float k32 = 0, b32 = 0;
	float k20 = 0, b20 = 0;
	//�궨���������ߵĽ���0-2 1-3
	float x0 = 0, y0 = 0;
	float dis = 1;
	//��⳵��������б��
	float k_road = 0, b_road = 0;
	float k_base = 0, b_base = 0;
	float pt_x = 0, pt_y = 0, pt1_x = 0, pt1_y = 0, pt2_x = 0, pt2_y = 0, pt3_x = 0, pt3_y = 0;

	int temp, temp0 = 0;
	int start_point = 0, end_point = 0;
	float calibration_base_line[MAX_IMAGE_HEIGHT] = { 0 };
	float dis_pixel[MAX_IMAGE_HEIGHT] = { 0 };
	float temp1 = 0, temp2 = 0;
	float base_length_y = base_length[0];//��ֱ��׼�߳�
	int max_calibration_height = MIN(MAX_IMAGE_HEIGHT, imgH);//���궨�߶�
	int max_calibration_width = MIN(MAX_IMAGE_WIDTH, imgW);//���궨���
	//��궨����ֱ��б�ʺͽؾ�
	if(calibration_point[0][0] != calibration_point[1][0])
	{
		k01 = 1.0 * (calibration_point[0][1] - calibration_point[1][1]) / (calibration_point[0][0] - calibration_point[1][0]);
		b01 = calibration_point[0][1] - k01 * calibration_point[0][0];
	}
	else
	{
		k01 = 1e+6;
	}
	if(calibration_point[1][0] != calibration_point[3][0])
	{
		k13 = 1.0 * (calibration_point[1][1] - calibration_point[3][1]) / (calibration_point[1][0] - calibration_point[3][0]);
		b13 = calibration_point[1][1] - k13 * calibration_point[1][0];
	}
	else
	{
		k13 = 1e+6;
	}
	if(calibration_point[3][0] != calibration_point[2][0])
	{
		k32 = 1.0 * (calibration_point[3][1] - calibration_point[2][1]) / (calibration_point[3][0] - calibration_point[2][0]);
		b32 = calibration_point[3][1] - k32 * calibration_point[3][0];
	}
	else
	{
		k32 = 1e+6;
	}
	if(calibration_point[2][0] != calibration_point[0][0])
	{
		k20 = 1.0 * (calibration_point[2][1] - calibration_point[0][1]) / (calibration_point[2][0] - calibration_point[0][0]);
		b20 = calibration_point[2][1] - k20 * calibration_point[2][0];
	}
	else
	{
		k20 = 1e+6;
	}
	if(base_line[0][1] > base_line[1][1])
	{
		start_point = base_line[0][1];
		end_point = base_line[1][1];
	}

	else
	{
		start_point = base_line[1][1];
		end_point = base_line[0][1];
	}
	//�����������Ǵ�ֱ��
	if(k20 > 1e+5 && k13 > 1e+5)
	{
		dis = base_length_y / (start_point - end_point + 1);
		calibration_base_line[max_calibration_height - 1] = near_point_length;
		for(i = max_calibration_height - 2; i >= 0; i--)
		{
			calibration_base_line[i] = calibration_base_line[i + 1] + dis * pCfgs->scale_y;
			dis_pixel[i] = dis * pCfgs->scale_y;
		}
	}
	else//����ʧ������
	{
		if(k20 > 1e+5)
		{
			x0 = calibration_point[0][0];
			y0 = k13 * x0 + b13;
		}
		else if(k13 > 1e+5)
		{
			x0 = calibration_point[1][0];
			y0 = k20 * x0 + b20;
		}
		else
		{
			x0 = (b13 - b20) / (k20 - k13);
			y0 = k13 * x0 + b13;
		}
		//�Ի�׼���Ͻ��б궨
		temp1 = 0, temp2 = 0;//ka + kb/(y- y0)
		/*temp1 = (float)(imgH - 1 - end_point) / (float)(end_point - y0);
		temp2 = (float)(imgH - 1 - start_point) / (float)(start_point - y0);
		dis = (float)((base_line[1][0] - base_line[0][0]) * (base_line[1][0] - base_line[0][0]));
		dis = (float)(dis + (base_line[1][1] - base_line[0][1]) * (base_line[1][1] - base_line[0][1]));
		dis = sqrt(dis);
		base_length_y = base_length_y * (float)(start_point - end_point) / dis;
		dis = base_length_y / (temp1 - temp2);
		calibration_base_line[imgH - 1] = near_point_length;
		for(i = imgH - 2; i >= 0; i--)
		{
			calibration_base_line[i] = near_point_length + dis * (float)(imgH - 1 - i) / (float)(i - y0);
			dis_pixel[i] = calibration_base_line[i] - calibration_base_line[i + 1];
			if(calibration_base_line[i] > 200)
			{
				calibration_base_line[i] = 200.0;
				break;
			}
		}
		for(j = i - 1; j >= 0; j--)//������200m��
		{
			calibration_base_line[j] = 200.0;
			dis_pixel[j] = 0;
		}*/
		//dis = ka + kb/(y - y0),һά����ģ��
		temp1 = 1.0 / (float)(end_point - y0);
		temp2 = 1.0 / (float)(start_point - y0);
		dis = (float)((base_line[1][0] - base_line[0][0]) * (base_line[1][0] - base_line[0][0]));
		dis = (float)(dis + (base_line[1][1] - base_line[0][1]) * (base_line[1][1] - base_line[0][1]));
		dis = sqrt(dis);
		base_length_y = base_length_y * (float)(start_point - end_point) / dis;
		dis = base_length_y / (temp1 - temp2);
		calibration_base_line[max_calibration_height - 1] = near_point_length;
		for(i = max_calibration_height - 2; i >= 0; i--)
		{
			calibration_base_line[i] = calibration_base_line[i + 1] + dis / (float)(i * pCfgs->scale_y - y0) - dis /(float)((i + 1) * pCfgs->scale_y - y0);
			dis_pixel[i] = calibration_base_line[i] - calibration_base_line[i + 1];
			//printf("[%d, %f]", i, calibration_base_line[i]);
			if(calibration_base_line[i] > 200)
			{
				calibration_base_line[i] = 200.0;
				break;
			}
		}
		for(j = i - 1; j >= 0; j--)//������200m��
		{
			calibration_base_line[j] = 200.0;
			dis_pixel[j] = 0;
			//printf("[%d, %f]", i, calibration_base_line[i]);
		}
	}

	//ÿ���������б궨
	if(abs(base_line[1][0] - base_line[0][0]) < 5)
	{
		pt_x = base_line[0][0];
		pt_y = k32 * pt_x + b32;

	}
	else
	{
		k_base = 1.0 * (base_line[1][1] - base_line[0][1]) / (base_line[1][0] - base_line[0][0]);
		b_base = base_line[1][1] - k_base * base_line[1][0];
		pt_x = (b32 - b_base) / (k_base - k32 + 0.0000001);
		pt_y = k32 * pt_x + b32;
	}
	//pt_x = x0;
	//pt_y = k32 * pt_x + b32;
	for(i = 0; i < laneNum; i++)
	{

		pt1_x = (pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[0].x + pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[1].x) / 2;
		pt1_y = (pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[0].y + pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[1].y) / 2;
		pt2_x = (pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[2].x + pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[3].x) / 2;
		pt2_y = (pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[2].y + pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[3].y) / 2;
		if(pt1_x != pt2_x)
		{
			k_road = 1.0 * (pt1_y - pt2_y) / (pt1_x - pt2_x);
			b_road = pt1_y - k_road * pt1_x;
			//�󽻵�
			pt3_x = (b32-b_road) / (k_road - k32);
			pt3_y = k32 * pt3_x + b32;
		}
		else
		{
			//�󽻵�
			pt3_x = pt1_x;
			pt3_y = k32 * pt3_x + b32;
		}
		temp = (int)((pt3_y + 0.5) / pCfgs->scale_y);
		temp = MAX(0, temp);
		temp = MIN(temp, max_calibration_height - 1);
		temp0 = MAX(0, (int)((pt_y + 0.5) / pCfgs->scale_y));
		temp0 = MIN(temp0, max_calibration_height - 1);
		pCfgs->actual_distance[i][temp] = calibration_base_line[temp0];
		for(j = temp + 1; j < max_calibration_height; j++)
		{
			pCfgs->actual_distance[i][j] = pCfgs->actual_distance[i][j - 1] - dis_pixel[j - 1];
		}
		for(j = temp - 1; j >= 0; j--)
		{
			pCfgs->actual_distance[i][j] = pCfgs->actual_distance[i][j + 1] + dis_pixel[j];
		}
	}
	/*for(j = 0; j < imgH; j++)
	{
		pCfgs->actual_degree_length[j] = pCfgs->actual_distance[laneNum - 1][j];
		//printf("[%d,%f]",j,pCfgs->actual_degree_length[j]);
	}*/
	//������ͼ��������б궨
	for(i = 0; i < max_calibration_width; i++)
	{
		if(k32 > 1e+5)//��ֱ��
		{

		}
		else
		{
			pt3_y = k32 * i * pCfgs->scale_x + b32;
		}
		temp = (int)((pt3_y + 0.5) / pCfgs->scale_y);
		temp = MAX(0, temp);
		temp = MIN(temp, max_calibration_height - 1);
		temp0 = MAX(0, (int)((pt_y + 0.5) / pCfgs->scale_y));
		temp0 = MIN(temp0, max_calibration_height - 1);
		pCfgs->image_actual[temp][i][1] = calibration_base_line[temp0];
		for(j = temp + 1; j < max_calibration_height; j++)
		{
			pCfgs->image_actual[j][i][1] = pCfgs->image_actual[j - 1][i][1] - dis_pixel[j - 1];
		}
		for(j = temp - 1; j >= 0; j--)
		{
			pCfgs->image_actual[j][i][1] = pCfgs->image_actual[j + 1][i][1] + dis_pixel[j];
		}
	}
}
//�õ�ʵ�ʵ㵽ͼ����ӳ�����
static void acutal_to_image(ALGCFGS *pCfgs, int imgW, int imgH)
{
	int i = 0, j = 0;
	int max_calibration_width = MIN(MAX_IMAGE_WIDTH, imgW);//���궨���
	int max_calibration_height = MIN(MAX_IMAGE_HEIGHT, imgH);//���궨�߶�
	//ʵ�����굽ͼ�������ӳ��
	pCfgs->mapping_ratio[0] = 1999.0 / abs(pCfgs->image_actual[0][0][0] - pCfgs->image_actual[0][max_calibration_width - 1][0] + 0.0000001);//x�����ӳ�����
	pCfgs->mapping_ratio[1] = 1999.0 / abs(pCfgs->image_actual[0][max_calibration_width - 1][1] - pCfgs->image_actual[max_calibration_height - 1][max_calibration_width - 1][1] + 0.0000001);//y�����ӳ�����
	pCfgs->actual_origin[0] = (pCfgs->image_actual[0][0][0] < pCfgs->image_actual[0][max_calibration_width - 1][0])? pCfgs->image_actual[0][0][0] : pCfgs->image_actual[0][max_calibration_width - 1][0];//x�������Сֵ
	pCfgs->actual_origin[1] = (pCfgs->image_actual[0][max_calibration_width - 1][1] < pCfgs->image_actual[max_calibration_height - 1][max_calibration_width - 1][1])? pCfgs->image_actual[0][max_calibration_width - 1][1] : pCfgs->image_actual[max_calibration_height - 1][max_calibration_width - 1][1];//y�������Сֵ
	printf("x = [%f,%f], y = [%f,%f]\n", pCfgs->image_actual[0][0][0], pCfgs->image_actual[0][max_calibration_width - 1][0], pCfgs->image_actual[0][max_calibration_width - 1][1], pCfgs->image_actual[max_calibration_height - 1][max_calibration_width - 1][1]);
	printf("min value =[%f,%f], ratio = [%f,%f]\n", pCfgs->actual_origin[0], pCfgs->actual_origin[1], pCfgs->mapping_ratio[0], pCfgs->mapping_ratio[1]);
	//��ʼ��ӳ�����
	for(i = 0; i < 2000; i++)
	{
		for(j = 0; j < 2000; j++)
		{
			pCfgs->actual_image[i][j][0] = -1;
			pCfgs->actual_image[i][j][1] = -1;
		}
	}
	float actual_x1,actual_x2, actual_x3, actual_y1, actual_y2, actual_y3;
	int x1, x2, x3, y1, y2, y3;
	int top, bottom, left, right;
	//��ʵ��ֵ��ͼ����ӳ�����
	for(i = 0; i < max_calibration_height; i++)
	{
		for(j = 0; j < max_calibration_width; j++)
		{
			actual_x1 = pCfgs->image_actual[i][j][0] - pCfgs->actual_origin[0];
			actual_y1 = pCfgs->image_actual[i][j][1] - pCfgs->actual_origin[1];
			actual_x1 = actual_x1 * pCfgs->mapping_ratio[0];
			actual_y1 = actual_y1 * pCfgs->mapping_ratio[1];
			x1 = (int)actual_x1;
			y1 = (int)actual_y1;
			if(j == 0)//��߽�
			{
				actual_x2 = actual_x1;
				actual_x3 = pCfgs->image_actual[i][j + 1][0] - pCfgs->actual_origin[0];
				actual_x3 = actual_x3 * pCfgs->mapping_ratio[0];
			}
			else if(j == (max_calibration_width - 1))//�ұ߽�
			{
				actual_x2 = pCfgs->image_actual[i][j - 1][0] - pCfgs->actual_origin[0];
				actual_x2 = actual_x2 * pCfgs->mapping_ratio[0];
				actual_x3 = actual_x1;
			}
			else//����
			{
				actual_x2 = pCfgs->image_actual[i][j - 1][0] - pCfgs->actual_origin[0];
				actual_x2 = actual_x2 * pCfgs->mapping_ratio[0];
			    actual_x3 = pCfgs->image_actual[i][j + 1][0] - pCfgs->actual_origin[0];
				actual_x3 = actual_x3 * pCfgs->mapping_ratio[0];
			}
			if(i == 0)//�ϱ߽�
			{
				actual_y2 = actual_y1;
				actual_y3 = pCfgs->image_actual[i + 1][j][1] - pCfgs->actual_origin[1];
				actual_y3 = actual_y3 * pCfgs->mapping_ratio[1];
			}
			else if(i == (max_calibration_height - 1))//�±߽�
			{
				actual_y2 = pCfgs->image_actual[i - 1][j][1] - pCfgs->actual_origin[1];
				actual_y2 = actual_y2 * pCfgs->mapping_ratio[1];
				actual_y3 = actual_y1;
			}
			else//����
			{
				actual_y2 = pCfgs->image_actual[i - 1][j][1] - pCfgs->actual_origin[1];
				actual_y3 = pCfgs->image_actual[i + 1][j][1] - pCfgs->actual_origin[1];
				actual_y2 = actual_y2 * pCfgs->mapping_ratio[1];
				actual_y3 = actual_y3 * pCfgs->mapping_ratio[1];
			}
			x2 = (int)actual_x2;
			y2 = (int)actual_y2;
			x3 = (int)actual_x3;
			y3 = (int)actual_y3;
			if(x1 >= 0 && x1 < 2000 && y1 >= 0 && y1 < 2000 && x2 >= 0 && x2 < 2000 && y2 >= 0 && y2 < 2000)
			{
				left = ((x1 + x2) / 2 < (x1 + x3)/ 2)? (x1 + x2) / 2 : (x1 + x3)/ 2;
				right = ((x1 + x2) / 2 > (x1 + x3)/ 2)? (x1 + x2) / 2 : (x1 + x3)/ 2;
				top = ((y1 + y2) / 2 < (y1 + y3)/ 2)? (y1 + y2) / 2 : (y1 + y3)/ 2;
				bottom = ((y1 + y2) / 2 > (y1 + y3)/ 2)? (y1 + y2) / 2 : (y1 + y3)/ 2;
				for(int m = left; m <= right; m++)
				{
					for(int n = top; n <= bottom; n++) 
					{
						pCfgs->actual_image[n][m][0] = int(j * pCfgs->scale_x);
						pCfgs->actual_image[n][m][1] = int(i * pCfgs->scale_y);
					}
				}
			}
		}
	}
	/*int ratio = 2;//��ʵ�����굽ͼ���ϵ�ӳ�� 0.5mӳ��һ��
	for(j = 0; j < 200; j++)
	{
		//x�����ӳ��
		dis_x = (float)j / ratio + pCfgs->actual_origin[1]; //0.5m���ӳ��һ��
		float min_dis = 1e6;
		int img_x = 0, img_y = 0;
		for(k = 0; k < imgW; k++)//x����
		{
			dis = abs(dis_x - pCfgs->image_actual[0][k][0]);
			if(dis < min_dis)
				min_dis = dis;
			else
				break;		
		}
		img_x = (k <= 0)? 0 : (k - 1);
		for(i = 0; i < 400; i++)
		{
			pCfgs->actual_image[i][j][0] = -1;//x����
			pCfgs->actual_image[i][j][1] = -1;//y����
			if(min_dis < 1)
				pCfgs->actual_image[i][j][0] = img_x;
			else
				break;
			//y�����ӳ��
			dis_y = (float)i / ratio + pCfgs->actual_origin[0]; //0.5m���ӳ��һ��
			if(dis_y < pCfgs->image_actual[imgH - 1][img_x][1] || dis_y > pCfgs->image_actual[0][img_x][1])//������Χ
				continue;
			min_dis = 1e6;
			for(k = 0; k < imgH; k++)
			{
				dis = abs(dis_y - pCfgs->image_actual[k][img_x][1]);
				if(dis < min_dis)
					min_dis = dis;
				else
					break;		
			}
			img_y = (k <= 0)? 0 : (k - 1);
			pCfgs->actual_image[i][j][1] = img_y;
		}
	}*/
}
//�����ͼ����б궨
void camera_calibration(int base_line[][2], float* base_length, int calibration_point[][2], float near_point_length, int laneNum, ALGCFGS *pCfgs, int imgW, int imgH)
{
	//�Ա궨������������, �궨����0����1����3����2����0
	sort_point(calibration_point, 4);
	//y����ı궨
	camera_calibration_y(base_line, base_length, calibration_point, near_point_length, laneNum, pCfgs, imgW, imgH);
	//x����ı궨
	camera_calibration_x(base_line, base_length, calibration_point, near_point_length, laneNum, pCfgs, imgW, imgH);
	//ʵ�ʵ㵽ͼ����ӳ��
	acutal_to_image(pCfgs, imgW, imgH);
}
#if 0
//�����ͼ����б궨
//camera_height �������װ��ʵ�ʸ߶�
//near_point_length ͼ������˵������ˮƽ����
//far_point_length ͼ����Զ�˵������ˮƽ����
void camera_calibration(float camera_height, float near_point_length, float far_point_length, ALGCFGS *pCfgs, int imgW, int imgH)
{
	int i = 0, j = 0;
	float angle1,angle2, angle3;
	float m;//�����ͼ��ƽ��Ĵ�ֱ����
	angle1 = atan(near_point_length / camera_height);//�����������ļн�
	angle2 = atan(far_point_length / camera_height);//��Զ��������ļн�
	m = imgH / (2 * tan((angle2 - angle1)/2));
	for(i = 0; i < imgH; i++)
	{
		angle3 = angle1 + (angle2 - angle1) / 2 + atan(imgH / 2 - i) / m;
		for(j = 0; j < pCfgs->LaneAmount; j++)
		{
			pCfgs->actual_distance[j][i] = tan(angle3) * camera_height;
		}
	}
}
#endif
//͸�ӱ任���б궨
void camera_calibration_transform(int base_line[][2], float* base_length, int calibration_point[][2], float near_point_length, ALGCFGS *pCfgs, int imgW, int imgH)
{
	int i = 0, j = 0, k = 0;
	//��⳵��������б��
	float k_road = 0, b_road = 0;
	float pt1_x = 0, pt1_y = 0, pt2_x = 0, pt2_y = 0, pt3_x = 0, pt3_y = 0;
	int max_calibration_height = MIN(MAX_IMAGE_HEIGHT, imgH);//���궨�߶�
	int max_calibration_width = MIN(MAX_IMAGE_WIDTH, imgW);//���궨���
	cv::Point2f src_vertices[4], dst_vertices[4];
	//�Ա궨������������
	sort_point(calibration_point, 4);
	//�任ǰ�������
	src_vertices[0] = cv::Point2f(calibration_point[0][0], calibration_point[0][1]);
	src_vertices[1] = cv::Point2f(calibration_point[1][0], calibration_point[1][1]);	
	src_vertices[2] = cv::Point2f(calibration_point[2][0], calibration_point[2][1]);	
	src_vertices[3] = cv::Point2f(calibration_point[3][0], calibration_point[3][1]);

	dst_vertices[0] = cv::Point2f(calibration_point[0][0], calibration_point[0][1]);		
	dst_vertices[1] = cv::Point2f(calibration_point[1][0] + 10, calibration_point[0][1]);		
	dst_vertices[2] = cv::Point2f(calibration_point[0][0], calibration_point[2][1] + 30);		
	dst_vertices[3] = cv::Point2f(calibration_point[1][0] + 10, calibration_point[2][1] + 30);
	//���ݱ궨�㵽ʵ�ʾ��ν���͸�ӱ任
	cv::Mat transform = cv::getPerspectiveTransform(src_vertices, dst_vertices);
	//��ͼ������͸�ӱ任
	std::vector<Point2f> points, points_trans;  
	for(i = 0; i < imgH; i++)
	{  
		for(j = 0; j < imgW; j++){  
			points.push_back(Point2f(j, i));  
		}  
	}  
	cv::perspectiveTransform( points, points_trans, transform);
	//��û�׼���ϵ�͸�ӵ�
	float dis_y = 0;
	cv::Point2f base_vertices[2];
	cv::Point2f pt_bottom;
	base_vertices[0] = points_trans[base_line[0][1] * imgW + base_line[0][0]];
	base_vertices[1] = points_trans[base_line[1][1] * imgW + base_line[1][0]];
	pt_bottom = points_trans[(imgH - 1) * imgW + imgW / 2];//ͼ�����¶˵��е�

	if(abs(base_vertices[1].y - base_vertices[0].y) < 0.0001)
		dis_y = 0;
	else 
		dis_y = base_length[0] / abs(base_vertices[1].y - base_vertices[0].y);
	//�õ����е��ʵ�ʾ��룬��Ϊͼ�����¶��е�Ϊ�������������
	for(i = 0; i < max_calibration_width; i++)
	{
		for(j = max_calibration_height - 1; j >= 0; j--)
		{
			pCfgs->image_actual[j][i][1] = near_point_length + (pt_bottom.y - points_trans[j * pCfgs->scale_y * imgW + i * pCfgs->scale_x].y) * dis_y;
			if(pCfgs->image_actual[j][i][1] > 200)
			{
				pCfgs->image_actual[j][i][1] = 200.0;
				break;
			}
		}
		for(k = j; k >= 0; k--)//������200m��
		{
			pCfgs->image_actual[k][i][1] = 200.0;
		}
	}
	//�Գ������Ͻ��б궨
	for(i = 0; i < pCfgs->LaneAmount; i++)
	{

		pt1_x = (pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[0].x + pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[1].x) / 2;
		pt1_y = (pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[0].y + pCfgs->DownDetectCfg.SpeedEachLane[i].MiddleCoil[1].y) / 2;
		pt2_x = (pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[2].x + pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[3].x) / 2;
		pt2_y = (pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[2].y + pCfgs->DownDetectCfg.SpeedEachLane[i].RearCoil[3].y) / 2;
		if(pt1_x != pt2_x)
		{
			k_road = 1.0 * (pt1_y - pt2_y) / (pt1_x - pt2_x);
			b_road = pt1_y - k_road * pt1_x;
			for(j = 0; j < max_calibration_height; j++)
			{
				pt3_x = (j * pCfgs->scale_y - b_road) / k_road;
				pt3_x = pt3_x / pCfgs->scale_x;
				if(pt3_x < 1e-6) pt3_x = 0;
				if(pt3_x > (max_calibration_width - 1)) pt3_x = max_calibration_width - 1;
				pCfgs->actual_distance[i][j] = pCfgs->image_actual[j][int(pt3_x)][1];
			}
		}
		else
		{
			pt3_x = pt2_x;
			pt3_x = pt3_x / pCfgs->scale_x;
			for(j = 0; j < max_calibration_height; j++)
			{
				if(pt3_x < 1e-6) pt3_x = 0;
				if(pt3_x > (max_calibration_width - 1)) pt3_x = max_calibration_width - 1;
				pCfgs->actual_distance[i][j] = pCfgs->image_actual[j][int(pt3_x)][1];
			}

		}
	}
	/*if(pCfgs->LaneAmount > 0)
	{
		for(j = 0; j < imgH; j++)
		{
			pCfgs->actual_degree_length[j] = pCfgs->actual_distance[pCfgs->LaneAmount - 1][j];
		}
	}*/
	//x����ı궨
	camera_calibration_x(base_line, base_length, calibration_point, near_point_length, pCfgs->LaneAmount, pCfgs, imgW, imgH);
	/*float dis_x = 0;
	base_vertices[0] = points_trans[base_line[2][1] * imgW + base_line[2][0]];
	base_vertices[1] = points_trans[base_line[3][1] * imgW + base_line[3][0]];
	pt_bottom = base_vertices[0];//��׼����˵�Ϊԭ��

	if(abs(base_vertices[1].x - base_vertices[0].x) < 0.0001)
		dis_x = 0;
	else 
		dis_x = base_length[1] / abs(base_vertices[1].x - base_vertices[0].x);
	//�õ����е��ʵ�ʾ��룬��Ϊͼ�����¶��е�Ϊ�������������
	for(j = 0; j < max_calibration_height; j++)
	{
		for(i = 0; i < max_calibration_width; i++)		
		{
			pCfgs->image_actual[j][i][0] = (pt_bottom.x - points_trans[j * pCfgs->scale_y * imgW + i * pCfgs->scale_x].x) * dis_x;
			printf("[%d,%f] ", i, pCfgs->image_actual[j][i][0]);
		}
		printf("\n\n\n");
	}*/
	//ʵ�ʵ㵽ͼ����ӳ��
	acutal_to_image(pCfgs, imgW, imgH);
}