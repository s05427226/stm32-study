
#include "stdlib.h"
#include "mem.h"

#define  CANOPEN_DEBUG

#ifdef CANOPEN_DEBUG
	int d_malloc_cnt = 	0;
	int d_curr_index = 0;
	int d_curr_cnt = 0;
#endif

naIndexTable* g_naIndexTable = NULL;

subindex* allocSubIndex(int iCount)
{
	char *pRet = NULL;
#ifdef CANOPEN_DEBGU
	d_malloc_cnt = sizeof(subindex)*iCount;
#endif
	if(iCount <= 0) return NULL;
	
	pRet = (char*)malloc(sizeof(subindex)*iCount);
	if(!pRet) 
	{
		return NULL;
	}
	memset(pRet,0,sizeof(subindex)*iCount);
	return (subindex*)pRet;
}

void setSubIndex(subindex	*p,  
						UNS8    bAccessType,
    					UNS8    bDataType, 
    					UNS32   size,      
    					void*   pObject)  
{
	if(!p)return;

	p->bAccessType = bAccessType;
	p->bDataType = bDataType ;
	p->size = size;
	switch(bDataType)
	{
		case boolean:
			p->pObject = malloc(sizeof(INTEGER8));
			*(INTEGER8*)p->pObject	 = *(INTEGER8 *)(pObject);
			break;
		case int8:
			p->pObject = malloc(sizeof(INTEGER8));
			*(INTEGER8*)p->pObject	 = *(INTEGER8 *)(pObject);
			break;
		case int16:
			p->pObject = malloc(sizeof(INTEGER16));
			*(INTEGER16*)p->pObject	 = *(INTEGER16 *)(pObject);
			break;
		case int32:
			p->pObject = malloc(sizeof(INTEGER32));
			*(INTEGER32*)p->pObject  = * (INTEGER32 *)(pObject);
			break;
		case uint8:
			p->pObject = malloc(sizeof(UNS8));
			*(UNS8*)p->pObject	 = *(UNS8 *)(pObject);
			break;
		case uint16:
			p->pObject = malloc(sizeof(UNS16));
			*(UNS16*)p->pObject	 = *(UNS16 *)(pObject);
			break;
		case uint32:
			p->pObject = malloc(sizeof(UNS32));
			*(UNS32*)p->pObject  = * (UNS32 *)(pObject);
			break;
		case real32:
			p->pObject = malloc(sizeof(REAL32));
			*(REAL32*)p->pObject  = * (REAL32 *)(pObject);
			break;
		default:
			break;
	}
}

void releaseSubIndex(subindex* p)
{
	if(p->pObject) 
	{
		free(p->pObject);
		p->pObject = NULL;
	}
	if(p) 
	{
		free(p);
		p = NULL;
	}
}

indextable* allocIndexTableEntry(int iEntry)
{
	indextable* pRet = NULL;

	if(iEntry == 0)	return NULL;
#ifdef CANOPEN_DEBGU
	d_malloc_cnt = sizeof(indextable)*iEntry;
#endif
	pRet = (indextable *)malloc(sizeof(indextable)*iEntry);
	if(!pRet) 
	{
		return NULL;
	}
	memset(pRet,0,sizeof(indextable)*iEntry);
	return pRet;
}

void setIndexTableEntry(indextable* pEntry,
							 subindex *pSubindex,
							 UNS8 bSubCount,
							 UNS16 index)
{
	if(!pEntry) 
		return;

	pEntry->pSubindex = pSubindex;
	pEntry->bSubCount = bSubCount;
	pEntry->index = index;
}

void setIndexTableEntryValue(indextable* pDest,indextable* pSrc)
{
	int i=0;
	if(!pDest || !pSrc) 
		return;

	for(i=0;i<pDest->bSubCount;i++)
	{
		setSubIndex(&pDest->pSubindex[i],
					pSrc->pSubindex[i].bAccessType,
					pSrc->pSubindex[i].bDataType,
					pSrc->pSubindex[i].size,
					pSrc->pSubindex[i].pObject);
	}
}

