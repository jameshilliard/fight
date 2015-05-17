#pragma once

#include "wmpclientdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

//��ʼ��wmp��
//return:�������
int WMP_Init();

//ע��wmp��
void WMP_Cleanup();

//����wmp���
//return:���,-1ʧ��
WMP_HANDLE WMP_Create();

//�ر�wmp���
//h:���
//return:�������
int WMP_Close(WMP_HANDLE h);

//����GPS�ص�����
//h:���
//cbf:�ص�����
//ctx:�ص�����������
//return:�������
int WMP_SetGpsCallback(WMP_HANDLE h,CBF_OnGps cbf,void* ctx);

//���ñ����ص�����
//h:���
//cbf:�ص�����
//ctx:�ص�����������
//return:�������
int WMP_SetAlarmCallback(WMP_HANDLE h,CBF_OnAlarm cbf,void* ctx);

//�����豸״̬�ص�����
//h:���
//cbf:�ص�����
//ctx:�ص�����������
//return:�������
int WMP_SetStateCallback(WMP_HANDLE h,CBF_OnState cbf,void* ctx);

//���ö��߻ص�����
//h:���
//cbf:�ص�����
//ctx:�ص�����������
//return:�������
int WMP_SetClosedCallback(WMP_HANDLE h,CBF_OnClosed cbf,void* ctx);

//��¼
//h:���
//host:CAG��ַ
//port:CAG�˿�
//user:��¼�û���
//pwd:��¼����
//line:��·��
//return:�������
int WMP_Login(WMP_HANDLE h,const char* host,unsigned short port,const char* user,const char* pwd,int line);




//��ʼԤ��
//h:���
//devid:�豸ID
//channel:�豸ͨ����
//stream_type:WMP_STREAM_MAIN-������   WMP_STREAM_SUB-������
//trans_mode:WMP_TRANS_TCP/WMP_TRANS_UDP
//dev_line:�豸��·��
//on_stream:�����ݻص�
//ctx:�ص�����������
//stream_handle:ȡ�����
//return:�������
int WMP_Play(WMP_HANDLE h,const char* devid,int channel,int stream_type,int trans_mode,int dev_line,
			 CBF_OnStream on_stream,void* ctx,int* stream_handle);

//�ر�ʵʱԤ��
int WMP_Stop(WMP_HANDLE h,int stream_handle);

//���������Խ�
int WMP_AudioPlay(WMP_HANDLE h,const char* devid,int audio_channel,CBF_OnAudioStream cbf,void* ctx,int* audio_handle);

//�ر������Խ�
int WMP_AudioStop(WMP_HANDLE h,int audio_handle);

//������������
int WMP_AudioInput(WMP_HANDLE h,int audio_handle,const char* pdata,unsigned int data_size);

//�����豸¼��
int WMP_SearchDeviceRecord(WMP_HANDLE h,
						   const char* devid,int channel,
						   const char* begintime,const char* endtime,
						   unsigned int rec_type,
						   unsigned int offset,unsigned int limit,
						   WMP_RECORDFILEINFO** files,unsigned int* count);

//��������¼��
int WMP_SearchCentralRecord(WMP_HANDLE h,
						   const char* devid,int channel,
						   const char* begintime,const char* endtime,
						   unsigned int rec_type,
						   unsigned int offset,unsigned int limit,
						   WMP_RECORDFILEINFO** files,unsigned int* count);

//�ͷ�WMP_SearchDeviceRecord������ڴ�
void WMP_FreeRecordFileInfo(WMP_RECORDFILEINFO* files);

//�����豸¼��
int WMP_PlayDeviceRecord(
	WMP_HANDLE h,
	const char* devid,
	int channel,
	const char* filename,
	unsigned long long seek,
	unsigned long long read_size,		//Ԥ��,��ʱ��������
	int dev_line,
	int central_record,		//0-���������´洢,1-ʹ��¼��ʱ��洢,2-ʹ�ñ���ʱ��洢
	int* rec_handle,
	CBF_OnDevRecord cbf,
	void* ctx);

//ֹͣ����¼��
int WMP_StopDeviceRecord(WMP_HANDLE h,int rec_handle);

//��������¼��
int WMP_PlayCentralRecord(
						 WMP_HANDLE h,
						 const char* devid,
						 int channel,
						 const char* begintime,
						 const char* endtime,
						 int rec_flag,
						 int stream_control,
						 int* rec_handle,
						 CBF_OnDevRecord cbf,
						 void* ctx);

//ֹͣ��������¼��
int WMP_StopCentralRecord(WMP_HANDLE h,int rec_handle);

//��ѯ�豸״̬
int WMP_QueryDeviceStatus(WMP_HANDLE h,const char* devid,int* status);

//�����豸
int WMP_UpdateDevice(WMP_HANDLE h,const char* devid,const WMP_DEVICE_UPDATE* du);

//��ʽ������
//disk_id_list:Ӳ�̺�,�ö��Ÿ���
int WMP_FormatDisk(WMP_HANDLE h,const char* devid,int disk_id);

//��̨����
int WMP_PtzControl(WMP_HANDLE h,const char* devid,int channel,int ptz_cmd,int action,int param);

//3D��̨����
int WMP_Ptz3DControl(WMP_HANDLE h,const char* devid,int channel,int x,int y,int cx,int cy);

//Ԥ�õ����
int WMP_SetPreset(WMP_HANDLE h,const char* devid,int channel,int action,int preset_id,const char* preset_name);

//����ͼ�����
int WMP_SetImageParam(WMP_HANDLE h,const char* devid,int channel,const WMP_IMAGEINFO* image);
//��ȡͼ�����
int WMP_GetImageParam(WMP_HANDLE h,const char* devid,int channel,WMP_IMAGEINFO* image);

//�����豸
int WMP_Reboot(WMP_HANDLE h,const char* devid);



#ifdef __cplusplus
}
#endif

