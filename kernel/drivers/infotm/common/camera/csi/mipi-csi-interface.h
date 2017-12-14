/*
 * mipi-csi-interface.h
 * INFOTM MIPI CSI
 */
#ifndef INFOTM_MIPI_CSI_H_
#define INFOTM_MIPI_CSI_H_

//#define S5P_CLKREG(x)           (S3C_VA_SYS + (x))
//#define INFOTM_MIPI_DPHY_CONTROL(x) 	INFOTM_CLKREG(0xE814)
#define INFOTM_MIPI_DPHY_ENABLE		(1 << 0)
#define INFOTM_MIPI_DPHY_SRESETN	(1 << 1)
#define INFOTM_MIPI_DPHY_MRESETN	(1 << 2)


/**
 * struct infotm_platform_mipi_csi - platform data for INFOTM MIPI-CSI driver
 * @clk_rate:    bus clock frequency
 * @wclk_source: CSI wrapper clock selection: 0 - bus clock, 1 - ext. SCLK_CAM
 * @lanes:       number of data lanes used
 * @hs_settle:   HS-RX settle time
 */
struct infotm_platform_mipi_csi {
	unsigned long clk_rate;
	u8 wclk_source;
	u8 lanes;
	u8 hs_settle;
};

/**
 * infotm_csi_phy_enable - global MIPI-CSI receiver D-PHY control
 * @id:     MIPI-CSI harware instance index (0...1)
 * @on:     true to enable D-PHY and deassert its reset
 *          false to disable D-PHY
 * @return: 0 on success, or negative error code on failure
 */
int infotm_csi_phy_enable(int id, bool on);

#endif
