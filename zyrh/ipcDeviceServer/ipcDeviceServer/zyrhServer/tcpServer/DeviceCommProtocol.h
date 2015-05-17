#ifndef _INCnetCommand_h
#define _INCnetCommand_h

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/NObserver.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Delegate.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"


#define 	RTSP	1  //zss++


using Poco::Net::StreamSocket;

#ifndef STATUS
#define STATUS	int
#endif

#ifndef IMPORT
#define IMPORT	extern
#endif

#ifndef LOCAL
#define LOCAL	static
#endif

#ifndef FOREVER
#define FOREVER for(;;)
#endif

#ifndef ERROR 
#define ERROR	-1
#endif

#ifndef OK
#define OK	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#define 	RTSP	1  //zss++

#define PRODDATELEN         8			/* �������� */
#define PRODNUMLEN          9			/* 9λ�������к� */
#define SERIALNO_LEN        48			/* ���кų��� */


#define NAME_LEN				32				/* �û������� */
#define PASSWD_LEN				16				/* ���볤�� */
#define MAX_DOMAIN_NAME			64				/* ����������� */
#define MACADDR_LEN				6				/* MAC��ַ���� */
#define PATHNAME_LEN			128				/* ·�������� */
#define PRODUCT_MODEL_LEN		64				/* ��Ʒ�ͺų���(�����ͺ�>16λ) */

#define MAX_VIDEOOUT			4				/* ���CVBS���� */
#define MAX_VIDEOOUT_8000		2				/* ���CVBS���� */
#define MAX_VGA					4				/* ���VGA���� */
#define MAX_VGA_8000			1				/* ���VGA���� */
#define MAX_AUXOUT				16				/* ���ģ������������ */
#define MAX_ETHERNET			2				/* ���ڸ��� */
#define MAX_ANALOG_CHANNUM		32				/* maximum analog channel count */
#define MAX_CHANNUM				32				/* �������ͨ���� */
#define MAX_CHANNUM_8000	    16				/* �������ͨ���� */
#define	MAX_DECODE_CHAN     	16				/* ������ͨ���� */
#define MAX_ALARMIN				32				/* ��󱨾����� */
#define MAX_ALARMIN_8000	    16				/* ��󱨾����� */
#define MAX_ALARMOUT			32				/* ��󱨾���� */
#define MAX_ALARMOUT_8000		4				/* ��󱨾���� */
#define MAX_SERIAL_PORT			8				/* ��󴮿ڸ��� */
#define MAX_USB_NUM         	2				/* USB���������� */

#define MAX_IPCHANNUM       	32				/* IPC������ */
#define MAX_IPC_ALARMIN     	6				/* IPC��󱨾�������� */
#define MAX_IPC_ENCCHAN     	4				/* IPC��󱨱���ͨ������ */
#define MAX_IPC_ALARMOUT    	2				/* IPC��󱨾�������� */

#define MAX_USERNUM				32				/* ����û��� */
#define MAX_USERNUM_8000		16				/* ����û��� */
#define MAX_EXCEPTIONNUM		32				/* ����쳣���� */
#define MAX_EXCEPTIONNUM_8000	16				/* ����쳣���� */
#define MAX_TIMESEGMENT			8				/* һ�����8��ʱ��� */
#define MAX_TIMESEGMENT_8000    4				/* 8000һ�����4��ʱ��� */
#define MAX_DAYS				7				/* 1��7�� */
#define PHONENUMBER_LEN			32				/* �绰���볤�� */


/*net client debug define*/
#define NET_INFO(x)  printf x
#define NET_ERR(x)   printf x
#define NET_FATAL(x) printf x

#define MAX_IP_DEVICE		32
#define MAX_IP_CHANNUM      32
#define MAX_IP_ALARMIN		128
#define MAX_IP_ALARMOUT		64


/*analog + IP*/
#define MAX_ANALOG_ALARMIN			MAX_ALARMIN 
#define MAX_ANALOG_ALARMOUT			MAX_ALARMOUT 
#define MAX_CHANNUM_V30   				(MAX_ANALOG_CHANNUM + MAX_IP_CHANNUM)
#define MAX_ALARMOUT_V30  				(MAX_ANALOG_ALARMOUT + MAX_IP_ALARMOUT)
#define MAX_ALARMIN_V30   				(MAX_ANALOG_ALARMIN + MAX_IP_ALARMIN)
#define MAX_DISKNUM_V30                 MAX_HD_COUNT 
#define MAX_DISKNUM_8000				16 

#define  ONESECOND       1000/*��λ:����*/
/* network SDK version
	day  : bit 0 - 5
	month: bit 6 - 9
	year : bit 10 - 15
	minor: bit 16 - 23
	major: bit 24 - 31
*/
#define MAKE_NETSDK_VERSION(_major, _minor, _year, _month, _day)	\
	(	\
	(((_major)&0xff)<<24) | 	\
	(((_minor)&0xff)<<16) |	\
	((((_year)-2000)&0x3f)<<10)	|	\
	(((_month)&0xf)<<6) |	\
	((_day)&0x3f)	\
	)

#define GET_NETSDK_MAJOR(version)	(((version)>>24)&0xff)
#define GET_NETSDK_MINOR(version)	(((version)>>16)&0xff)
#define GET_NETSDK_YEAR(version)	((((version)>>10)&0x3f)+2000)
#define GET_NETSDK_MONTH(version)	(((version)>>6)&0xf)
#define GET_NETSDK_DAY(version)		((version)&0x3f)

#define GET_NETSDK_BUILDDATE(version)	((version)&0xffff)
#define RELOGIN_FLAG					0x80000000
#define NETSDK_VERSION1_0	MAKE_NETSDK_VERSION(1, 0, 2004, 10, 4)
#define NETSDK_VERSION1_2	MAKE_NETSDK_VERSION(1, 2, 2005, 3, 15)		/* �û���/������ܣ�Ӳ����Ϊ16 */
#define NETSDK_VERSION1_3	MAKE_NETSDK_VERSION(1, 3, 2005, 4, 1)		/* �ƶ����/��Ƶ��ʧ/�ڵ���������ʱ��� */
#define NETSDK_VERSION1_4	MAKE_NETSDK_VERSION(1, 4, 2005, 5, 30)		/* Ԥ�õ���Ŀ 16 -> 128 */
#define NETSDK_VERSION1_5	MAKE_NETSDK_VERSION(1, 5, 2005,12, 28)		/* �û���Ȩ��ϸ����ͨ��(���ػطš�Զ�̻طš�Զ��Ԥ��)*/
#define NETSDK_VERSION2_0   MAKE_NETSDK_VERSION(2, 0, 2006,4, 27)
#define NETSDK_VERSION2_1	MAKE_NETSDK_VERSION(2, 1, 2006,8, 14)		/* ����汾���ϵ�SDK��Ҫ������֤ */
#define NETSDK_VERSION3_0	MAKE_NETSDK_VERSION(3, 0, 2008,2, 28)		
#define NETSDK_VERSION3_1	MAKE_NETSDK_VERSION(3, 1, 2009,7, 30)		
#define NETSDK_VERSION4_0   MAKE_NETSDK_VERSION(4, 0, 2010,3, 1)

#ifdef HI3515
#define CURRENT_NETSDK_VERSION  NETSDK_VERSION4_0
#define CURRENT_NETSDK_VERSION	0x040028C1
#else
//#define CURRENT_NETSDK_VERSION	NETSDK_VERSION3_1
#define CURRENT_NETSDK_VERSION	0x040028C1
#endif

/* network command defines */
/* �¡�������SDK�Ľӿ� */
#define NEW_NETSDK_INTERFACE	90
#define OLD_NETSDK_INTERFACE	60
#ifdef NET3G_SDK
#define FOR3G_NETSDK_INTERFACE	30
#define FOR3G_NETSDK_SDK_INTERFACE   99
#endif

//login/logout 
#define NETCMD_LOGIN				0x010000	/* login */
#define NETCMD_RELOGIN				0x010010	/* verify username/password again */
#define NETCMD_LOGOUT				0x010100	/* logout */
#define NETCMD_USEREXCHANGE		0x010200	/* user keep alive */