indextable* index2IndexTableEntry(naIndexTable* it,UNS16 index)
{
	int i=0;
	if(!it)
		return NULL;
	for(i=0;i<it->pNumOfIndexTableEntrys;i++) 
	{
		if(it->pIndexTable[i].index == index)
			return it->pIndexTable+i;
		else continue;
	}
	
	return NULL;
}
indextable * na_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks)
{
	int i=0;

	for(i=0;i<g_naIndexTable->pNumOfIndexTableEntrys;i++) 
	{
		if(g_naIndexTable->pIndexTable[i].index == wIndex)
		{
			*errorCode = OD_SUCCESSFUL;
			*callbacks = (ODCallback_t *)g_naIndexTable->pIndexTable[i].pSubindex->pObject;
			return g_naIndexTable->pIndexTable+i;
		}
		else continue;
	}
	
	*errorCode = OD_NO_SUCH_OBJECT;
	return NULL;
		
}


void releaseIndexTable()
{
	int i=0,j=0;
	char* p=NULL;
	p=p;
	if(!g_naIndexTable) 
		return;

	for(i=0;i<g_naIndexTable->pNumOfIndexTableEntrys;i++) 
	{
		for(j=0;j<g_naIndexTable->pIndexTable[i].bSubCount;j++)	
		{
		  	releaseSubIndex(g_naIndexTable->pIndexTable[i].pSubindex + j);		
		}
		free(g_naIndexTable->pIndexTable+i);
		p=(char*)(g_naIndexTable->pIndexTable+i); 
		p= NULL;
	}
	free(g_naIndexTable);
	g_naIndexTable = NULL;
}

indextable* createIndexTable(int nEntry)
{
	if( g_naIndexTable ) 
	{
		releaseIndexTable();
	}

	if(!g_naIndexTable)
	{
		g_naIndexTable = (naIndexTable*) malloc(sizeof(naIndexTable));
		if(!g_naIndexTable)
			return NULL;
		memset(g_naIndexTable,0,sizeof(naIndexTable));
		
		g_naIndexTable->pIndexTable = allocIndexTableEntry(nEntry);
		g_naIndexTable->pNumOfIndexTableEntrys = nEntry;
	}
	return 	g_naIndexTable->pIndexTable;
}

void setObjDict(CO_Data *pCOData,indextable* objDict,int ObjDictSize)
{
	pCOData->objdict= objDict;

	if(!pCOData->ObjdictSize) {
		pCOData->ObjdictSize = (UNS16 *)malloc(sizeof(UNS16));
		if(!pCOData->ObjdictSize)
			return;
	}
	*pCOData->ObjdictSize = ObjDictSize;
}

CO_Data *CreateNodeData(int nNumOfIndexTableItems)
{
	CO_Data *pRet = NULL;
	int i=0;
	pRet = (CO_Data *)malloc(sizeof(CO_Data));
	if(!pRet)
	{
		return NULL;
	}
	
	createIndexTable(nNumOfIndexTableItems);
	setObjDict(pRet,g_naIndexTable->pIndexTable,nNumOfIndexTableItems);
	
	if(!pRet->objdict)
		goto out;

	memset(pRet->transfers,0,sizeof(s_transfer)*SDO_MAX_SIMULTANEOUS_TRANSFERS);
	for(i=0;i<SDO_MAX_SIMULTANEOUS_TRANSFERS;i++)
	{
		pRet->transfers[i].timer = -1;
	}
	
out:
	if(pRet)
	{
		free(pRet);
		return NULL;
	}
}

