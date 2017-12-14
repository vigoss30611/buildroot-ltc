#include "commandhandler.hpp"
#include "algorithms.hpp"
#include "paramsocket/paramsocket.hpp"
#include "img_errors.h"
#include <fstream>

#ifndef WIN32
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#endif

#define N_TRIES 5
#define CMD_DELAY 500

//
// Public Functions
//

VisionLive::CommandHandler::CommandHandler(QThread *parent): QThread(parent)
{
	_continueRun = false;

	_pSocket = NULL;
	_DPFInputMap = NULL;

	_port = 2347;
	_clientName = QString();

	_AEState = false;
	_AFState = false;
	_DNSState = false;
	_AWBMode = 0;
	_DPFNDefs = 0;
	_TNMState = false;
	_LBCState = false;
}

VisionLive::CommandHandler::~CommandHandler()
{
	if(_pSocket)
	{
		delete _pSocket;
	}
	if(_DPFInputMap)
	{
		free(_DPFInputMap);
	}
}

//
// Protected Functions
//

void VisionLive::CommandHandler::run()
{
    _continueRun = true;
	if(startConnection() != IMG_SUCCESS)
	{
		return;
	}
    _continueRun = false;
	emit connected();

    while (!_continueRun) { yieldCurrentThread(); }
	emit logMessage(tr("Running..."), tr("CommandHandler::run()"));

	emit logMessage("Executing GUICMD_GET_HWINFO", tr("CommandHandler::run()"));
	if(getHWInfo(_pSocket) != IMG_SUCCESS)
	{
		emit logError("getHWInfo() failed!", tr("CommandHandler::run()"));
		_continueRun = false;
	}

	while(_continueRun)
	{
		if(!_cmds.isEmpty())
		{
			GUIParamCMD cmd = popCommand();

			switch(cmd)
			{
			case GUICMD_QUIT:
				emit logMessage("Executing GUICMD_QUIT", tr("CommandHandler::run()"));
				if(setQuit(_pSocket) != IMG_SUCCESS)
				{
					emit logError("setQuit() failed!", tr("CommandHandler::run()"));
				}
				_continueRun = false;
				break;
			case GUICMD_SET_PARAMETER_LIST:
				emit logMessage("Executing GUICMD_SET_PARAMETER_LIST", tr("CommandHandler::run()"));
				if(applyConfiguration(_pSocket) != IMG_SUCCESS)
				{
					emit logError("applyConfiguration() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_AE_ENABLE:
				emit logMessage("Executing GUICMD_SET_AE_ENABLE", tr("CommandHandler::run()"));
				if(AE_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("AE_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_AF_ENABLE:
				emit logMessage("Executing GUICMD_SET_AF_ENABLE", tr("CommandHandler::run()"));
				if(AF_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("AF_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_AF_TRIG:
				emit logMessage("Executing GUICMD_SET_AF_TRIG", tr("CommandHandler::run()"));
				if(AF_search(_pSocket) != IMG_SUCCESS)
				{
					emit logError("AF_search() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_AWB_MODE:
				emit logMessage("Executing GUICMD_SET_AWB_MODE", tr("CommandHandler::run()"));
				if(AWB_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("AWB_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_DNS_ENABLE:
				emit logMessage("Executing GUICMD_SET_DNS_ENABLE", tr("CommandHandler::run()"));
				if(DNS_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("DNS_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_LSH_GRID:
				emit logMessage("Executing GUICMD_SET_LSH_GRID", tr("CommandHandler::run()"));
				if(LSH_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("LSH_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_DPF_MAP:
				emit logMessage("Executing GUICMD_SET_DPF_MAP", tr("CommandHandler::run()"));
				if(DPF_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("DPF_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_TNM_ENABLE:
				emit logMessage("Executing GUICMD_SET_TNM_ENABLE", tr("CommandHandler::run()"));
				if(TNM_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("TNM_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_LBC_ENABLE:
				emit logMessage("Executing GUICMD_SET_LBC_ENABLE", tr("CommandHandler::run()"));
				if(LBC_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("LBC_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_GMA:
				emit logMessage("Executing GUICMD_SET_GMA", tr("CommandHandler::run()"));
				if(GMA_set(_pSocket) != IMG_SUCCESS)
				{
					emit logError("GMA_set() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_GET_GMA:
				emit logMessage("Executing GUICMD_GET_GMA", tr("CommandHandler::run()"));
				if(GMA_get(_pSocket) != IMG_SUCCESS)
				{
					emit logError("GMA_get() failed!", tr("CommandHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_GET_FB:
			emit logMessage("Executing GUICMD_GET_FB", tr("CommandHandler::run()"));
			if(FB_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("FB_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
			break;
			default:
				emit logError("Unrecognized command!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
		}

		if(_continueRun)
		{
			if(SENSOR_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("SENSOR_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
			if(AE_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("AE_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
			if(AF_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("AF_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
			if(ENS_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("ENS_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
			if(AWB_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("AWB_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
			if(DPF_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("DPF_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}

			if(TNM_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("TNM_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}

			if(LBC_get(_pSocket) != IMG_SUCCESS)
			{
				emit logError("LBC_get() failed!", tr("CommandHandler::run()"));
				_continueRun = false;
			}
		}

		msleep(CMD_DELAY);
		yieldCurrentThread();
	}

	emit logMessage(tr("Stopped"), tr("CommandHandler::run()"));
	emit disconnected();
}

//
// Private Functions
//

void VisionLive::CommandHandler::pushCommand(GUIParamCMD cmd)
{
	_lock.lock();
	_cmds.push_back(cmd);
	_lock.unlock();
}

GUIParamCMD VisionLive::CommandHandler::popCommand()
{
	GUIParamCMD retCmd;

	_lock.lock();
	retCmd = _cmds[0];
	_cmds.pop_front();
	_lock.unlock();

	return retCmd;
}

int VisionLive::CommandHandler::startConnection()
{
	int ret = 0;

	_pSocket = new ParamSocketServer(_port);

    while(_continueRun)
    {
		if((ret = _pSocket->wait_for_client("VisionLive2.0", 1)) == IMG_SUCCESS)
		{
			// Connected
			emit logMessage(tr("Connected"), tr("CommandHandler::startConnection()"));
			_clientName = QString::fromStdString(_pSocket->clientName());
			break;
		}
		else if(ret == -3)
		{
			// Timeout
			continue;
		}
		else
		{
			// Error
			emit logError(tr("Failed to connect!"), tr("CommandHandler::startConnection()"));
			break;
		}
    }
	if(!_continueRun)
	{
		emit logMessage(tr("Connecting terminated"), tr("CommandHandler::startConnection()"));
		emit connectionCanceled();
		return IMG_ERROR_FATAL;
	}
	else
	{
		emit logMessage(tr("Connecting stopped"), tr("CommandHandler::startConnection()"));
	}

	return ret;
}

int VisionLive::CommandHandler::setQuit(ParamSocket *pSocket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_QUIT);
	if(pSocket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::setQuit()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::getHWInfo(ParamSocket *pSocket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;

	cmd[0] = htonl(GUICMD_GET_HWINFO);
	if(pSocket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError("Failed to write to socket!", tr("CommandHandler::getHWInfo()"));
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(pSocket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError("Failed to read from socket!", tr("CommandHandler::getHWInfo()"));
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	if(nParams != 55)
	{
		emit logError("Wrong number of HWINFO params (" + QString::number(nParams) + ")!", tr("CommandHandler::getHWInfo()"));
		return IMG_ERROR_FATAL;
	}

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	std::vector<QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(pSocket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logMessage("Failed to read from socket!", tr("CommandHandler::getHWInfo()"));
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(pSocket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logMessage("Failed to read from socket!", tr("CommandHandler::getHWInfo()"));
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		QString param = std::string(buff).c_str();
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		params.push_back(param);
	}
	
	_hwInfo.ui8GroupId = params[0].toInt();
	_hwInfo.ui8AllocationId = params[1].toInt();
	_hwInfo.ui16ConfigId = params[2].toInt();

	_hwInfo.rev_ui8Designer = params[3].toInt();
	_hwInfo.rev_ui8Major = params[4].toInt();
	_hwInfo.rev_ui8Minor = params[5].toInt();
	_hwInfo.rev_ui8Maint = params[6].toInt();
	_hwInfo.rev_uid = params[7].toInt();

	_hwInfo.config_ui8PixPerClock = params[8].toInt();
	_hwInfo.config_ui8Parallelism = params[9].toInt();
	_hwInfo.config_ui8BitDepth = params[10].toInt();
	_hwInfo.config_ui8NContexts = params[11].toInt();
	_hwInfo.config_ui8NImagers = params[12].toInt();
	_hwInfo.config_ui8NIIFDataGenerator = params[13].toInt();
	_hwInfo.config_ui16MemLatency = params[14].toInt();
	_hwInfo.config_ui32DPFInternalSize = params[15].toInt();

	_hwInfo.scaler_ui8EncHLumaTaps = params[16].toInt();
	_hwInfo.scaler_ui8EncVLumaTaps = params[17].toInt();
	_hwInfo.scaler_ui8EncVChromaTaps = params[18].toInt();
	_hwInfo.scaler_ui8DispHLumaTaps = params[19].toInt();
	_hwInfo.scaler_ui8DispVLumaTaps = params[20].toInt();

	for(int i = 0; i < CI_N_IMAGERS; i++)
	{
		_hwInfo.gasket_aImagerPortWidth[i] = params[21+4*i].toInt();
		_hwInfo.gasket_aType[i] = params[22+4*i].toInt();
		_hwInfo.gasket_aMaxWidth[i] = params[23+4*i].toInt();
		_hwInfo.gasket_aBitDepth[i] = params[24+4*i].toInt();
	}

	for(int i = 0; i < CI_N_CONTEXT; i++)
	{
		_hwInfo.context_aMaxWidthMult[i] = params[37+5*i].toInt();
		_hwInfo.context_aMaxHeight[i] = params[38+5*i].toInt();
		_hwInfo.context_aMaxWidthSingle[i] = params[39+5*i].toInt();
		_hwInfo.context_aBitDepth[i] = params[40+5*i].toInt();
		_hwInfo.context_aPendingQueue[i] = params[41+5*i].toInt();
	}

	_hwInfo.ui32MaxLineStore = params[47].toInt();

	_hwInfo.eFunctionalities = params[48].toInt();

	_hwInfo.uiSizeOfPipeline = params[49].toInt();
	_hwInfo.uiChangelist = params[50].toInt();
	_hwInfo.uiTiledScheme = params[51].toInt();

	_hwInfo.uiMMUEnabled = params[52].toInt();
	_hwInfo.ui32MMUPageSize = params[53].toInt();

	_hwInfo.ui32RefClockMhz = params[54].toInt();
	
	emit HWInfoReceived(_hwInfo);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::applyConfiguration(ParamSocket *pSocket)
{
	ISPC::ParameterList tmpParamsList;
	_lock.lock();
	tmpParamsList = _config;
	_lock.unlock();

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	std::string param;

	// Start receiving on the FPGA side
	cmd[0] = htonl(GUICMD_SET_PARAMETER_LIST);
	if(_pSocket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
		return IMG_ERROR_FATAL;
    }

	std::map<std::string, ISPC::Parameter>::const_iterator it;
	for(it = tmpParamsList.begin(); it != tmpParamsList.end(); it++)
	{
		param = it->first;

		// Send command informing of upcomming parameter name
		cmd[0] = htonl(GUICMD_SET_PARAMETER_NAME);
		cmd[1] = htonl(param.length());
		if(_pSocket->write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
			return IMG_ERROR_FATAL;
		}

		// Send parameter name
		strcpy(buff, param.c_str());
		if(_pSocket->write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		for(unsigned int i = 0; i < it->second.size(); i++)
		{
			param = it->second.getString(i);

			// Send command informing of upcomming parameter value
			cmd[0] = htonl(GUICMD_SET_PARAMETER_VALUE);
			cmd[1] = htonl(param.length());
			if(_pSocket->write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
			{
				emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
				return IMG_ERROR_FATAL;
			}

			// Send parameter value
			strcpy(buff, param.c_str());
			if(_pSocket->write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
			{
				emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
				return IMG_ERROR_FATAL;
			}
			memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		}

		// Send command informing of upcomming parameter value
		cmd[0] = htonl(GUICMD_SET_PARAMETER_END);
		cmd[1] = htonl(0);
		if(_pSocket->write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
			return IMG_ERROR_FATAL;
		}
	}

	// Send ending commend
	cmd[0] = htonl(GUICMD_SET_PARAMETER_LIST_END);
	cmd[1] = htonl(0);
	if(_pSocket->write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		emit logError(tr("Failed to write to socket!"), "CommandHandler::applyConfiguration()");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::FB_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_FB);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			params.insert(words[0], words[1]);
		}
		else
		{
			emit logError(tr("Feedback parameter lacks name or value!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit FB_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AE_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_AE_ENABLE);
	cmd[1] = htonl((int)_AEState);
	if(socket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AE_set()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AE_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_AE);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			params.insert(words[0], words[1]);
		}
		else
		{
			emit logError(tr("AE parameter lacks name or value!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit AE_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::SENSOR_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_SENSOR);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::SENSOR_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::SENSOR_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::SENSOR_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::SENSOR_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			params.insert(words[0], words[1]);
		}
		else
		{
			emit logError(tr("Sensor parameter lacks name or value!"), "CommandHandler::SENSOR_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit SENSOR_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AF_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_AF_ENABLE);
	cmd[1] = htonl(_AFState);
	if(socket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AF_set()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AF_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_AF);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			params.insert(words[0], words[1]);
		}
		else
		{
			emit logError(tr("AE parameter lacks name or value!"), "CommandHandler::FB_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit AF_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AF_search(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_AF_TRIG);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AF_search()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::DNS_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_DNS_ENABLE);
	cmd[1] = htonl(_DNSState);
	if(socket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::DNS_set()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::ENS_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_ENS);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::ENS_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive size of TNM curve
	if(socket->read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		emit logError(tr("Failed to write to socket!"), "CommandHandler::ENS_get()");
		return IMG_ERROR_FATAL;
	}

	IMG_UINT32 size = ntohl(cmd[0]);

	//printf("ENS - size %d\n", size);

	emit ENS_received(size);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AWB_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_AWB_MODE);
	cmd[1] = htonl(_AWBMode);
	if(socket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_set()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::AWB_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_AWB);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			QString p;
			for(int i = 1; i < words.size(); i++)
			{
				p = p + words[i] + " ";
			}
			params.insert(words[0], p);
		}
		else
		{
			emit logError(tr("AWB parameter lacks name or value!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit AWB_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::LSH_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	_lock.lock();

	// Add double slashs to file path to ctreate proper file path
	std::string filename;
	for(unsigned int i = 0; i < _LSHGridFile.size(); i++)
	{
		filename.push_back(_LSHGridFile[i]);
		if(_LSHGridFile[i] == '/')
			filename.push_back('/');
	}

	std::ifstream file(filename.c_str(), std::ios::binary);
	if(!file.is_open()) return IMG_ERROR_FATAL;

	char label[4];
	memset(label, 0, 4*sizeof(char));
	file.read(label, 4*sizeof(char));

	// Check if label is LSH else its a wron file
	if(strncmp(label, "LSH\0", 4*sizeof(char)) != IMG_SUCCESS)
	{
		emit logError(tr("Invalid LSH file!"), "CommandHandler::LSH_set()");
		file.close();
		_lock.unlock();
		return IMG_SUCCESS;
	}

	int version;
	file.read((char *)(&version), sizeof(int));

	IMG_UINT32 cols;
	file.read((char *)(&cols), sizeof(IMG_UINT32));

	IMG_UINT32 rows;
	file.read((char *)(&rows), sizeof(IMG_UINT32));

	IMG_UINT32 tileSize;
	file.read((char *)(&tileSize), sizeof(IMG_UINT32));

	cmd[0] = htonl(GUICMD_SET_LSH_GRID);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::LSH_set()");
		return IMG_ERROR_FATAL;
    }

	cmd[0] = htonl(tileSize);
	cmd[1] = htonl(cols);
	cmd[2] = htonl(rows);
	if(socket->write(&cmd, 3*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::LSH_set()");
		return IMG_ERROR_FATAL;
    }

	float *matrix[4];
	for(int channel = 0; channel < 4; channel++)
	{
		matrix[channel] = (float *)malloc((IMG_UINT16)cols*(IMG_UINT16)rows*sizeof(float));
		memset(matrix[channel], 0, (IMG_UINT16)cols*(IMG_UINT16)rows*sizeof(float));
	}

	for(int channel = 0; channel < 4; channel++)
		file.read((char *)(matrix[channel]), (IMG_UINT16)cols*(IMG_UINT16)rows*sizeof(float));

	for(int channel = 0; channel < 4; channel++)
	{
		if(socket->write(matrix[channel], (IMG_UINT16)cols*(IMG_UINT16)rows*sizeof(float), nWritten, -1) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::LSH_set()");
			return IMG_ERROR_FATAL;
		}
	}

	free(matrix[0]);
	free(matrix[1]);
	free(matrix[2]);
	free(matrix[3]);

	file.close();

	_lock.unlock();

	emit logMessage(tr("LSH grid updated!"), "CommandHandler::LSH_set()");

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::DPF_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	_lock.lock();

	// Send command
	cmd[0] = htonl(GUICMD_SET_DPF_MAP);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::DPF_set()");
		return IMG_ERROR_FATAL;
    }

	// Send number of defects (indicates size of wright map)
	cmd[0] = htonl(_DPFNDefs);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::DPF_set()");
		return IMG_ERROR_FATAL;
    }

	// Send wright map
	size_t toSend = _DPFNDefs*sizeof(IMG_UINT32);
	size_t w = 0;
	size_t maxChunk = PARAMSOCK_MAX;

	//LOG_DEBUG("Writing dpfMap data %d bytes\n", toSend);
	while(w < toSend)
	{
		if(socket->write(((IMG_UINT8*)_DPFInputMap)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::DPF_set()");
			return IMG_ERROR_FATAL;
		}
		//if(w%(PARAMSOCK_MAX) == 0)LOG_DEBUG("dpfMap write %ld/%ld\n", w, toSend);
		w += nWritten;
	}

	_lock.unlock();

	emit logMessage(tr("DPF input map applyed!"), "CommandHandler::DPF_set()");

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::DPF_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_DPF);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			QString p;
			for(int i = 1; i < words.size(); i++)
			{
				p = p + words[i] + " ";
			}
			params.insert(words[0], p);
		}
		else
		{
			emit logError(tr("AWB parameter lacks name or value!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit DPF_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::TNM_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_TNM_ENABLE);
	cmd[1] = htonl(_TNMState);
	if(socket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::TNM_set()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::TNM_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_TNM);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::TNM_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive size of TNM curve
	if(socket->read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		emit logError(tr("Failed to write to socket!"), "CommandHandler::TNM_get()");
		return IMG_ERROR_FATAL;
	}

	int size = ntohl(cmd[0]);

	// Receive TNM curve points
	QVector<QPointF> curve;
	curve.push_back(QPointF(0.0f, 0.0f)); // First point
	//printf("TNM CURVE\n");
	for(int i = 0; i < size; i++)
	{
		if(socket->read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::TNM_get()");
			return IMG_ERROR_FATAL;
		}
		QPointF point = QPointF(i + 1, ntohl(cmd[0])/1000.0f);
		curve.push_back(point);
		//printf("%f  ", ntohl(cmd[0])/1000.0f);
	}
	//printf("\n");
	curve.push_back(QPointF(size + 1.0f, 1.0f)); // Last point

	///////////////////////////////////////////////////////////////////////////////////////////

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			QString p;
			for(int i = 1; i < words.size(); i++)
			{
				p = p + words[i] + " ";
			}
			params.insert(words[0], p);
		}
		else
		{
			emit logError(tr("AWB parameter lacks name or value!"), "CommandHandler::AWB_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit TNM_received(params, curve);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::LBC_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_LBC_ENABLE);
	cmd[1] = htonl(_LBCState);
	if(socket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::LBC_set()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::LBC_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_LBC);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::LBC_get()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::LBC_get()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::LBC_get()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::LBC_get()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			QString p;
			for(int i = 1; i < words.size(); i++)
			{
				p = p + words[i] + " ";
			}
			params.insert(words[0], p);
		}
		else
		{
			emit logError(tr("LBC parameter lacks name or value!"), "CommandHandler::LBC_get()");
			return IMG_ERROR_FATAL;
		}
	}

	emit LBC_received(params);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::GMA_set(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	_lock.lock();

	// Send GMA set command
	cmd[0] = htonl(GUICMD_SET_GMA);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::GMA_set()");
		return IMG_ERROR_FATAL;
    }

	// Send GMA LUT
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(_GMA_data.aRedPoints[i]);
		if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::GMA_set()");
			return IMG_ERROR_FATAL;
		}
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(_GMA_data.aGreenPoints[i]);
		if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::GMA_set()");
			return IMG_ERROR_FATAL;
		}
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(_GMA_data.aBluePoints[i]);
		if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::GMA_set()");
			return IMG_ERROR_FATAL;
		}
	}

	_lock.unlock();

	emit logMessage(tr("GMA LUT updated!"), "CommandHandler::GMA_set()");

	GMA_get(socket);

	return IMG_SUCCESS;
}

int VisionLive::CommandHandler::GMA_get(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	cmd[0] = htonl(GUICMD_GET_GMA);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::LBC_get()");
		return IMG_ERROR_FATAL;
    }

	CI_MODULE_GMA_LUT data;

	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to read from socket!"), "CommandHandler::LBC_get()");
			return IMG_ERROR_FATAL;
		}
		data.aRedPoints[i] = ntohl(cmd[0]);
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to read from socket!"), "CommandHandler::LBC_get()");
			return IMG_ERROR_FATAL;
		}
		data.aGreenPoints[i] = ntohl(cmd[0]);
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to read from socket!"), "CommandHandler::LBC_get()");
			return IMG_ERROR_FATAL;
		}
		data.aBluePoints[i] = ntohl(cmd[0]);
	}

	emit logMessage(tr("GMA LUT received!"), "CommandHandler::GMA_set()");

	emit GMA_received(data);

	return IMG_SUCCESS;
}

//
// Public Slots
//

void VisionLive::CommandHandler::connect(IMG_UINT16 port)
{
	_port = port;

	if(!isRunning()) 
	{
		start();
	}
}

void VisionLive::CommandHandler::disconnect()
{
	if(isRunning())
	{
		emit logMessage(tr("Waiting for disconnection..."), tr("CommandHandler::disconnect()"));
		pushCommand(GUICMD_QUIT);
	}
}

void VisionLive::CommandHandler::cancelConnection()
{
	_continueRun = false;
}

void VisionLive::CommandHandler::applyConfiguration(const ISPC::ParameterList &config)
{
	_lock.lock();
	_config = config;
	_lock.unlock();
	pushCommand(GUICMD_SET_PARAMETER_LIST);
}

void VisionLive::CommandHandler::FB_get()
{
	pushCommand(GUICMD_GET_FB);
}

void VisionLive::CommandHandler::AE_set(bool enable)
{
	_lock.lock();
	_AEState = enable;
	_lock.unlock();
	pushCommand(GUICMD_SET_AE_ENABLE);
}

void VisionLive::CommandHandler::AF_set(bool enable)
{
	_lock.lock();
	_AFState = enable;
	_lock.unlock();
	pushCommand(GUICMD_SET_AF_ENABLE);
}

void VisionLive::CommandHandler::AF_search()
{
	pushCommand(GUICMD_SET_AF_TRIG);
}

void VisionLive::CommandHandler::DNS_set(bool enable)
{
	_lock.lock();
	_DNSState = enable;
	_lock.unlock();
	pushCommand(GUICMD_SET_DNS_ENABLE);
}

void VisionLive::CommandHandler::AWB_set(int mode)
{
	_lock.lock();
	_AWBMode = mode;
	_lock.unlock();
	pushCommand(GUICMD_SET_AWB_MODE);
}

void VisionLive::CommandHandler::LSH_set(std::string filename)
{
	_lock.lock();
	_LSHGridFile = filename;
	_lock.unlock();
	pushCommand(GUICMD_SET_LSH_GRID);
}

void VisionLive::CommandHandler::DPF_set(IMG_UINT8 *map, IMG_UINT32 nDefs)
{
	_lock.lock();
	if(!_DPFInputMap)
	{
		free(_DPFInputMap);
	}
	_DPFInputMap = (IMG_UINT8 *)malloc(nDefs*sizeof(IMG_UINT32));
	memcpy(_DPFInputMap, map, nDefs*sizeof(IMG_UINT32));
	_DPFNDefs = nDefs;
	_lock.unlock();
	pushCommand(GUICMD_SET_DPF_MAP);
}

void VisionLive::CommandHandler::TNM_set(bool enable)
{
	_lock.lock();
	_TNMState = enable;
	_lock.unlock();
	pushCommand(GUICMD_SET_TNM_ENABLE);
}

void VisionLive::CommandHandler::LBC_set(bool enable)
{
	_lock.lock();
	_LBCState = enable;
	_lock.unlock();
	pushCommand(GUICMD_SET_LBC_ENABLE);
}

void VisionLive::CommandHandler::GMA_set(CI_MODULE_GMA_LUT data)
{
	_lock.lock();
	_GMA_data = data;
	_lock.unlock();
	pushCommand(GUICMD_SET_GMA);
}

void VisionLive::CommandHandler::GMA_get()
{
	pushCommand(GUICMD_GET_GMA);
}