//parameter 
#define NETCMD_GET_DEVICECFG		0x020000	/* device parameter */
#define NETCMD_SET_DEVICECFG		0x020001

#define NETCMD_GET_DEVICECFG_V40	0x1110c2     /* ��ȡ��չ�豸���� */
#define NETCMD_SET_DEVICECFG_V40	0x1110c3     /* ��ȡ��չ�豸���� */

#define NETCMD_GET_NETCFG			0x020100	/* network parameter */
#define NETCMD_SET_NETCFG			0x020101
#define NETCMD_GET_NETCFG_V30   	0x110000
#define NETCMD_SET_NETCFG_V30   	0x110001

#define NETCMD_GET_NETCFG_MULTI	0x111082 /*��ȡ�����ڲ���*/
#define NETCMD_SET_NETCFG_MULTI	0x111083 /*���ö����ڲ���*/
#define NETCMD_GET_NETAPPCFG    	0x020110
#define NETCMD_SET_NETAPPCFG    	0x020111
#define NETCMD_GET_NTPCFG			0x020112  /*��ȡ����Ӧ�ò��� NTP*/
#define NETCMD_SET_NTPCFG			0x020113  /*��������Ӧ�ò��� NTP*/
#define NETCMD_GET_DDNSCFG		0x020114  /*��ȡ����Ӧ�ò��� DDNS*/
#define NETCMD_SET_DDNSCFG		0x020115  /*��������Ӧ�ò��� DDNS*/
#define NETCMD_GET_DDNSCFG_EX   	0x02011a  /*��ȡ����Ӧ�ò�����չDDNS*/
#define NETCMD_SET_DDNSCFG_EX   	0x02011b  /*��������Ӧ�ò�����չDDNS*/
#define NETCMD_GET_DDNSCFG_V30 	0x110140  /*��ȡ����Ӧ�ò���9000 DDNS*/
#define NETCMD_SET_DDNSCFG_V30  	0x110141  /*��������Ӧ�ò���9000 DDNS*/
#define NETCMD_GET_EMAILCFG		0x020116  /*��ȡ����Ӧ�ò��� EMAIL*/
#define NETCMD_SET_EMAILCFG		0x020117  /*��������Ӧ�ò��� EMAIL*/
#define NETCMD_GET_EMAILCFG_V30  	0x1100b0
#define NETCMD_SET_EMAILCFG_V30 	0x1100b1



#define NETCMD_EMAIL_TEST		0x111081 	/*Email���Խӿ�*/
#define NETCMD_GET_NFSCFG       0x020120
#define NETCMD_SET_NFSCFG       0x020121
#define NETCMD_GET_NASCFG_V30   	0x110090
#define NETCMD_SET_NASCFG_V30   	0x110091

#define NETCMD_GET_PICCFG		0x020200	/* picture parameter */
#define NETCMD_SET_PICCFG		0x020201
#define NETCMD_GET_PICCFG_EX	0x020232
#define NETCMD_SET_PICCFG_EX	0x020233
#define NETCMD_GET_PICCFG_V30   	0x110010
#define NETCMD_SET_PICCFG_V30   	0x110011

#define NETCMD_SET_PICCFG_ENHANCE	0x020234

#define NETCMD_GET_HDCFG_NEW    0x10001a
#define NETCMD_SET_HDCFG_NEW    0x10001b

#define NETCMD_GET_COMPRESSCFG_AUD		0x110042
#define NETCMD_SET_COMPRESSCFG_AUD		0x110043
#define NETCMD_GET_COMPRESSCFG_AUD_CURRENT	0x110044

#define NETCMD_GET_COMPRESSCFG		0x020210	/* compression parameter */
#define NETCMD_SET_COMPRESSCFG		0x020211
#define NETCMD_GET_COMPRESSCFG_EX	0x020218	/* compression parameter */
#define NETCMD_SET_COMPRESSCFG_EX	0x020219
#define NETCMD_GET_COMPRESSCFG_V30	0x110040	/* compression parameter */
#define NETCMD_SET_COMPRESSCFG_V30 	0x110041
#define NETCMD_GET_EVENTCOMPCFG		0x020212	/* compression parameter */
#define NETCMD_SET_EVENTCOMPCFG		0x020213

#define NETCMD_GET_RELATIONCFG		0x020214	/* relation between alarm and motion */
#define NETCMD_SET_RELATIONCFG		0x020215

#define NETCMD_GET_RECORDCFG		0x020220	/* record parameter */
#define	NETCMD_SET_RECORDCFG		0x020221
#define NETCMD_GET_RECORDCFG_V30		0x110020
#define NETCMD_SET_RECORDCFG_V30		0x110021

#define NETCMD_GET_DECODERCFG		0x020230	/* decoder parameter */
#define NETCMD_SET_DECODERCFG		0x020231
#define NETCMD_GET_PTZPROTOCOL      	0x111080
#define NETCMD_GET_DECODERCFG_V30	0x110070	/* decoder parameter */
#define NETCMD_SET_DECODERCFG_V30	0x110071

#define NETCMD_GET_SHOWSTRING		0x020234	/* ��ȡ�����ַ����� */
#define NETCMD_SET_SHOWSTRING		0x020235	/* ���õ����ַ����� */
#define NETCMD_GET_SHOWSTRING_V30	0x110130	/* ��ȡ�����ַ����� */
#define NETCMD_SET_SHOWSTRING_V30	0x110131	/* ���õ����ַ����� */
#define NETCMD_GET_RS232CFG			0x020310	/* rs232 parameter */
#define NETCMD_SET_RS232CFG			0x020311
#define NETCMD_GET_RS232CFG_V30		0x110060	/* rs232 parameter */
#define NETCMD_SET_RS232CFG_V30		0x110061
#define NETCMD_GET_ALARMINCFG		0x020410	/* alarm in parameter */
#define NETCMD_SET_ALARMINCFG		0x020411
#define NETCMD_GET_ALARMINCFG_V30	0x110050	/* alarm in parameter */
#define NETCMD_SET_ALARMINCFG_V30	0x110051
#define NETCMD_GET_ALARMOUTCFG		0x020420	/* alarm out parameter */
#define NETCMD_SET_ALARMOUTCFG		0x020421
#define NETCMD_GET_ALARMOUTCFG_V30	0x1100a0	/* alarm out parameter */
#define NETCMD_SET_ALARMOUTCFG_V30	0x1100a1


#define NETCMD_GET_TIMECFG			0x020500	/* DVR date/time */
#define NETCMD_SET_TIMECFG			0x020501
#define NETCMD_GET_DSTCFG           0x090009
#define NETCMD_SET_DSTCFG           0x09000a
#define NETCMD_GET_RTCTYPE			0x090352	/*rtc ʱ������0 no rtc;1 has */

#define NETCMD_GET_PREVIEWCFG		0x020600	/* preview parameter */
#define NETCMD_SET_PREVIEWCFG		0x020601
#define NETCMD_GET_PREVIEWCFG_V30	0x110100	/* preview parameter */
#define NETCMD_SET_PREVIEWCFG_V30	0x110101
#define NETCMD_GET_AUXPREVCFG		0x020602	/* aux Vout preview parameter,for HF-S */
#define NETCMD_SET_AUXPREVCFG		0x020603
#define NETCMD_GET_AUXPREVCFG_V30	0x110102	/* aux Vout preview parameter,for HF-S */
#define NETCMD_SET_AUXPREVCFG_V30	0x110103

#define NETCMD_GET_AUXVOUTCFG		0x020610	/* aux video out parameter */
#define NETCMD_SET_AUXVOUTCFG		0x020611
#define NETCMD_GET_VIDEOOUTCFG		0x020700	/* video out parameter */
#define NETCMD_SET_VIDEOOUTCFG		0x020701
#define NETCMD_GET_VIDEOOUTCFG_V30	0x110110	/* video out parameter */
#define NETCMD_SET_VIDEOOUTCFG_V30	0x110111

#define NETCMD_GET_MATRIXPARA_V30   0x100008
#define NETCMD_SET_MATRIXPARA_V30   0x100009

