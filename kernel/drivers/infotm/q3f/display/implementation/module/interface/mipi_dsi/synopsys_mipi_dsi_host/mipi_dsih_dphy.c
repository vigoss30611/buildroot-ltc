/**
 * @file mipi_dsih_dphy.c
 * @brief D-PHY driver
 *
 *  Synopsys Inc.
 *  SG DWC PT02
 */
#include <linux/delay.h>
#include "mipi_dsih_dphy.h"

extern dsih_error_t mipi_dsih_wait_pll_lock(void);

/**
 * Initialise D-PHY module and power up
 * @param phy pointer to structure which holds information about the d-phy module
 * @return error code
 */
dsih_error_t mipi_dsih_dphy_open(dphy_t * phy)
{
	if (phy == 0)
	{
		return ERR_DSI_PHY_INVALID;
	}
	else if ((phy->core_read_function == 0) || (phy->core_write_function == 0))
	{
		return ERR_DSI_INVALID_IO;
	}
	else if (phy->status == INITIALIZED)
	{
		return ERR_DSI_PHY_INVALID;
	}
	phy->status = NOT_INITIALIZED;
	mipi_dsih_dphy_reset(phy, 0);
	mipi_dsih_dphy_reset(phy, 1);
	mipi_dsih_dphy_clock_en(phy, 1);
	mipi_dsih_dphy_shutdown(phy, 1);
	mipi_dsih_dphy_stop_wait_time(phy, 1);
	mipi_dsih_dphy_no_of_lanes(phy, 1);
	phy->status = INITIALIZED;
	return OK;
}
/**
 * Configure D-PHY module to desired operation mode
 * @param phy pointer to structure which holds information about the d-phy module
 * @param no_of_lanes active
 * @param output_freq desired high speed frequency
 * @return error code
 */
dsih_error_t mipi_dsih_dphy_configure(dphy_t * phy, uint8_t no_of_lanes, uint32_t output_freq)
{
	uint32_t divider = 0;
	uint8_t data[4]; /* maximum data for now are 4 bytes per test mode*/
	uint8_t no_of_bytes = 0;
	uint8_t i = 0;
	if (phy == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (phy->status < INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
#ifdef DWC_MIPI_DPHY_BIDIR_TSMC40LP
	/* for this computations, check databook page 59 */
	/* PLL input divider ratio is not computed since reference_freq < 40MHz */
	divider = (output_freq * 1000/ phy->reference_freq);
	divider /= 1000;
	divider -= 1;
	mipi_dsih_dphy_reset(phy, 1);
	mipi_dsih_dphy_clock_en(phy, 1);
	mipi_dsih_dphy_shutdown(phy, 1);
	no_of_bytes = 2; /* pll loop divider takes only 2 bytes (10 bits in data) */
	for (i = 0; i < no_of_bytes; i++)
	{
		data[i] = ((uint8_t)(divider >> (8 * i)) | (i << 7) );
		/* 7 is dependent on no_of_bytes
		(7 = sizeof(uint8_t) - min_needed_bits to write no_of_bytes)*/
	}
	/* test mode = PLL's loop divider ratio */
	mipi_dsih_dphy_write(phy, 0x18, data, no_of_bytes);

	mipi_dsih_dphy_no_of_lanes(phy, no_of_lanes);
	/*TODO - check */
	//mipi_dsih_dphy_if_control(phy, 1);
	//mipi_dsih_dphy_stop_wait_time(phy, 1);
#endif

/* Sam's */
#ifdef DWC_MIPI_DPHY_TSMC40LP_RTL
	mipi_dsih_dphy_reset(phy, 1);
	mipi_dsih_dphy_clock_en(phy, 1);
	mipi_dsih_dphy_shutdown(phy, 1);
	mipi_dsih_dphy_no_of_lanes(phy, no_of_lanes);

	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_RSTZ , 0x04);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x01);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);

	// congfigure the phy output clock
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x44);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x10044);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x02);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x44);
	if (output_freq > 750000){
		mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x74); // 999M bps
	}
	else if (output_freq > 600000){
		mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x12); // 750M bps
	}
	else if (output_freq > 480000){
		mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x2E); // 600M bps
		//mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x0E); // 549M bps
	}
	else if (output_freq > 240000){
		mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x2C); // 480M bps
	}
	else if (output_freq > 108000){
		mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x26); // 240M bps
	}
	else{
		mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x40); // 108M bps
	}
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x02);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);

#if 0
	//	configure clk lane clk polarity, for 7.85 mipi clk-lane P&N exchange
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x35);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x10035);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x02);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x35);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x01);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x02);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);
#endif

	// configure the HS prepare time. LP -> HS transition time.
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x71);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x10071);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x02);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x71);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL1, 0x94);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x02);
	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_TST_CRTL0, 0x00);

	mipi_dsih_dphy_write_word(phy, R_DSI_HOST_PHY_RSTZ, 0x07);
