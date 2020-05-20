///////////////////////////////////////////////////////
//		DSPARMProto.h
//
//		dsp��armͨѶЭ��
//  	BY DAVID 
//      20130512
//		VER: 1.01.00.00
///////////////////////////////////////////////////////

#ifndef __DSP_PROTO_H__
#define __DSP_PROTO_H__

#include "../client_obj.h"

#define MAX_DETECTOR_TYPES		2			//���֧�����ּ����
#define MAX_DETECTOR_ONETYPE	8			//ÿ�ּ�������֧��8��
////20131222
#define MAX_LANE		8	//	8			//ÿ����������֧��8������

//#define FULL_COLS  					(720)
#define FULL_COLS  					(640)
#define FULL_ROWS  					(480)
#define  CALIBRATION_POINT_NUM   8  //�궨����2015102
#define MAX_DETECTION_NUM 300//�������
#define MAX_REGION_NUM 6//���˼���������
#define MAX_DIRECTION_NUM 2//���˷�����
#define MAX_EVENT_TYPE 27//�¼���������
#define MAX_EVENT_NUM 20//�����¼����������
#define MAX_EVENT_REGION_NUM 100//����¼����������
//////////////////////////////////////////////////
//		�ṹ����: 	�����������Ϣ�ṹ
/////////////////////////////////////////////////

#ifndef __ALG_POINT__
#define __ALG_POINT__

typedef  unsigned int 	Uint32;
typedef  int 			Int32;
typedef  unsigned short Uint16;
typedef  short 			Int16;
typedef  int             BOOL;
typedef  unsigned char	Uint8;

typedef struct 
{
	Uint16 x;
	Uint16 y;
}CPoint;
#endif

#ifndef __ALG_RECT__
#define __ALG_RECT__
typedef struct 
{
	Uint16 x;
	Uint16 y;
	Uint16 width;
	Uint16 height;
}CRect;
#endif

typedef struct tagSPEEDLANE
{
	Uint16				uLaneID; //������
	Uint16              LaneType;//�������� 0������  1������
	//detect region
	CPoint FrontCoil[4];//ռλ��Ȧ
	CPoint RearCoil[4];//������Ȧ
    CPoint MiddleCoil[4];//ռ����Ȧ
	CPoint LaneRegion[4];//��������
	//CPoint				ptFourCorner[6];//�ĸ��������
	//CPoint				ptCornerQ[2];//�Ŷ�ǰ����
	//CPoint				ptCornerQA[2];//������
	//CPoint				ptCornerLB[2];//20150918
	//CPoint				ptCornerRB[2];//20150918
	Uint16				uDetectDerection;//����
	Uint16				ptFrontLine;//Ǯֱ��
	Uint16				ptBackLine;//��ֱ��
	Uint16				uReserved0[30];//����

	//vehicle length and speed transformation params
	Uint16				uTransFactor;//ת��ϵ��
	
	//extended params by david 20130904
	Uint32 			uSpeedCounterChangedThreshold;
	Uint32 			uSpeedCounterChangedThreshold1;
	Uint32 			uSpeedCounterChangedThreshold2;
	Uint32				uGraySubThreshold;//�ҶȲ��ֵ��
	
	
	Uint16				uTrackParams[20];		//Ԥ��
	Uint16				uReserved1[20];			//Ԥ��
}SPEEDLANE;


/*�������ڼ��������,start*/
typedef struct tagZENITH_SPEEDDETECTOR {
	
	Uint16				uLaneTotalNum;//��������
	SPEEDLANE 			SpeedEachLane[MAX_LANE];
	Uint16				uEnvironmentStatus;			//����״̬, 1������  2����������(����) 3������·�� 4��������·�� 0������  //20130930 by david
	//alg params
	Uint16				uDayNightJudgeMinContiuFrame;	//����ת�������� 15
	Uint16				uComprehensiveSens;		//�ۺ������� 60
	Uint16				uDetectSens1;			//���������1	 20	
	Uint16				uDetectSens2;			//���������2    10
	Uint16				uStatisticsSens1;		//ͳ��������1   15
	Uint16				uStatisticsSens2;		//ͳ��������2   3
	Uint16				uSobelThreshold;		//��ֵ������    3
	Uint16             uEnvironment;           //20140320,���� ���ϲ���������ת��
	CPoint					ptactual[8];//�궨��
	CPoint					ptimage[8];//�궨��	
    //CPoint calibration_point[4];//�궨�����
	//CPoint base_line[2];//�궨��׼�ߵ�
	float base_length[2];//��׼�߳�
	float near_point_length;//��������
	float cam2stop;//�����ֹͣ�ߵľ���
	//CPoint PersonDetectArea[MAX_REGION_NUM][4];
	mPersonDetectArea PersonDetectArea;//���˼�����
	mEventInfo       EventDetectArea;//�¼�������
	Uint16				uReserved1[10];			//Ԥ��   
}ZENITH_SPEEDDETECTOR;
/*�������ڼ��������,end*/


