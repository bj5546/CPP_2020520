#ifdef DETECT_PERSON_ATTRIBUTE
#include "attribute_detect.h"
#define StandardHeight 200
int attri_init_flag = 0;//���߳�ִֻ��һ��,��ʼ��ȫ�ֱ���
///////////////////////////////////////////////////////////////////////////�������Գ�ʼ��
bool HumanAttributeInit(ALGCFGS *pCfgs)
{
	pCfgs->uPersonNum = 0;//������
	memset(pCfgs->PersonAttributeBox, 0, MAX_PERSON_NUM * sizeof(HumanAttributeBox));//��������
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////�������Գ�ʼ��
bool BicycleAttributeInit(ALGCFGS *pCfgs)
{
	pCfgs->uBicycleNum = 0;//������
	memset(pCfgs->BikeAttributeBox, 0, MAX_BICYCLE_NUM * sizeof(BicycleAttributeBox));//��������
	return TRUE;
}
#ifndef USE_PYTHON
#include <pthread.h>
#define  NET_NUM  8
#define MAX_GPU_NUM 8
#define DARKNET_NET_NUM  8 //���ٸ�darknet net ����һ��������������
int darknet_num = 0;//���ص�darknet����
int human_attri_init[MAX_GPU_NUM];//��ÿ��gpu��ʼ��һ��������������
pthread_t attri_detect_thread[NET_NUM];
int init_attri_lock[NET_NUM];
pthread_mutex_t attri_lock[NET_NUM];
typedef struct attri_detect_args{
	int thread_idx;//�߳�ID
	IplImage* imgROI;//ͼ������
	int net_idx;//�������id
	int* result;//������
} attri_detect_args;
//////////////////////////////////////////////////////////////////////////////caffe c++���м��
int attri_net_num = 0;
int detect_net_idx = 0;
const char* deploy_file = "deploy_mcnn_Attri.prototxt";
const char* trained_file = "mcnnsolver_iter_15000.caffemodel";
//const char* deploy_file = "deploy.prototxt";
//const char* trained_file = "solver_iter_40000.caffemodel";

void attri_detect_in_thread(void* ptr)
{
	attri_detect_args args  = *(attri_detect_args *)ptr;
	int thread_id = args.thread_idx;
	unsigned char* imgdata = (unsigned char* )(args.imgROI->imageData);
	int w = args.imgROI->width;
	int h = args.imgROI->height;
	int net_idx = args.net_idx;
	int* result = args.result;
	if(init_attri_lock[thread_id] == 0)
	{
		pthread_mutex_init(&(attri_lock[thread_id]), NULL);
		init_attri_lock[thread_id] = 1;
	}	
	pthread_mutex_lock(&(attri_lock[thread_id]));
	AttriDetect(imgdata, w, h, net_idx, result);
	pthread_mutex_unlock(&(attri_lock[thread_id]));

}
void LoadAttriNet(int gpu_idx)
{
	//��������ʶ������
	if(attri_net_num >= NET_NUM)//���ﵽ�����������ʱ������������
		return;
	if(human_attri_init[gpu_idx] == 0)//��gpuδ����������������
	{
		LoadAttriNet(deploy_file, trained_file, gpu_idx, attri_net_num);//����������������
		human_attri_init[gpu_idx] = 1;//���ô�gpu�м�������
	}
	attri_net_num++;//��������ʶ������
}
///////////////////////////////////////////////////////////////////////////��������ʶ��
/*HumanAttribute HumanAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs)
{
	int result[10] = { 0 };
	struct attri_detect_args attri_arg;
	attri_arg.thread_idx = detect_net_idx;
	attri_arg.imgROI = imgROI;
	attri_arg.net_idx = detect_net_idx;
	attri_arg.result = result;
	if(pthread_create(&attri_detect_thread[detect_net_idx], 0, attri_detect_in_thread, &attri_arg)) 
		error("Thread creation failed");
	pthread_join(attri_detect_thread[detect_net_idx], 0);

	detect_net_idx++;
	detect_net_idx = (detect_net_idx >= attri_net_num)? 0 : detect_net_idx; //�����ĸ��������ʶ��
	HumanAttribute val;
	val.age = result[0];//����
	val.sex = result[1];//�Ա�
	val.uppercolor = result[2];//������ɫ
	val.lowercolor = result[3];//������ɫ
	val.shape = result[4];//����
	val.head = result[5];//ͷ��
	val.glasses = result[6];//�۾�
	val.upstyle = result[7];//��������
	val.lowerstyle = result[8];//��������
	val.face = result[9];//����
	if(imgROI)
	{
		cvReleaseImage(&imgROI);
		imgROI = NULL;
	}
	return val;
}*/
HumanAttribute HumanAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs)
{
	int result[10] = { 0 };
	if(init_attri_lock[detect_net_idx] == 0)
	{
		pthread_mutex_init(&(attri_lock[detect_net_idx]), NULL);
		init_attri_lock[detect_net_idx] = 1;
	}	
	pthread_mutex_lock(&(attri_lock[detect_net_idx]));
	AttriDetect((unsigned char*)imgROI->imageData, imgROI->width, imgROI->height, detect_net_idx, result);
	pthread_mutex_unlock(&(attri_lock[detect_net_idx]));
	detect_net_idx++;
	detect_net_idx = (detect_net_idx >= attri_net_num)? 0 : detect_net_idx; //�����ĸ��������ʶ��
	HumanAttribute val;
	val.age = result[0];//����
	val.sex = result[1];//�Ա�
	val.uppercolor = result[2];//������ɫ
	val.lowercolor = result[3];//������ɫ
	val.shape = result[4];//����
	val.head = result[5];//ͷ��
	val.glasses = result[6];//�۾�
	val.upstyle = result[7];//��������
	val.lowerstyle = result[8];//��������
	val.face = result[9];//����
	if(imgROI)
	{
		cvReleaseImage(&imgROI);
		imgROI = NULL;
	}
	return val;
}
///////////////////////////////////////////////////////////////////////////��������ʶ��
BicycleAttribute BicycleAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs)
{

	/*int result[10] = { 0 };
	AttriDetect((unsigned char*)imgROI, imgROI->width, imgROI->height, detect_net_idx, result);
	detect_net_idx++;
	detect_net_idx = (detect_net_idx >= attri_net_num)? 0 : detect_net_idx; //�����ĸ��������ʶ��*/
	BicycleAttribute val;
	return val;
}
#else
#include "pthread.h"
#include "Python.h"
#include <numpy/arrayobject.h>
//////////////////////////////////////////////////////////////////////////////����python
PyObject *pModule;
PyGILState_STATE gstate;
int AttributeDetectWidth = 0;//����ʶ����
int AttributeDetectHeight = 0;//����ʶ��߶�
int init_numpy()
{
	import_array();
	return 1;
}
void py_attri_init()
{
	Py_Initialize();
	if ( !Py_IsInitialized() ) {
		printf("init err\n");
	}else{
		printf("init ok\n");
	}
	printf("finding ...\n");
	init_numpy();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	if(pModule)
		Py_DECREF(pModule);
	pModule = PyImport_ImportModule("tttest");
	if ( !pModule ) {
		printf("can't find .py");
	}else{
		printf("py found\n");
	}
	//����������������
	PyObject* result;
	PyObject* pFunc;
	pFunc = PyObject_GetAttrString(pModule, "init");
	result = PyObject_CallObject(pFunc, NULL);//��ʼ�������������������磬����������������ͼ��ߴ�
	PyObject* ret_objs;
	PyArg_Parse(result, "O!", &PyList_Type, &ret_objs);
	AttributeDetectWidth = PyLong_AsLong(PyList_GetItem(ret_objs,0));
	AttributeDetectHeight = PyLong_AsLong(PyList_GetItem(ret_objs,1));
	Py_DECREF(result);
	Py_DECREF(pFunc);
	PyEval_InitThreads(); 
	PyEval_ReleaseThread(PyThreadState_Get()); 

}
///////////////////////////////////////////////////////////////////////////��������ʶ��
HumanAttribute HumanAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs)
{
	int width = AttributeDetectWidth;//��������ʶ����
	int height = AttributeDetectHeight;//��������ʶ��߶�
	HumanAttribute val;
	IplImage* imgROIResize = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	cvResize(imgROI, imgROIResize, CV_INTER_LINEAR);
	//cvSaveImage("roi.jpg", imgROIResize, 0);
	//�����ݸ�python,���м��
	unsigned char* imagedata = (unsigned char *)malloc(width * height * 3);
	memcpy(imagedata, imgROIResize->imageData, width * height * 3);
	npy_intp Dims[3]= { height, width, 3}; //����ά����Ϣ
	gstate = PyGILState_Ensure();   //���û��GIL���������ȡGIL
	//Py_BEGIN_ALLOW_THREADS;
	//Py_BLOCK_THREADS;
	PyObject* PyListRGB = PyArray_SimpleNewFromData(3, Dims, NPY_UBYTE, imagedata);
	PyObject* ArgList = PyTuple_New(1);
	PyTuple_SetItem(ArgList, 0, PyListRGB);//��PyList�������PyTuple������
	PyObject* pFunc = PyObject_GetAttrString(pModule, "classify");
	PyObject* Pyresult = PyObject_CallObject(pFunc, ArgList);//���ú�������ɴ���
	PyObject* ret_objs;
	PyArg_Parse(Pyresult, "O!", &PyList_Type, &ret_objs);
	val.age = PyLong_AsLong(PyList_GetItem(ret_objs, 0));//����
	val.sex = PyLong_AsLong(PyList_GetItem(ret_objs, 1));//�Ա�
	val.uppercolor = PyLong_AsLong(PyList_GetItem(ret_objs, 2));//������ɫ
	val.lowercolor = PyLong_AsLong(PyList_GetItem(ret_objs, 3));//������ɫ
	val.shape = PyLong_AsLong(PyList_GetItem(ret_objs, 4));//����
	val.head = PyLong_AsLong(PyList_GetItem(ret_objs, 5));//ͷ��
	val.glasses = PyLong_AsLong(PyList_GetItem(ret_objs, 6));//�۾�
	val.upstyle = PyLong_AsLong(PyList_GetItem(ret_objs, 7));//��������
	val.lowerstyle = PyLong_AsLong(PyList_GetItem(ret_objs, 8));//��������
	val.face = PyLong_AsLong(PyList_GetItem(ret_objs, 9));//����
	//printf("age = %d, sex = %d, uppercolor = %d, lowercolor = %d,shape = %d, head = %d, glasses = %d, upstyle = %d, lowerstyle =%d, face = %d\n", val.age, val.sex, val.uppercolor, val.lowercolor, val.shape, val.head, val.glasses, val.upstyle, val.lowerstyle, val.face);
	if(Pyresult)
		Py_DECREF(Pyresult);
	if(ArgList)
		Py_DECREF(ArgList);
	if(pFunc)
		Py_DECREF(pFunc);
	//Py_UNBLOCK_THREADS;  
	//Py_END_ALLOW_THREADS; 
	PyGILState_Release(gstate);    //�ͷŵ�ǰ�̵߳�GIL
	if(imagedata)
	{
		free(imagedata);
		imagedata = NULL;
	}
	if(imgROIResize)
	{
		cvReleaseImage(&imgROIResize);
		imgROIResize = NULL;
	}
	if(imgROI)
	{
		cvReleaseImage(&imgROI);
		imgROI = NULL;
	}
	return val;
}
///////////////////////////////////////////////////////////////////////////��������ʶ��
BicycleAttribute BicycleAttributeRecognition(IplImage* imgROI, ALGCFGS* pCfgs)
{

	int width = AttributeDetectWidth;//��������ʶ����
	int height = AttributeDetectHeight;//��������ʶ��߶�
	BicycleAttribute val;
	/*IplImage* imgROIResize = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	cvResize(imgROI, imgROIResize, CV_INTER_LINEAR);
	//cvSaveImage("roi.jpg", imgROIResize, 0);
	//�����ݸ�python,���м��
	unsigned char* imagedata = (unsigned char *)malloc(width * height * 3);
	memcpy(imagedata, imgROIResize->imageData, width * height * 3);
	npy_intp Dims[3]= { height, width, 3}; //����ά����Ϣ
	gstate = PyGILState_Ensure();   //���û��GIL���������ȡGIL
	Py_BEGIN_ALLOW_THREADS;
	Py_BLOCK_THREADS;
	PyObject* PyListRGB = PyArray_SimpleNewFromData(3, Dims, NPY_UBYTE, imagedata);
	PyObject* ArgList = PyTuple_New(1);
	PyTuple_SetItem(ArgList, 0, PyListRGB);//��PyList�������PyTuple������
	PyObject* pFunc = PyObject_GetAttrString(pModule, "classify");
	PyObject* Pyresult = PyObject_CallObject(pFunc, ArgList);//���ú�������ɴ���
	PyObject* ret_objs;
	PyArg_Parse(Pyresult, "O!", &PyList_Type, &ret_objs);
	if(Pyresult)
		Py_DECREF(Pyresult);
	if(ArgList)
		Py_DECREF(ArgList);
	if(pFunc)
		Py_DECREF(pFunc);
	Py_UNBLOCK_THREADS;  
	Py_END_ALLOW_THREADS; 
	PyGILState_Release(gstate);    //�ͷŵ�ǰ�̵߳�GIL
	if(imagedata)
	{
		free(imagedata);
		imagedata = NULL;
	}
	if(imgROIResize)
	{
		cvReleaseImage(&imgROIResize);
		imgROIResize = NULL;
	}*/
	return val;
}
#endif
//void HumanAttributeDetect(ALGCFGS *pCfgs, IplImage* img)//�������Լ�����
//{
//	int i = 0, j = 0;
//	int val1 = 0, val2 = 0;
//	//�����������Կ�Ϊδ���
//	for(i = 0; i < pCfgs->uPersonNum; i++)
//	{
//		pCfgs->PersonAttributeBox[i].detected = FALSE;
//	}
//	for(i = 0; i < pCfgs->event_targets_size; i++)
//	{
//		if(strcmp(pCfgs->event_targets[i].names, "person") != 0)
//			continue;
//		if(pCfgs->event_targets[i].continue_num < 10)//��ʼһ��ʱ�䲻������������ʶ��
//			continue;
//		//������������ʶ��
//		if(pCfgs->event_targets[i].attribute_detected == FALSE)//��Ŀ��û�н�����������ʶ��
//		{
//			//printf("detect person %d,%d,%d\n", pCfgs->event_targets[i].detected, (pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height), img->height / 2);
//			//val1 = pCfgs->k * (pCfgs->event_targets[i].box.x) + pCfgs->b - pCfgs->event_targets[i].box.y;
//			//val2 = pCfgs->k * (pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width) + pCfgs->b - pCfgs->event_targets[i].box.y - pCfgs->event_targets[i].box.height;
//			//if(pCfgs->event_targets[i].detected && (val1 * val2 < 0))//��⵽�����Ҽ����������ཻ
//			int bottom = pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height;
//			//if(pCfgs->event_targets[i].detected && bottom > img->height / 2 && bottom < (img->height - 10))//����ߵ�ͼ���¶�,���Ҳ���ͼ��߽�
//			{
//				CvRect roi = cvRect(pCfgs->event_targets[i].box.x, pCfgs->event_targets[i].box.y, pCfgs->event_targets[i].box.width, pCfgs->event_targets[i].box.height);
//				IplImage* imgROI = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 3);
//				//����ROI����  
//				/*cvSetImageROI(img, roi);   
//				printf("copy start\n");
//				cvCopy(img, imgROI, NULL);  
//				printf("copy end\n");
//				cvResetImageROI(img);*/
//				for(int ii = 0; ii < roi.height; ii++)
//				{
//					memcpy(imgROI->imageData + ii * imgROI->widthStep, img->imageData + (ii + roi.y) * img->widthStep + roi.x * 3, roi.width * 3);
//				}
//				HumanAttribute val = HumanAttributeRecognition(imgROI, pCfgs);//��������ʶ��
//				if(imgROI)
//				{
//					cvReleaseImage(&imgROI);
//					imgROI = NULL;
//				}
//				pCfgs->event_targets[i].attribute_detected = TRUE;
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.age = val.age;//����
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.sex = val.sex;//�Ա�
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.uppercolor = val.uppercolor;//������ɫ
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.lowercolor = val.lowercolor;//������ɫ
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.shape = val.shape;//����
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.head = val.head;//ͷ��
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.glasses = val.glasses;//�۾�
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.upstyle = val.upstyle;//��������
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.lowerstyle = val.lowerstyle;//��������
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.face = val.face;//����
//				//���ݱ�׼�߶����������
//				int height = (float)pCfgs->event_targets[i].box.height / (float)StandardHeight * 170;
//				height = (height > 180)? 180 : height;
//				height = (height < 150)? 150 : height;
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.height = height;//���
//				//�����ٶ�
//				float speed = 0;
//				int pos1 = pCfgs->event_targets[i].trajectory[0].y;
//				int pos2 = pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height / 2;
//				float len = (pos1 - pos2) * 1.7 / (pCfgs->event_targets[i].box.height);//�����˵����Ϊ1.7m,��������˵��˶�ʵ�ʾ���
//				speed = len * 3.6 / (pCfgs->currTime - pCfgs->event_targets[i].start_time);
//				speed = (speed < 0)? -speed : speed;
//				speed = speed + rand() % 5;//�ٶ�
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.speed = (speed > 15)? 15 : speed;
//				printf("speed = %d\n", pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.speed);
//				//������ʻ����,����Ϊ0������Ϊ1
//				int direction = (pCfgs->event_targets[i].trajectory[pCfgs->event_targets[i].trajectory_num - 1].y < pCfgs->event_targets[i].trajectory[0].y)? 1 : 0;//����
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.direction = direction;
//				//�ж��Ƿ�����
//				bool isCycling = FALSE;
//				for( j = 0; j < pCfgs->event_targets_size; j++)
//				{
//					if(strcmp(pCfgs->event_targets[j].names, "bicycle") != 0 && strcmp(pCfgs->event_targets[j].names, "motorbike") != 0)
//						continue;
//					CRect rct;
//					rct.x = pCfgs->event_targets[j].box.x + 10;
//					rct.y = pCfgs->event_targets[j].box.y;
//					rct.width = pCfgs->event_targets[j].box.width - 20;
//					rct.height = pCfgs->event_targets[j].box.height;
//					int overlapratio = overlapRatio(rct, pCfgs->event_targets[i].box);
//					if(overlapratio > 5)
//					{
//						isCycling = TRUE;//����
//						break;
//					}
//				}
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.isCycling = isCycling;//�Ƿ�����
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.box = pCfgs->event_targets[i].box;
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.flag = 1;//�¼������
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].detected = TRUE;
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].uBoxID = pCfgs->event_targets[i].target_id;
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].ReportTime = pCfgs->currTime;
//				pCfgs->PersonAttributeBox[pCfgs->uPersonNum].lost_detected = pCfgs->event_targets[i].lost_detected;
//				pCfgs->uPersonNum++;
//			}
//		}
//		else//��Ŀ���Ѿ���������������ʶ��
//		{
//			for(j = 0; j < pCfgs->uPersonNum; j++)
//			{
//				if(pCfgs->PersonAttributeBox[j].uBoxID == pCfgs->event_targets[i].target_id)//ͬһĿ��
//				{
//					pCfgs->PersonAttributeBox[j].AttributeInfo.box = pCfgs->event_targets[i].box;
//					pCfgs->PersonAttributeBox[j].lost_detected = pCfgs->event_targets[i].lost_detected;
//					pCfgs->PersonAttributeBox[j].AttributeInfo.flag = 0;//��������
//					pCfgs->PersonAttributeBox[j].detected = TRUE;
//					break;
//				}
//			}
//
//		}
//	}
//	//ȥ��û�м�⵽���������Կ�
//	for(i = 0; i < pCfgs->uPersonNum; i++)
//	{
//		if(pCfgs->PersonAttributeBox[i].detected == FALSE)
//		{
//			for( j = i + 1; j < pCfgs->uPersonNum; j++)
//			{
//				pCfgs->PersonAttributeBox[j - 1] = pCfgs->PersonAttributeBox[j];
//			}
//			i--;
//			pCfgs->uPersonNum--;
//		}
//		else
//		{
//			if(pCfgs->currTime - pCfgs->PersonAttributeBox[i].ReportTime > (5 * 60))//�ϱ����Ϊ5����
//			{
//				pCfgs->PersonAttributeBox[i].AttributeInfo.flag = 1;//�ϱ�
//				pCfgs->PersonAttributeBox[i].ReportTime = pCfgs->currTime;
//			}
//		}
//	}
//
//}
void HumanAttributeDetect(ALGCFGS *pCfgs, IplImage* img)//�������Լ�����
{
	int i = 0, j = 0, k = 0;
	int val1 = 0, val2 = 0;
	//�����������Կ�Ϊδ���
	for(i = 0; i < pCfgs->uPersonNum; i++)
	{
		pCfgs->PersonAttributeBox[i].detected = FALSE;
	}
	for(i = 0; i < pCfgs->objPerson_size; i++)
	{
		if(strcmp(pCfgs->objPerson[i].names, "person") != 0)
			continue;
		if(pCfgs->objPerson[i].cal_flow == FALSE)//û�н�������ͳ�ƣ���������������ʶ��
			continue;
		//������������ʶ��
		if(pCfgs->objPerson[i].attribute_detected == FALSE)//��Ŀ��û�н�����������ʶ��
		{
			CvRect roi = cvRect(pCfgs->objPerson[i].box.x, pCfgs->objPerson[i].box.y, pCfgs->objPerson[i].box.width, pCfgs->objPerson[i].box.height);
			IplImage* imgROI = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 3);
			//����ROI����  
			/*cvSetImageROI(img, roi);   
			printf("copy start\n");
			cvCopy(img, imgROI, NULL);  
			printf("copy end\n");
			cvResetImageROI(img);*/
			for(int ii = 0; ii < roi.height; ii++)
			{
				memcpy(imgROI->imageData + ii * imgROI->widthStep, img->imageData + (ii + roi.y) * img->widthStep + roi.x * 3, roi.width * 3);
			}
			HumanAttribute val = HumanAttributeRecognition(imgROI, pCfgs);//��������ʶ��
			pCfgs->objPerson[i].attribute_detected = TRUE;
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.age = val.age;//����
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.sex = val.sex;//�Ա�
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.uppercolor = val.uppercolor;//������ɫ
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.lowercolor = val.lowercolor;//������ɫ
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.shape = val.shape;//����
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.head = val.head;//ͷ��
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.glasses = val.glasses;//�۾�
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.upstyle = val.upstyle;//��������
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.lowerstyle = val.lowerstyle;//��������
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.face = val.face;//����
			//���ݱ�׼�߶����������
			int height = (float)pCfgs->objPerson[i].box.height / (float)StandardHeight * 170;
			height = (height > 180)? 180 : height;
			height = (height < 150)? 150 : height;
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.height = height;//���
			//�����ٶ�
			float speed = 0;
			int pos1 = pCfgs->objPerson[i].trajectory[0].y;
			int pos2 = pCfgs->objPerson[i].box.y + pCfgs->objPerson[i].box.height / 2;
			float len = (pos1 - pos2) * 1.7 / (pCfgs->objPerson[i].box.height);//�����˵����Ϊ1.7m,��������˵��˶�ʵ�ʾ���
			speed = len * 3.6 / (pCfgs->currTime - pCfgs->objPerson[i].start_time);
			speed = (speed < 0)? -speed : speed;
			speed = speed + rand() % 5;//�ٶ�
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.speed = (speed > 15)? 15 : speed;
			printf("speed = %d\n", pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.speed);
			//������ʻ����,����Ϊ0������Ϊ1
			int direction = (pCfgs->objPerson[i].trajectory[pCfgs->objPerson[i].trajectory_num - 1].y < pCfgs->objPerson[i].trajectory[0].y)? 1 : 0;//����
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.direction = direction;
			//�ж��Ƿ�����
			bool isCycling = FALSE;
			for( j = 0; j < pCfgs->classes; j++)
			{
				if(strcmp(pCfgs->detClasses[j].names, "bicycle") != 0 && strcmp(pCfgs->detClasses[j].names, "motorbike") != 0)
					continue;
				for(k = 0; k < pCfgs->detClasses[j].classes_num; k++)
				{
					CRect rct;
					rct.x = pCfgs->detClasses[j].box[k].x + 10;
					rct.y = pCfgs->detClasses[j].box[k].y;
					rct.width = pCfgs->detClasses[j].box[k].width - 20;
					rct.height = pCfgs->detClasses[j].box[k].height;
					int overlapratio = overlapRatio(rct, pCfgs->objPerson[i].box);
					if(overlapratio > 5)
					{
						isCycling = TRUE;//����
						break;
					}
				}
				if(isCycling)
					break;
			}
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.isCycling = isCycling;//�Ƿ�����
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.box = pCfgs->objPerson[i].box;
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].AttributeInfo.flag = 1;//�¼������
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].detected = TRUE;
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].uBoxID = pCfgs->objPerson[i].target_id;
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].ReportTime = pCfgs->currTime;
			pCfgs->PersonAttributeBox[pCfgs->uPersonNum].lost_detected = pCfgs->objPerson[i].lost_detected;
			pCfgs->uPersonNum++;
		}
		else//��Ŀ���Ѿ���������������ʶ��
		{
			for(j = 0; j < pCfgs->uPersonNum; j++)
			{
				if(pCfgs->PersonAttributeBox[j].uBoxID == pCfgs->objPerson[i].target_id)//ͬһĿ��
				{
					pCfgs->PersonAttributeBox[j].AttributeInfo.box = pCfgs->objPerson[i].box;
					pCfgs->PersonAttributeBox[j].lost_detected = pCfgs->objPerson[i].lost_detected;
					pCfgs->PersonAttributeBox[j].AttributeInfo.flag = 0;//��������
					pCfgs->PersonAttributeBox[j].detected = TRUE;
					break;
				}
			}

		}
	}
	//ȥ��û�м�⵽���������Կ�
	for(i = 0; i < pCfgs->uPersonNum; i++)
	{
		if(pCfgs->PersonAttributeBox[i].detected == FALSE)
		{
			for( j = i + 1; j < pCfgs->uPersonNum; j++)
			{
				pCfgs->PersonAttributeBox[j - 1] = pCfgs->PersonAttributeBox[j];
			}
			i--;
			pCfgs->uPersonNum--;
		}
		else
		{
			if(pCfgs->currTime - pCfgs->PersonAttributeBox[i].ReportTime > (5 * 60))//�ϱ����Ϊ5����
			{
				pCfgs->PersonAttributeBox[i].AttributeInfo.flag = 1;//�ϱ�
				pCfgs->PersonAttributeBox[i].ReportTime = pCfgs->currTime;
			}
		}
	}

}
void BicycleAttributeDetect(ALGCFGS *pCfgs, IplImage* img)//�������Լ�����
{
	int i = 0, j = 0;
	int val1 = 0, val2 = 0;
	//���õ������Կ�Ϊδ���
	for(i = 0; i < pCfgs->uBicycleNum; i++)
	{
		pCfgs->BikeAttributeBox[i].detected = FALSE;
	}
	for(i = 0; i < pCfgs->event_targets_size; i++)
	{
		if(strcmp(pCfgs->event_targets[i].names, "bicycle") != 0)
			continue;
		//���е�������ʶ��
		if(pCfgs->event_targets[i].attribute_detected == FALSE)//��Ŀ��û�н��е�������ʶ��
		{
			//val1 = pCfgs->k * (pCfgs->event_targets[i].box.x) + pCfgs->b - pCfgs->event_targets[i].box.y;
			//val2 = pCfgs->k * (pCfgs->event_targets[i].box.x + pCfgs->event_targets[i].box.width) + pCfgs->b - pCfgs->event_targets[i].box.y - pCfgs->event_targets[i].box.height;
			//if(pCfgs->event_targets[i].detected && (val1 * val2 < 0))//��⵽�����Ҽ����������ཻ
			if(pCfgs->event_targets[i].detected && (pCfgs->event_targets[i].box.y + pCfgs->event_targets[i].box.height) > img->height / 2)//����ߵ�ͼ���¶�
			{
				CvRect roi = cvRect(pCfgs->event_targets[i].box.x, pCfgs->event_targets[i].box.y, pCfgs->event_targets[i].box.width, pCfgs->event_targets[i].box.height);
				IplImage* imgROI = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 3);
				//����ROI����  
				/*cvSetImageROI(img, roi);   
				printf("copy start\n");
				cvCopy(img, imgROI, NULL);  
				printf("copy end\n");
				cvResetImageROI(img);*/
				for(int ii = 0; ii < roi.height; ii++)
				{
					memcpy(imgROI->imageData + ii * imgROI->widthStep, img->imageData + (ii + roi.y) * img->widthStep + roi.x * 3, roi.width * 3);
				}
				BicycleAttribute val = BicycleAttributeRecognition(imgROI, pCfgs);//��������ʶ��
				if(imgROI)
				{
					cvReleaseImage(&imgROI);
					imgROI = NULL;
				}
				pCfgs->event_targets[i].attribute_detected = TRUE;
				//pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].AttributeInfo.brand = val.brand;//Ʒ��
				pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].AttributeInfo.colour = val.colour;//��ɫ
				//���ݵ��������Ƿ��ཻ�����ж�
				/*bool isCycling = FALSE;
				for( j = 0; j < pCfgs->event_targets_size; j++)
				{
					if(strcmp(pCfgs->event_targets[j].names, "person") != 0)
						continue;
					CRect rct;
					rct.x = pCfgs->event_targets[i].box.x + 10;
					rct.y = pCfgs->event_targets[i].box.y;
					rct.width = pCfgs->event_targets[i].box.width - 20;
					rct.height = pCfgs->event_targets[i].box.height;
					int overlapratio = overlapRatio(rct, pCfgs->event_targets[j].box);
					if(overlapratio > 5)
					{
						isCycling = TRUE;//����
						break;
					}
				}
				pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].AttributeInfo.isCycling = isCycling;//�Ƿ�����*/
				pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].AttributeInfo.box = pCfgs->event_targets[i].box;
				pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].AttributeInfo.flag = 1;//�¼������
				pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].detected = TRUE;
				pCfgs->BikeAttributeBox[pCfgs->uBicycleNum].uBoxID = pCfgs->event_targets[i].target_id;

				pCfgs->uBicycleNum++;
			}
		}
		else//��Ŀ���Ѿ������˵�������ʶ��
		{
			for(j = 0; j < pCfgs->uBicycleNum; j++)
			{
				if(pCfgs->BikeAttributeBox[j].uBoxID == pCfgs->event_targets[i].target_id)//ͬһĿ��
				{
					pCfgs->BikeAttributeBox[j].AttributeInfo.box = pCfgs->event_targets[i].box;
					pCfgs->BikeAttributeBox[j].AttributeInfo.flag = 0;//��������
					pCfgs->BikeAttributeBox[j].detected = TRUE;
					break;
				}
			}

		}
	}
	//ȥ��û�м�⵽�ĵ������Կ�
	for(i = 0; i < pCfgs->uBicycleNum; i++)
	{
		if(pCfgs->BikeAttributeBox[i].detected == FALSE)
		{
			for( j = i + 1; j < pCfgs->uBicycleNum; j++)
			{
				pCfgs->BikeAttributeBox[j - 1] = pCfgs->BikeAttributeBox[j];
			}
			i--;
			pCfgs->uBicycleNum--;
		}
	}

}
///////////////////////////////////////////////////////////////////////////
void attri_init()
{
	if(attri_init_flag == 0)//��ʼ��python
	{
#ifdef USE_PYTHON
		py_attri_init();
#else
		memset(human_attri_init, 0, MAX_GPU_NUM * sizeof(int));//��gpu����Ϊδ����������������
#endif
	}
	attri_init_flag++;
}
#endif