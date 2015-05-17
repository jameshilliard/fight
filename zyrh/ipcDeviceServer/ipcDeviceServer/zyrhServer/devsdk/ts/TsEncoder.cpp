#include "StdAfx.h"
#include "TsEncoder.h"
#include "x264/H264FrameParser.h"
#include <iostream>
#define system_clock_frequency 27000000
#define basicValue 8589934592
#define AV_TIME_BASE            1000000
#define  FRISTTSLEN      176     //包含TS包头 和 PCR 
#define  MINTSLEN        184     //包含TS
#define  LASTLEN        182     //包含TS

#define  FRISTTSLENAUDIO      182    
const uint64_t delay = 200;//(200 * 90000/  AV_TIME_BASE) * 2;
uint8_t arrarff[188] = {0};
CTsEncoder::CTsEncoder()
{
	m_outbuf = new uint8_t[1000*500];
	m_nprogram_map_PID = 0x0100;
	m_nVideoPID = 0x0102;
	m_nAudioPID = 0x0101;
	if (arrarff[0] == 0x00)
	{
		for (int i = 0;i<188;i++)
		{
			arrarff[i] = 0xff;
		}
	}
	ResetParam();
}
void CTsEncoder::ResetParam()
{
	m_bVideobegin = false;
	m_nWidth = 0;
	m_nHeight = 0;
	nFrameRate = 0;
	m_outlen = 0;
	m_lastTic = 0;
	m_nSampleRate = 0;
	m_nNumChannels = 0;
	m_file = NULL;
	m_continuity_counter = 0;
	m_lastDts = 0;
	m_VideoCount = 0;
	m_AduioCount = 0;
	m_PATCount = 0;
	m_PMTCount = 0;
	m_beginTic = 0;


}
CTsEncoder::~CTsEncoder(void)
{
	delete[] m_outbuf;

}
int CTsEncoder::OpenFile(std::string dir,std::string tsFileName)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	std::string fulldirandName = dir +tsFileName;
	m_file = fopen(fulldirandName.c_str(),"wb");
	m_strFileName = tsFileName;
	if (!m_file)
	{
		g_logger.TraceInfo("fopen failer %s",tsFileName.c_str());
		return -1;
	}
	return 1;
}
bool CTsEncoder::IsOpen()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_file == NULL)
	{
		return false;
	}
	return true;

}
void CTsEncoder::SetTsName(std::string TsFileName)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_strFileName = TsFileName;
}
void CTsEncoder::PutPmt( )
{
	buffer *ptsbuf = new buffer;
	//插入ts 包头
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 1;	   //1bit 有效荷载单元起始指示符 
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nprogram_map_PID;             //13bit PMT 是 0x100
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control =  1 ;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_PMTCount;		       //4bit 连续计数器 一个4bit的计数器，范围0-15

	m_PMTCount++;
	if (m_PMTCount > 15)
	{
		m_PMTCount = 0;
	}
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);

	//ts包头结束
	uint8_t Point_field = 0;
	put_byte(ptsbuf,Point_field);

	int startCrc = ptsbuf->d_cur;
	//开始写入pmt数据

	// 6byte
	uint8_t table_id = 0x02;                 // 8bit pmt table_id 是0x02
	put_byte(ptsbuf,table_id);

	uint8_t	section_syntax_indicator = 1;    // 1 bslbf 
	uint8_t zero = 0;	                     // 1bsl bf 
	uint8_t	reserved_1 = 0x03;               // 2 bslbf 固定为0x03
	uint16_t section_length = 23;          // 12  长度 uimsbf 首先两位bit置为00，它指示段的byte数，由段
	//长度域开始，包含CRC

	// 8byte
	put_be16(ptsbuf,section_syntax_indicator<<15|
		zero<<14|
		reserved_1<<12|
		section_length);


	//2 byte  NO: 10byte
	uint16_t program_number = 0x01;          // 16 uimsbf 
	put_be16(ptsbuf,program_number);

	//1 byte NO: 11byte
	uint8_t	reserved_2 = 0x03;               // 2bsl bf 固定为0x03
	uint8_t	version_number = 0x0;            // 5ui m sbf 固定为0
	uint8_t	current_next_indicator = 0x01;   // 1 bslbf 固定为0x01

	put_byte(ptsbuf,reserved_2<<6|
		version_number<<1 |
		current_next_indicator
		);
	//2 byte  NO: 13byte
	uint8_t	section_number = 0x0;            // 8 uimsbf 固定为0x0
	uint8_t	last_section_number =  0x0;      // 8 uimsbf  固定为0x0
	put_byte(ptsbuf,section_number);
	put_byte(ptsbuf,last_section_number);

	//2 byte NO: 15byte
	uint8_t	reserved3 = 0x07;                // 3 bslbf 固定为0x07
	uint16_t PCR_PID =  m_nVideoPID;         // 13 uimsbf 指明TS包的PID值，该TS包含有PCR域，

	put_byte(ptsbuf,reserved3<<5|PCR_PID>>8);
	put_byte(ptsbuf,PCR_PID);


	//2 byte NO: 17byte
	uint8_t	reserved4 = 0x0F;                //4 bslbf 
	uint16_t program_info_length = 0x0000;    //12 u i m s b f 

	put_byte(ptsbuf,reserved4<<4|program_info_length>>8);
	put_byte(ptsbuf,program_info_length);

	//write vidoe 5byte
	uint8_t steam_type ;      //8bit  
	uint8_t	reserved5 ;        //3bit  固定为 0x07
	uint16_t elementary_PID ; //13bit 
	uint8_t	 reserved6 ;       //4bit  固定为 0x0f 
	uint16_t ES_info_length ; //12bit 固定为 0x00 

	//write aac 5byte
	steam_type = 0x0f;      //8bit  音频的steam_type 为0x0f
	reserved5 = 0x07;        //3bit  固定为 0x07
	elementary_PID = m_nAudioPID;   //13bit 视频的 
	reserved6 = 0x0f;        //4bit  固定为 0x0f 
	ES_info_length = 0x00;  //12bit 固定为 0x00 
	put_byte(ptsbuf,steam_type);
	put_byte(ptsbuf,reserved5<<5|
		elementary_PID>>8);
	put_byte(ptsbuf,elementary_PID);

	put_byte(ptsbuf,reserved6<<4|ES_info_length>>8);
	put_byte(ptsbuf,ES_info_length);

	//write vidoe 5byte
	steam_type = 0x1b;      //8bit  视频的steam_type 为0x1b
	reserved5 = 0x07;        //3bit  固定为 0x07
	elementary_PID = m_nVideoPID; //13bit 视频的 
	reserved6 = 0x0f;       //4bit  固定为 0x0f 
	ES_info_length = 0x00; //12bit 固定为 0x00 

	put_byte(ptsbuf,steam_type);
	put_byte(ptsbuf,reserved5<<5|
		elementary_PID>>8);
	put_byte(ptsbuf,elementary_PID);

	put_byte(ptsbuf,reserved6<<4|ES_info_length>>8);
	put_byte(ptsbuf,ES_info_length);

	// 4byte
	uint32_t nCrc32 = CRC32(ptsbuf->data + startCrc,ptsbuf->d_cur - startCrc); 
	put_be32(ptsbuf,nCrc32);
	ptsbuf->d_cur = 188;
	WritetoFile(ptsbuf);
	delete ptsbuf;
}