/*���ӳ��ȼ����,start*/
typedef struct tagPRESENCELANE {
	Uint16 uReserved[256];
}PRESENCELANE;

typedef struct tagZENITH_PRESENCEDETECTOR {
	
	Uint16			uLaneTotalNum;
	PRESENCELANE		PresenseEachLane[MAX_LANE];
}ZENITH_PRESENCEDETECTOR;
/*���ӳ��ȼ����,end*/

//////////////////////////////////////////////////
//		�ṹ����: 	��Ϣ���� arm ---> dsp
/////////////////////////////////////////////////

typedef struct tagSpeedCfgSeg {
	Uint16				uType;		//���������:1���������ڼ����,2�����ӳ��ȼ����,3��256 ����,0���޼����
	Uint16				uNum; 		//���������
	ZENITH_SPEEDDETECTOR	uSegData[1];//ͬ�������м��������������
} SPEEDCFGSEG;

typedef struct tagPresenceCfgSeg {
	Uint16				uType;		//���������:1���������ڼ����,2�����ӳ��ȼ����,3��256 ����,0���޼����
	Uint16				uNum; 		//���������
	ZENITH_PRESENCEDETECTOR	uSegData[1];//ͬ�������м��������������
} PRESENCECFGSEG;

//union NormalCfgSeg {
//	SPEEDCFGSEG  		cSpeedCfgSeg;
//	PRESENCECFGSEG		cPresenceCfgSeg;
//};

typedef struct tagMsgHeader {
    Uint16   	uFlag;
    Uint16  	uCmd;
    Uint16  	uMsgLength;		
} MSGHEADER;

typedef struct tagConfigInfoHeader {
    Uint16		uDetectPosition;
    Uint16		uDetectFuncs[2];	//
} CFGINFOHEADER;

typedef struct tagCfgMsg {
	MSGHEADER		uMsgHeader;
	CFGINFOHEADER	uCfgHeader;			
} CFGMSG;
//////////////////////////////////////////////////
//		�ṹ����: 	��Ϣ����  dsp ---> arm
//////////////////////////////////////////////////
typedef struct 
{
	Uint32 DetectInSum;//����Ȧ������
	Uint32 DetectOutSum;//����Ȧ������
	unsigned int calarflag;//��Ȧռ��״̬
	Uint16	uVehicleSpeed;//�����ٶ�km/h
	Uint16  uVehicleType; //��������
	Uint16  uVehicleDirection;//�������з���
	Uint16	uVehicleLength;//��������
	Uint16	uVehicleHeight;//�����߶�
	Uint16	uVehicleHeadtime; //��ͷʱ��
}RegionAttribute;

typedef struct tagSpeedDetectInfo {
	BOOL		bInfoValid;				//����������Ч
	Uint16	bVehicleSta;			//���복��״̬
	CPoint		ptVehicleCoordinate;	//����λ��
	RegionAttribute CoilAttribute[2];//��Ȧ����
	Uint16  uLastVehicleLength;//���һ������λ��
	CPoint	LineUp[2];
	int AlarmLineflag;
	bool     IsCarInTailFlag;    //β������ռ�б�־
	bool     getQueback_flag;	//txl,20160104
	Uint16 uDetectRegionVehiSum; //��������
	Uint16 uVehicleQueueLength; //�Ŷӳ���
	CPoint QueLine[2]; //�Ŷӳ�����
	Uint16 uQueueHeadDis;//���׵�ͣ���ߵľ��룬��λ��m
	Uint16 uQueueTailDis;//��β��ͣ���ߵľ��룬��λ��m
	Uint16 uQueueVehiSum;//ͨ���Ŷ���������λ����
	Uint16 uVehicleDensity;//�ռ�ռ���ʣ���λ���ٷֱ�
	Uint16 uVehicleDistribution;//�����ֲ��������λ��m
	Uint16 uHeadVehiclePos;//ͷ��λ�ã���λ��m
	Uint16 uHeadVehicleSpeed;//ͷ���ٶȣ���λ��km/h
	Uint16 uLastVehiclePos;//ĩ��λ�ã���λ��m
	Uint16 uLastVehicleSpeed;//ĩ���ٶȣ���λ��km/h
	Uint32 uBicycleFlow;//���г�����
	Uint32 uBusFlow;//����������
	Uint32 uCarFlow;//С������
	Uint32 uTruckFlow;//��������
	Uint32 uMotorbikeFlow;//Ħ�г�����
	Uint32 nVehicleFlow; //�ǻ���������
	Uint16	uReserved[20];			//Ԥ��
}SpeedDetectInfo_t;

typedef struct tagPresenceDetectInfo {
	BOOL		bInfoValid;				//����������Ч
	Uint16	uMotorCadeLength;		//���ӳ���
	CPoint		ptLisnceCoordinate[4];	//��������
	Uint16	uLisnceID[10];			//���ƺ���
	Uint16	uLisnceColor;			//������ɫ
	Uint16	uVehicleColor;			//������ɫ
	Uint16	uVehicleBrand;			//��������(����Ʒ��)
	Uint16	uSignalLightSta;		//�źŵ�״̬			
	Uint16	uEnvirenmentSta;		//����״̬			
	Uint16	uReserved[100];			//Ԥ��
}PresenceDetectInfo_t;

