#ifdef WIN32
#include <windows.h>
#else
#include <sched.h>
#endif

#include <time.h>
#include "transif_adaptor.h"
#include "osa.h"
#include "img_defs.h"
#include "img_errors.h"
#include "device_interface_transif.h"
using namespace std;

#ifdef TEST_OUT_OF_ORDER_MEM_RESP
#define MEM_RESP_DELAY 50 // increase delay so that more requests queued
#else
#define MEM_RESP_DELAY 1
#endif

// The following is used to delay response to memory request
// This struct contains a response which has not been sent to simulator
class TransifMemoryResponse
{
public:
    IMGBUS_ADDRESS_TYPE         m_addr;
    IMGBUS_DATA_TYPE * const    m_data;
    imgbus_transaction_control  m_ctrl;
    IMG_UINT32                  m_delay;        // number of clock cycles since its request has received
    bool                        m_readNotWrite;
#ifdef DELAYED_MEM_REQ_ACK
    bool                        m_reqAcked;
#endif // DELAYED_MEM_REQ_ACK

    TransifMemoryResponse(IMGBUS_ADDRESS_TYPE addr, IMG_UINT32 busWidth, IMG_UINT32 numBusWords, imgbus_transaction_control ctrl, bool readNotWrite, IMG_UINT32 delay = 0)
        : m_addr(addr)
        , m_data(NULL)
        , m_ctrl(ctrl)
        , m_delay(delay)
        , m_readNotWrite(readNotWrite)
#ifdef DELAYED_MEM_REQ_ACK
        , m_reqAcked(false)
#endif // DELAYED_MEM_REQ_ACK
    {
        if (numBusWords > 0) {
            const_cast<IMGBUS_DATA_TYPE *&>(m_data) = new IMGBUS_DATA_TYPE(busWidth, numBusWords);
        }

        // TO-DO, create constructor for imgbus_transaction_control that deep copies mask,
        // doing it here to avoid changes in sim_wrapper_api.h
        if (ctrl.write_mask_ptr != 0)
        {
            // got this num int from transif_scsimif_adaptor.h, burst_write
            int maskSize = ((busWidth / 8) * numBusWords + 31) / 32;
            m_ctrl.write_mask_ptr = new int[maskSize];
            memcpy(m_ctrl.write_mask_ptr, ctrl.write_mask_ptr, maskSize * sizeof(int));
        }
    }

    ~TransifMemoryResponse()
    {
        if (m_data) delete m_data;
        if (m_ctrl.write_mask_ptr) delete [] m_ctrl.write_mask_ptr;
    }
private:
    TransifMemoryResponse();
    // prevent default assignment and copy constructors from being created
    TransifMemoryResponse(const TransifMemoryResponse &memoryResponse);
    TransifMemoryResponse &operator=(const TransifMemoryResponse &memoryResponse);
};

class TransifDevifHandler : public TransifDevifModel // handles calls from pdump Devif
{
public:
    /**
        * Constructor
        * @param parent - the adaptor using this DevifHandler
        * @param toIMGIP - transif IMGIP
        * @param busWidht - transaction bus width
        */
    TransifDevifHandler(TransifAdaptor *parent, toIMGIP_if *toIMGIP, IMG_UINT32 busWidth) :
        TransifDevifModel(toIMGIP, busWidth), m_pParent(parent)
    {
        // nothing to do
    }

    virtual ~TransifDevifHandler() { }