void CTsEncoder::PutPat()
{
	buffer *ptsbuf = new buffer;
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 1;	   //1bit 有效荷载单元起始指示符 
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = 0x0000;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control = 1;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_PATCount;		       //4bit 连续计数器 一个4bit的计数器，范围0-15

	m_PATCount++;

	if (m_PATCount > 15)
	{
		m_PATCount = 0;
	}
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID<<8);
	put_byte(ptsbuf,PID>>8);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);

	//ts包头结束
	uint8_t Point_field = 0;
	put_byte(ptsbuf,Point_field);
	int startCrc = ptsbuf->d_cur;
	//开始写入PAT数据

	uint8_t table_id = 0x00;                 // 8bit PAT table_id 是0x00
	put_byte(ptsbuf,table_id);

	uint8_t	section_syntax_indicator = 1;    // 1 bslbf 
	uint8_t zero = 0;	                     // 1bsl bf 
	uint8_t	reserved_1 = 0x3;               // 2 bslbf 固定为0x3
	uint16_t section_length = 13;          // 12  长度 uimsbf 首先两位bit置为00，它指示段的byte数 
	//12;//表示这个字节后面有用的字节数，包括CRC32
	//长度域开始，包含CRC

	put_be16(ptsbuf,section_syntax_indicator<<15|
		zero<<14|
		reserved_1<<12|
		section_length
		);


	//2 byte
	uint16_t transport_stream_id = 0x0001;          // 16 uimsbf 
	put_be16(ptsbuf,transport_stream_id);

	//1 byte
	uint8_t	reserved_2 = 0x03;               // 2bsl bf 固定为0x03
	uint8_t	version_number = 0x0;            // 5ui m sbf 固定为0
	uint8_t	current_next_indicator = 0x01;   // 1 bslbf 固定为0x01

	put_byte(ptsbuf,reserved_2<<6|
		version_number<<1 |
		current_next_indicator
		);
	//2 byte
	uint8_t	section_number = 0x0;            // 8 uimsbf 固定为0x0
	uint8_t	last_section_number =  0x0;      // 8 uimsbf  固定为0x0
	put_byte(ptsbuf,section_number);
	put_byte(ptsbuf,last_section_number);


	//2 byte
	uint16_t program_number = 0x0001;  
	put_be16(ptsbuf,program_number);

	//2 byte
	uint8_t	 reserved3 = 0x07;           // 3 bslbf 固定为0x07
	uint16_t program_map_PID  = m_nprogram_map_PID;  //13bit 节目映射表的PID，节目号大于0时对应的PID，每个节目对应一个
	put_byte(ptsbuf,reserved3<<5|program_map_PID>>8);
	put_byte(ptsbuf,program_map_PID);


	// 4byte
	uint32_t nCrc32 = CRC32(ptsbuf->data + startCrc,ptsbuf->d_cur - startCrc); 
	put_be32(ptsbuf,nCrc32);
	ptsbuf->d_cur = 188;
	WritetoFile(ptsbuf);
	delete ptsbuf;

}
//filenum 填充ff的大小
void CTsEncoder::PutFristTsVideo(buffer *ptsbuf,__int64 tic,uint8_t fileNum)
{
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 0x1;    //1bit 有效荷载单元起始指示符   
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nVideoPID;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control =  3;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_VideoCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

	if (m_VideoCount >  15)
	{
		m_VideoCount = 0;
	}
	// 4byte
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);
	//8byte

	uint8_t adaptation_field_length = 7 + fileNum;// 长度  8bit 
	uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
	uint8_t random_access_indicator = 0x1; //1bit 为0x0   
	uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
	uint8_t PCR_flag = 0x1;//1bit 为0x0  时间戳标志位
	uint8_t OPCR_flag = 0x0;//1bit 为0x0  
	uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
	uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
	uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
	//2 byte
	put_byte(ptsbuf,adaptation_field_length);
	put_byte(ptsbuf,PCR_flag<<4);
	//时间戳
	__int64 pcr_base  = (tic*system_clock_frequency/1000/300) % basicValue; //bit 33

	uint16_t PCR_ext   =  (system_clock_frequency * tic /1000) % 300;

	__int64 PCR = pcr_base*300 + PCR_ext;

	uint32_t pcr_base32 = pcr_base>>1;

	//6byte
	put_be32(ptsbuf,pcr_base32);

	uint8_t nextbit = pcr_base&0x0000000000000001;
	if (PCR_ext > 255)
	{
		put_byte(ptsbuf,nextbit<<7|0x01);
		PCR_ext -= 256;
	}
	else
	{
		put_byte(ptsbuf,nextbit<<7|0x00);
	}
	put_byte(ptsbuf,PCR_ext);
	if (fileNum > 0)
	{
		append_data(ptsbuf,arrarff,fileNum);
	}

}
void CTsEncoder::PutMiddleTsVideo(buffer *ptsbuf)
{
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nVideoPID;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control =  01;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_VideoCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

	if (m_VideoCount >  15)
	{
		m_VideoCount = 0;
	}
	// 4byte
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);
}
//填充 ff 的大小
void CTsEncoder::PutlastTsVideo(buffer *ptsbuf,uint8_t filesze)
{
	uint8_t sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nVideoPID;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control =  03;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_VideoCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

	if (m_VideoCount >  15)
	{
		m_VideoCount = 0;
	}
	// 4byte
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);

	uint8_t adaptation_field_length = 0 + filesze;// 长度  8bit 
	uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
	uint8_t random_access_indicator = 0x1; //1bit 为0x0   
	uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
	uint8_t PCR_flag = 0x0;//1bit 为0x0  时间戳标志位
	uint8_t OPCR_flag = 0x0;//1bit 为0x0  
	uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
	uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
	uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
	//2 byte
		
	if (filesze == 1)
	{
		adaptation_field_length = 0;;
		put_byte(ptsbuf,adaptation_field_length);
	}
	else if(filesze == 2)
	{
		adaptation_field_length = 1;
		put_byte(ptsbuf,adaptation_field_length);
		put_byte(ptsbuf,random_access_indicator<<6);
	}
	else
	{
		adaptation_field_length = filesze - 1;
		put_byte(ptsbuf,adaptation_field_length);
		put_byte(ptsbuf,random_access_indicator<<6);
		append_data(ptsbuf,arrarff,filesze -2);
	}
	
}
void CTsEncoder::CreatePesVideoPacket(buffer *ptsbuf,uint8_t* videobuf,uint32_t videoSize,bool bkey,__int64 timestamp)
{
	uint8_t	nPTS_DTS_flags = 0x3;  //变量

	const uint8_t *p = videobuf, *buf_end = p+videoSize;
	uint32_t state = -1;
	uint32_t packet_start_code_prefix = 0x000001;		// 24bit PES包起始前缀码 固定为0x000001
	uint8_t	 stream_id	= 0xE0;	        		//8bit PES流标识 视频为0xE0
	uint32_t pes_packet_length32 = 6;;		            //16bit PES包长度

	if (nPTS_DTS_flags == 0x2)
	{
		pes_packet_length32 += videoSize  + 5 + 3;
	}
	else if(nPTS_DTS_flags == 0x3)
	{
		pes_packet_length32 += videoSize  + 10 + 3;
	}
	uint16_t pes_packet_length16 = pes_packet_length32;
	if (pes_packet_length32 > 0xffff )
	{
		pes_packet_length16 = 0x00;
	}
	
	put_be24(ptsbuf,packet_start_code_prefix);
	put_byte(ptsbuf,stream_id);
	put_be16(ptsbuf,pes_packet_length16);

	//1byte 
	uint8_t fix = 0x2;  //2bit 固定10
	uint8_t	pes_scrambling_control = 0x0;		//2bit PES加扰控制
	uint8_t	pes_priority = 0x0;					//1bitPES优先级
	uint8_t	data_alignment_indicator = 0x0;		//1bit数据定位指示符
	uint8_t	copyright = 0x0;		     //1bit版权
	uint8_t	original_or_copy = 0x0;		//1bit 原版或拷贝
	put_byte(ptsbuf,fix<<6|data_alignment_indicator<<2);

	//1byte 
	uint8_t	PTS_DTS_flags = nPTS_DTS_flags; 		//2bit  PTS/DTS标志
	uint8_t	ESCR_flag  = 0x0;		     //1bit ESCR_flag
	uint8_t ES_rate_flag = 0x0;          //1bit ES_rate_flag
	uint8_t DSM_trick_mode_flag = 0x0;          //1bit DSM_trick_mode_flag
	uint8_t additional_copy_info_flag = 0x0;          //1bit additional_copy_info_flag
	uint8_t PES_CRC_flag = 0x0;          //1bit PES_CRC_flag
	uint8_t PES_extension_flag = 0x0;          //1bit PES_extension_flag
	put_byte(ptsbuf,PTS_DTS_flags<<6);

	//1byte 
	uint8_t PES_header_data_length = 0xa; //8bit PES_header_data_length
	if (nPTS_DTS_flags == 0x2)
	{
		PES_header_data_length -= 5;
	}  
	put_byte(ptsbuf,PES_header_data_length);
	if (nPTS_DTS_flags == 0x3 )     
	{
		__int64 Dts64 = timestamp*90 + delay;

		uint8_t fix1 = 0x3;//4bit 固定为0x3
		uint16_t PTS30_32 = Dts64>>30&0x7;  //3bit PTS 30——32 
		uint8_t marker_bit = 0x1;//1bit 固定为1
		put_byte(ptsbuf,fix1<<4|PTS30_32<<1|marker_bit);

		uint16_t PTS15_29 = (Dts64&0x3FFF8000)>>15 ;  //3bit PTS 15——29 
		uint8_t marker_bit1 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf,PTS15_29<<1|marker_bit1);

		uint16_t PTS0_14 = Dts64&0x7FFF;  //3bit PTS 15——29 
		uint8_t marker_bit2 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf,PTS0_14<<1|marker_bit2);

		//DTS 5byte
		uint8_t fix2 = 0x1;//4bit 固定为0x1
		uint16_t DTS30_32 = PTS30_32;  //3bit DTS 30——32 
		uint8_t marker_bit3 = 0x1;//1bit 固定为1 
		put_byte(ptsbuf,fix2<<4|DTS30_32<<1|marker_bit3);


		uint16_t DTS15_29 = PTS15_29;  //15bit DTS 15——29 
		uint8_t marker_bit4 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf,DTS15_29<<1|marker_bit4);

		uint16_t DTS0_14 = PTS0_14;  //15bit DTS 0——15
		uint8_t marker_bit5 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf,DTS0_14<<1|marker_bit5);

		//if (bkey)
		{
			put_be32(ptsbuf,0x00000001);
			put_byte(ptsbuf,0x09);
			put_byte(ptsbuf,0xf0);
		}
	} 
	else if (PTS_DTS_flags == 0x02)
	{

		__int64 nowDts = timestamp*90 + delay;

		uint8_t fix1 = 0x3;//4bit 固定为0x3
		uint16_t PTS30_32 = nowDts>>30&0x7;  //3bit PTS 30——32 
		uint8_t marker_bit = 0x1;//1bit 固定为1
		put_byte(ptsbuf,fix1<<4|PTS30_32<<1|marker_bit);

		uint16_t PTS15_29 = (nowDts&0x3FFF8000)>>15 ;  //3bit PTS 15——29 
		uint8_t marker_bit1 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf,PTS15_29<<1|marker_bit1);


		uint16_t PTS0_14 = nowDts&0x7FFF;  //3bit PTS 15——29 
		uint8_t marker_bit2 = 0x1;//1bit 固定为1 
		put_be16(ptsbuf,PTS0_14<<1|marker_bit2);


		//if (bkey)    // AUD NAL
		{
			put_be32(ptsbuf,0x00000001);
			put_byte(ptsbuf,0x09);
			put_byte(ptsbuf,0xff);
		}
	}
	append_data(ptsbuf,videobuf,videoSize);
}
void CTsEncoder::CreatePesAudioPacket(buffer *ptsbuf,uint8_t* audiobuf,uint32_t audioSize,__int64 timestamp)
{
	uint32_t packet_start_code_prefix = 0x000001;		// 24bit PES包起始前缀码 固定为0x000001
	uint8_t	 stream_id	= 0xC0;	        		//8bit PES流标识 音频为0xC0
	uint16_t pes_packet_length = audioSize + 5 + 3;;		            //16bit PES包长度

	put_be24(ptsbuf,packet_start_code_prefix);
	put_byte(ptsbuf,stream_id);
	put_be16(ptsbuf,pes_packet_length);

	//1byte 
	uint8_t fix = 0x2;  //2bit 固定10
	uint8_t	pes_scrambling_control = 0x0;		//2bit PES加扰控制
	uint8_t	pes_priority = 0x0;					//1bitPES优先级
	uint8_t	data_alignment_indicator = 0x1;		//1bit数据定位指示符
	uint8_t	copyright = 0x0;		     //1bit版权
	uint8_t	original_or_copy = 0x0;		//1bit 原版或拷贝
	put_byte(ptsbuf,fix<<6|data_alignment_indicator<<2);

	//1byte 
	uint8_t	PTS_DTS_flags = 0x2; 		 //2bit  PTS/DTS标志
	uint8_t	ESCR_flag  = 0x0;		     //1bit ESCR_flag
	uint8_t ES_rate_flag = 0x0;          //1bit ES_rate_flag
	uint8_t DSM_trick_mode_flag = 0x0;          //1bit DSM_trick_mode_flag
	uint8_t additional_copy_info_flag = 0x0;          //1bit additional_copy_info_flag
	uint8_t PES_CRC_flag = 0x0;          //1bit PES_CRC_flag
	uint8_t PES_extension_flag = 0x0;          //1bit PES_extension_flag
	put_byte(ptsbuf,PTS_DTS_flags<<6);


	//1byte 
	uint8_t PES_header_data_length = 0x5; //8bit PES_header_data_length
	put_byte(ptsbuf,PES_header_data_length);


	__int64 nowPts = timestamp*90 + delay ;//+ delay;
	//PTS 5byte
	uint8_t fix1 = 0x2;//4bit 固定为0x3
	uint16_t PTS30_32 = nowPts>>30&0x7;  //3bit PTS 30——32 
	uint8_t marker_bit = 0x1;//1bit 固定为1
	put_byte(ptsbuf,fix1<<4|PTS30_32<<1|marker_bit);

	uint16_t PTS15_29 = (nowPts&0x3FFF8000)>>15 ;  //3bit PTS 15——29 
	uint8_t marker_bit1 = 0x1;//1bit 固定为1 
	put_be16(ptsbuf,PTS15_29<<1|marker_bit1);


	uint16_t PTS0_14 = nowPts&0x7FFF;  //3bit PTS 15——29 
	uint8_t marker_bit2 = 0x1;//1bit 固定为1 
	put_be16(ptsbuf,PTS0_14<<1|marker_bit2);
	append_data(ptsbuf,audiobuf,audioSize);
}
void CTsEncoder::CloseFile()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_file)
	{
		fclose(m_file);
	}
	m_file = NULL;


}