#define NETCMD_GET_USERCFG			0x020800	/* user parameter */
#define NETCMD_SET_USERCFG			0x020801
#define NETCMD_GET_USERCFG_EX		0x020802  	/* user parameter */
#define NETCMD_SET_USERCFG_EX		0x020803
#define NETCMD_GET_USERCFG_V30		0x110030
#define NETCMD_SET_USERCFG_V30		0x110031

#define NETCMD_GET_EXCEPTIONCFG		0x020900	/* exception parameter */
#define NETCMD_SET_EXCEPTIONCFG		0x020901
#define NETCMD_GET_EXCEPTIONCFG_V30	0x110120	/* exception parameter */
#define NETCMD_SET_EXCEPTIONCFG_V30	0x110121



#define NETCMD_GET_FRAMEFORMAT_V30  0x110080
#define NETCMD_SET_FRAMEFORMAT_V30  0x110081

/*add by xijw*/
#define NETCMD_GET_ATMPROTOCOL          0x110082

#define NETCMD_GET_HDCFG			0x100018
#define NETCMD_SET_HDCFG			0x100019
#define NETCMD_GET_AUTOBOOTCFG		    0x1100c0
#define NETCMD_SET_AUTOBOOTCFG		    0x1100c1
#define NETCMD_GET_CRUISECFG			0x1100e0
#define NETCMD_SET_CRUISECFG			0x1100e1
#define NETCMD_GET_HOLIDAYCFG			0x1100f0
#define NETCMD_SET_HOLIDAYCFG			0x1100f1

#define NETCMD_GET_IPCORECFG			0x110150
#define NETCMD_SET_IPCORECFG			0x110151
#define NETCMD_GET_IPCORECFG_V31			0x110152 /*SDK V3.1���������֣�֧��ʹ��IPC  ��������*/
#define NETCMD_SET_IPCORECFG_V31			0x110153 /*SDK V3.1���������֣�֧��ʹ��IPC  ��������*/
#define NETCMD_GET_IPALARMINCFG		    0x110160
#define NETCMD_SET_IPALARMINCFG		    0x110161
#define NETCMD_GET_IPALARMOUTCFG		0x110170
#define NETCMD_SET_IPALARMOUTCFG		0x110171

#define DVR_GET_SCALECFG			0x020a04 /* ��ȡ��������*/
#define DVR_SET_SCALECFG			0x020a05  /*������������*/
#define DVR_GET_SCALECFG_V30			0x110180 /* ��ȡ��������*/
#define DVR_SET_SCALECFG_V30			0x110181  /*������������*/

/*ATM���˿�����*/
#define DVR_GET_ATMPORT    0x020a06  /*��ȡATM PORT����*/
#define DVR_SET_ATMPORT    0x020a07  /*����ATM PORT����*/
#define DVR_PTZSELZOOMIN 	0x030290	/*�������ѡ������Ŵ�*/

#ifdef USEIMG
#define NETCMD_GET_FTPCFG				0x020002		/*��ȡͼƬ�ϴ�FTP����	*/
#define NETCMD_SET_FTPCFG				0x020003		/*����ͼƬ�ϴ�FTP����*/
#define NETCMD_GET_JPEGCFG			0x020004		/*��ȡͼƬ����	*/
#define NETCMD_SET_JPEGCFG			0x020005		/*����ͼƬ����*/
#define NETCMD_GET_JPEGPICTURE_EX				0x03000a		/*����jpegץͼ*/
#endif

#define NETCMD_GET_DEVCFGFILE   0x020a00	/*��ȡ��ǰ�����������ļ�*/
#define NETCMD_PUT_DEVCFGFILE	0x020a01	/*���������ļ���������*/
#ifdef NET3G_SDK
#define NETCMD_GET_COMPRESSCFG_AUD  0x110042     /* ��ȡ�Խ���Ƶѹ������ */
#define NETCMD_SET_COMPRESSCFG_AUD  0x110043     /* ���öԽ���Ƶѹ������*/
#define NETCMD_GET_COMPRESSCFG_AUD_EX   0x110044    /* ��ȡ��ǰ��Ч�ĶԽ���Ƶѹ������ */
#define NETCMD_GET_CMSCFG   0x111300
#define NETCMD_SET_CMSCFG   0x111301
#define NETCMD_GET_DIALCFG  0x111302        /* ��ȡ���Ų��� */
#define NETCMD_SET_DIALCFG  0x111303        /* ���ò��Ų��� */
#define NETCMD_GET_DIALSTATUS   0x111304    /* ��ȡ����״̬ */
#define NETCMD_GET_SMSCFG   0x111305        /* ��ȡ���Ų��� */
#define NETCMD_SET_SMSCFG   0x111306        /* ���ö��Ų��� */
#define NETCMD_GET_SMSLIST  0x111307        /* ��ȡ�����б� */
#define NETCMD_SEND_SMS     0x111308        /* ���Ͷ������� */
#define NETCMD_GET_SMSCONTENT   0x111309    /* ��ȡ�������� */
#define NETCMD_GET_PINSTATUS    0x11130a    /* ��ȡPin״̬ */
#define DVR_SET_PINCMD          0x11130b    /* ����PIN���� */
#define DVR_TRA_JPEGPICCENTER   0x11130c  //�����豸ץͼ�ϴ����ϴ���ͼƬ������

#define NETCMD_ALARMCHAN    0x030400        /* ���������ϴ�ͨ��(����) */
#define NETCMD_ALARMCHAN_V30   0x111020        /* ���������ϴ�ͨ�� */
#ifdef SDK_NAT
#define NETCMD_QUERYMAPPEDADDR  0x111420   /* ��ѯ������ַ */
#define NETCMD_NATPREVIEWQUEST  0x111421   /* UDPԤ������ */
#endif
#endif

/*
 * ============================================================================
 *                            3. remote  operation
 * ============================================================================
 */
/* preview */
#define NETCMD_TCP_PREV			0x030000	/* TCP preview */
#define NETCMD_UDP_PREV			0x030001	/* UDP preview */
#define NETCMD_MCAST_PREV		0x030002	/* MCAST preview */
#define NETCMD_RTP_PREV			0x030003	/* RTP preview */
#define NETCMD_VIDEO_ONLY		0x030004	/* ����Ƶ�ֿ� */
#define NETCMD_AUDIO_ONLY		0x030005
#define NETCMD_DATAEXCHANGE		0x030006	/* data exchange */
#define NETCMD_GET_VIDEOEFFECT	0x030007
#define NETCMD_SET_VIDEOEFFECT	0x030008
#define NETCMD_GET_VIDEOEFFECT_V30	0x053943 /*��ʱʵ�֣���Ҫ��SDK��������*/
#define NETCMD_SET_VIDEOEFFECT_V30	0x073456

#define NETCMD_GET_VOLUME		0x030010	/*��ȡÿ��ͨ����������*/

/* playback */
#define NETCMD_FINDFILE			0x030100	/* �����ļ� */
#define NETCMD_FINDFILE_V30			0x111040	/* �����ļ� */
#define NETCMD_PLAYBYFILE		0x030101	/* ���ļ��ط� */
#define NETCMD_PLAYBYTIME		0x030102	/* ��ʱ��ط� */
#define NETCMD_STARTPLAY		0x030103	/* ��ʼ�ط� */
#define NETCMD_STOPPLAY			0x030104	/* ֹͣ�ط�,�ͷ����� */
#define NETCMD_SETPOS			0x030105	/* ���ļ��ط�,�ı䲥�ŵĽ��� */
#define NETCMD_BACKWARD			0x030106	/* ����n�� */
#define NETCMD_PAUSE			0x030107	/* ��ͣ */
#define NETCMD_RESTART			0x030108	/* ���¿�ʼ */
#define NETCMD_EXCHANGE			0x030109	/* �طŽ������� */
#define NETCMD_STREAMSPEED      0x03010d    /* ���ص��ٶ� */
#define NETCMD_LOCK_FILE        0x030112
#define NETCMD_UNLOCK_FILE      0x030113
#define NETCMD_LOCK_BYTIME      0x030114
#define NETCMD_UNLOCK_BYTIME    0x030115
#ifdef USEIMG
#define NETCMD_FINDPICTURE      0x03010a	/*����ͼƬ*/
#define NETCMD_GETPICTURE		0x03010b	/*����ͼƬ*/
#endif
#define NETCMD_GET_JPEGPICTURE			0x030009		/*ץJPEGͼ��*/

