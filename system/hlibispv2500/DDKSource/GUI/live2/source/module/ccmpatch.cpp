#include "ccmpatch.hpp"
#include "modulebase.hpp"

#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ParameterList.h"

//
// Public Functions
//

VisionLive::CCMPatch::CCMPatch(int index, QWidget *parent)
{
	Ui::CCMPatch::setupUi(this);

	retranslate();

	_index = index;

	initPatch();
}

VisionLive::CCMPatch::~CCMPatch()
{
}

void VisionLive::CCMPatch::retranslate()
{
	Ui::CCMPatch::retranslateUi(this);
}

//
// Private Functions
//

void VisionLive::CCMPatch::initPatch()
{
	setObjectName("CCM_" + QString::number(_index));

	CCM_Gains[0] = CCM_gain0_sb;
	CCM_Gains[1] = CCM_gain1_sb;
	CCM_Gains[2] = CCM_gain2_sb;
	CCM_Gains[3] = CCM_gain3_sb;

	CCM_Clips[0] = CCM_clip0_sb;
	CCM_Clips[1] = CCM_clip1_sb;
	CCM_Clips[2] = CCM_clip2_sb;
	CCM_Clips[3] = CCM_clip3_sb;
	
	CCM_Matrix[0] = CCM_matrix0_sb;
	CCM_Matrix[1] = CCM_matrix1_sb;
	CCM_Matrix[2] = CCM_matrix2_sb;
	CCM_Matrix[3] = CCM_matrix3_sb;
	CCM_Matrix[4] = CCM_matrix4_sb;
	CCM_Matrix[5] = CCM_matrix5_sb;
	CCM_Matrix[6] = CCM_matrix6_sb;
	CCM_Matrix[7] = CCM_matrix7_sb;
	CCM_Matrix[8] = CCM_matrix8_sb;

	CCM_Offsets[0] = CCM_offsets0_sb;
	CCM_Offsets[1] = CCM_offsets1_sb;
	CCM_Offsets[2] = CCM_offsets2_sb;

	// Set minimum
	for(int i = 0; i < 4; i++)
	{
		CCM_Gains[i]->setMinimum(ISPC::ModuleWBC::WBC_GAIN.min);
		CCM_Clips[i]->setMinimum(ISPC::ModuleWBC::WBC_CLIP.min);
	}
	for(int i = 0; i < 9; i++)
	{
		CCM_Matrix[i]->setMinimum(ISPC::ModuleCCM::CCM_MATRIX.min);
	}
	for(int i = 0; i < 3; i++)
	{
		CCM_Offsets[i]->setMinimum(ISPC::ModuleCCM::CCM_OFFSETS.min);
	}

	// Set maximum
	for(int i = 0; i < 4; i++)
	{
		CCM_Gains[i]->setMaximum(ISPC::ModuleWBC::WBC_GAIN.max);
		CCM_Clips[i]->setMaximum(ISPC::ModuleWBC::WBC_CLIP.max);
	}
	for(int i = 0; i < 9; i++)
	{
		CCM_Matrix[i]->setMaximum(ISPC::ModuleCCM::CCM_MATRIX.max);
	}
	for(int i = 0; i < 3; i++)
	{
		CCM_Offsets[i]->setMaximum(ISPC::ModuleCCM::CCM_OFFSETS.max);
	}

	// Set precision
	for(int i = 0; i < 4; i++)
	{
		DEFINE_PREC(CCM_Gains[i], 1024.0000f);
		DEFINE_PREC(CCM_Clips[i], 4096.0000f);
	}
	for(int i = 0; i < 9; i++)
	{
		DEFINE_PREC(CCM_Matrix[i], 512.000f);
	}
	for(int i = 0; i < 3; i++)
	{
		DEFINE_PREC(CCM_Offsets[i], 1.0f);
	}

	QObject::connect(CCM_remove_btn, SIGNAL(clicked()), this, SLOT(remove()));
	QObject::connect(CCM_default_btn, SIGNAL(clicked()), this, SLOT(setDefault()));
	QObject::connect(CCM_promote_btn, SIGNAL(clicked()), this, SLOT(promote()));
	QObject::connect(CCM_identity_btn, SIGNAL(clicked()), this, SLOT(identity()));
}

//
// Protected Slots
//

void VisionLive::CCMPatch::promote()
{
	emit requestPromote();
}

void VisionLive::CCMPatch::setDefault()
{
	emit requestDefault(this);
}

void VisionLive::CCMPatch::remove()
{
	emit requestRemove(this);
}

void VisionLive::CCMPatch::identity()
{
	for(int i = 0; i < 4; i++)
	{
		CCM_Gains[i]->setValue(ISPC::ModuleWBC::WBC_GAIN.def[i]);
	}
	for(int i = 0; i < 4; i++)
	{
		CCM_Clips[i]->setValue(ISPC::ModuleWBC::WBC_CLIP.def[i]);
	}
	for(int i = 0; i < 9; i++)
	{
		CCM_Matrix[i]->setValue(ISPC::ModuleCCM::CCM_MATRIX.def[i]);
	}
	for(int i = 0; i < 3; i++)
	{
		CCM_Offsets[i]->setValue(ISPC::ModuleCCM::CCM_OFFSETS.def[i]);
	}
}
