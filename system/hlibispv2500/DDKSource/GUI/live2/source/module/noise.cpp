#include "noise.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleDNS.h"
#include "ispc/ModuleSHA.h"

//
// Public Functions
//

VisionLive::ModuleNoise::ModuleNoise(QWidget *parent): ModuleBase(parent)
{
	Ui::Noise::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleNoise::~ModuleNoise()
{
}

void VisionLive::ModuleNoise::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		primaryDenoiser_Strength_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		primaryDenoiser_GrayscaleThreshold_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sharpening_Radius_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sharpening_Strength_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sharpening_Threshold_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sharpening_Detail_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		noiseSuppression_EdgeAvoidance_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		noiseSuppression_Strength_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		primaryDenoiser_Strength_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		primaryDenoiser_GrayscaleThreshold_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sharpening_Radius_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sharpening_Strength_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sharpening_Threshold_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sharpening_Detail_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		noiseSuppression_EdgeAvoidance_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		noiseSuppression_Strength_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		break;
	}
}

void VisionLive::ModuleNoise::retranslate()
{
	Ui::Noise::retranslateUi(this);
}

void VisionLive::ModuleNoise::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleDNS dns;
	dns.load(config);

	ISPC::ModuleSHA sha;
	sha.load(config);

	//
	// Set minimum
	//

	primaryDenoiser_Strength_sb->setMinimum(ISPC::ModuleDNS::DNS_STRENGTH.min);
	primaryDenoiser_GrayscaleThreshold_sb->setMinimum(ISPC::ModuleDNS::DNS_GREYSCALE_THRESH.min);
	sharpening_Radius_sb->setMinimum(ISPC::ModuleSHA::SHA_RADIUS.min);
	sharpening_Strength_sb->setMinimum(ISPC::ModuleSHA::SHA_STRENGTH.min);
	sharpening_Threshold_sb->setMinimum(ISPC::ModuleSHA::SHA_THRESH.min);
	sharpening_Detail_sb->setMinimum(ISPC::ModuleSHA::SHA_DETAIL.min);
	noiseSuppression_EdgeAvoidance_sb->setMinimum(ISPC::ModuleSHA::SHADN_TAU.min);
	noiseSuppression_Strength_sb->setMinimum(ISPC::ModuleSHA::SHADN_SIGMA.min);

	//
	// Set maximum
	//

	primaryDenoiser_Strength_sb->setMaximum(ISPC::ModuleDNS::DNS_STRENGTH.max);
	primaryDenoiser_GrayscaleThreshold_sb->setMaximum(ISPC::ModuleDNS::DNS_GREYSCALE_THRESH.max);
	sharpening_Radius_sb->setMaximum(ISPC::ModuleSHA::SHA_RADIUS.max);
	sharpening_Strength_sb->setMaximum(ISPC::ModuleSHA::SHA_STRENGTH.max);
	sharpening_Threshold_sb->setMaximum(ISPC::ModuleSHA::SHA_THRESH.max);
	sharpening_Detail_sb->setMaximum(ISPC::ModuleSHA::SHA_DETAIL.max);
	noiseSuppression_EdgeAvoidance_sb->setMaximum(ISPC::ModuleSHA::SHADN_TAU.max);
	noiseSuppression_Strength_sb->setMaximum(ISPC::ModuleSHA::SHADN_SIGMA.max);
	
	//
	// Set precision
	//

	DEFINE_PREC(primaryDenoiser_Strength_sb, 1024.0000f);
	DEFINE_PREC(primaryDenoiser_GrayscaleThreshold_sb, 1024.0000f);
	DEFINE_PREC(sharpening_Radius_sb, 100.000f);
	DEFINE_PREC(sharpening_Strength_sb, 16.00f);
	DEFINE_PREC(sharpening_Threshold_sb, 255.000f);
	DEFINE_PREC(sharpening_Detail_sb, 63.00f);
	DEFINE_PREC(noiseSuppression_EdgeAvoidance_sb, 256.000f);
	DEFINE_PREC(noiseSuppression_Strength_sb, 256.000f);

	//
	// Set value
	//

	primaryDenoiser_Strength_sb->setValue(dns.fStrength);
	primaryDenoiser_GrayscaleThreshold_sb->setValue(dns.fGreyscaleThreshold);
	sharpening_Radius_sb->setValue(sha.fRadius);
	sharpening_Strength_sb->setValue(sha.fStrength);
	sharpening_Threshold_sb->setValue(sha.fThreshold);
	sharpening_Detail_sb->setValue(sha.fDetail);
	noiseSuppression_EdgeAvoidance_sb->setValue(sha.fDenoiseTau);
	noiseSuppression_Strength_sb->setValue(sha.fDenoiseSigma);
}

void VisionLive::ModuleNoise::save(ISPC::ParameterList &config) const
{
	ISPC::Parameter dnsStrength(ISPC::ModuleDNS::DNS_STRENGTH.name, std::string(QString::number(primaryDenoiser_Strength_sb->value()).toLatin1()));
	config.addParameter(dnsStrength, true); // Override existing one

	ISPC::Parameter dnsGreyscaleThreshold(ISPC::ModuleDNS::DNS_GREYSCALE_THRESH.name, std::string(QString::number(primaryDenoiser_GrayscaleThreshold_sb->value()).toLatin1()));
	config.addParameter(dnsGreyscaleThreshold, true); // Override existing one

	ISPC::Parameter radius(ISPC::ModuleSHA::SHA_RADIUS.name, std::string(QString::number(sharpening_Radius_sb->value()).toLatin1()));
	config.addParameter(radius, true); // Override existing one

	ISPC::Parameter shaStrength(ISPC::ModuleSHA::SHA_STRENGTH.name, std::string(QString::number(sharpening_Strength_sb->value()).toLatin1()));
	config.addParameter(shaStrength, true); // Override existing one

	ISPC::Parameter threshold(ISPC::ModuleSHA::SHA_THRESH.name, std::string(QString::number(sharpening_Threshold_sb->value()).toLatin1()));
	config.addParameter(threshold, true); // Override existing one

	ISPC::Parameter detail(ISPC::ModuleSHA::SHA_DETAIL.name, std::string(QString::number(sharpening_Detail_sb->value()).toLatin1()));
	config.addParameter(detail, true); // Override existing one

	// Bypass always off
	ISPC::Parameter bDenoiseBypass(ISPC::ModuleSHA::SHA_DENOISE_BYPASS.name, "0");
	config.addParameter(bDenoiseBypass, true); // Override existing one

	ISPC::Parameter denoise_tauMultiplier(ISPC::ModuleSHA::SHADN_TAU.name, std::string(QString::number(noiseSuppression_EdgeAvoidance_sb->value()).toLatin1()));
	config.addParameter(denoise_tauMultiplier, true); // Override existing one

	ISPC::Parameter denoise_sigmaMultiplier(ISPC::ModuleSHA::SHADN_SIGMA.name, std::string(QString::number(noiseSuppression_Strength_sb->value()).toLatin1()));
	config.addParameter(denoise_sigmaMultiplier, true); // Override existing one
}

void VisionLive::ModuleNoise::resetConnectionDependantControls()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->DNS_set(DNSenable_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleNoise::initModule()
{
	_name = windowTitle();

	connect(DNSenable_cb, SIGNAL(stateChanged(int)), this, SLOT(DNS_set(int)));
}

//
// Protected Slots
//

void VisionLive::ModuleNoise::DNS_set(int enable)
{
	if(_pProxyHandler)
	{
		_pProxyHandler->DNS_set(enable);
	}
}
