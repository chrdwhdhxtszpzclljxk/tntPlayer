#ifndef __PROTOCAL_H_
#define __PROTOCAL_H_
#include <stdint.h>

//#include "socketport.h"

	namespace xiny120{
	//const LONGLONG MILLISECONDS = (1000);            // 10 ^ 3
	//const LONGLONG NANOSECONDS = (1000000000);       // 10 ^ 9
	//const LONGLONG UNITS = (NANOSECONDS / 100);      // 10 ^ 7

	//typedef LONGLONG REFERENCE_TIME;

	const uint64_t MILLISECONDS = (1000);            // 10 ^ 3
	const uint64_t NANOSECONDS = (1000000000);       // 10 ^ 9
	const uint64_t UNITS = (NANOSECONDS / 100);      // 10 ^ 7


	// 协议版本
	const uint8_t	VERSION_OLD = 1;
	const uint8_t	VERSION_2 = 2;	// 此协议版本主要为了兼容android平台。	// 把所有交互时的字符串统一转换成UTF8格式。
	const uint8_t	VERSION = 3;	// 加入声音加密的新版。
	// 命令字
	const uint8_t	C_II = 0;		// 客户端连接发起后，服务器马上发送初始化标签给客户端（Init ）。
	const uint8_t	C_LI = 1;		// 登录命令字（Login）。
	const uint8_t	C_KO = 2;		// 踢出命令字（KickOut)
	const uint8_t	C_MZGP = 3;		// 每周光盘的命令字
	const uint8_t	C_KEEP = 4;		// 心跳命令字
	const uint8_t	C_FT = 5;		// 文件传输命令（File Transport）。
	const uint8_t	C_AVS = 6;		// 在线主播命令字(Audio Video Show)
	const uint8_t	C_QU = 7;		// 查询命令字(Query)
	const uint8_t	C_PU = 8;		// 通过文件更新课程命令。
	const uint8_t	C_MSG = 9;		// 来往消息。

	

	// 小头的命令字~ 小头一般只用于服务器下载信息！
	const uint8_t	C_PUBLISHER = 0;
	const uint8_t	C_CUSTOMER = 1;
	// C_LI response status
	const uint8_t	C_LI_OK = 0;	// 登录成功
	const uint8_t	C_LI_UE = 1;	// 用户名错误（Username Error）
	const uint8_t	C_LI_PE = 2;	// 密码错误（Password Error）
	const uint8_t	C_LI_RD = 3;	// 重定向（ReDirecttion）
	const uint8_t	C_LI_ME = 4;	// 未知错误（Misc Error）
	const uint8_t	C_LI_EXP = 5;	// 用户过期（EXPIRE）
	const uint8_t	C_LI_PO = 6;	// 用户设备超标(Pckey Overlow)
	const uint8_t	C_LI_TO = 7;	// 用户登录超时(TimeOut)
	const uint8_t	C_LI_SE = 8;	// 取得用户资料时错误(Sql Error)
	const uint8_t	C_LI_HE = 9;	// 尝试连接所有服务器后，所有服务器都失败(host error)

	// C_MZGP	 每周光盘的子命令字
	const uint8_t	C_MZGP_CP = 1;		// Can Play 是否可以播放！
	// C_FT	向服务器上传数据
	const uint8_t	C_FT_BEGIN = 0;	// 协商开始传送文件。
	const uint8_t	C_FT_DATA = 1;	// 文件传输过程
	const uint8_t	C_FT_END = 2;	// 文件传输完毕
	const uint8_t	C_FT_SPAN = 3;	// 设置客户端抓屏的参数。
	const uint8_t	C_FT_BREAK = 4;	// 文件传输中断
	const uint8_t	C_FT_DEL = 5;	// 删除文件
	const uint8_t	C_FT_CRC = 6;	// 文件效验码

	// C_PU
	const uint8_t	C_PU_AUDIO = 0;
	const uint8_t	C_PU_VIDEO = 1;
	const uint8_t	C_PU_MT = 2;
	const uint8_t	C_PU_STOP = 3;		// 收到 HOT_CMD_QUERYMICORDER_RELEASE 向客户短广告一下 STOP 信息！
	const uint8_t	C_PU_AVI_MT = 4;
	const uint8_t	C_PU_AVI_SOUND = 5;
	const uint8_t	C_PU_AVI_VIDEO = 6;
	// C_CUSTOMER				= 0x0002;
	const uint8_t	C_CUSTOMER_DW_SOUND_MT = 0x0000;		// 客户端申请Sound MediaType
	const uint8_t	C_CUSTOMER_DW_VIDEO_MT = 0x0001;		// 客户端申请Video MediaType
	// C_CUSTOMER
	const uint8_t	C_CUSTOMER_SEEK = 0x0001;
	// C_QUERYMICORDER_RE		= 0x01;			// 对于自己申请上麦的回复，是否允许用 HOT_CMD_QUERYMICORDER_ACQUIRE
	const uint8_t	C_AVS_BD = 2;			// 上麦状态变更后给所有人发通知！我上麦了。0表示上麦 1表示下麦
	const uint8_t	C_AVS_APPROVE = 3;		// 由服务器发送此命令给客户，表明客户得到麦，可以上线讲话！
	const uint8_t	C_AVS_ACQUIRE = 4;		// 客户申请麦序，由客户发起~
	const uint8_t	C_AVS_RELEASE = 5;		// 客户放弃麦序，由客户发起~
	const uint8_t	C_AVS_BREAK = 6;			// 麦权被高等级用户/或服务器主动踢出。
	// C_QU		// 向服务器申请一些系统必备数据。具体申请的数据类型，在SecCmd中体现
	const uint8_t	C_QU_SYS_INFO_HOSTS = 0;
	const uint8_t	C_QU_HIS_IDX_EX_NEAR = 1; // 获取时间点大于指定点的所有节点(最近更新的)。
	const uint8_t	C_QU_FILE_KEY = 2;	// 文件密钥。这里判断是否有权限播放，是否有余额播放
	const uint8_t	C_QU_FILE_BEGIN = 3; // 开始申请文件。
	const uint8_t	C_QU_FILE_DATA = 4; // 申请文件中。
	const uint8_t	C_QU_FILE_END = 5; // 文件整个下发完毕。
	const uint8_t	C_QU_FILE_BREAK = 6; // 中断文件下载
	const uint8_t	C_QU_FILE_DEL = 7; // 删除文件。
	const uint8_t	C_QU_PUB = 8;	// 出版者列表
	const uint8_t	C_QU_HIS_IDX_EX_FAR = 9; // 获取时间点小于指定点的所有节点(最早更新的)。

	// C_MSG		// 信息！
	const uint8_t	C_MSG_WATERMARK = 1;// 广播的是视频水印信息！
	const uint8_t	C_MSG_SYS = 2;		// 服务器发送的消息。
	const uint8_t	C_MSG_100 = 100;	// 客户聊天信息。
	

	//  用来填充 ClientContext 中的 m_dwStatus
	const uint32_t	CLIENT_LOGINREADY = 0x00000001;
	const uint32_t	CLIENT_LOGINREADY_01 = 0x00000002;
	const uint32_t	CLIENT_LOGINREADY_02 = 0x00000004;
	const uint32_t	CLIENT_LOGINREADY_03 = 0x00000008;
	const uint32_t	CLIENT_LOGINREADY_04 = 0x00000016;


	enum UserGroupType{
		SERVER_LINK = 0,	// 级联服务器，最高权限。
		SERVER_ADMIN = 1,	// 服务器管理员。
		SERVER_VIP = 2,		// 服务器VIP(所有东西都能看，但是不能管理)
		SERVER_MIC = 3,		// 服务器mai
		ROOM_ADMIN = 4,
		ROOM_VIP = 5,
		ROOM_MIC = 6,
		TEMP_SPEAK = 7,
		AUTHORIZATIO = 8,
		AUTHORIZATIOREAD = 9,			// 只读
		AUTHORIZATBOOK = 19,
		UNAUTHORIZATIO = 20
	};

	// 头标志
#pragma pack(1)

	struct _H{ uint8_t c, sc;};	// 最小头
	struct HEAD{ _H h; uint8_t md5[16]; int64_t now; };	// 基本头 // md5 结果 // 开机ticks数	// 本包生成时间
	struct INIT{ HEAD oh; int32_t verlowest; }; // 连接后服务器发送一个可以处理的最低版本的协议。如果客户端协议低于这个版本就要升级！
	struct KEEPALIVE{ _H oh; };
	struct LI{ HEAD oh; int32_t ver; char un[50], pwd[16], pc[66]; };	// 登录包, 用户名(un)(UTF8编码), md5后的密码，UTF8编码, 客户端硬件唯一编码标识。
	struct LI_RES{ HEAD oh; int32_t status, id; uint64_t permission; }; // 0 表示用户名和密码正确 1表示用户名和密码错误// 用户的id// 用户权限
	struct SERVER{ int32_t port; int8_t ip[20], type;	};

	struct VAPUSHMT{
		_H oh;
		uint16_t cnt;	// count
		uint32_t wid;	// 捕捉桌面的长度，就是横的那个 width
		uint32_t hei;	// 捕捉桌面的宽度，就是竖的那个(height)。
		uint32_t frm;	// 每秒捕捉多少次屏幕(frames)！
		uint32_t sps;	// 声音频率！8000 Hz 11025 Hz...(samples per second)
		uint32_t bps;	// 声音宽度 8bit、16bit(bits per samples)
		uint32_t BPS;	// 播放器的buffer!(Bytes per Second)
		uint32_t key;	// 关键帧(Key Frames)间隔！ // u32Frames * 30
		uint32_t tts;	// 文字聊天的最小发言间隔！(Text Talk Span) -1表示禁言
		int64_t now;	// 记录目前的时间。
		int32_t talk;
	};

	struct VAPUSHMTEX{
		VAPUSHMT vamt;
		int32_t	id;		// 本头的录制者ID！
		int32_t	mtIdx;	// 当前分配的media type 编号。也就是 20个在线上麦权中的一个编号
	};

	struct VAPUSHEX{
		HEAD oh;
		int64_t stop;
		uint32_t id;		// 本包的创建者ID！
		int32_t mtId;		// 上麦的编号~
		uint8_t data[1];
	};

	struct GTMVHEADER{
		uint32_t v,a,v1,a1;	// video audio
		int32_t len;
	} ;

	struct GTMVNODE{
		int32_t len;
		uint8_t type, data[1];
	};

	// 文字对话！
	struct TEXTTALK{ HEAD oh; uint32_t len; int8_t msg[1];	}; //

	// 历史录像资料索引
	struct QUERYHISIDX{
		HEAD oh;
		uint32_t pubid, year;
		int64_t filetimelast;
	};

	struct QUERYHISIDXRE{
		HEAD oh; uint32_t id, year, count; int64_t	data[1]; // 标准头，出版者ID,出版年份，后面的data的个数，data数据，这里值时间time64_t类型
	};

	struct ID2NAME2{
		uint32_t pubid;
		int32_t start, end;
		uint8_t	name[100],name1[20];	// 最大26个字符哦！
	};

	// 申请有历史文件的出版者
	struct QUERYPUBLISHER02{
		HEAD oh;
		uint32_t count;
		ID2NAME2 data[1];
	};

	// 申请历史文件开始，结尾。
	struct QUERYHISFILE_BEGIN{
		HEAD oh; uint32_t pubid, year, len, err; // 客户端申请时，表示断点续传的长度，服务器下发时表示文件总长度。			// 1表示有错误！
		int64_t filetime;
		uint8_t	key, syserr; // 播放密钥，syserr为0时标识自定义错误，syserr为非零时标下errorno.
	};


	// 申请历史文件删除。
	struct QUERYHISFILE_DEL{
		HEAD oh; uint32_t pubid, year;		// 出版都id, 出版年份，本地文件长度（断点续传用）
		int64_t now; uint32_t len;			// 客户端申请时，表示断点续传的长度，服务器下发时表示文件总长度。
	};

	// 申请历史文件数据
	struct QUERYHISFILE_DATA{
		_H h; uint32_t pubid, year,len; int64_t filetime; uint16_t checksum; char data[1];
	};

	struct QUERYHISFILEKEY{
		HEAD oh; uint32_t pubid, year, len, charge;	// 出版都id,出版年份，服务器方该文件长度（断点续传用），余额
		int64_t filetime, lastPay;						// 文件序列号（时间），最后一次付费日期。
		int32_t play; uint8_t	key;					// 是否可以播放, 播放密钥。
	};

	struct QUERYSYSINFO_HOSTS{
		HEAD oh; int32_t count; SERVER	server[1];
	};

	struct RGB { uint8_t b, g, r; };

#pragma pack()

	struct FILEHEADER{
		uint32_t ver;
		uint32_t subVer;
		uint8_t	info[100];
		uint8_t	infoEnd;
		uint8_t fuck[200];
		uint8_t end;
	} ;


	struct FILEHEADER84{
		uint32_t	nVer;
		uint32_t	nSubVer;
		int64_t	m_time;
		int8_t	szFuck[200];
		int8_t	bInfoEnd;
		int8_t	szInfo[100];
		int8_t	bEnd;
	} ;

	struct GtmvData{
		uint32_t id;
		int64_t now;
		uint32_t ticks;
		uint64_t start;
		uint64_t end;
		uint64_t width;
		uint64_t height;
		uint32_t type;					// 0 for Audio, 1 for Video
		uint32_t len;
		uint8_t data[1];
	};

};

#endif