void ReleaseNodeData(CO_Data *pCOData)
{
	if(pCOData->bDeviceNodeId) 
		free(pCOData->bDeviceNodeId);
	if(pCOData->PDO_status) 
		free(pCOData->PDO_status);
	if(pCOData->ObjdictSize) 
		free(pCOData->ObjdictSize);
	if(pCOData->RxPDO_EventTimers) 
		free(pCOData->RxPDO_EventTimers);
	if(pCOData->firstIndex) 
		free(pCOData->firstIndex);
	if(pCOData->lastIndex) 
		free(pCOData->lastIndex);
	if(pCOData->iam_a_slave) 
		free(pCOData->iam_a_slave);
	if(pCOData->ConsumerHeartBeatTimers) 
		free(pCOData->ConsumerHeartBeatTimers);
	if(pCOData->ConsumerHeartbeatCount) 
		free(pCOData->ConsumerHeartbeatCount);
	if(pCOData->ConsumerHeartbeatEntries) 
		free(pCOData->ConsumerHeartbeatEntries);
	if(pCOData->ProducerHeartBeatTime) 
		free(pCOData->ProducerHeartBeatTime);
	if(pCOData->GuardTime) 
		free(pCOData->GuardTime);
	if(pCOData->LifeTimeFactor) 
		free(pCOData->LifeTimeFactor);
	if(pCOData->COB_ID_Sync) 
		free(pCOData->COB_ID_Sync);
	if(pCOData->Sync_Cycle_Period) 
		free(pCOData->Sync_Cycle_Period);
	if(pCOData->canHandle) 
		free(pCOData->canHandle);
	if(pCOData->error_number) 
		free(pCOData->error_number);
	if(pCOData->error_first_element) 
		free(pCOData->error_first_element);
	if(pCOData->error_register) 
		free(pCOData->error_register);
	if(pCOData->error_cobid) 
		free(pCOData->error_cobid);	
	if(pCOData->ProducerHeartBeatTime) 
		free(pCOData->ProducerHeartBeatTime);	
		
	releaseIndexTable();
	
	if(pCOData)
	{
		free(pCOData);
		pCOData = NULL;
	}
}
indextable* getIndexTableFromCOData(CO_Data *pCOData)
{
	if(!pCOData)	return NULL;
	return pCOData->objdict;
}

void setDeviceNodeID(CO_Data *pCOData,UNS8 nodeid)
{
	if(!pCOData->bDeviceNodeId) {
		pCOData->bDeviceNodeId = (UNS8 *)malloc(sizeof(UNS8));
		if(!pCOData->bDeviceNodeId)
			return;
	}
	*pCOData->bDeviceNodeId = nodeid;
}



void setPdoStatus(CO_Data *pCOData,s_PDO_status* status)
{
	if(!pCOData->PDO_status) {
		pCOData->PDO_status = (s_PDO_status*)malloc(sizeof(s_PDO_status));
		if(!pCOData->PDO_status)
			return;
	}
	memcpy(pCOData->PDO_status,status,sizeof(s_PDO_status));
}

void setRXPdoEventTimers(CO_Data *pCOData,TIMER_HANDLE timers)
{
	if(!pCOData->RxPDO_EventTimers) {
		pCOData->RxPDO_EventTimers = (TIMER_HANDLE*)malloc(sizeof(TIMER_HANDLE));
		if(!pCOData->RxPDO_EventTimers)
			return;
	}
	*pCOData->RxPDO_EventTimers = timers;
}

typedef void (* RxPDO_EventTimers_Handler)(CO_Data *,UNS32);

void setRXPdoEventTimersHandle(CO_Data *pCOData,
											RxPDO_EventTimers_Handler* handle)
{
	pCOData->RxPDO_EventTimers_Handler = (void (*)(CO_Data *,UNS32))handle;
}

