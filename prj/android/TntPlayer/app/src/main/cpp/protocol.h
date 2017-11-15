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


	// Э��汾
	const uint8_t	VERSION_OLD = 1;
	const uint8_t	VERSION_2 = 2;	// ��Э��汾��ҪΪ�˼���androidƽ̨��	// �����н���ʱ���ַ���ͳһת����UTF8��ʽ��
	const uint8_t	VERSION = 3;	// �����������ܵ��°档
	// ������
	const uint8_t	C_II = 0;		// �ͻ������ӷ���󣬷��������Ϸ��ͳ�ʼ����ǩ���ͻ��ˣ�Init ����
	const uint8_t	C_LI = 1;		// ��¼�����֣�Login����
	const uint8_t	C_KO = 2;		// �߳������֣�KickOut)
	const uint8_t	C_MZGP = 3;		// ÿ�ܹ��̵�������
	const uint8_t	C_KEEP = 4;		// ����������
	const uint8_t	C_FT = 5;		// �ļ��������File Transport����
	const uint8_t	C_AVS = 6;		// ��������������(Audio Video Show)
	const uint8_t	C_QU = 7;		// ��ѯ������(Query)
	const uint8_t	C_PU = 8;		// ͨ���ļ����¿γ����
	const uint8_t	C_MSG = 9;		// ������Ϣ��

	

	// Сͷ��������~ Сͷһ��ֻ���ڷ�����������Ϣ��
	const uint8_t	C_PUBLISHER = 0;
	const uint8_t	C_CUSTOMER = 1;
	// C_LI response status
	const uint8_t	C_LI_OK = 0;	// ��¼�ɹ�
	const uint8_t	C_LI_UE = 1;	// �û�������Username Error��
	const uint8_t	C_LI_PE = 2;	// �������Password Error��
	const uint8_t	C_LI_RD = 3;	// �ض���ReDirecttion��
	const uint8_t	C_LI_ME = 4;	// δ֪����Misc Error��
	const uint8_t	C_LI_EXP = 5;	// �û����ڣ�EXPIRE��
	const uint8_t	C_LI_PO = 6;	// �û��豸����(Pckey Overlow)
	const uint8_t	C_LI_TO = 7;	// �û���¼��ʱ(TimeOut)
	const uint8_t	C_LI_SE = 8;	// ȡ���û�����ʱ����(Sql Error)
	const uint8_t	C_LI_HE = 9;	// �����������з����������з�������ʧ��(host error)

	// C_MZGP	 ÿ�ܹ��̵���������
	const uint8_t	C_MZGP_CP = 1;		// Can Play �Ƿ���Բ��ţ�
	// C_FT	��������ϴ�����
	const uint8_t	C_FT_BEGIN = 0;	// Э�̿�ʼ�����ļ���
	const uint8_t	C_FT_DATA = 1;	// �ļ��������
	const uint8_t	C_FT_END = 2;	// �ļ��������
	const uint8_t	C_FT_SPAN = 3;	// ���ÿͻ���ץ���Ĳ�����
	const uint8_t	C_FT_BREAK = 4;	// �ļ������ж�
	const uint8_t	C_FT_DEL = 5;	// ɾ���ļ�
	const uint8_t	C_FT_CRC = 6;	// �ļ�Ч����

	// C_PU
	const uint8_t	C_PU_AUDIO = 0;
	const uint8_t	C_PU_VIDEO = 1;
	const uint8_t	C_PU_MT = 2;
	const uint8_t	C_PU_STOP = 3;		// �յ� HOT_CMD_QUERYMICORDER_RELEASE ��ͻ��̹��һ�� STOP ��Ϣ��
	const uint8_t	C_PU_AVI_MT = 4;
	const uint8_t	C_PU_AVI_SOUND = 5;
	const uint8_t	C_PU_AVI_VIDEO = 6;
	// C_CUSTOMER				= 0x0002;
	const uint8_t	C_CUSTOMER_DW_SOUND_MT = 0x0000;		// �ͻ�������Sound MediaType
	const uint8_t	C_CUSTOMER_DW_VIDEO_MT = 0x0001;		// �ͻ�������Video MediaType
	// C_CUSTOMER
	const uint8_t	C_CUSTOMER_SEEK = 0x0001;
	// C_QUERYMICORDER_RE		= 0x01;			// �����Լ���������Ļظ����Ƿ������� HOT_CMD_QUERYMICORDER_ACQUIRE
	const uint8_t	C_AVS_BD = 2;			// ����״̬�����������˷�֪ͨ���������ˡ�0��ʾ���� 1��ʾ����
	const uint8_t	C_AVS_APPROVE = 3;		// �ɷ��������ʹ�������ͻ��������ͻ��õ��󣬿������߽�����
	const uint8_t	C_AVS_ACQUIRE = 4;		// �ͻ����������ɿͻ�����~
	const uint8_t	C_AVS_RELEASE = 5;		// �ͻ����������ɿͻ�����~
	const uint8_t	C_AVS_BREAK = 6;			// ��Ȩ���ߵȼ��û�/������������߳���
	// C_QU		// �����������һЩϵͳ�ر����ݡ�����������������ͣ���SecCmd������
	const uint8_t	C_QU_SYS_INFO_HOSTS = 0;
	const uint8_t	C_QU_HIS_IDX_EX_NEAR = 1; // ��ȡʱ������ָ��������нڵ�(������µ�)��
	const uint8_t	C_QU_FILE_KEY = 2;	// �ļ���Կ�������ж��Ƿ���Ȩ�޲��ţ��Ƿ�������
	const uint8_t	C_QU_FILE_BEGIN = 3; // ��ʼ�����ļ���
	const uint8_t	C_QU_FILE_DATA = 4; // �����ļ��С�
	const uint8_t	C_QU_FILE_END = 5; // �ļ������·���ϡ�
	const uint8_t	C_QU_FILE_BREAK = 6; // �ж��ļ�����
	const uint8_t	C_QU_FILE_DEL = 7; // ɾ���ļ���
	const uint8_t	C_QU_PUB = 8;	// �������б�
	const uint8_t	C_QU_HIS_IDX_EX_FAR = 9; // ��ȡʱ���С��ָ��������нڵ�(������µ�)��

	// C_MSG		// ��Ϣ��
	const uint8_t	C_MSG_WATERMARK = 1;// �㲥������Ƶˮӡ��Ϣ��
	const uint8_t	C_MSG_SYS = 2;		// ���������͵���Ϣ��
	const uint8_t	C_MSG_100 = 100;	// �ͻ�������Ϣ��
	

	//  ������� ClientContext �е� m_dwStatus
	const uint32_t	CLIENT_LOGINREADY = 0x00000001;
	const uint32_t	CLIENT_LOGINREADY_01 = 0x00000002;
	const uint32_t	CLIENT_LOGINREADY_02 = 0x00000004;
	const uint32_t	CLIENT_LOGINREADY_03 = 0x00000008;
	const uint32_t	CLIENT_LOGINREADY_04 = 0x00000016;


	enum UserGroupType{
		SERVER_LINK = 0,	// ���������������Ȩ�ޡ�
		SERVER_ADMIN = 1,	// ����������Ա��
		SERVER_VIP = 2,		// ������VIP(���ж������ܿ������ǲ��ܹ���)
		SERVER_MIC = 3,		// ������mai
		ROOM_ADMIN = 4,
		ROOM_VIP = 5,
		ROOM_MIC = 6,
		TEMP_SPEAK = 7,
		AUTHORIZATIO = 8,
		AUTHORIZATIOREAD = 9,			// ֻ��
		AUTHORIZATBOOK = 19,
		UNAUTHORIZATIO = 20
	};

	// ͷ��־
