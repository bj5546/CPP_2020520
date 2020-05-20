#include "../include/PlateRecognize.h"
#include "../include/Pipeline.h"
using namespace std;
typedef pr::PipelinePR* pPipelinePR;
#define PLATE_RECOGNIZE_NET_NUM 8//����Ƽ��������
pPipelinePR prc[PLATE_RECOGNIZE_NET_NUM];
int plate_init_flag[PLATE_RECOGNIZE_NET_NUM] = { 0 };
/*pr::PipelinePR prc("model/cascade.xml",
				   "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
				   "model/Segmentation.prototxt","model/Segmentation.caffemodel",
				   "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
				   "model/SegmenationFree-Inception.prototxt","model/SegmenationFree-Inception.caffemodel"
				   );*/
int LoadPlateNet()//���س�������
{
	int flag = -1;
	int i = 0;
	for(i = 0; i < PLATE_RECOGNIZE_NET_NUM; i++)
	{
		if(plate_init_flag[i] == 0)//��������
		{
			prc[i] = new pr::PipelinePR("model/cascade.xml",
				"model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
				"model/Segmentation.prototxt","model/Segmentation.caffemodel",
				"model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
				"model/SegmenationFree-Inception.prototxt","model/SegmenationFree-Inception.caffemodel"
				);
			plate_init_flag[i] = 1;
			flag = i;
			return flag;
		}
	}
	return flag;
}
int FreePlateNet(int flag)//�ͷų�������
{
	prc[flag]->~PipelinePR();
	if(plate_init_flag[flag] == 1)
	{
		plate_init_flag[flag] = 0;//����־����Ϊ0
	}
	return 0;
}
int PlateDetectandRecognize(cv::Mat image, const int segmentation_method, PlateInfo*  plateInfo, int flag)//����ʶ��
{
	int i = 0;
	int plateNum = 0;
	std::vector<pr::PlateInfo> res = prc[flag]->RunPiplineAsImage(image, segmentation_method);
	plateNum = res.size();
	for(i = 0; i < plateNum; i++)
	{
		cv::Rect rct = res[i].getPlateRect();//��������
		cv::String str = res[i].getPlateName();//���ƺ�
		plateInfo[i].plateRect.x = rct.x;
		plateInfo[i].plateRect.y = rct.y;
		plateInfo[i].plateRect.width = rct.width;
		plateInfo[i].plateRect.height = rct.height;
		strcpy(plateInfo[i].plateName, str.c_str());
		plateInfo[i].plateType = (PlateColor)(res[i].getPlateType());//������ɫ
		plateInfo[i].confidence = res[i].confidence;//���Ŷ�

	}
	return plateNum;
}
PlateInfo PlateRecognizeOnly(cv::Mat plateROI, const int segmentation_method, int flag)//����ʶ��
{
	int i = 0;
	PlateInfo plateInfo;
	pr::PlateInfo res = prc[flag]->RunPiplineAsPlate(plateROI, segmentation_method);

	cv::Rect rct = res.getPlateRect();//��������
	cv::String str = res.getPlateName();//���ƺ�
	plateInfo.plateRect.x = rct.x;
	plateInfo.plateRect.y = rct.y;
	plateInfo.plateRect.width = rct.width;
	plateInfo.plateRect.height = rct.height;
	strcpy(plateInfo.plateName, str.c_str());
	plateInfo.plateType = (PlateColor)(res.getPlateType());//������ɫ
	plateInfo.confidence = res.confidence;//���Ŷ�
	printf("plate %s %f\n",plateInfo.plateName,plateInfo.confidence);
	return plateInfo;
}
