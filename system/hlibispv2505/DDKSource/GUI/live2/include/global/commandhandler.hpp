#ifndef COMMANDSENDER_H
#define COMMANDSENDER_H

#include <qthread.h>

#include "paramsocket/demoguicommands.hpp"
#include "ci/ci_api_structs.h"
#include "cv.h"
#include "ispc/ParameterList.h"

#include <qmap.h>
#include <qmutex.h>
#include <qpoint.h>
#include <queue>

class ParamSocketServer;

namespace VisionLive
{

class CommandHandler: public QThread
{
	Q_OBJECT

public:
	CommandHandler(QThread *parent = 0); // Class constructor
	~CommandHandler(); // Class destructor
    void startRunning() { _continueRun = true; } // Sets _continueRun to true

protected:
	void run(); // Thread loop

private:
	bool _continueRun; // Indicates if thread loop should continue

	QMutex _lock; // Mutex used to syncronise use of _cmds
	QList<GUIParamCMD> _cmds; // Queue of commands to run
	void pushCommand(GUIParamCMD cmd); // Thread safe funk to add commands to queue
	GUIParamCMD popCommand(); // Thread safe funk to remove commands from queue

	ParamSocketServer *_pSocket; // Connection socket
	IMG_UINT16 _port; // Connection port
	QString _clientName; // Name of connected app
	int startConnection(); // Starts connection

	int setQuit(ParamSocket *pSocket); // Called to end connection on both sides

	CI_HWINFO _hwInfo; // Struct holding HW info data
	int getHWInfo(ParamSocket *pSocket); // Called tu update _hwInfo data

	ISPC::ParameterList _config; // Holds ParameterList copied from MainWindow class
	int applyConfiguration(ParamSocket *pSocket); // Sends _config to ISPC_tcp

	int FB_get(ParamSocket *socket); // Gets feedback from ISPC_tcp
    int receiveConfiguration(ParamSocket *socket, ISPC::ParameterList &list); // Gets current configuration from ISPC_tcp

	bool _AEState; // Indicates state of AE control module
	int AE_set(ParamSocket *socket); // Enables AE control module
	int AE_get(ParamSocket *socket); // Gets AE control module info

	int SENSOR_get(ParamSocket *socket); // Gets sensor informations from ISPC_tcp

	bool _AFState; // Indicates state of AF control module
	int AF_set(ParamSocket *socket); // Enables AF control module
	int AF_get(ParamSocket *socket); // Gets AF control module information
	int AF_search(ParamSocket *socket); // Triggers focus search

	bool _DNSState; // Indicates state of DNS control module
	int DNS_set(ParamSocket *socket); // Enables DNS control module

	int ENS_get(ParamSocket *socket); // Gets DNS module statistics size

	int _AWBMode; // Holds current mode of AWB control module
	int AWB_set(ParamSocket *socket); // Sets AWB control module mode
	int AWB_get(ParamSocket *socket); // Gets AWB control module params

    std::queue<std::string> _LSHGridFile; // Holds filename of lsh grid
    std::queue<int> _LSHTemp;
    std::queue<unsigned int> _LSHBitsPerDiff;
    std::queue<double> _LSHWBScale;
    bool _LSHState;
	int LSH_add(ParamSocket *socket); // Adds LSH grid
    int LSH_remove(ParamSocket *socket); // Removes LSH grid
    int LSH_apply (ParamSocket *socket); // Apply LSH grid
    int LSH_set(ParamSocket *socket); // Enable ControlLSH
    int LSH_get(ParamSocket *socket); // Gets LSH module params

	IMG_UINT8 *_DPFInputMap; // Holds pointer to DPF input map
	IMG_UINT32 _DPFNDefs; // Holds number of defects in _DPFInputMap
	int DPF_set(ParamSocket *socket); // Sets DPF module input map
	int DPF_get(ParamSocket *socket); // Gets DPF module params

	bool _TNMState; // Indicates state of TNM control module
	int TNM_set(ParamSocket *socket); // Enables TNM control module
	int TNM_get(ParamSocket *socket); // Gets TNM module curve