void setFirstIndex(CO_Data *pCOData,quick_index* index)
{
	if(!pCOData->firstIndex)
	{
		pCOData->firstIndex = (quick_index *)malloc(sizeof(quick_index));
		if(!pCOData->firstIndex)
			return;
	}
	memcpy(pCOData->firstIndex,index,sizeof(quick_index));
}
void setLastIndex(CO_Data *pCOData,quick_index* index)
{
	if(!pCOData->lastIndex)
	{
		pCOData->lastIndex = (quick_index *)malloc(sizeof(quick_index));
		if(!pCOData->lastIndex)
			return;
	}
	memcpy(pCOData->lastIndex,index,sizeof(quick_index));
}
void setSlaveFlag(CO_Data *pCOData,UNS8 slaveFlag)
{
	if(!pCOData->iam_a_slave)
	{
		pCOData->iam_a_slave = (UNS8 *)malloc(sizeof(UNS8));
		if(!pCOData->iam_a_slave)
			return;
	}
	
	*pCOData->iam_a_slave = slaveFlag;
}

void setValueRangeTest(CO_Data *pCOData,valueRangeTest_t valueRangeTest)
{
	pCOData->valueRangeTest= valueRangeTest;
}

void setNodeState(CO_Data *pCOData,e_nodeState nodeState)
{
	pCOData->nodeState= nodeState;
}

void setCurrCommunicationState(CO_Data *pCOData,
										s_state_communication CurrentCommunicationState)
{
	pCOData->CurrentCommunicationState = CurrentCommunicationState;
}

void setInitialisation(CO_Data *pCOData,initialisation_t initialisation)
{
	pCOData->initialisation = initialisation;
}

void setPreOperational(CO_Data *pCOData,preOperational_t preOperational)
{
	pCOData->preOperational = preOperational;
}
void setOperational(CO_Data *pCOData,operational_t Operational)
{
	pCOData->operational = Operational;
}
void setStopped(CO_Data *pCOData,operational_t stopped)
{
	pCOData->stopped = stopped;
}

typedef void (* ResetCallBack)(CO_Data *);

void setNmtSlaveNodeResetCallBack(CO_Data *pCOData,ResetCallBack callback)
{
	pCOData->NMT_Slave_Node_Reset_Callback = callback;
}

void setNmtSlaveCommunicationResetCallBack(CO_Data *pCOData,
											ResetCallBack callback)
{
	pCOData->NMT_Slave_Communications_Reset_Callback = callback;
}

void setConsumerHeartbeatCount(CO_Data *pCOData,UNS8 ConsumerHeartbeatCount)
{
	if(!pCOData->ConsumerHeartbeatCount)
	{
		pCOData->ConsumerHeartbeatCount = (UNS8 *)malloc(sizeof(UNS8));
		if(!pCOData->ConsumerHeartbeatCount)
			return;
	}
	*pCOData->ConsumerHeartbeatCount = ConsumerHeartbeatCount;
}

void setConsumerHeartbeatEntrys(CO_Data *pCOData,UNS32 ConsumerHeartbeatEntries)
{
	if(!pCOData->ConsumerHeartbeatEntries)
	{
		pCOData->ConsumerHeartbeatEntries = (UNS32 *)malloc(sizeof(UNS32));
		if(!pCOData->ConsumerHeartbeatEntries)
			return;
	}

	*pCOData->ConsumerHeartbeatEntries = ConsumerHeartbeatEntries;
}

void setConsumerHeartBeatTimers(CO_Data *pCOData,TIMER_HANDLE ConsumerHeartBeatTimers)
{	   
	if(!pCOData->ConsumerHeartBeatTimers)
	{
		pCOData->ConsumerHeartBeatTimers = (TIMER_HANDLE *)malloc(sizeof(TIMER_HANDLE));
		if(!pCOData->ConsumerHeartBeatTimers)
			return;
	}
	*pCOData->ConsumerHeartBeatTimers = ConsumerHeartBeatTimers;
}

void setProducerHeartBeatTime(CO_Data *pCOData,UNS16 ProducerHeartBeatTime)
{
	if(!pCOData->ProducerHeartBeatTime)
	{
		pCOData->ProducerHeartBeatTime = (UNS16 *)malloc(sizeof(UNS16));
		if(!pCOData->ProducerHeartBeatTime)
			return;
	}
	
	*pCOData->ProducerHeartBeatTime = ProducerHeartBeatTime;
}

