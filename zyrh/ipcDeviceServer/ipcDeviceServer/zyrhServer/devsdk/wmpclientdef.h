#pragma once

#ifdef __cplusplus
extern "C"
{
#endif



typedef int WMP_HANDLE;				//ע����

#define WMP_TRANS_TCP	1
#define WMP_TRANS_UDP	2

#define WMP_STREAM_MAIN		1	//������
#define WMP_STREAM_SUB		2	//������

#define WMP_VIDEO_HEADER	1	//��Ƶͷ
#define WMP_VIDEO_DATA		2	//��Ƶ����


//¼������
#define WMP_RECORD_ALL		0xff	//����¼��
#define WMP_RECORD_TIMING	0		//��ʱ¼��
#define WMP_RECORD_MONTION	1		//�ƶ����¼��
#define WMP_RECORD_ALARM	2			//����¼��
#define WMP_RECORD_MONTION_AND_ALARM	3	//�ƶ�����ұ���
#define WMP_RECORD_MONTION_OR_ALARM		4	//�ƶ����򱨾�
#define WMP_RECORD_COMMAND	5		//�����¼��
#define WMP_RECORD_MANUAL	6		//�ֶ�¼��


//��̨����
#define WMP_PTZ_UP		1			//��
#define WMP_PTZ_DOWN	2			//��
#define WMP_PTZ_LEFT	3			//��
#define WMP_PTZ_RIGHT	4			//��
#define WMP_PTZ_UP_RIGHT	5		//����
#define WMP_PTZ_RIGHT_DOWN	6		//����
#define WMP_PTZ_DOWN_LEFT	7		//����
#define WMP_PTZ_LEFT_UP		8			//����
#define WMP_PTZ_AUTO		9				//�Զ�
#define WMP_PTZ_ZOOMIN		10			//�Ŵ�
#define WMP_PTZ_ZOOMOUT		11			//��С
#define WMP_PTZ_FOCUS_NEAR	12		//�۽�
#define WMP_PTZ_FOCUS_FAR	13		//ɢ��
#define WMP_PTZ_DIAPHRAGM_ENLARGE	14	//��Ȧ�Ŵ�(����)
#define WMP_PTZ_DIAPHRAGM_SHRINK	15	//��Ȧ��С(�䰵)
#define WMP_PTZ_WIPER	16			//��ˢ
#define WMP_PTZ_LIGHT	17			//�ƹ�
#define WMP_PTZ_HEAT	18			//����

#define WMP_PTZ_ACTION_START	1
#define WMP_PTZ_ACTION_STOP		0



//Ԥ�õ�
#define WMP_PRESET_SET	1
#define WMP_PRESET_DEL	2
#define WMP_PRESET_GOTO	3




//GPS����
typedef struct _WMP_GPSINFO
{

}WMP_GPSINFO;

//��������
typedef struct _WMP_ALARMINFO
{

}WMP_ALARMINFO;

//�豸״̬����
typedef struct _WMP_STATEINFO
{

}WMP_STATEINFO;


//¼���ļ�
typedef struct _WMP_RECORDFILEINFO
{
	unsigned int file_id;		//�ļ����
	int channel;				//ͨ����
	int rec_type;				//¼������
	unsigned long long file_size;	//�ļ���С
	char begin_time[32];	//��ʼʱ��
	char end_time[32];		//����ʱ��
	char file_name[64];		//�ļ���
	char full_name[256];
}WMP_RECORDFILEINFO;


typedef struct _WMP_IMAGEINFO
{
	unsigned char hue;		//ɫ�� 0~255
	unsigned char contrast;	//�Աȶ� 0~255
	unsigned char bright;		//���� 0~255
	unsigned char saturation;	//���Ͷ� 0~255
}WMP_IMAGEINFO;




//Ӳ��״̬
typedef struct _WMP_DISK_STATUS
{
	int disk_no;				//Ӳ�̱��
	int status;					//Ӳ��״̬:0-����;1-����;2-�쳣
	unsigned long long total_size;//Ӳ��������,��λ:�ֽ�
	unsigned long long free_size;//Ӳ�̿�������,��λ:�ֽ�
}WMP_DISK_STATUS;

//ͨ����Ƶ״̬
typedef struct _WMP_VIDEO_STATUS
{
	int channel;				//ͨ����
	int rec_status;				//¼��״̬:1-¼��;0-δ¼��
	int video_status;			//��Ƶ״̬:0-����;1-�쳣
	int dsp_status;				//����״̬:0-����;1-�쳣
	unsigned int bit_rate;		//��ǰ����
	unsigned int client_number;	//�ͻ���������
}WMP_VIDEO_STATUS;


//����ͨ��״̬
typedef struct _WMP_ALARM_CHANNEL_STATUS
{
	int alarm_channel;	//����ͨ��
	int status;			//ͨ��״̬:1-����,0-δ����
}WMP_ALARM_CHANNEL_STATUS;

//�豸����״̬
typedef struct _WMP_DEVICE_WORK_STATUS
{
	int cpu;		//CPUʹ�ðٷֱ�
	int memory;		//�ڴ�ʹ�ðٷֱ�

	WMP_DISK_STATUS* disks;
	unsigned int disk_count;

	WMP_VIDEO_STATUS* videos;
	unsigned int video_count;

	WMP_ALARM_CHANNEL_STATUS* alarm_in;
	unsigned int alarm_in_count;

	WMP_ALARM_CHANNEL_STATUS* alarm_out;
	unsigned int alarm_out_count;

}WMP_DEVICE_WORK_STATUS;




//�豸������Ϣ
typedef struct _WMP_DEVICE_UPDATE
{
	const char* ftp_ip;
	const char* ftp_account;
	const char* ftp_pwd;
	const char* file;
	int ftp_port;
}WMP_DEVICE_UPDATE;

//���ӶϿ��ص�
typedef void (*CBF_OnClosed)(char* ip,unsigned int len,unsigned short* port,void* ctx);
//GPS��Ϣ�ص�
typedef void (*CBF_OnGps)(WMP_HANDLE h,const WMP_GPSINFO* gps,void* ctx);
//������Ϣ�ص�
typedef void (*CBF_OnAlarm)(WMP_HANDLE h,const WMP_ALARMINFO* alarm,void* ctx);
//G״̬��Ϣ�ص�
typedef void (*CBF_OnState)(WMP_HANDLE h,const WMP_STATEINFO* state,void* ctx);
//��Ƶ���ݻص�
typedef void (*CBF_OnStream)(int h,int sflag,const char* pdata,unsigned int data_size,void* ctx);
//��Ƶ���ݻص�
typedef void (*CBF_OnAudioStream)(int h,const char* pdata,unsigned int data_size,void* ctx);
//�豸¼�����ݻص�
typedef void (*CBF_OnDevRecord)(int h,int sflag,const char* pdata,unsigned int data_size,void* ctx);




#ifdef __cplusplus
}
#endif

