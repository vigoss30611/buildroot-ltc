/**
*******************************************************************************
@file main.cpp

@brief Create of the GUI

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "visiontuning.hpp"
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include "gma/widget.hpp"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Register unusual parameters
    // Needed for later passing "QVector<QPointF>" in signals
    qRegisterMetaType<QVector<QPointF> >("QVector<QPointF>");
    // Needed for later passing "GLUTWidget *" in signals
    qRegisterMetaType<GMAWidget * >("GMAWidget *");

    int ret = EXIT_FAILURE;

    VisionTuning tune;

    tune.show();

    ret = app.exec();

    return ret;
}

#ifdef WIN32
#include <Windows.h>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