void CTsEncoder::handleAudio(uint8_t* aacbuf,uint32_t bufsize,__int64 tic)
{
	if (!m_bVideobegin)
	{
		return;
	}
	
	__int64 ticFrameRate = tic;
	//ticFrameRate = CheckTimeStamp(tic);
	// 查找NAL起始字节
	bool bBegin = true;
	buffer pesBuffer;
	buffer tsBuffer;
	CreatePesAudioPacket(&pesBuffer,(uint8_t *)aacbuf,bufsize,ticFrameRate);
	bool bFritsTsPacket = true;
	uint32_t nread = 0;
	while(nread < pesBuffer.d_cur)
	{
		if (bFritsTsPacket)
		{
			bFritsTsPacket = false;
			int remainSize = pesBuffer.d_cur - nread;
			if(remainSize <= FRISTTSLENAUDIO)
			{
				PutFristTsAudio(&tsBuffer,ticFrameRate,FRISTTSLENAUDIO - pesBuffer.d_cur);
				append_data(&tsBuffer,pesBuffer.data,pesBuffer.d_cur);
			}
			else
			{
				PutFristTsAudio(&tsBuffer,ticFrameRate,0);
				append_data(&tsBuffer,pesBuffer.data,FRISTTSLENAUDIO);
			}
			nread = FRISTTSLENAUDIO;
		}
		else
		{
			int remainSize = pesBuffer.d_cur - nread;
			if ( remainSize >= MINTSLEN )
			{
				PutMiddleTsAudio(&tsBuffer);
				append_data(&tsBuffer,pesBuffer.data + nread,MINTSLEN);
				nread += MINTSLEN;
			}
			else 
			{
				PutlastTsAudio(&tsBuffer,MINTSLEN - remainSize);
				append_data(&tsBuffer,pesBuffer.data + nread,remainSize);
				nread += remainSize;
			}
		}
		WritetoFile(&tsBuffer);
		tsBuffer.Clear();
	}
}
void CTsEncoder::handleVideo(uint8_t* vidoebuf,uint32_t bufsize,__int64 tic,bool bkey)
{
	__int64 ticFrameRate = tic;
	//ticFrameRate = CheckTimeStamp(tic);
	 
	PutPat();
	PutPmt();
	// 查找NAL起始字节
	bool bBegin = true;
	buffer pesBuffer;
	buffer tsBuffer;
	CreatePesVideoPacket(&pesBuffer,(uint8_t *)vidoebuf,bufsize,bkey,ticFrameRate);
	bool bFritsTsPacket = true;;
	uint32_t nread = 0;
	int remainSize;
	while(nread < pesBuffer.d_cur)
	{
		if (bFritsTsPacket)
		{
			bFritsTsPacket = false;
			remainSize = pesBuffer.d_cur - nread;
			if(remainSize <= FRISTTSLEN)
			{
				PutFristTsVideo(&tsBuffer,ticFrameRate,FRISTTSLEN - pesBuffer.d_cur);
				append_data(&tsBuffer,pesBuffer.data,pesBuffer.d_cur);
			}
			else
			{
				PutFristTsVideo(&tsBuffer,ticFrameRate,0);
				append_data(&tsBuffer,pesBuffer.data,FRISTTSLEN);
			}
			nread = FRISTTSLEN;
		}
		else
		{
			remainSize = pesBuffer.d_cur - nread;
			if ( remainSize >= MINTSLEN )
			{
				PutMiddleTsVideo(&tsBuffer);
				append_data(&tsBuffer,pesBuffer.data + nread,MINTSLEN);
				nread += MINTSLEN;
			}
			else 
			{
				PutlastTsVideo(&tsBuffer,MINTSLEN - remainSize);
				append_data(&tsBuffer,pesBuffer.data + nread,remainSize);	
				nread += remainSize;
			}
		}	
	}
	WritetoFile(&tsBuffer);
	tsBuffer.Clear();
	m_bVideobegin = true;
}