void setProducerHeartBeatTimer(CO_Data *pCOData,TIMER_HANDLE ProducerHeartBeatTimer)
{
	if(!pCOData->ProducerHeartBeatTime)
	{
		pCOData->ProducerHeartBeatTime = (TIMER_HANDLE *)malloc(sizeof(TIMER_HANDLE));
		if(!pCOData->ProducerHeartBeatTime)
			return;
	}
	*pCOData->ProducerHeartBeatTime = ProducerHeartBeatTimer;
}

void setHeartbeatError(CO_Data *pCOData,heartbeatError_t heartbeatError)
{
	pCOData->heartbeatError = heartbeatError;
}

void setGuardTimeTimer(CO_Data *pCOData,TIMER_HANDLE GuardTimeTimer)
{
	pCOData->GuardTimeTimer = GuardTimeTimer;
}
void setLifeTimeTimer(CO_Data *pCOData,TIMER_HANDLE LifeTimeTimer)
{
	pCOData->LifeTimeTimer = LifeTimeTimer;
}
void setNodeguardError(CO_Data *pCOData,nodeguardError_t nodeguardError)
{
	pCOData->nodeguardError = nodeguardError;
}

void setGuardTime(CO_Data *pCOData,UNS16 GuardTime)
{
	if(!pCOData->GuardTime)
	{
		pCOData->GuardTime = (UNS16 *)malloc(sizeof(UNS16));
		if(!pCOData->GuardTime)
			return;
	}

	*pCOData->GuardTime = GuardTime;
}

void setLifeTimeFactor(CO_Data *pCOData,UNS8 LifeTimeFactor)
{
	if(!pCOData->LifeTimeFactor)
	{
		pCOData->LifeTimeFactor = (UNS8 *)malloc(sizeof(UNS8));
		if(!pCOData->LifeTimeFactor)
			return;
	}

	*pCOData->LifeTimeFactor = LifeTimeFactor;
}

void setSyncTimer(CO_Data *pCOData,TIMER_HANDLE syncTimer)
{
	pCOData->syncTimer = syncTimer;
}

void setCobIdSync(CO_Data *pCOData,UNS32 COB_ID_Sync)
{
	if(!pCOData->COB_ID_Sync)
	{
		pCOData->COB_ID_Sync = (UNS32 *)malloc(sizeof(UNS32));
		if(!pCOData->COB_ID_Sync)
			return;
	}

	*pCOData->COB_ID_Sync = COB_ID_Sync;
}

void setSyncCyclePeriod(CO_Data *pCOData,UNS32 Sync_Cycle_Period)
{
	if(!pCOData->Sync_Cycle_Period)
	{
		pCOData->Sync_Cycle_Period = (UNS32 *)malloc(sizeof(UNS32));
		if(!pCOData->COB_ID_Sync)
			return;
	}

	*pCOData->Sync_Cycle_Period = Sync_Cycle_Period;
}

void setPostSync(CO_Data *pCOData,post_sync_t post_sync)
{
	pCOData->post_sync = post_sync;
}

void setPostTPDO(CO_Data *pCOData,post_sync_t post_TPDO)
{
	pCOData->post_TPDO = post_TPDO;
}

void setPostSlaveBootup(CO_Data *pCOData,post_SlaveBootup_t post_SlaveBootup)
{
	pCOData->post_SlaveBootup = post_SlaveBootup;
}
void setPostSlaveStateChange(CO_Data *pCOData,post_SlaveStateChange_t post_SlaveStateChange)
{
	pCOData->post_SlaveStateChange = post_SlaveStateChange;
}

void setToggle(CO_Data *pCOData,UNS8 toggle)
{
	pCOData->toggle = toggle;
}

void setCANPort(CO_Data *pCOData,CAN_PORT canHandle)
{
	if(!pCOData->canHandle)
	{
		pCOData->canHandle = (CAN_PORT)malloc(sizeof(CAN_PORT));
		if(!pCOData->canHandle)
			return;
	}
	pCOData->canHandle = canHandle;
}