typedef struct tagResultData {
	Uint16			uLaneID;				//����ID
	Uint16			uReserved0;				//
	SpeedDetectInfo_t		SpeedDetectInfo1;
	PresenceDetectInfo_t PresenceDetectInfo;
	Uint16			uReserved[95];			//Ԥ�� 
}RESULTDATA;

typedef struct {
	int x;
	int y;
	int w;
	int h;
	int label;//���ͱ�ǩ��1Ϊbus 2Ϊcar 3Ϊtruck 4Ϊmotorbike 5Ϊbicycle 6Ϊperson 7Ϊ����
	int confidence;//���Ŷ�
	int id;//Ŀ��id
	float distance[2];//Ŀ�����ĵ����������
	float border_distance[4][2];//Ŀ������ ���ϡ����¡�����������ľ���
	int laneid;//������
	int speed;//Ŀ��y�����ٶ�
	int speed_Vx;//Ŀ��x�����ٶ�
	Uint16 width;//Ŀ�����
	Uint16 length;//Ŀ�곤��
}OBJECTINFO;

typedef enum eventType{
	OVER_SPEED=1,//����
	REVERSE_DRIVE, //����
	STOP_INVALID, //ͣ��
	NO_PEDESTRIANTION, //����
	DRIVE_AWAY, //ʻ��
	CONGESTION,//ӵ��
	DROP,//������
	PERSONFALL = 10,//���˵���
	NONMOTORFALL = 12,//�ǻ���������
	NONMOTOR = 20,//���зǻ�����
	ACCIDENTTRAFFIC = 21,//��ͨ�¹�
	GREENWAYDROP = 22,//�̵�������
	ROADWATER = 23,//��·��ˮ
	ROADHOLLOW = 24, //��·����
	ROADDAMAGE = 25,//��·����
	ROADCRACK = 26//��·�ѷ�
} EventType;//�¼�����

typedef struct{
	Uint16 uRegionID;//�¼�����ID
	Uint16 uNewEventFlag;//�Ƿ����¼����
	Uint16 uEventID;//�¼�ID
	EventType uEventType;//�¼�����
	CPoint EventBox[4];//�¼���
}EVENTBOX;//�¼�����Ϣ

typedef struct{
	Uint16 uNewEventFlag;//�²����¼����
	Uint16 uEventNum;//�¼�����
	EVENTBOX EventBox[MAX_EVENT_NUM];//�¼���
}EVENTOUTBUF;

#define MAX_HELMET_NUM 5//�ǻ����������İ�ȫñ����
#define MAX_NONMONTOR_NUM 50//���ǻ���������
typedef struct{
	CRect nonMotorBox;//�ǻ���������
	CRect helmetBox[MAX_HELMET_NUM];//��ȫñ����
	Uint16 helmetNum;//��ȫñ��
	Uint16 riderNum;//��Ա���� 1 2 3 4 5
	bool hasHelmet;//�Ƿ����ȫñ true:����ȫñ��false:δ����ȫñ
	bool overLoad;//�Ƿ��� true:����2��(3�˻�3������)��false:δ����
}NonMotorInfo;

typedef struct{
	Uint16 uNonMotorNum;//�ǻ���������
	NonMotorInfo nonMotorInfo[MAX_NONMONTOR_NUM];//�ǻ����������
}NPOUTBUF;//�ǻ�������Ա��Ϣ
typedef struct tagResultInfo {
	Uint16 		LaneSum;						//
	Uint16			uEnvironmentStatus;		//����״̬, 1������  2����������(����) 3������·�� 4��������·�� 0������  //20130930 by david
	//CRect udetBox[100];
	OBJECTINFO udetBox[MAX_DETECTION_NUM];
	Uint16 udetNum;//����
	OBJECTINFO udetPersonBox[MAX_DETECTION_NUM];
	Uint16 udetPersonNum;//���˼���
	Uint16 udetStatPersonNum;//ͳ�����˼���
	Uint16 uPersonDirNum[MAX_DIRECTION_NUM];//�ַ���������
	Uint16 uBicycleDirNum[MAX_DIRECTION_NUM];//�ַ������г���
	Uint16 uMotorbikeDirNum[MAX_DIRECTION_NUM];//�ַ���Ħ�г���
	Uint16 uPersonRegionNum[MAX_REGION_NUM];//����������
	car_objects car_number[MAX_DETECTION_NUM];//���Ƽ���
	Uint16 udetPlateNum;//���Ƽ�����
	EVENTOUTBUF eventData;
	NPOUTBUF NPData;//���Ա�����Ϣ
    RESULTDATA			uEachLaneData[8];				//����8���������еļ����Ϣ
} RESULTINFO;

typedef struct tagResultMsg {
    MSGHEADER		uMsgHeader;
    RESULTINFO 		uResultInfo;
} RESULTMSG;


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#endif