#endif


	mipi_dsih_wait_pll_lock();
	
	return OK;
}
/**
 * Close and power down D-PHY module
 * @param phy pointer to structure which holds information about the d-phy module
 * @return error code
 */
dsih_error_t mipi_dsih_dphy_close(dphy_t * phy)
{
	if (phy == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	else if ((phy->core_read_function == 0) || (phy->core_write_function == 0))
	{
		return ERR_DSI_INVALID_IO;
	}
	if (phy->status < NOT_INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	mipi_dsih_dphy_reset(phy, 0);
	mipi_dsih_dphy_reset(phy, 1);
	mipi_dsih_dphy_shutdown(phy, 0);
	phy->status = NOT_INITIALIZED;
	return OK;
}
/**
 * Enable clock lane module
 * @param instance pointer to structure which holds information about the d-phy module
 * @param en
 */
void mipi_dsih_dphy_clock_en(dphy_t * instance, int en)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_RSTZ, en, 2, 1);
}
/**
 * Reset D-PHY module
 * @param instance pointer to structure which holds information about the d-phy module
 * @param reset
 */
void mipi_dsih_dphy_reset(dphy_t * instance, int reset)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_RSTZ, reset, 1, 1);
}
/**
 * Power up/dpwn D-PHY module
 * @param instance pointer to structure which holds information about the d-phy module
 * @param powerup (1) shutdown (0)
 */
void mipi_dsih_dphy_shutdown(dphy_t * instance, int powerup)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_RSTZ, powerup, 0, 1);
}
/**
 * Configure minimum wait period for HS transmission request after a stop state
 * @param instance pointer to structure which holds information about the d-phy module
 * @param no_of_byte_cycles [in byte (lane) clock cycles]
 */
void mipi_dsih_dphy_stop_wait_time(dphy_t * instance, uint8_t no_of_byte_cycles)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_IF_CFG, no_of_byte_cycles, 2, 8);
}
/**
 * Set number of active lanes
 * @param instance pointer to structure which holds information about the d-phy module
 * @param no_of_lanes
 */
void mipi_dsih_dphy_no_of_lanes(dphy_t * instance, uint8_t no_of_lanes)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_IF_CFG, no_of_lanes - 1, 0, 2);
}
/**
 * Get number of currently active lanes
 * @param instance pointer to structure which holds information about the d-phy module
 * @return number of active lanes
 */
uint8_t mipi_dsih_dphy_get_no_of_lanes(dphy_t * instance)
{
	return mipi_dsih_dphy_read_part(instance, R_DSI_HOST_PHY_IF_CFG, 0, 2);
}
/**
 * D-PHY PPI interface control configuration
 * @param instance pointer to structure which holds information about the d-phy module
 * @param mask
 */
void mipi_dsih_dphy_if_control(dphy_t * instance, uint8_t mask)
{
	mipi_dsih_dphy_write_word(instance, R_DSI_HOST_PHY_IF_CTRL, mask);
}
/**
 * Get set D-PHY PPI interface control configurations
 * @param instance pointer to structure which holds information about the d-phy module
 * @param mask
 * @return value stored in register
 */
uint32_t mipi_dsih_dphy_get_if_control(dphy_t * instance, uint8_t mask)
{
	return (mipi_dsih_dphy_read_word(instance, R_DSI_HOST_PHY_IF_CTRL) & mask);
}
/**
 * Get D-PHY PPI status
 * @param instance pointer to structure which holds information about the d-phy module
 * @param mask
 * @return status
 */
uint32_t mipi_dsih_dphy_status(dphy_t * instance, uint16_t mask)
{
	return mipi_dsih_dphy_read_word(instance, R_DSI_HOST_PHY_STATUS) & mask;
}
/**
 * @param instance pointer to structure which holds information about the d-phy module
 * @param value
 */
void mipi_dsih_dphy_test_clock(dphy_t * instance, int value)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_TST_CRTL0, value, 1, 1);
}
/**
 * @param instance pointer to structure which holds information about the d-phy module
 * @param value
 */
void mipi_dsih_dphy_test_clear(dphy_t * instance, int value)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_TST_CRTL0, value, 0, 1);
}
/**
 * @param instance pointer to structure which holds information about the d-phy module
 * @param on_falling_edge
 */
void mipi_dsih_dphy_test_en(dphy_t * instance, uint8_t on_falling_edge)
{
	mipi_dsih_dphy_write_part(instance, R_DSI_HOST_PHY_TST_CRTL1, on_falling_edge, 16, 1);
}
/**
 * @param instance pointer to structure which holds information about the d-phy module
 */