#ifdef SUP_PICTURE
#define NETCMD_GET_SCHED_CAPTURE                0x111146        // ��ȡץͼ�ƻ�
#define NETCMD_SET_SCHED_CAPTURE                0x111147        // ����ץͼ�ƻ�
#define NETCMD_SEARCH_PICTURE_V30	            0x111148
#define NETCMD_GET_PICTURE_V30		            0x111149
#define NETCMD_GET_PICTURE_CAPTURE_CFG          0x111150        // ��ȡDVRץͼ����
#define NETCMD_SET_PICTURE_CAPTURE_CFG          0x111151        // ����DVRץͼ����
#define NETCMD_GET_FTP_CFG				        0x020002	//��ȡͼƬ�ϴ�FTP����
#define NETCMD_SET_FTP_CFG					    0x020003	//����ͼƬ�ϴ�FTP����
#endif

#define NETCMD_GET_IPC_COMP_ABILITY     0x1110d0    /* ��ȡѹ�����������б�*/
#define NETCMD_GET_PIC_CAPTURE_ABILITY  0x1110d1    /* ��ȡץͼ�����б�*/


//PTZ control 
#define NETCMD_PTZ				0x030200	/* ��̨���� */
#define NETCMD_TRANSPTZ		0x030201	/* ͸����̨���� (û��ʹ��)*/
#define NETCMD_GETPTZCONTROL	0x030202	/* ��ȡ��̨����Ȩ */
#define DVR_PTZWITHSPEED		0x030203    /*���ٶȵ���̨����*/

//remote control local display 
#define NETCMD_LOCALPREV		0x030300
#define NETCMD_LOCALPLAYBYNAME	0x030301
#define NETCMD_LOCALPLAYBYTIME	0x030302
#define NETCMD_LOCALFAST		0x030303
#define NETCMD_LOCALSLOW		0x030304
#define NETCMD_LOCALSINGLEFRAME	0x030305
#define NETCMD_LOCALNORMAL		0x030306
#define NETCMD_LOCALFORWARD		0x030307
#define NETCMD_LOCALBACKWARD	0x030308
#define NETCMD_LOCALSETPOS		0x030309
#define NETCMD_LOCALGETPOS		0x03030a
#define NETCMD_LOCALSTOPPLAY	0x03030b
#define NETCMD_CLICKKEY			0x03030c
#define NETCMD_START_PANELKEY			0x111060
#define NETCMD_STOP_PANELKEY			0x111061
#define NETCMD_PREVIEWONECHAN	0x03030d
/* alarm */
#define NETCMD_ALARMCHAN		0x030400	/* ���������ϴ�ͨ�� */
#define NETCMD_ALARMCHAN_V30			0x111020	/* ���������ϴ�ͨ�� */
#define NETCMD_SETALARMOUT		0x030401	/* ���Ʊ������ */
#define NETCMD_GETALARMOUT		0x030402	/* ��ȡ������� */
#define NETCMD_GETALARMOUT_V30		0x111010	/* ��ȡ������� */
/* voice talk */
#define NETCMD_STARTVOICECOM	 0x030500	/* ��ʼ�����Խ� */
#define NETCMD_STARTVOICECOM_V30 		0x111030	/* ��ʼ�����Խ� */
/* transparent channel */
#define NETCMD_SERIALSTART		0x030600	/* ����͸��ͨ�� */
#define NETCMD_SERIALSEND		0x030601	/* ������͸��ͨ��ֱ�ӷ������� */
/* control */
#define NETCMD_RESTORE_CFG		0x030700	/* �ָ�ȱʡ���� */
#define NETCMD_SAVE_CFG			0x030800	/* ������� */
#define NETCMD_REBOOT			0x030900	/* reboot DVR */
#define NETCMD_SHUTDOWN			0x030a00	/* shutdown DVR */
#define NETCMD_UPGRADE			0x030b00	/* upgrade software */
#define NETCMD_FORMAT			0x030c00	/* format hard disk */
#define NETCMD_GET_HDMANAGE  			0x111050	/* format hard disk */
#define NETCMD_SET_HDATTRIBUTE		0x111051	/* format hard disk */
#define NETCMD_GET_HDGROUP  			0x111052	/* format hard disk */
#define NETCMD_SET_HDGROUP			0x111053	/* format hard disk */
#define NETCMD_STARTRECORD		0x030d00	/* start record */
#define NETCMD_STOPRECORD		0x030d01	/* stop record */
#define DVR_SETALARMINPUT   	0x030d02  	/*�ֶ�����*/
#define NETCMD_GET_LOG			0x030e00	/* get log */
#define NETCMD_GET_LOG_V30			0x111000	/* get log */
#define NETCMD_GET_SMART_LOG			0x111001	/* get HD SMART log */

#define NETCMD_GET_WORKSTATUS	0x040000	/* ��ȡ����״̬ */
#define NETCMD_GET_WORKSTATUS_V30	0x111070	/* ��ȡ����״̬ */
#define NETCMD_GET_FRAMEFORMAT  0x040200  	/* ��ȡ������Ϣ */
#define NETCMD_SET_FRAMEFORMAT  0x040201  	/* ���ý�����Ϣ */
#define NETCMD_GET_FRAMEFORMAT_NEW  0x110080  	/* ��ȡ������Ϣ */
#define NETCMD_SET_FRAMEFORMAT_NEW  0x110081  	/* ���ý�����Ϣ */


#define NETCMD_SET_PIPMODE		0x040400	/* ���û��л�ģʽ */
#define NETCMD_GETBMPBYTIME		0x090004	/* ��ʱ��ץBMPͼ */
#define NETCMD_MAKEIFRAME		0x090100	/* ǿ��I֡���� */
#define NETCMD_MAKESUBIFRAME	0x090101	/* ǿ��I֡���� */

#ifdef USEIMG
#define NETCMD_GET_FTPCFG				0x020002		/*��ȡͼƬ�ϴ�FTP����	*/
#define NETCMD_SET_FTPCFG				0x020003		/*����ͼƬ�ϴ�FTP����*/
#define NETCMD_GET_JPEGCFG			0x020004		/*��ȡͼƬ����	*/
#define NETCMD_SET_JPEGCFG			0x020005		/*����ͼƬ����*/
#define NETCMD_GET_JPEGPICTURE_EX				0x03000a		/*����jpegץͼ*/
#endif

#define NETCMD_GET_JPEGPICTURE			0x030009		/*ץJPEGͼ��*/

#ifdef VIDEO_SWITCH
#define NETCMD_VIDEO_SWITCH 0x090909 /* �л�5150������ */
#endif

#define NETCMD_FINDFILE_EVENTSEARCH	0x090413	/*�¼������ӿ�*/
#define NETCMD_GET_RTSPCFG      0x020c02
#define NETCMD_SET_RTSPCFG	    0x020c03
#define NETCMD_GET_RTSPPORT     0x020c04

#ifdef SUPPORT_SNMP
#define DVR_GET_SNMPCFG_V30       0x1110d7         //��ȡSNMPv30���� 
#define DVR_SET_SNMPCFG_V30       0x1110d8         //��ȡSNMPv30���� 
#define NETCMD_GET_SNMPCFG      0x1110d5 // get net app cfg, snmp
#define NETCMD_SET_SNMPCFG      0x1110d6 // set net app cfg, snmp
#endif