void CTsEncoder::WritetoFile(buffer* buf)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	if (m_file)
	{
		fwrite(buf->data,1,buf->d_cur,m_file);
	}

}
uint8_t CTsEncoder::GetCounter()
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	m_continuity_counter++;
	uint8_t nRet = m_continuity_counter;
	if (nRet >= 15)
	{
		m_continuity_counter = 0;
	}
	return nRet;
}
void CTsEncoder::PutFristTsAudio(buffer *ptsbuf,__int64 tic,uint8_t fileNum)
{
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 0x1;    //1bit 有效荷载单元起始指示符   
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nAudioPID;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control = 0x3;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_AduioCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

	if (m_AduioCount >  15)
	{
		m_AduioCount = 0;
	}
	// 4byte
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);
	//8byte

	uint8_t adaptation_field_length = 1 + fileNum;// 长度  8bit 
	uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
	uint8_t random_access_indicator = 0x1; //1bit 为0x0   
	uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
	uint8_t PCR_flag = 0x0;//1bit 为0x0  时间戳标志位
	uint8_t OPCR_flag = 0x0;//1bit 为0x0  
	uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
	uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
	uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
	//2 byte
	put_byte(ptsbuf,adaptation_field_length);
	put_byte(ptsbuf,0x0);

	if (fileNum > 0)
	{
		append_data(ptsbuf,arrarff,fileNum);
	}

}
void CTsEncoder::PutMiddleTsAudio(buffer *ptsbuf)
{
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nAudioPID;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control = 0x01;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_AduioCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

	if (m_AduioCount >  15)
	{
		m_AduioCount = 0;
	}
	// 4byte
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);

}
void CTsEncoder::PutlastTsAudio(buffer *ptsbuf,uint8_t filesze)
{
	uint8_t  sync_byte	 = 0x47;	               //8bit同步字节, 固定为0x47,表示后面的是一个TS分组
	uint8_t transport_error_indicator  = 0;		   //1bit 传输误码指示符 
	uint8_t payload_unit_start_indicator = 0x0;    //1bit 有效荷载单元起始指示符  
	uint8_t transport_priority	= 0 ;		       //1bit 传输优先, 1表示高优先级,传输机制可能用到，解码用不着 
	uint16_t PID = m_nAudioPID;                          //13bit Pat 是 0x0000
	uint8_t transport_scrambling_control	= 0x0; //2bit 传输加扰控制 
	uint8_t adaptation_field_control =  03;		   //2bit 自适应控制 01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00解码器不进行处理
	uint8_t continuity_counter	= m_AduioCount++;		        //4bit 连续计数器 一个4bit的计数器，范围0-15

	if (m_AduioCount >  15)
	{
		m_AduioCount = 0;
	}
	// 4byte
	put_byte(ptsbuf,0x47);//同步字节, 固定为0x47,表示后面的是一个TS分组
	put_byte(ptsbuf,transport_priority<<7|
		payload_unit_start_indicator<<6|
		transport_priority<<5|
		PID>>8);
	put_byte(ptsbuf,PID);

	put_byte(ptsbuf,transport_scrambling_control<<6|
		adaptation_field_control<<4|
		continuity_counter);

	uint8_t adaptation_field_length = 0 + filesze;// 长度  8bit 
	uint8_t discontinuity_indicator = 0x0; //1bit 为0x0  
	uint8_t random_access_indicator = 0x1; //1bit 为0x0   
	uint8_t elementary_stream_priority_indicator = 0x0;//1bit 为0x0   
	uint8_t PCR_flag = 0x0;//1bit 为0x0  时间戳标志位
	uint8_t OPCR_flag = 0x0;//1bit 为0x0  
	uint8_t splicing_point_flag = 0x0;//1bit 为0x0    
	uint8_t transport_private_data_flag = 0x0;//1bit 为0x0  
	uint8_t	adaptation_field_extension_flag = 0x0;//1bit 为0x0  
	//2 byte

	if (filesze == 1)
	{
		adaptation_field_length = 0;;
		put_byte(ptsbuf,adaptation_field_length);
	}
	else if(filesze == 2)
	{
		adaptation_field_length = 1;
		put_byte(ptsbuf,adaptation_field_length);
		put_byte(ptsbuf,random_access_indicator<<6);
	}
	else
	{
		adaptation_field_length = filesze - 1;
		put_byte(ptsbuf,adaptation_field_length);
		put_byte(ptsbuf,random_access_indicator<<6);
		append_data(ptsbuf,arrarff,filesze -2);
	}

}
__int64 CTsEncoder::CheckTimeStamp(__int64 nowTimeStamp)
{
	boost::asio::detail::mutex::scoped_lock lock(mutex_Lock);
	return nowTimeStamp;
	__int64 retts = 0;
	if (m_lastTic == 0)
	{
		m_beginTic = nowTimeStamp;
		retts = 0;
	}
	else
	{
		retts = nowTimeStamp - m_beginTic;
	}
	m_lastTic = nowTimeStamp;
	return retts;

}
//获取系统当前时间
std::string CTsEncoder::GetFileName()
{
	
	return m_strFileName;	
}