#pragma pack(1)

	struct _H{ uint8_t c, sc;};	// ��Сͷ
	struct HEAD{ _H h; uint8_t md5[16]; int64_t now; };	// ����ͷ // md5 ��� // ����ticks��	// ��������ʱ��
	struct INIT{ HEAD oh; int32_t verlowest; }; // ���Ӻ����������һ�����Դ������Ͱ汾��Э�顣����ͻ���Э���������汾��Ҫ������
	struct KEEPALIVE{ _H oh; };
	struct LI{ HEAD oh; int32_t ver; char un[50], pwd[16], pc[66]; };	// ��¼��, �û���(un)(UTF8����), md5������룬UTF8����, �ͻ���Ӳ��Ψһ�����ʶ��
	struct LI_RES{ HEAD oh; int32_t status, id; uint64_t permission; }; // 0 ��ʾ�û�����������ȷ 1��ʾ�û������������// �û���id// �û�Ȩ��
	struct SERVER{ int32_t port; int8_t ip[20], type;	};

	struct VAPUSHMT{
		_H oh;
		uint16_t cnt;	// count
		uint32_t wid;	// ��׽����ĳ��ȣ����Ǻ���Ǹ� width
		uint32_t hei;	// ��׽����Ŀ�ȣ����������Ǹ�(height)��
		uint32_t frm;	// ÿ�벶׽���ٴ���Ļ(frames)��
		uint32_t sps;	// ����Ƶ�ʣ�8000 Hz 11025 Hz...(samples per second)
		uint32_t bps;	// ������� 8bit��16bit(bits per samples)
		uint32_t BPS;	// ��������buffer!(Bytes per Second)
		uint32_t key;	// �ؼ�֡(Key Frames)����� // u32Frames * 30
		uint32_t tts;	// �����������С���Լ����(Text Talk Span) -1��ʾ����
		int64_t now;	// ��¼Ŀǰ��ʱ�䡣
		int32_t talk;
	};

	struct VAPUSHMTEX{
		VAPUSHMT vamt;
		int32_t	id;		// ��ͷ��¼����ID��
		int32_t	mtIdx;	// ��ǰ�����media type ��š�Ҳ���� 20����������Ȩ�е�һ�����
	};

	struct VAPUSHEX{
		HEAD oh;
		int64_t stop;
		uint32_t id;		// �����Ĵ�����ID��
		int32_t mtId;		// ����ı��~
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

	// ���ֶԻ���
	struct TEXTTALK{ HEAD oh; uint32_t len; int8_t msg[1];	}; //

	// ��ʷ¼����������
	struct QUERYHISIDX{
		HEAD oh;
		uint32_t pubid, year;
		int64_t filetimelast;
	};

	struct QUERYHISIDXRE{
		HEAD oh; uint32_t id, year, count; int64_t	data[1]; // ��׼ͷ��������ID,������ݣ������data�ĸ�����data���ݣ�����ֵʱ��time64_t����
	};

	struct ID2NAME2{
		uint32_t pubid;
		int32_t start, end;
		uint8_t	name[100],name1[20];	// ���26���ַ�Ŷ��
	};

	// ��������ʷ�ļ��ĳ�����
	struct QUERYPUBLISHER02{
		HEAD oh;
		uint32_t count;
		ID2NAME2 data[1];
	};

	// ������ʷ�ļ���ʼ����β��
	struct QUERYHISFILE_BEGIN{
		HEAD oh; uint32_t pubid, year, len, err; // �ͻ�������ʱ����ʾ�ϵ������ĳ��ȣ��������·�ʱ��ʾ�ļ��ܳ��ȡ�			// 1��ʾ�д���
		int64_t filetime;
		uint8_t	key, syserr; // ������Կ��syserrΪ0ʱ��ʶ�Զ������syserrΪ����ʱ����errorno.
	};


	// ������ʷ�ļ�ɾ����
	struct QUERYHISFILE_DEL{
		HEAD oh; uint32_t pubid, year;		// ���涼id, ������ݣ������ļ����ȣ��ϵ������ã�
		int64_t now; uint32_t len;			// �ͻ�������ʱ����ʾ�ϵ������ĳ��ȣ��������·�ʱ��ʾ�ļ��ܳ��ȡ�
	};

	// ������ʷ�ļ�����
	struct QUERYHISFILE_DATA{
		_H h; uint32_t pubid, year,len; int64_t filetime; uint16_t checksum; char data[1];
	};

	struct QUERYHISFILEKEY{
		HEAD oh; uint32_t pubid, year, len, charge;	// ���涼id,������ݣ������������ļ����ȣ��ϵ������ã������
		int64_t filetime, lastPay;						// �ļ����кţ�ʱ�䣩�����һ�θ������ڡ�
		int32_t play; uint8_t	key;					// �Ƿ���Բ���, ������Կ��
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