    virtual void Idle(IMG_UINT32 numCycles)
    {
        for (unsigned int i = 0; i < numCycles; i++)
        {
            // one clock at a time
            if (m_pParent->m_timedModel)
            {
                // perform memory responses before advancing to next clock
                m_pParent->ProcessMemoryResponses(MEM_RESP_DELAY);
                m_pParent->m_simulator->Clock(m_pParent->m_numClockCycles);
            }
            else
            {
                // yield every other cycle
                if (i & 1)
                {
                // let C-sim run
#ifdef WIN32
                    //SwitchToThread();
#else /* WIN32 */
                    sched_yield();
#endif /* !WIN32 */
                }
                m_pParent->ProcessMemoryResponses(0);
            }
        }
    }

protected:
    virtual void WaitForSlaveResponse(IMG_UINT32& retValue)
    {
        if (m_pParent->m_timedModel)
        {
            // run the simulator until we get a reply
            while (!m_pParent->m_slaveResponded)
            {
                m_pParent->m_simulator->Clock(m_pParent->m_numClockCycles);
                // needed if Register request blocked on Memory requests
                m_pParent->ProcessMemoryResponses(MEM_RESP_DELAY);

            }
            retValue = m_pParent->m_slaveReadData->data[0];
            m_pParent->m_slaveResponded = false; // for next resp
        }
        else
        {
            // untimed model, the sim automatically runs, wait until we get a reply
            OSA_ThreadConditionLock(m_pParent->m_slaveRespCond);

            while (m_pParent->m_doWaitResp)
            {
                IMG_RESULT res = OSA_ThreadConditionWait(m_pParent->m_slaveRespCond, 2000);

                if (res == IMG_ERROR_TIMEOUT)
                {
                    // Let's wait a bit more...
                    continue;
                }
                else if (res == IMG_ERROR_FATAL)
                {
                    std::cout << "IMG_ERROR_FATAL: waiting for slave response failed!" << std::endl;
                    IMG_ASSERT(false);
                    break;
                }
                else
                    // do memory responses, then repeat
                    m_pParent->ProcessMemoryResponses(0);
            }

            retValue = m_pParent->m_slaveReadData->data[0];
            m_pParent->m_doWaitResp = true; // next time this is called do wait

            OSA_ThreadConditionUnlock(m_pParent->m_slaveRespCond);
        }
    }

private:
    TransifAdaptor *m_pParent;
};

// construct with args list, and devifName from pdump config file
TransifAdaptor::TransifAdaptor(int argc, char **argv, string libName, string devifName, bool debug, bool forceAlloc,
                    bool timedModel, SimpleDevifMemoryModel* pMem, const sHandlerFuncs* handlers,
                    SYSMEM_eMemModel memModel, IMG_UINT64 ui64BaseAddr, IMG_UINT64 ui64Size)
    : m_pMem(pMem)
    , m_timedModel(timedModel)
    , m_doWaitResp(true)
{
    bool responded;
    IMG_RESULT rResult;
    IMGBUS_DATA_TYPE d(128);
    if (handlers)
        m_Handlers = *handlers;

    m_simulator = LoadSimulatorLibrary(libName.c_str(), 0, this, argc, argv);

    if (!m_simulator) {
        std::cerr << "Failed to load library " << libName << std::endl;
        exit(1);
    }

    if (m_pMem == NULL)
    {
        switch(memModel)
        {
            case IMG_MEMORY_MODEL_DYNAMIC:
                m_pMem = getMemoryHandle(memModel, (void*)&forceAlloc);
                break;
            case IMG_MEMORY_MODEL_PREALLOC:
                {
                    SYSMEM_sPrealloc sPreallocInfo;
                    sPreallocInfo.ui64BaseAddr = ui64BaseAddr;
                    sPreallocInfo.ui64Size = ui64Size;
                    m_pMem = getMemoryHandle(memModel, (void*)&sPreallocInfo);
                }
                break;
            case IMG_MEMORY_MODEL_SIM:
                {
                    SYSMEM_sSim sMemInfo;
                    sMemInfo.ui64BaseAddr = ui64BaseAddr;
                    sMemInfo.ui64Size = ui64Size;
                    sMemInfo.bTimed = timedModel;
                    sMemInfo.pSim = m_simulator;
                    m_pMem = getMemoryHandle(memModel, (void*)&sMemInfo);
                }
                break;
            default:
                cerr << "Invalid Transif Memory Model" << endl;
                exit(1);
        }
    }

    if (m_timedModel)
    {
        m_slaveResponded = false;
    }
    else
    {
        m_doWaitResp = true;

        // create thread mutexs for slave access
        OSA_ThreadConditionCreate(&m_slaveRespCond);
    }

    rResult = OSA_CritSectCreate(&m_delayedRespLock);
    IMG_ASSERT(rResult == IMG_SUCCESS);


    m_simulator->SlaveControl(IMG_CONTROL_GET_BUS_WIDTH, &d, responded);
    if (!responded) {
        std::cerr << "Library " << " failed to specify a bus width" << std::endl;
        exit(1);
    }

    m_busWidth = d.data[0];
    cout << "Library reports bus width of " << m_busWidth << endl;

    m_slaveReadData = new IMGBUS_DATA_TYPE(m_busWidth);

    if (m_timedModel)
    {
        m_simulator->SlaveControl(IMG_CONTROL_GET_PREFERRED_NUM_CLOCK_CYCLES, &d, responded);
        m_numClockCycles = responded ? d.data[0] : 1;
        cout << "Using clock() value of " << m_numClockCycles << endl;
    }

    // need to create transif DEVIF to work with pdump
    m_pDevifHandler = new TransifDevifHandler(this, m_simulator, m_busWidth);
    if (devifName == "")
    {
        cerr << "You must supply a device name from the pdump target config file, using the -device option" << endl;
        exit(1);
    }

    m_pDevIFTransif = new DeviceInterfaceTransif(devifName, m_pMem, m_pDevifHandler);

    if (debug) {
        d.data[0] = 1;
        m_simulator->SlaveControl(IMG_CONTROL_SET_DEBUG_LEVEL, &d, responded);
    }
}

