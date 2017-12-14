#include "hwinfo.hpp"
#include <qspinbox.h>
#include <qlabel.h>

//
// Public Functions
//

VisionLive::HWInfo::HWInfo(QWidget *parent): QWidget(parent)
{
	Ui::HWINFO::setupUi(this);

	retranslate();

	_pSpacer = NULL;

	initHWInfo();
}

VisionLive::HWInfo::~HWInfo()
{
}

//
// Private Functions
//

void VisionLive::HWInfo::initHWInfo()
{
	hwinfoListLayout->setAlignment(Qt::AlignTop);

	addEntry(tr("Imagination Technologies identifier"), QString::number(0));
	addEntry(tr("Identify the core familly"), QString::number(0));
	addEntry(tr("Configuration ID"), QString::number(0));

	addEntry(tr("Revision IMG division ID"), QString::number(0));
	addEntry(tr("Revision major"), QString::number(0));
	addEntry(tr("Revision minor"), QString::number(0));
	addEntry(tr("Revision maintenance"), QString::number(0));
	addEntry(tr("Unique identifier for the HW build"), QString::number(0));

	addEntry(tr("Pixels per clock"), QString::number(0));
	addEntry(tr("The maximum number of pixels that the ISP pipeline can process in parallel"), QString::number(0));

	addEntry(tr("Number of bits used at the input of the pipeline after the imager interface block"), QString::number(0));
	addEntry(tr("The number of context that can be simultaneously used"), QString::number(0));
	addEntry(tr("Number of imagers supported"), QString::number(0));
	addEntry(tr("Number of IIF Data Generator"), QString::number(0));
	addEntry(tr("Memory Latency"), QString::number(0));
	addEntry(tr("DPF internal FIFO size (shared between all contexts)"), QString::number(0));

	addEntry(tr("The number of horizontal taps used by the encoder scaler for the luma component"), QString::number(0));
	addEntry(tr("The number of veritcal taps used by the encoder scaler for the luma component"), QString::number(0));
	addEntry(tr("The number of veritcal taps used by the encoder scaler for the chorma component"), QString::number(0));
	addEntry(tr("The number of horizontal taps used by the display scaler for the luma component"), QString::number(0));
	addEntry(tr("The number of vertical taps used by the display scaler for the luma component"), QString::number(0));

	for(int i = 0; i < CI_N_IMAGERS; i++)
	{
		addEntry(tr("Mipi Lanes / Parallel Bus Width [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("Combinaisons of CI_GASKETTYPE [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("One per imager [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("One per imager [") + QString::number(i) + "]", QString::number(0));
	}

	for(int i = 0; i < CI_N_CONTEXT; i++)
	{
		addEntry(tr("Maximum supported width for the context [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("Maximum supported height for the context [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("Size of the buffer used to arbitrate the context [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("Bit depth of the context [") + QString::number(i) + "]", QString::number(0));
		addEntry(tr("Size of the HW waiting queue [") + QString::number(i) + "]", QString::number(0));
	}

	addEntry(tr("Information on the maximum size of the linestore"), QString::number(0));
	addEntry(tr("Combination of the supported functionalities @ref CI_INFO_eSUPPORTED"), QString::number(0));

	addEntry(tr("Size of the CI_PIPELINE object the kernel-driver was compiled with"), QString::number(0));
	addEntry(tr("Changelist saved into the driver when building it"), QString::number(0));
	addEntry(tr("256B or 512B tiles (256Bx16 or 512Bx8)"), QString::number(0));

	addEntry(tr("0 for bypass MMU, otherwise bitdepth of the MMU (e.g. 40 = 40b physical addresses)"), QString::number(0));
	addEntry(tr("Device mmu page size"), QString::number(0));
	addEntry(tr("Reference clock in Mhz"), QString::number(0));

	_pSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	hwinfoListLayout->addItem(_pSpacer, hwinfoListLayout->rowCount(), 0);
}

void VisionLive::HWInfo::addEntry(QString description, QString value)
{
	int row = hwinfoListLayout->rowCount();
	QLabel *label = new QLabel(description);
	label->setAlignment(Qt::AlignRight);
	hwinfoListLayout->addWidget(label, row, 0);
	QSpinBox *spinbox = new QSpinBox();
	params.push_back(spinbox);
	spinbox->setReadOnly(true);
	spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	spinbox->setSpecialValueText(value);
	hwinfoListLayout->addWidget(spinbox, row, 1);
}

void VisionLive::HWInfo::retranslate()
{
	Ui::HWINFO::retranslateUi(this);
}

//
// Public Slots
//

void VisionLive::HWInfo::HWInfoReceived(CI_HWINFO hwInfo)
{
	params[0]->setSpecialValueText(QString::number(hwInfo.ui8GroupId));
	params[1]->setSpecialValueText(QString::number(hwInfo.ui8AllocationId));
	params[2]->setSpecialValueText(QString::number(hwInfo.ui16ConfigId));

	params[3]->setSpecialValueText(QString::number(hwInfo.rev_ui8Designer));
	params[4]->setSpecialValueText(QString::number(hwInfo.rev_ui8Major));
	params[5]->setSpecialValueText(QString::number(hwInfo.rev_ui8Minor));
	params[6]->setSpecialValueText(QString::number(hwInfo.rev_ui8Maint));
	params[7]->setSpecialValueText(QString::number(hwInfo.rev_uid));

	params[8]->setSpecialValueText(QString::number(hwInfo.config_ui8PixPerClock));
	params[9]->setSpecialValueText(QString::number(hwInfo.config_ui8Parallelism));

	params[10]->setSpecialValueText(QString::number(hwInfo.config_ui8BitDepth));
	params[11]->setSpecialValueText(QString::number(hwInfo.config_ui8NContexts));
	params[12]->setSpecialValueText(QString::number(hwInfo.config_ui8NImagers));
	params[13]->setSpecialValueText(QString::number(hwInfo.config_ui8NIIFDataGenerator));
	params[14]->setSpecialValueText(QString::number(hwInfo.config_ui16MemLatency));
	params[15]->setSpecialValueText(QString::number(hwInfo.config_ui32DPFInternalSize));

	params[16]->setSpecialValueText(QString::number(hwInfo.scaler_ui8EncHLumaTaps));
	params[17]->setSpecialValueText(QString::number(hwInfo.scaler_ui8EncVLumaTaps));
	params[18]->setSpecialValueText(QString::number(hwInfo.scaler_ui8EncVChromaTaps));
	params[19]->setSpecialValueText(QString::number(hwInfo.scaler_ui8DispHLumaTaps));
	params[20]->setSpecialValueText(QString::number(hwInfo.scaler_ui8DispVLumaTaps));

	for(int i = 0; i < CI_N_IMAGERS; i++)
	{
		params[21+4*i]->setSpecialValueText(QString::number(hwInfo.gasket_aImagerPortWidth[i]));
		params[22+4*i]->setSpecialValueText(QString::number(hwInfo.gasket_aType[i]));
		params[23+4*i]->setSpecialValueText(QString::number(hwInfo.gasket_aMaxWidth[i]));
		params[24+4*i]->setSpecialValueText(QString::number(hwInfo.gasket_aBitDepth[i]));
	}

	for(int i = 0; i < CI_N_CONTEXT; i++)
	{
		params[37+5*i]->setSpecialValueText(QString::number(hwInfo.context_aMaxWidthMult[i]));
		params[38+5*i]->setSpecialValueText(QString::number(hwInfo.context_aMaxHeight[i]));
		params[39+5*i]->setSpecialValueText(QString::number(hwInfo.context_aMaxWidthSingle[i]));
		params[40+5*i]->setSpecialValueText(QString::number(hwInfo.context_aBitDepth[i]));
		params[41+5*i]->setSpecialValueText(QString::number(hwInfo.context_aPendingQueue[i]));
	}

	params[47]->setSpecialValueText(QString::number(hwInfo.ui32MaxLineStore));
	params[48]->setSpecialValueText(QString::number(hwInfo.eFunctionalities));

	params[49]->setSpecialValueText(QString::number(hwInfo.uiSizeOfPipeline));
	params[50]->setSpecialValueText(QString::number(hwInfo.uiChangelist));
	params[51]->setSpecialValueText(QString::number(hwInfo.uiTiledScheme));

	params[52]->setSpecialValueText(QString::number(hwInfo.uiMMUEnabled));
	params[53]->setSpecialValueText(QString::number(hwInfo.ui32MMUPageSize));
	params[54]->setSpecialValueText(QString::number(hwInfo.ui32RefClockMhz));
}