/* define status that return to client */
#define NETRET_QUALIFIED		1
#define NETRET_EXCHANGE			2
#define NETRET_ERRORPASSWD		3
#define NETRET_LOWPRI			4
#define NETRET_OPER_NOPERMIT	5
#define NETRET_VER_DISMATCH		6
#define NETRET_NO_CHANNEL		7
#define NETRET_NO_SERIAL		8
#define NETRET_NO_ALARMIN		9
#define NETRET_NO_ALARMOUT		10
#define NETRET_NO_DISK			11
#define NETRET_NO_HARDDISK		12
#define NETRET_NOT_SUPPORT		13
#define NETRET_ERROR_DATA		14
#define NETRET_CHAN_ERROR		15
#define NETRET_DISK_ERROR		16
#define NETRET_CMD_TIMEOUT		17
#define NETRET_MAXLINK			18
#define NETRET_NEEDRECVHEADER	19
#define NETRET_NEEDRECVDATA		20
#define NETRET_NEEDRECVDATA_V30	104
#define NETRET_IPCCFG_CHANGE	105
#define NETRET_IPCCFG_CHANGE_V31 106
#define NETRET_KEEPAUDIOTALK    NETRET_EXCHANGE
#define NETRET_RECVKEYDATA      90       /* SETPOS����շ��������ǹؼ�֡���ݣ���ǰ��I֡��*/
#define NETRET_RECVSETPOSDATA   91       /* SETPOS����ոı�λ�ú�ʵ��λ�õ����� */
#define NETRET_SAME_USER              49
#define NETRET_DEVICETYPE_ERROR        50 /*�������ʱ�豸�ͺŲ�ƥ��*/
#define NETRET_LANGUAGE_ERROR           51 /*�������ʱ������ƥ��*/
#define NETRET_PARAVERSION_ERROR      52/*�������ʱ����汾��ƥ��*/
#define NETRET_IPC_NOTCONNECT      	    53/*IPC������*/
/*ȥ��NETRET_720P_SUB_NOTSUPPORT, ��Ҫʱ���� NETRET_NOT_SUPPORT*/
//#define NETRET_720P_SUB_NOTSUPPORT         54/*720P ��������֧��*/
#define NETRET_IPC_COUNT_OVERFLOW      	54	/*IPC�������*/
#define NETRET_EMAIL_TEST_ERROR			55	/*�ʼ�����ʧ�� 9000_1.1*/

#define NETRET_PLAY_END			21
#define NETRET_WRITEFLASHERROR	22
#define NETRET_UPGRADE_FAILED	23
#define NETRET_UPGRADING		24
#define NETRET_NEEDWAIT			25
#define NETRET_FILELISTOVER		26
#define NETRET_RECVFILEINFO		27
#define NETRET_FORMATING		28
#define NETRET_FORMAT_END		29
#define NETRET_NO_USERID		30
#define NETRET_EXCEED_MAX_USER	31
#define NETRET_DVR_NO_RESOURCE	32
#define NETRET_DVR_OPER_FAILED	33		/* operation failed */
#define NETRET_IP_MISMATCH		34
#define NETRET_SOCKIP_MISMATCH	35
#define NETRET_MAC_MISMATCH		36
#define NETRET_ENC_NOT_STARTED	37
#define NETRET_LANG_MISMATCH	38		/* ����ʱ���Բ�ƥ�� */
#define NETRET_NEED_RELOGIN		39		/* ��Ҫ��������/�û���������֤ */
#define NETRET_HD_READONLY			48
#ifdef NET3G_SDK
#define NETRET_ALREADYLOGON     101    /* �豸�Ѿ�ע�ᵽCMS */
#define NETRET_ADDR_ALREADYUSED 102     /* udpԤ����ַ�ص� */
#define	NETRET_SNDALARM_GPS	140         /* GPS�����ϴ���Ϣ */
#define NETRET_SNDALARM_3GPICINFO    141 /* ͼƬ�ϴ� */
#define NETRET_NEEDRECVPUSHALARMDATA 142    /* ��ģʽ�豸����������Ϣ�ϴ� */
#define	NETRET_NEEDRECVPUSHALARMDATA_V30   143 /* ��ģʽ�豸����������Ϣ�ϴ� */
#endif

#define NETRET_NET_OVERFLOW       800    /*�����������������������*/

/*
 * ====================================================================================
 *									 net data struct
 * ====================================================================================
 */
typedef struct{
    UINT32  length;             /* total length */
    UINT8   ifVer;              /* version: 90 -- new interface/60 -- old interface */	
	UINT8   ipVer;
	UINT8	byRetVal;			/* ���λ��ʾ�������ͣ�REQUEST(0)��RESPONSE(1)������7λ���� */
	UINT8	res1[1];			/* just used in RESPONSE */
    UINT32  checkSum;           /* checksum */
    UINT32  netCmd;             /* client request command */
    UINT32  clientIp;           /* clinet IP address */
    UINT32  userID;             /* user ID */
    UINT8   clientMac[6];
    UINT8   res[2];
    struct in6_addr clientIp6;
}NETCMD_HEADER_V6;

#ifdef SUPPORT_IPV6
typedef struct{
    UINT32  length;             /* total length */
    UINT8   ifVer;              /* version: 90 -- new interface/60 -- old interface */	
	UINT8   ipVer;
	UINT8	byRetVal;			/* ���λ��ʾ�������ͣ�REQUEST(0)��RESPONSE(1)������7λ���� */
	UINT8	res1[1];			/* just used in RESPONSE */
    UINT32  checkSum;           /* checksum */
    UINT32  netCmd;             /* client request command */
    UINT32  clientIp;           /* clinet IP address */
    UINT32  userID;             /* user ID */
    UINT8   clientMac[6];
    UINT8   res[2];
    struct in6_addr clientIp6;
}NETCMD_HEADER;

typedef struct{
	UINT32 	length;				/* total length */
	UINT8  	ifVer;				/* version: 90 -- new interface/60 -- old interface */
	UINT8   ipVer;
	UINT8	byRetVal;			/* ���λ��ʾ�������ͣ�REQUEST(0)��RESPONSE(1)������7λ���� */
	UINT8	res1[1];
	UINT32	checkSum;			/* checksum */
	UINT32	netCmd;				/* client request command */
	UINT32	clientIp;			/* clinet IP address */
	UINT32	userID;				/* user ID */
	UINT8	clientMac[6];
	UINT8	res[2];
}NETCMD_HEADER_V4;

#else
typedef struct{
	UINT32 	length;				/* total length */
	UINT8  	ifVer;				/* version: 90 -- new interface/60 -- old interface */
	#ifdef NET3G_SDK
	UINT8	byCmdType;			/* ���λ��ʾ�������ͣ�REQUEST(0)��RESPONSE(1)������7λ���� */
	UINT8	byRetVal;			/* just used in RESPONSE */
	UINT8	res1[1];
	#else
	UINT8	res1[3];
	#endif
	UINT32	checkSum;			/* checksum */
	UINT32	netCmd;				/* client request command */
	UINT32	clientIp;			/* clinet IP address */
	UINT32	userID;				/* user ID */
	UINT8	clientMac[6];
	UINT8	res[2];
}NETCMD_HEADER;//32

#endif

typedef struct{
	NETCMD_HEADER header;
	UINT32 channel;
}NETCMD_CHAN_HEADER;

typedef struct{
	UINT8	startHour;
	UINT8	startMin;
	UINT8	stopHour;
	UINT8	stopMin;
}NETPARAM_TIMESEG;

//���ظ��ͻ���״̬��ͷ 
typedef struct{
	UINT32	length;
	UINT32	checkSum;
	UINT32	retVal;
	UINT8	res[4];
}NETRET_HEADER;

/* PTZ control when preview*/
typedef struct{
	NETCMD_HEADER header;
	UINT32 channel;
	UINT32 command;
	union{
	UINT32 presetNo;
	UINT32 speed;
	};
}NET_PTZ_CTRL_DATA;

#ifndef SUPPORT_IPV6
//login
typedef struct{
	UINT32 	length;
	UINT8	ifVer;
	UINT8	res1[3];
	UINT32	checkSum;
	UINT32	netCmd;
	UINT32	version;
	UINT8	res2[4];
	UINT32	clientIp;
	UINT8	clientMac[6];
	UINT8	res3[2];
	UINT8	username[NAME_LEN];
	UINT8	password[PASSWD_LEN];
}NET_LOGIN_REQ;
#else
//login
typedef struct{
	UINT32 	length;
	UINT8	ifVer;
	UINT8	res1[3];
	UINT32	checkSum;
	UINT32	netCmd;
	UINT32	version;
	UINT8	res2[4];
	UINT32	clientIp;
	UINT8	clientMac[6];
	UINT8	res3[2];
}NET_LOGIN_REQ;