TransifAdaptor::~TransifAdaptor()
{
    delete m_pDevifHandler;
    delete m_pDevIFTransif;
    if (!m_timedModel)
    {
        // clean up thread mutexs for slave access
        OSA_ThreadConditionDestroy(m_slaveRespCond);
    }

    OSA_CritSectDestroy(m_delayedRespLock);
    UnloadSimulatorLibrary(m_simulator);
}

// ####################################################################################################
// This where we implement the pure virtual methods in the fromIMGIP class

imgbus_status TransifAdaptor::SlaveResponse(const IMGBUS_DATA_TYPE& d)
{
    if (m_timedModel)
    {
        // copy the data, needs to be deep copy
        m_slaveReadData->copyInFrom(d.data, 1);
        m_slaveResponded = true;
    }
    else
    {
        OSA_ThreadConditionLock(m_slaveRespCond);

        m_slaveReadData->copyInFrom(d.data, 1);
        m_doWaitResp = false;

        OSA_ThreadConditionSignal(m_slaveRespCond);
        OSA_ThreadConditionUnlock(m_slaveRespCond);
    }

    return IMGBUS_OK;
}

void TransifAdaptor::MasterRequest(const IMGBUS_ADDRESS_TYPE a, IMGBUS_DATA_TYPE *d_ptr, bool readNotWrite,
                            const imgbus_transaction_control &ctrl, bool& ack , bool& responded)
{
    if(m_timedModel==false){
        // do actual memory operation
        if (readNotWrite)
        {
            BlockingMasterRead(a, d_ptr, ctrl);
        }
        else
        {
            BlockingMasterWrite(a, d_ptr, ctrl);
        }

        ack = true;
        responded = true;
    }
    else{
    // queue the response to happen later, actual operation happens after the delay
        const IMG_UINT32 burstLength = d_ptr->getBurstLength();
        TransifMemoryResponse* response = new TransifMemoryResponse(a, m_busWidth, burstLength, ctrl, readNotWrite);
        // copy the data
        if (!readNotWrite)
        {
            response->m_data->copyInFrom(d_ptr->data, burstLength);
        }

        // add to queue to respond to simulator at a later point
        OSA_CritSectLock(m_delayedRespLock);
        m_delayedMemResponse.push_back(response);
        OSA_CritSectUnlock(m_delayedRespLock);

        // do instant ack, but delayed response
#ifdef DELAYED_MEM_REQ_ACK
        ack         = false;
#else
        ack         = true;
#endif // DELAYED_MEM_REQ_ACK
        responded   = false;
    } //    INSTANT_MEM_RESP
}

