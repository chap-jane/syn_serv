#ifndef _DC_MANAGER_H_
#define _DC_MANAGER_H_



#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"
#include "DcConfig.h"

#include "DcThreadProcess.h"
#include "DcThreadProcessReSyn.h"
#include "DcThreadProcessReSynCCA.h"
#include "DcIndb.h"
#include <queue>


class CDCCMsg;
class CDCCEntry;

class DcParseXml;

class DcManager
{

private:

    DcManager();

    ~DcManager();

public:
    static DcManager* GetInstance();

    int Init();

    int Destroy();

	CDCCMsg* GetDCCMsgFromQueue();
	CDCCMsg* GetDCCMsgFromBackQueue();

public:
    static DcManager* m_instance;

    DcConfig *m_config;

    CDCCEntry *m_dccEntry;

    DcParseXml* m_xml ;
    Indb*           m_pIndb ;
    std::list<DcThreadProcess*> m_lstThread;
    
private:
	char wBuf[160][2048];
	char buf[327680];
	DcThreadProcessReSyn *pThreadReSyn ;
	DcThreadProcessReSynCCA *pThreadReSynCCA ;
	std::queue<CDCCMsg*> m_QueBackSyn; //反向同步队列
	std::queue<CDCCMsg*> m_QueSyn;  //正向同步队列
	pthread_mutex_t m_mutex_Syn;
	pthread_mutex_t m_mutex_ReSyn;

};

#endif