typedef struct{
    UINT32  length;
    UINT8   ifVer;
    UINT8   ipVer;   /*0: IPv4; 1:IPv6*/
    UINT8   res1[2];
    UINT32  checkSum;
    UINT32  netCmd;
    UINT32  version;
    UINT8   res2[4];
    UINT32  clientIp;
    UINT8   clientMac[6];
    UINT8   res3[2];
    struct in6_addr clientIp6;
}NET_LOGIN_REQ_V6;

#endif




typedef struct{
	UINT32	length;
	UINT32	checkSum;
	UINT32	retVal;
	UINT32  devSdkVer;
	UINT32	userID;
	UINT8	serialno[SERIALNO_LEN];//68
	UINT8	devType;
	UINT8	channelNums;
	UINT8	firstChanNo;
	UINT8	alarmInNums;
	UINT8	alarmOutNums;
	UINT8	hdiskNums;
	UINT8   	supportProtocols;/*��֧�ֵ�Ԥ��Э�����ͣ���λ*/
	UINT8	audioChanNums;/*�����Խ�ͨ������*/
	UINT8      maxIpcChanNums;/*�豸֧�ֵ����IPC����*/
//#ifdef DVS65
	UINT8	subProto;
	UINT8	zeroChanNum; 		/* ��ͨ��������� */
	UINT8   support;	/*֧������*/
						/*support & 0x1, ��ʾ�Ƿ�֧����������*/
						/*support & 0x2, ��ʾ�Ƿ�֧�ֱ���*/
						/*support & 0x4, ��ʾ�Ƿ�֧��ѹ������������ȡ*/
						/*support & 0x8, ��ʾ�Ƿ�֧��˫����*/
    UINT8   support1;	//support1 & 0x1����ʾ�Ƿ�֧��snmp v30
	UINT8	res[27];
//#else
//	UINT8      res[31];
//#endif
}NET_LOGIN_RET;

typedef struct {
		UINT32	length;			/* ���������ܳ��� */
		UINT32  checkSum;		/* ����У��� */
		UINT32  retVal;			/* ����״̬���ض��ı�ʶ��ʾ��9000�豸 */
		UINT32  devSdkVer;
		UINT8	challenge[60];	/* BASE64����֮�����ս�� */
} NET_LOGIN_CHALLENGE;


#ifdef RTSP
typedef struct 
{
	UINT32	length;
	UINT16  rtspPort;/*RTSPЭ��˿�,Ĭ����554*/	
	UINT8	res[54];
} NETPARAM_RTSP_CFG;
#endif

//device parameter
typedef struct{
	UINT32 	length;
	UINT8	DVRName[NAME_LEN];
	UINT32	deviceID;
	UINT32	recycleRecord;
	UINT8	serialno[SERIALNO_LEN];
	UINT32	softwareVersion;
	UINT32	softwareBuildDate;
	UINT32	dspSoftwareVersion;
	UINT32	dspSoftwareBuildDate;
	UINT32	panelVersion;
	UINT32	hardwareVersion;
	UINT8	alarmInNums;
	UINT8	alarmOutNums;
	UINT8	rs232Nums;
	UINT8	rs485Nums;
	UINT8	netIfNums;
	UINT8	hdiskCtrlNums;
	UINT8	hdiskNums;
	UINT8	devType;
	UINT8	channelNums;
	UINT8	firstChanNo;
	UINT8	decodeChans;
	UINT8	vgaNums;
	UINT8	usbNums;
	UINT8	auxOutNum;
	UINT8	audioNum;
	UINT8 	ipChanNum;
}NETPARAM_DEVICE_CFG;


typedef struct{
    UINT32  length;
    UINT8   DVRName[NAME_LEN];
    UINT32  deviceID;
    UINT32  recycleRecord;
    UINT8   serialno[SERIALNO_LEN];
    UINT32  softwareVersion;
    UINT32  softwareBuildDate;
    UINT32  dspSoftwareVersion;
    UINT32  dspSoftwareBuildDate;
    UINT32  panelVersion;
    UINT32  hardwareVersion;
    UINT8   alarmInNums;
    UINT8   alarmOutNums;
    UINT8   rs232Nums;
    UINT8   rs485Nums;
    UINT8   netIfNums;
    UINT8   hdiskCtrlNums;
    UINT8   hdiskNums;
    UINT8   devType;
    UINT8   channelNums;
    UINT8   firstChanNo;
    UINT8   decodeChans;
    UINT8   vgaNums;
    UINT8   usbNums;
    UINT8   auxOutNum;
    UINT8   audioNum;
    UINT8   ipChanNum;
    UINT8   zeroChanNum;        /* ��ͨ��������� */
    UINT8   supportAbility;     /* ������λ����Ϊ0��ʾ��֧�֣�1��ʾ֧�֣�
                                 supportAbility & 0x1, ��ʾ�Ƿ�֧����������
                                supportAbility & 0x2, ��ʾ�Ƿ�֧��һ������
                                supportAbility & 0x4, ��ʾ�Ƿ�֧��ѹ������������ȡ
                                supportAbility & 0x8, ��ʾ�Ƿ�֧��˫���� 
                                supportAbility & 0x10, ��ʾ֧��Զ��SADP 
                                supportAbility & 0x20  ��ʾ֧��Raid����*/
    
    UINT8   bESataFun;          /* eSATA��Ĭ����;��0-Ĭ��¼��1-Ĭ�ϱ��� */
    UINT8   bEnableIPCPnp;      /* 0-��֧�ּ��弴�ã�1-֧�ּ��弴�� */
	
    UINT8 byStorageMode; //0-����ģʽ,1-�������
    UINT8 bySupport1;   //������λ����Ϊ0��ʾ��֧�֣�1��ʾ֧��
                        //bySupport1 & 0x1, ֧�� SNMP v30
                        //bySupport1 & 0x2, ֧�����ֻطź�����
    UINT16 wDevType;//�豸�ͺ�
    UINT8  byDevTypeName[24];//�豸�ͺ����� 
    UINT8  res[16]; //����
}NETPARAM_DEVICE_CFG_V40;



/*
	IP��ַ������IPv4��IPv6��ʽ�ĵ�ַ
*/
typedef struct
{		/* 24 bytes */
	struct in_addr	v4;							/* IPv4��ַ */
	struct in6_addr	v6;							/* IPv6��ַ */
	UINT8			res[4];
}U_IN_ADDR;


//NETPARAM_ETHER_CFG
#define MACADDR_LEN  6
typedef struct{
	U_IN_ADDR devIp;
	U_IN_ADDR devIpMask;
	UINT32	mediaType;		/* network interface type */
	UINT16	ipPortNo;		/* command port */
	UINT16  mtu;            /*MTU*/
	UINT8	macAddr[MACADDR_LEN];
	UINT8	res2[2];
}NETPARAM_ETHER_CFG_NEW;



//NETPARAM_PPPOE_CFG
typedef struct{
	UINT32 enablePPPoE;
	char PPPoEUser[NAME_LEN];
	char PPPoEPassword[PASSWD_LEN];
    char   res[4];
	U_IN_ADDR PPPoEIp;	
}NETPARAM_PPPOE_CFG;



//NETPARAM_NETWORK_CFG
typedef struct{
	UINT32	length;
	NETPARAM_ETHER_CFG_NEW etherCfg[MAX_ETHERNET];
	U_IN_ADDR manageHost1IpAddr;
	U_IN_ADDR manageHost2IpAddr;
	U_IN_ADDR alarmHostIpAddr;
	UINT16    manageHost1Port;
	UINT16    manageHost2Port;
	UINT16    alarmHostIpPort;
	UINT8	bUseDhcp;
	UINT8     res1[9];
	U_IN_ADDR dnsServer1IpAddr;
	U_IN_ADDR dnsServer2IpAddr;
	UINT8     ipResolver[MAX_DOMAIN_NAME];
	UINT16    ipResolverPort;
	UINT16    httpPortNo;
	UINT8     res2[4];
	U_IN_ADDR  multicastIpAddr;
	U_IN_ADDR  gatewayIpAddr;
	NETPARAM_PPPOE_CFG PPPoECfg;
	UINT8     res3[24];
}NETPARAM_NETWORK_CFG_V30;

