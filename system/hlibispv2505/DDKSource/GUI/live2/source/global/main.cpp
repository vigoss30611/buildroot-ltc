#include <QApplication>
#include "mainwindow.hpp"
#include "ispc/ParameterList.h"

#include "ci/ci_api_structs.h"
#include <dyncmd/commandline.h>

#include <qpoint.h>

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	// Register unusual parameters
	qRegisterMetaType<CI_HWINFO>("CI_HWINFO");
	qRegisterMetaType<std::vector<int> >("std::vector<int>");
	qRegisterMetaType<QMap<int, std::pair<int, QRgb> > >("QMap<int, std::pair<int, QRgb> >");
	qRegisterMetaType<QMap<QString, QString> >("QMap<QString, QString>");
	qRegisterMetaType<IMG_UINT32>("IMG_UINT32");
	qRegisterMetaType<QVector<QPointF> >("QVector<QPointF>");
	qRegisterMetaType<QVector<QPointF> >("QVector<QVector<QPointF> >");
	qRegisterMetaType<CI_MODULE_GMA_LUT>("CI_MODULE_GMA_LUT");
	qRegisterMetaType<std::vector<double> >("std::vector<double>");
	qRegisterMetaType<std::vector<std::vector<int> > >("<std::vector<std::vector<int> >");
	qRegisterMetaType<QMap<QString, std::vector<std::vector<int> > > >("QMap<QString, std::vector<std::vector<int> > >");
    qRegisterMetaType<ISPC::ParameterList>("ISPC::ParameterList");

	char *mode = NULL;
	char *testFile = NULL;
	IMG_BOOL8 testStart = IMG_FALSE;
	int ret = RET_FOUND;
	IMG_UINT32 unregistered;
	bool finish = false;

	ret = DYNCMD_AddCommandLine(argc, argv, "-source");
	if(ret != RET_FOUND)
    {
        printf("ERROR: unable to parse command line\n");
        DYNCMD_ReleaseParameters();
    }

	ret = DYNCMD_RegisterParameter("-mode", DYNCMDTYPE_STRING, "mode to start VisionLive in", &(mode));
	if(ret == RET_FOUND)
    {
		if(mode && strcmp(mode, "TEST") != 0)
		{
			printf("ERROR: unsuported mode - %s\n", mode);
			mode = NULL;
		}
    }

	ret = DYNCMD_RegisterParameter("-testScript", DYNCMDTYPE_STRING, "path to test script file", &(testFile));
	if(ret != RET_FOUND)
    {
		if(mode)
		{
			printf("INFO: test script file location not provided\n");
		}
    }

	ret = DYNCMD_RegisterParameter("-testStart", DYNCMDTYPE_BOOL8, "start test right away", &(testStart));
	if(ret != RET_FOUND)
    {
		if(mode)
		{
			printf("INFO: test will not be started right away\n");
		}
    }

	if((ret=DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "usage information", NULL)) == RET_FOUND)
    {
        DYNCMD_PrintUsage();
		finish = true;
    }

    DYNCMD_HasUnregisteredElements(&unregistered);

    DYNCMD_ReleaseParameters();

	if(finish) return 0;

	VisionLive::MainWindow mainWindow((mode && strcmp(mode, "TEST") == 0), QString(testFile), testStart);
	//VisionLive::MainWindow mainWindow(true, QString(), false);
	mainWindow.show();

    return app.exec();
}

#ifdef WIN32
#include <Windows.h>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}
#endif