uint8_t mipi_dsih_dphy_test_data_out(dphy_t * instance)
{
	return mipi_dsih_dphy_read_part(instance, R_DSI_HOST_PHY_TST_CRTL1, 8, 8);
}
/**
 * @param instance pointer to structure which holds information about the d-phy module
 * @param test_data
 */
void mipi_dsih_dphy_test_data_in(dphy_t * instance, uint8_t test_data)
{
	mipi_dsih_dphy_write_word(instance, R_DSI_HOST_PHY_TST_CRTL1, test_data);
}
/**
 * Write to D-PHY module (encapsulating the digital interface)
 * @param instance pointer to structure which holds information about the d-phy module
 * @param address offset inside the D-PHY digital interface
 * @param data array of bytes to be written to D-PHY
 * @param data_length of the data array
 */
void mipi_dsih_dphy_write(dphy_t * instance, uint8_t address, uint8_t * data, uint8_t data_length)
{
	unsigned i = 0;
	if (data != 0)
	{
#ifdef DWC_MIPI_DPHY_BIDIR_TSMC40LP
		mipi_dsih_dphy_reset(instance, 1);
		mipi_dsih_dphy_clock_en(instance, 1);
		mipi_dsih_dphy_shutdown(instance, 1);
		/* provide an initial active-high test clear pulse in TESTCLR  */
		mipi_dsih_dphy_test_clear(instance, 1);
		mipi_dsih_dphy_test_clear(instance, 0);
		/* clear the 8-bit input bus to 00h (code for normal operation) */
		mipi_dsih_dphy_test_data_in(instance, 0);
		/* set the TESTCLK input high in preparation to latch in the desired test mode */
		mipi_dsih_dphy_test_clock(instance, 1);
		/* set the desired test code in the input 8-bit bus TESTDIN[7:0] */
		mipi_dsih_dphy_test_data_in(instance, address);
		/* set TESTEN input high  */
		mipi_dsih_dphy_test_en(instance, 1);
		/* drive the TESTCLK input low; the falling edge captures the chosen test code into the transceiver */
		mipi_dsih_dphy_test_clock(instance, 0);
		/* set TESTEN input low to disable further test mode code latching  */
		mipi_dsih_dphy_test_en(instance, 0);
		/* to clear the data */
		mipi_dsih_dphy_test_data_in(instance, 0);
		/* start writing MSB first */
		for (i = data_length; i > 0 ; i--)
		{
			/* set TESTDIN[7:0] to the desired test data appropriate to the chosen test mode */
			mipi_dsih_dphy_test_data_in(instance, data[i - 1]);
			/* pulse TESTCLK high to capture this test data into the macrocell; repeat these two steps as necessary */
			mipi_dsih_dphy_test_clock(instance, 1);
			mipi_dsih_dphy_test_clock(instance, 0);
		}
#endif
	}
}



/* abstracting BSP */
/**
 * Write word to D-PHY module (encapsulating the bus interface)
 * @param instance pointer to structure which holds information about the d-phy module
 * @param reg_address offset
 * @param data 32-bit word
 */
void mipi_dsih_dphy_write_word(dphy_t * instance, uint32_t reg_address, uint32_t data)
{
	instance->core_write_function(instance->address, reg_address, data);
}
/**
 * Write bit field to D-PHY module (encapsulating the bus interface)
 * @param instance pointer to structure which holds information about the d-phy module
 * @param reg_address offset
 * @param data bits to be written to D-PHY
 * @param shift from the right hand side of the register (big endian)
 * @param width of the bit field
 */
void mipi_dsih_dphy_write_part(dphy_t * instance, uint32_t reg_address, uint32_t data, uint8_t shift, uint8_t width)
{
	uint32_t mask = (1 << width) - 1;
	uint32_t temp = mipi_dsih_dphy_read_word(instance, reg_address);
	temp &= ~(mask << shift);
	temp |= (data & mask) << shift;
	mipi_dsih_dphy_write_word(instance, reg_address, temp);
}
/**
 * Read word from D-PHY module (encapsulating the bus interface)
 * @param instance pointer to structure which holds information about the d-phy module
 * @param reg_address offset
 * @return data 32-bit word
 */
uint32_t mipi_dsih_dphy_read_word(dphy_t * instance, uint32_t reg_address)
{
	return instance->core_read_function(instance->address, reg_address);
}
/**
 * Read bit field from D-PHY module (encapsulating the bus interface)
 * @param instance pointer to structure which holds information about the d-phy module
 * @param reg_address offset
 * @param shift from the right hand side of the register (big endian)
 * @param width of the bit field
 * @return data bits to be written to D-PHY
 */
uint32_t mipi_dsih_dphy_read_part(dphy_t * instance, uint32_t reg_address, uint8_t shift, uint8_t width)
{
	return (mipi_dsih_dphy_read_word(instance, reg_address) >> shift) & ((1 << width) - 1);
}