typedef struct{
	UINT8	streamType;
	UINT8	resolution;
	UINT8	bitrateType;
	UINT8	quality;
	UINT32	maxBitRate;
	UINT32	videoFrameRate;
	UINT16  intervalFrameI;
	UINT8	BFrameNum;			/* B֡����: 0:BBP, 1:BP, 2:P */
	UINT8   EFrameNum;
	UINT8   videoEncType;
	UINT8   audioEncType;
	UINT8   res[10];
}NETPARAM_COMP_PARA_V30;

typedef struct{
	UINT32	length;
	NETPARAM_COMP_PARA_V30 normHighCompPara;
	NETPARAM_COMP_PARA_V30 normLowCompPara;
	NETPARAM_COMP_PARA_V30 eventCompPara;
	NETPARAM_COMP_PARA_V30 netCompPara;
}NETPARAM_COMPRESS_CFG_V30;

//8000 network parameter 
typedef struct{
	UINT32	devIp;
	UINT32	devIpMask;
	UINT32	mediaType;		/* network interface type */
	UINT16	ipPortNo;		/* command port */
	UINT16	mtu;			/* MTU */
	UINT8	macAddr[6];
	UINT8	res2[2];
}NETPARAM_ETHER_CFG;

typedef struct{
	UINT32	length;
	NETPARAM_ETHER_CFG etherCfg[MAX_ETHERNET];
	UINT32	manageHostIp;
	UINT16	manageHostPort;
	UINT16	httpPort;
	UINT32	ipResolverIpAddr;
	UINT32	mcastAddr;
	UINT32	gatewayIp;
	UINT32	nfsIp;
	UINT8	nfsDirectory[PATHNAME_LEN];
	UINT32	bEnablePPPoE;
	UINT8	pppoeName[NAME_LEN];
	UINT8	pppoePassword[PASSWD_LEN];
	UINT8	res2[4];
	UINT32	pppoeIp;
}NETPARAM_NETWORK_CFG;

typedef struct{
	UINT32 	length;
	UINT8	ifVer;
	UINT8	res1[3];
	UINT32	checkSum;
	UINT32	netCmd;
	UINT8   rec[16];
	UINT32  cmd0;
	UINT32  cmd1;
}NET_CMD11000_REQ;


/*
 * ========================================================
 *			PIC_CFG	aparameter
 * ========================================================
 */
typedef struct{
	UINT32 		handleType;						/* �쳣����,�쳣����ʽ��"��"��� */
	UINT32		alarmOutTriggered;				/* �������ı������(�˴�������󱨾������Ϊ32) */
}NETPARAM_EXCEPTION;

typedef struct{
	UINT32 		handleType;						/* �쳣����,�쳣����ʽ��"��"��� */
	UINT8		alarmOutTrigStatus[16];				/* �������ı������(�˴�������󱨾������Ϊ32) */
}NETPARAM_EXCEPTION_V30;

/*
 * ========================================================
 *			 alarmIn parameter 
 * ========================================================
 */
typedef struct{
	UINT8		alarmInName[NAME_LEN];				/* ���� */
	UINT8		sensorType;							/* ���������ͣ�0Ϊ������1Ϊ���� */
	UINT8		bEnableAlarmIn;						/* ���������� */
	UINT8		res[2];
	NETPARAM_EXCEPTION alarmInAlarmHandleType;				/* �������봦�� */
	NETPARAM_TIMESEG armTime[MAX_DAYS][MAX_TIMESEGMENT_8000];	/* ����ʱ��� */
	UINT8		recordChanTriggered[16];			/* ��������¼��(�˴��������ͨ����Ϊ128) */
	UINT8		bEnablePreset[MAX_CHANNUM_8000];			/* �Ƿ����Ԥ�õ� */
	UINT8		presetNo[MAX_CHANNUM_8000];				/* ���õ���̨Ԥ�õ����,һ������������Ե���
												   	   ���ͨ������̨Ԥ�õ�, 0xff��ʾ������Ԥ�õ㡣
												   	 */
	UINT8       bEnablePtzCruise[MAX_CHANNUM_8000];		/* �Ƿ����Ѳ�� */
	UINT8		ptzCruise[MAX_CHANNUM_8000];				/* Ѳ�� */
	UINT8       bEnablePtzTrack[MAX_CHANNUM_8000];		/* �Ƿ���ù켣 */
	UINT8		trackNo[MAX_CHANNUM_8000];				/* ��̨�Ĺ켣���(ͬԤ�õ�) */
}NETPARAM_ALARMIN;

typedef struct{
	UINT8		alarmInName[NAME_LEN];				/* ���� */
	UINT8		sensorType;							/* ���������ͣ�0Ϊ������1Ϊ���� */
	UINT8		bEnableAlarmIn;						/* ���������� */
	UINT8		res[2];
	NETPARAM_EXCEPTION_V30 alarmInAlarmHandleType;				/* �������봦�� */
	NETPARAM_TIMESEG armTime[MAX_DAYS][MAX_TIMESEGMENT];	/* ����ʱ��� */
	UINT8		recordChanTriggered[16];			/* ��������¼��(�˴��������ͨ����Ϊ128) */
	UINT8		bEnablePreset[MAX_CHANNUM_V30];			/* �Ƿ����Ԥ�õ� */
	UINT8		presetNo[MAX_CHANNUM_V30];				/* ���õ���̨Ԥ�õ����,һ������������Ե���
												   	   ���ͨ������̨Ԥ�õ�, 0xff��ʾ������Ԥ�õ㡣
												   	 */
	UINT8		bEnablePresetRevert[MAX_CHANNUM_V30];	
	UINT16		presetRevertDelay[MAX_CHANNUM_V30];
	UINT8       bEnablePtzCruise[MAX_CHANNUM_V30];		/* �Ƿ����Ѳ�� */
	UINT8		ptzCruise[MAX_CHANNUM_V30];				/* Ѳ�� */
	UINT8       bEnablePtzTrack[MAX_CHANNUM_V30];		/* �Ƿ���ù켣 */
	UINT8		trackNo[MAX_CHANNUM_V30];				/* ��̨�Ĺ켣���(ͬԤ�õ�) */
}NETPARAM_ALARMIN_V30;

typedef struct{
	UINT32	length;
	NETPARAM_ALARMIN alarmIn;
}NETPARAM_ALARMIN_CFG;

typedef struct{
	UINT32	length;
	NETPARAM_ALARMIN_V30 alarmIn;
}NETPARAM_ALARMIN_CFG_V30;







typedef struct{
	BYTE brightness;
	BYTE contrast;
	BYTE saturation;
	BYTE tonality;
}NETPARAM_COLOR;

typedef struct
{
	NETPARAM_COLOR	  color[MAX_TIMESEGMENT];
	NETPARAM_TIMESEG  handleTime[MAX_TIMESEGMENT];
}NETPARAM_VICOLOR_V30;

typedef struct{
	UINT32 		enableSignalLostAlarm;
	NETPARAM_EXCEPTION_V30 signalLostAlarmHandleType;	/* �źŶ�ʧ���� */
	NETPARAM_TIMESEG signalLostArmTime[MAX_DAYS][MAX_TIMESEGMENT];
}VI_SIGNAL_LOST_CFG_V30;

typedef struct{
	UINT8 	motionLine[64][12];
	UINT8	motionLevel;
	UINT8	bEnableHandleMotion;
	UINT8   precision;
	UINT8	res;
	NETPARAM_EXCEPTION_V30 motionHandleType;
	NETPARAM_TIMESEG motDetArmTime[MAX_DAYS][MAX_TIMESEGMENT];
	UINT8	motTriggerRecChans[16];
}NETPARAM_MOTDET_CFG_V30;

typedef struct{
	UINT32	bEnableMaskAlarm;
	UINT16	maskX;
	UINT16	maskY;
	UINT16	maskW;
	UINT16	maskH;
	NETPARAM_EXCEPTION_V30 maskAlarmHandleType;
	NETPARAM_TIMESEG maskAlarmArmTime[MAX_DAYS][MAX_TIMESEGMENT];
}NETPARAM_MASKALARM_CFG_V30;