	bool _LBCState; // Indicates state of LBC control module
	int LBC_set(ParamSocket *socket); // Enables LBC control module
	int LBC_get(ParamSocket *socket); // Gets LBC control module params

	CI_MODULE_GMA_LUT _GMA_data; // Holds current GLUT
	int GMA_set(ParamSocket *socket); // Sets GLUT
	int GMA_get(ParamSocket *socket); // Gets current GLUT

    bool _PDPState;// Indicates state of PDP output
    int PDP_set(ParamSocket *socket); // Enables PDP output

signals:
	void logError(const QString &error, const QString &src); // Emited to log Error
	void logWarning(const QString &warning, const QString &src); // Emited to log Warning
	void logMessage(const QString &message, const QString &src); // Emited to log Info
	void logAction(const QString &action); // Emited to log Action

	void riseError(const QString &error); // Emited to popup error window
	void notifyStatus(const QString &status); // Emited to notify status (displayed in statusBar)

	void connected(); // Emited to inticated connection
	void disconnected(); // Emited to indicate disconnection
	void connectionCanceled(); // Emited to indicate connecting terminated

	void HWInfoReceived(CI_HWINFO hwInfo); // Emited to indicate HW Info received
    void FB_received(const ISPC::ParameterList extList); // Emited to indicate feedback received
	void SENSOR_received(QMap<QString, QString> params); // Emited to indicate sensor params received
	void AE_received(QMap<QString, QString> params); // Emited to indicate AE params received
	void AF_received(QMap<QString, QString> params); // Emited to indicate AF params received
	void ENS_received(IMG_UINT32 size); // Emited to indicate ENS params receive
	void AWB_received(QMap<QString, QString> params); // Emited to indicate AWB params received
	void DPF_received(QMap<QString, QString> params); // Emited to indicate DPF params received
	void TNM_received(QMap<QString, QString> params, QVector<QPointF> curve); // Emited to indicate TNM curve received
	void LBC_received(QMap<QString, QString> params); // Emited to indicate LBC params received
	void GMA_received(CI_MODULE_GMA_LUT data); // Emited to indicate GLUT received
    void LSH_added(QString filename); // Emited after succesfull adding LSH grid
    void LSH_removed(QString filename); // Emited after succesfull removing LSH grid
    void LSH_applyed(QString filename); // Emited after succesfull applying LSH grid
    void LSH_received(QString filename); // Emmited after receiving current LSH grid name

public slots:
	void connect(IMG_UINT16 port); // Callec do start connection
	void disconnect(); // Called to disconnect
	void cancelConnection(); // Called to cancel connecting

	void applyConfiguration(const ISPC::ParameterList &config); // Called from MainWindow to send config to ISPC_tcp
	void FB_get(); // Called from Test to get Feedback
	void AE_set(bool enable); // Called from Exposure to enable AE control module
	void AF_set(bool enable); // Called from Focus to enable AF control module
	void AF_search(); // Called from Focus to trigger focus search
	void DNS_set(bool enable); // Called from Noise to enable DNS control module
	void AWB_set(int mode); // Called from WBC to enable AWB control module
    void LSH_add(std::string filename, int temp, unsigned int bitsPerDiff, double wbScale); // Called from LSH to add LSH grid
    void LSH_remove(std::string filename); // Called from LSH to remove LSH grid
    void LSH_apply(std::string filename); // Called from LSH to apply LSH grid
    void LSH_set(bool enable); // Called from LSH to enable ControlLSH
	void DPF_set(IMG_UINT8 *map, IMG_UINT32 nDefs); // Called from DPF to apply DPF module input map
	void TNM_set(bool enable); // Called from TNM to enable TNM control module
	void LBC_set(bool enable); // Called from TNM to enable LBC control module
	void GMA_set(CI_MODULE_GMA_LUT data); // Called from GMA to update GMA module GLUT
	void GMA_get(); // Called from GMA to get GMA module current GLUT
    void PDP_set(bool enable); // Called from MainWindow to enable/disable generation of PDP image

};

} // namespace VisionLive

#endif // COMMANDSENDER_H
