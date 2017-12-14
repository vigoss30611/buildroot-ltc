/**
*******************************************************************************
@file ModuleLSH.cpp

@brief Implementation of ISPC::ModuleLSH

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
#include "ispc/ModuleLSH.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_LSH"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#define LSH_GRADIENT_MIN -4.0f
#define LSH_GRADIENT_MAX 4.0f

static const double LSH_GRADIENT_DEF[LSH_GRADS_NO] = {0.0, 0.0, 0.0, 0.0};
IMG_STATIC_ASSERT(LSH_GRADS_NO == 4, LSH_GRADS_NO_CHANGED)
const ISPC::ParamDefArray<double> ISPC::ModuleLSH::LSH_GRADIENT_X(
    "LSH_GRADIENTX", LSH_GRADIENT_MIN, LSH_GRADIENT_MAX, LSH_GRADIENT_DEF,
    LSH_GRADS_NO);
const ISPC::ParamDefArray<double> ISPC::ModuleLSH::LSH_GRADIENT_Y(
    "LSH_GRADIENTY", LSH_GRADIENT_MIN, LSH_GRADIENT_MAX, LSH_GRADIENT_DEF,
    LSH_GRADS_NO);

const ISPC::ParamDefSingle<bool> ISPC::ModuleLSH::LSH_MATRIX(
    "LSH_MATRIX_ENABLE", false);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleLSH::LSH_FILE(
    "LSH_MATRIX_FILE", "");
// used as default when saving min/max/def as default is no file
#define LSH_FILE_DEF "<matrixfile>"

ISPC::ParameterGroup ISPC::ModuleLSH::getGroup()
{
    ParameterGroup group;

    group.header = "// Lens Shading parameters";

    group.parameters.insert(LSH_GRADIENT_X.name);
    group.parameters.insert(LSH_GRADIENT_Y.name);
    group.parameters.insert(LSH_FILE.name);
    group.parameters.insert(LSH_MATRIX.name);

    return group;
}

ISPC::ModuleLSH::ModuleLSH() : SetupModuleBase(LOG_TAG)
{
    // initialize the lsh grid to NULL
    for (int c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        sGrid.apMatrix[c] = NULL;
    }
    ParameterList defaults;
    load(defaults);
}

ISPC::ModuleLSH::~ModuleLSH()
{
    if ( pipeline )
    {
        MC_PIPELINE *pMCPipeline = pipeline->getMCPipeline();

        // clean the lsh matrix if allocated
        for (int c = 0 ; c < LSH_GRADS_NO ; c++ )
        {
            if (sGrid.apMatrix[c] )
            {
                IMG_FREE(sGrid.apMatrix[c]);
                sGrid.apMatrix[c] = NULL;
            }
            pMCPipeline->sLSH.sGrid.apMatrix[c] = NULL;
        }
    }
}

bool ISPC::ModuleLSH::hasGrid() const
{
    bool ret = true;
    for ( int i = 0 ; i < LSH_MAT_NO ; i++ )
    {
        ret = sGrid.apMatrix[i] != NULL && ret;
    }
    return ret;
}

IMG_RESULT ISPC::ModuleLSH::load(const ParameterList &parameters)
{
    int i;
    IMG_RESULT ret;

    bEnableMatrix = parameters.getParameter(LSH_MATRIX);
    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        aGradientX[i] = parameters.getParameter(LSH_GRADIENT_X, i);
    }
    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        aGradientY[i] = parameters.getParameter(LSH_GRADIENT_Y, i);
    }

    // Allocate and load LSH matrix if LSH file is specified
    // check if we have to load LSH from a file
    if (parameters.exists(LSH_FILE.name) && !hasGrid())
    {
        ret = loadMatrix(parameters.getParameter(LSH_FILE));
        if (ret)
        {
            MOD_LOG_WARNING("failed to load matrix from file parameters\n");
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::loadMatrix(const std::string &filename)
{
    IMG_RESULT ret;
    lshFilename = filename;
    MOD_LOG_DEBUG("loading LSH matrix from %s\n", lshFilename.c_str());

    ret = LSH_Load_bin(&(sGrid), lshFilename.c_str());
    if (ret)
    {
        int c;
        MOD_LOG_WARNING("Failed to load the LSH matrix %s - no matrix "\
            "will be loaded\n", lshFilename.c_str());
        for (c = 0; c < LSH_GRADS_NO; c++)
        {
            if (sGrid.apMatrix[c] != NULL)
            {
                IMG_FREE(sGrid.apMatrix[c]);
                sGrid.apMatrix[c] = NULL;
            }
        }
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleLSH::getGroup();
    }

    parameters.addGroup("ModuleLSH", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(Parameter(LSH_MATRIX.name,
            toString(this->bEnableMatrix)));

        values.clear();
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            values.push_back(toString(this->aGradientX[i]));
        }
        parameters.addParameter(Parameter(LSH_GRADIENT_X.name, values));

        values.clear();
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            values.push_back(toString(this->aGradientY[i]));
        }
        parameters.addParameter(Parameter(LSH_GRADIENT_Y.name, values));

        if ( !lshFilename.empty() )
        {
            parameters.addParameter(Parameter(LSH_FILE.name,
                this->lshFilename));
        }
        /** @ add additional function to save the matrix to a
         * different file */
        break;

    case SAVE_MIN:
        // matrix does not have a min
        parameters.addParameter(Parameter(LSH_MATRIX.name,
            toString(LSH_MATRIX.def)));

        values.clear();
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            values.push_back(toString(LSH_GRADIENT_MIN));
        }
        // all gradients have same min
        parameters.addParameter(Parameter(LSH_GRADIENT_X.name, values));
        parameters.addParameter(Parameter(LSH_GRADIENT_Y.name, values));

        // file does not have a min
        parameters.addParameter(Parameter(LSH_FILE.name, LSH_FILE_DEF));
        break;

    case SAVE_MAX:
        // matrix does not have a max
        parameters.addParameter(Parameter(LSH_MATRIX.name,
            toString(LSH_MATRIX.def)));

        values.clear();
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            values.push_back(toString(LSH_GRADIENT_MAX));
        }
        // all gradients have same max
        parameters.addParameter(Parameter(LSH_GRADIENT_X.name, values));
        parameters.addParameter(Parameter(LSH_GRADIENT_Y.name, values));

        // file does not have a max
        parameters.addParameter(Parameter(LSH_FILE.name, LSH_FILE_DEF));
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(LSH_MATRIX.name,
            toString(LSH_MATRIX.def) + " // " + getParameterInfo(LSH_MATRIX)));

        values.clear();
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            values.push_back(toString(LSH_GRADIENT_DEF[i]));
        }
        values.push_back("// " + getParameterInfo(LSH_GRADIENT_X));
        // all gradients have same default
        parameters.addParameter(Parameter(LSH_GRADIENT_X.name, values));
        parameters.addParameter(Parameter(LSH_GRADIENT_Y.name, values));

        // file does not have a default
        parameters.addParameter(Parameter(LSH_FILE.name, LSH_FILE_DEF));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::setup()
{
    MC_PIPELINE *pMCPipeline = NULL;
    if (!pipeline)
    {
        MOD_LOG_ERROR("pipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pMCPipeline = pipeline->getMCPipeline();
    if (!pMCPipeline)
    {
        MOD_LOG_ERROR("pMCPipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    const Global_Setup &globalSetup = pipeline->getGlobalSetup();

    // specifies if we are using the lens shading matrix
    pMCPipeline->sLSH.bUseDeshadingGrid = bEnableMatrix
        && (sGrid.apMatrix != NULL);

    // in CFAs
    pMCPipeline->sLSH.aOffset[0] =
        pMCPipeline->sIIF.ui16ImagerOffset[0]/(globalSetup.CFA_WIDTH);
    pMCPipeline->sLSH.aOffset[1] =
        pMCPipeline->sIIF.ui16ImagerOffset[1]/(globalSetup.CFA_HEIGHT);

    pMCPipeline->sLSH.aSkip[0] = pMCPipeline->sIIF.ui16ImagerDecimation[0];
    pMCPipeline->sLSH.aSkip[1] = pMCPipeline->sIIF.ui16ImagerDecimation[1];

    if (pMCPipeline->sLSH.bUseDeshadingGrid)  // using deshading matrix
    {
        // WARNING: the pointers are copied, which means freeing them in ISPC
        // or MC affects the other!
        // The pointers should be free in ISPC if allocated by the parameter
        // function
        IMG_MEMCPY(&(pMCPipeline->sLSH.sGrid), &(sGrid), sizeof(LSH_GRID));
    }
    else
    {
        int c;
        for (c = 0 ; c < LSH_GRADS_NO ; c++)
        {
            // to make sure it does not use some memory that should not
            // be used!
            pMCPipeline->sLSH.sGrid.apMatrix[c] = NULL;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        pMCPipeline->sLSH.aGradients[0][i] = aGradientX[i];
        pMCPipeline->sLSH.aGradients[1][i] = aGradientY[i];
    }

    this->setupFlag = true;
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::saveMatrix(const std::string &filename) const
{
    if ( hasGrid() )
    {
        return LSH_Save_bin(&sGrid, filename.c_str());
    }
    return IMG_ERROR_NOT_SUPPORTED;
}