#define MAX_MASK_REGION_NUM		4				/* ����ڵ�������� */
/*
	����
*/
typedef struct
{		/* 4 bytes */
	UINT16	x;									/* ������ */
	UINT16	y;									/* ������ */
} COORDINATE;

/*
	��������
*/
typedef struct
{		/* 8 bytes */
	COORDINATE	upLeft;							/* ���Ͻ����� */
	SIZE		size;							/* ��С(��Ⱥ͸߶�) */
} ZONE;

#define CHANNEL_MAIN			1				/*����*/

//NETPARAM_PIC_CFG_V30
typedef struct{
	UINT32 	length;
	UINT8	chanName[NAME_LEN];
	UINT32	videoFormat;
	NETPARAM_VICOLOR_V30 viColor;
	UINT32	bShowChanName;
	UINT16	chanNameX;
	UINT16	chanNameY;
	VI_SIGNAL_LOST_CFG_V30		viLostCfg;
	VI_SIGNAL_LOST_CFG_V30		aiLostCfg;
	NETPARAM_MOTDET_CFG_V30 	motionCfg;
	NETPARAM_MASKALARM_CFG_V30	maskAlarmCfg;
	UINT32	bEnableHide;
	ZONE hideArea[MAX_MASK_REGION_NUM];
	UINT32	bShowOsd;
	UINT16	osdX;
	UINT16	osdY;
	UINT8	osdType;
	UINT8	bDispWeek;
	UINT8	osdAttrib;
	UINT8	hourOsdType;
	UINT8	res2[24-16];
}NETPARAM_PIC_CFG_V30;



//security.h
/* defines */
#define MAX_LOGIN_USERS		128		/* maximum logined user numbers */
#ifdef NET3G_SDK
#define REG_USER_MAX_CMS    	16
#define REG_USER_BASE_INDEX 	(MAX_LOGIN_USERS - REG_USER_MAX_CMS)
#endif
#define BASE_USER_ID			0x10000
#define KEEP_ALIVE_INTERVAL	(5*60)		/* logined user must send keep alive message */
#define CHECK_USER_INTERVAL	5		/* check user timeout at interval 5 seconds */
#define	NO_USER					(-1)
//#ifdef PSIA_PAD
#define PSIA_USER           	NO_USER
//#endif

#define	ACTUAL_MAX_NAME_LEN	32
#define LIMITED_NAME_LEN		LOG_NAME_LEN			/* Now we limit username length to 32 bytes */

/* user login return status */
#define INVALID_USER			0x1000
#define EXCEED_MAX_USER		0x1001
#define EMPTY_USER				0x1002
#define USER_NO_PERMISSION 	0x1003

/* typedefs */
typedef struct
{
	int 	userID;				/* user ID, DVR assign an exclusive ID when user login */
	int    	cfgUserID;			/*��userID��Ӧ�����ò����е��û�ID*/
	UINT8	userName[NAME_LEN];		/* user name */
	UINT32	permission;			/* permission */
	UINT64  localPrevPermission;	
	UINT64	netPrevPermission;			/* Զ��Ԥ��ͨ�� */
	UINT64	localPlayPermission;			/* ���ػط�ͨ�� */
	UINT64	netPlayPermission;			/* Զ�̻ط�ͨ�� */
	UINT64	localRecordPermission;		/* ���ػط�ͨ�� */
	UINT64	netRecordPermission;		/* Զ�̻ط�ͨ�� */
	UINT64	localPtzCtrlPermission;		/* ���ػط�ͨ�� */
	UINT64	netPtzCtrlPermission;		/* Զ�̻ط�ͨ�� */
	UINT64	localBackupPermission;		/* ���ػط�ͨ�� */
	UINT32	priority;
	BOOL	bLocalUser;			/* local user or network user */
	time_t	loginTime;			/* timer of user login */
	time_t	expireTime;			/* client should send a 'keepalive' at intervals of 'KEEP_ALIVE_INTERVAL' seconds when idle */	
#ifndef SUPPORT_IPV6
	struct in_addr peerClientIp;		/* peer's IP address get from client program */
	struct in_addr peerSocketIp;		/* peer's Ip address get form socket accept() */
#else
	U_IN_ADDR peerClientIp; 	/* peer's IP address get from client program */
	U_IN_ADDR peerSocketIp; 	/* peer's Ip address get form socket accept() */
#endif
//	struct in_addr peerClientIp;		/* peer's IP address get from client program */
//	struct in_addr peerSocketIp;		/* peer's Ip address get form socket accept() */
	UINT16  persistOpNums;			/* persistent operation(e.g. preview, download file) numbers of this user */
	char	peerMacAddr[MACADDR_LEN];	/* peer's MAC address */
	UINT32 	sdkVersion;			/* SDK version of network user */
}USER_INFO;



#define CLOSE_ALL		0	/* �ر�(��Ͽ�)���п��� */
#define CAMERA_PWRON	1	/* ��ͨ�������Դ */
#define LIGHT_PWRON		2	/* ��ͨ�ƹ��Դ */
#define WIPER_PWRON		3	/* ��ͨ��ˢ���� */
#define FAN_PWRON		4	/* ��ͨ���ȿ��� */
#define HEATER_PWRON	5	/* ��ͨ���������� */
#define AUX_PWRON1		6	/* ��ͨ�����豸���� */
#define AUX_PWRON2		7	/* ��ͨ�����豸���� */
#define SET_PRESET		8	/* ����Ԥ�õ� */
#define CLE_PRESET		9	/* ���Ԥ�õ� */

#define STOP_ALL		10	/* ֹͣ����������(��ͷ, ��̨)���� */
#define ZOOM_IN			11	/* �������ٶ�SS���(���ʱ��) */
#define ZOOM_OUT		12	/* �������ٶ�SS��С(���ʱ�С) */
#define FOCUS_IN		13	/* �������ٶ�SSǰ�� */
#define FOCUS_OUT		14	/* �������ٶ�SS��� */
#define IRIS_ENLARGE	15	/* ��Ȧ���ٶ�SS���� */
#define IRIS_SHRINK		16	/* ��Ȧ���ٶ�SS��С */
#define AUTO_ZOOM		17	/* ���Զ�����(�Զ�����) */
#define AUTO_FOCUS		18	/* ���Զ����� */
#define AUTO_IRIS		19	/* ���Զ���Ȧ */

#define TILT_UP			21	/* ��̨��SS���ٶ����� */
#define TILT_DOWN		22	/* ��̨��SS���ٶ��¸� */
#define PAN_LEFT		23	/* ��̨��SS���ٶ���ת */
#define PAN_RIGHT		24	/* ��̨��SS���ٶ���ת */
#define UP_LEFT			25	/* ��̨��SS���ٶ���������ת */
#define UP_RIGHT		26	/* ��̨��SS���ٶ���������ת */
#define DOWN_LEFT		27	/* ��̨��SS���ٶ��¸�����ת */
#define DOWN_RIGHT		28	/* ��̨��SS���ٶ��¸�����ת */
#define AUTO_PAN		29	/* ��̨��SS���ٶ������Զ�ɨ�� */

#define FILL_PRE_SEQ	30	/* ��Ԥ�õ����Ѳ������ */
#define SET_SEQ_DWELL	31	/* ����Ѳ����ͣ��ʱ�� */
#define SET_SEQ_SPEED	32	/* ����Ѳ���ٶ� */
#define CLE_PRE_SEQ		33	/* ��Ԥ�õ��Ѳ���ٶ���ɾ�� */
#define STA_MEM_CRUISE	34	/* Set Cruise start memorize */
#define STO_MEM_CRUISE	35	/* Set Cruise stop memorize */
#define RUN_CRUISE		36	/* ��ʼ�켣 */
#define RUN_SEQ			37	/* ��ʼѲ�� */
#define STOP_SEQ		38	/* ֹͣѲ�� */
#define GOTO_PRESET		39  /* ����ת��Ԥ�õ� */
#define SYSTEM_RESET	40	/* ϵͳ��λ */

#endif