void TransifAdaptor::SlaveResponse(IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE &data, bool &acknowledged)
{
    m_pMem->csim_mem_access_cb(tag_id, data);
  acknowledged = true;
}

void TransifAdaptor::MasterResponseAcknowledge(IMG_UINT32 requestID)
{
    (void)requestID; // not implemented
}

void TransifAdaptor::SlaveStreamOut(const IMG_UINT32 keyID, const IMGBUS_DATA_TYPE& data)
{
    if (m_Handlers.pfnSlaveStream)
        m_Handlers.pfnSlaveStream(m_Handlers.userData, keyID, data);
}

void TransifAdaptor::MasterInfoRequest(const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool& responded)
{
    (void)keyID, (void)d_ptr, (void)responded; // not implemented
}

void TransifAdaptor::SlaveInfoResponse(IMG_UINT32 key, const IMGBUS_DATA_TYPE* data)
{
    (void)key, (void)data; // not implemented
}

void TransifAdaptor::SlaveInterruptRequest(IMG_UINT32 interruptLines)
{
    if (m_Handlers.pfnInterrupt != IMG_NULL)
        m_Handlers.pfnInterrupt(m_Handlers.userData, interruptLines);
}

void TransifAdaptor::EndOfTest()
{
    // not implemented
}

//#ifndef DEBUG_OUTPUT
//#define DEBUG_OUTPUT
//#endif

// Internal method to write to memory
void TransifAdaptor::BlockingMasterWrite( const IMGBUS_ADDRESS_TYPE &a , const IMGBUS_DATA_TYPE *d_ptr, const imgbus_transaction_control &ctrl )
{
#ifdef DEBUG_OUTPUT
    cout << "master1" << ": requesting blocking write at address=" << (IMGBUS_ADDRESS_TYPE)(a) << " data=" << d_ptr[0] << endl;
#endif
    const IMG_UINT32 burstLength = d_ptr->getBurstLength();

    IMG_UINT32 *puiData = new IMG_UINT32[burstLength * d_ptr->getWidthInWords()];
    d_ptr->copyOutTo(puiData, burstLength);

    if (ctrl.write_mask_ptr == 0)
    {
        m_pMem->block_write((IMG_UINT32) a, (IMG_BYTE *) puiData, burstLength * 4 * d_ptr->getWidthInWords());
    }
    else
    {
        for (unsigned int i = 0; i < burstLength * 4 * d_ptr->getWidthInWords(); i++)
        {
            if (!(ctrl.write_mask_ptr[i / 32] & (1 << (i % 32))))
                m_pMem->block_write((IMG_UINT32) a + i, (IMG_BYTE *) puiData + i, 1);
        }
    }
    delete [] puiData;
}

void TransifAdaptor::BlockingMasterRead( const IMGBUS_ADDRESS_TYPE &a, IMGBUS_DATA_TYPE *d_ptr, const imgbus_transaction_control &ctrl )
{
    (void)ctrl; // unused;

#ifdef DEBUG_OUTPUT
    cout << "master1" << ": requesting blocking read at address=" << (IMGBUS_ADDRESS_TYPE)(a) << endl;
#endif

    const IMG_UINT32 burstLength = d_ptr->getBurstLength();

    IMG_UINT32 *puiData = new IMG_UINT32[burstLength * d_ptr->getWidthInWords()];

    m_pMem->block_read((IMG_UINT32) a, (IMG_BYTE *) puiData, burstLength * 4 * d_ptr->getWidthInWords());
    d_ptr->copyInFrom(puiData, burstLength);
    delete [] puiData;
}

/**
    * Sends out delayed memory responses, after each call, every queue response is has its delay incremented by 1 clock.
    * Will send out delays after specified number of clocks has passed, if untimed, the delay is ignored
    */