void setScanIndexOD(CO_Data *pCOData,scanIndexOD_t scanIndexOD)
{
	pCOData->scanIndexOD = scanIndexOD;
}
void setStoreODSubIndex(CO_Data *pCOData,storeODSubIndex_t storeODSubIndex)
{
	pCOData->storeODSubIndex = storeODSubIndex;
}

void setDcfODEntry(CO_Data *pCOData,indextable *dcf_odentry)

{
	pCOData->dcf_odentry = dcf_odentry;
}

void setErrorState(CO_Data *pCOData,e_errorState error_state)
{
	pCOData->error_state = error_state;
}
void setErrorHistorySize(CO_Data *pCOData,UNS8 error_history_size)
{
	pCOData->error_history_size = error_history_size;
}

void setErrorNumber(CO_Data *pCOData,UNS8 error_number)
{
	if(!pCOData->error_number)
	{
		pCOData->error_number = (UNS8 *)malloc(sizeof(UNS8));
		if(!pCOData->error_number)
			return;
	}

	*pCOData->error_number = error_number;
}

void setErrorFirstElement(CO_Data *pCOData,UNS32 error_first_element)
{
	if(!pCOData->error_first_element)
	{
		pCOData->error_first_element = (UNS32 *)malloc(sizeof(UNS32));
		if(!pCOData->error_first_element)
			return;
	}

	*pCOData->error_first_element = error_first_element;
}

void setErrorRegister(CO_Data *pCOData,UNS8 error_register)
{
	if(!pCOData->error_register)
	{
		pCOData->error_register = (UNS8 *)malloc(sizeof(UNS8));
		if(!pCOData->error_register)
			return;
	}

	*pCOData->error_register = error_register;
}
void setErrorCobid(CO_Data *pCOData,UNS32 error_cobid)
{
	if(!pCOData->error_cobid)
	{
		pCOData->error_cobid = (UNS32 *)malloc(sizeof(UNS32));
		if(!pCOData->error_cobid)
			return;
	}

	*pCOData->error_cobid = error_cobid;
}

void setPostEmcy(CO_Data *pCOData,post_emcy_t post_emcy)
{
	pCOData->post_emcy = post_emcy;
}

#ifdef CANOPEN_DEBUG
#include "testmaster.h"
indextable *g_Global=NULL;

extern indextable TestMaster_objdict[];
void test()
{
	int i=0,j=0;
	int bSubCount = 0;
	int index = 0;
	void *pSubIndex = NULL;
	
	indextable *pTestMaster_objdict = TestMaster_objdict;
	g_Global = createIndexTable(32);

	
	for(i=0;i<32;i++) {
		bSubCount = pTestMaster_objdict[i].bSubCount;
		index = pTestMaster_objdict[i].index;
		pSubIndex = pTestMaster_objdict[i].pSubindex;
#ifdef CANOPEN_DEBUG
		d_curr_index = index;
		d_curr_cnt = bSubCount;
#endif
		setIndexTableEntry(&g_Global[i],
							allocSubIndex(bSubCount),
							bSubCount,
							index);

		
		setIndexTableEntryValue(&g_Global[i],
			&pTestMaster_objdict[i]);

	}


	g_Global = createIndexTable(32);
	
	for(i=0;i<32;i++) {
		bSubCount = pTestMaster_objdict[i].bSubCount;
		index = pTestMaster_objdict[i].index;
		pSubIndex = pTestMaster_objdict[i].pSubindex;
#ifdef CANOPEN_DEBUG
		d_curr_index = index;
		d_curr_cnt = bSubCount;
#endif
		setIndexTableEntry(&g_Global[i],
							allocSubIndex(bSubCount),
							bSubCount,
							index);

		
		setIndexTableEntryValue(&g_Global[i],
			&pTestMaster_objdict[i]);

	}
}
#endif