void TransifAdaptor::ProcessMemoryResponses(unsigned int delay)
{
    OSA_CritSectLock(m_delayedRespLock);

    // send out responses
    if (!m_timedModel)
    {
        // for untimed model, set delay to 0 so send response to all queued
        delay = 0;
    }
#ifdef DELAYED_MEM_REQ_ACK
    else
    {
        unsigned int ackDelay = delay;
        delay *= 2;
    }
#endif // DELAYED_MEM_REQ_ACK

    while (!m_delayedMemResponse.empty())
    {
#ifdef TEST_OUT_OF_ORDER_MEM_RESP
        TransifMemoryResponse* memResp = m_delayedMemResponse.back(); // first in last out
#else
        TransifMemoryResponse* memResp = m_delayedMemResponse.front(); // first in first out
#endif
        if (memResp->m_delay >= delay)
        {
#ifdef TEST_OUT_OF_ORDER_MEM_RESP
            m_delayedMemResponse.pop_back();
#else
            m_delayedMemResponse.pop_front();
#endif
            bool ack; // the ack from this is not really used
            // do actual memory operation
            if (memResp->m_readNotWrite)
            {
                BlockingMasterRead(memResp->m_addr, memResp->m_data, memResp->m_ctrl);
            }
            else
            {
                BlockingMasterWrite(memResp->m_addr, memResp->m_data, memResp->m_ctrl);
            }

            m_simulator->MasterResponse(memResp->m_ctrl.tag_id, memResp->m_data, ack);
            // clean up
            delete memResp;
        }
        // optimisation, as we are inserting from the back, if delay is less, then the rest also will be less
        else if (m_timedModel)
        {
            break;
        }
    }

    if (m_timedModel)
    {
#ifdef DELAYED_MEM_REQ_ACK
        std::list<TransifMemoryResponse *>::iterator iter;
        for (iter = m_delayedMemResponse.begin(); iter != m_delayedMemResponse.end(); iter++)
        {
            if(((*iter)->m_delay >= ackDelay) && !((*iter)->m_reqAcked))
            {
                // send ack
                m_simulator->MasterRequestAcknowledge((*iter)->m_ctrl.tag_id);
                (*iter)->m_reqAcked = true;

            }
        }
#endif // DELAYED_MEM_REQ_ACK

        // increment delays for remaining responses
        std::list<TransifMemoryResponse *>::iterator memRespIter;
        for (memRespIter = m_delayedMemResponse.begin(); memRespIter != m_delayedMemResponse.end(); memRespIter++)
        {
            (*memRespIter)->m_delay++;
        }
    }

    OSA_CritSectUnlock(m_delayedRespLock);
}

extern "C" {

IMG_HANDLE TransifAdaptor_Create(const TransifAdaptorConfig *pConfig)
{
    bool forceAlloc, timedModel, debug;
    string libName, deviceName;
    struct sHandlerFuncs sHandler;

    IMG_ASSERT(pConfig);
    IMG_ASSERT(pConfig->pszLibName != IMG_NULL);
    IMG_ASSERT(pConfig->pszDevifName != IMG_NULL);

    libName = pConfig->pszLibName;
    deviceName = pConfig->pszDevifName;
    timedModel = (pConfig->bTimedModel) ? true : false;
    debug = (pConfig->bDebug) ? true : false;
    forceAlloc = (pConfig->bForceAlloc) ? true : false;

    sHandler.pfnSlaveStream = NULL;
    sHandler.pfnInterrupt = pConfig->sFuncs.pfnInterrupt;
    sHandler.userData = pConfig->sFuncs.userData;

    return (IMG_HANDLE)(new TransifAdaptor(pConfig->nArgCount, pConfig->ppszArgs, libName, deviceName, debug, forceAlloc,
        timedModel, NULL, &sHandler, pConfig->eMemType, pConfig->ui64BaseAddr, pConfig->ui64Size));
}

void TransifAdaptor_Destroy(IMG_HANDLE hAdaptor)
{
    TransifAdaptor *pAdaptor;

    IMG_ASSERT(hAdaptor != IMG_NULL);

    pAdaptor = (TransifAdaptor *)hAdaptor;
    delete pAdaptor;
}

}
