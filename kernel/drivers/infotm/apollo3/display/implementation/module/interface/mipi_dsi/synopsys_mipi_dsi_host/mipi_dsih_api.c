/**
 * @file mipi_dsih_api.c
 * @brief DWC MIPI DSI Host driver
 *
 *  Synopsys Inc.
 *  SG DWC PT02
 */
#include <linux/delay.h>
#include "mipi_dsih_api.h"
#include "mipi_dsih_hal.h"
#include "mipi_dsih_dphy.h"
/* whether to get debug messages (1) or not (0) */
#define DEBUG 					1

#define PRECISION_FACTOR 		1000
#define VIDEO_PACKET_OVERHEAD 	6
#define NULL_PACKET_OVERHEAD 	6
#define SHORT_PACKET			4
#define BLANKING_PACKET         6
static const uint32_t mipi_dsih_supported_versions[] = {0x3130312A};	// 0x3130302A ?
static const uint32_t mipi_dsih_no_of_versions = sizeof(mipi_dsih_supported_versions) / sizeof(uint32_t);

dsih_ctrl_t * ginstance;


/**
 * Open controller instance
 * - Check if driver is compatible with core version
 * - Check if struct is not NULL
 * - Bring up PHY for any transmissions in low power
 * - Bring up controller and perform initial configuration
 * @param instance pointer to structure holding the DSI Host core information
 * @return dsih_error_t
 * @note this function must be called before any other function in this API
 */
dsih_error_t mipi_dsih_open(dsih_ctrl_t * instance)
{
	dsih_error_t err = OK;
	uint32_t version = 0;
	int i = 0;

	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	else if ((instance->core_read_function == 0) || (instance->core_write_function == 0))
	{
		return ERR_DSI_INVALID_IO;
	}
/* sam
	else if (instance->status == INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	else if (mipi_dsih_dphy_open(&(instance->phy_instance)))
	{
		return ERR_DSI_PHY_INVALID;
	}
*/
	else
	{
		instance->status = NOT_INITIALIZED;
		version = mipi_dsih_hal_get_version(instance);
		for (i = 0; i < mipi_dsih_no_of_versions; i++)
		{
			if (version == mipi_dsih_supported_versions[i])
			{
				break;
			}
		}
		/* no matching supported version has been found*/
		if (i >= mipi_dsih_no_of_versions)
		{
			if (instance->log_info != 0)
			{
				instance->log_info("driver does not with this core version 0x%lX", version);
			}
			return ERR_DSI_CORE_INCOMPATIBLE;
		}
	}
	//mipi_dsih_hal_power(instance, 1);
	mipi_dsih_hal_dpi_color_mode_pol(instance, !instance->color_mode_polarity);
	mipi_dsih_hal_dpi_shut_down_pol(instance, !instance->shut_down_polarity);
	err = mipi_dsih_phy_hs2lp_config(instance, instance->max_hs_to_lp_cycles);
	err |= 	mipi_dsih_phy_lp2hs_config(instance, instance->max_lp_to_hs_cycles);
	err |= mipi_dsih_phy_bta_time(instance, instance->max_bta_cycles);
	if (err)
	{
		return ERR_DSI_OVERFLOW;
	}
	/* by default, return to LP during ALL, unless otherwise specified*/
	mipi_dsih_hal_dpi_lp_during_hfp(instance, 1);
	mipi_dsih_hal_dpi_lp_during_hbp(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vactive(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vfp(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vbp(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vsync(instance, 1);
	/* by default, all commands are sent in LP */
	mipi_dsih_hal_dcs_wr_tx_type(instance, 0, 1);
	mipi_dsih_hal_dcs_wr_tx_type(instance, 1, 1);
	mipi_dsih_hal_dcs_wr_tx_type(instance, 3, 1); /* long packet*/
	mipi_dsih_hal_dcs_rd_tx_type(instance, 0, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 0, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 1, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 2, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 3, 1); /* long packet*/
	mipi_dsih_hal_gen_rd_tx_type(instance, 0, 1);
	mipi_dsih_hal_gen_rd_tx_type(instance, 1, 1);
	mipi_dsih_hal_gen_rd_tx_type(instance, 2, 1);
	/* by default, RX_VC = 0, NO EOTp, EOTn, BTA, ECC rx and CRC rx */
	mipi_dsih_hal_gen_rd_vc(instance, 0);
	mipi_dsih_hal_gen_eotp_rx_en(instance, 0);
	mipi_dsih_hal_gen_eotp_tx_en(instance, 0);
	mipi_dsih_hal_bta_en(instance, 0);
	mipi_dsih_hal_gen_ecc_rx_en(instance, 0);
	mipi_dsih_hal_gen_crc_rx_en(instance, 0);
	instance->status = INITIALIZED;

	//sam : 
	mipi_dsih_ecc_rx(instance ,1);
	mipi_dsih_peripheral_ack(instance, 1); 
	mipi_dsih_tear_effect_ack(instance, 1);
	mipi_dsih_hal_max_rd_size_tx_type(instance, 1);
	
	ginstance = instance;
	
	return OK;
}

/**
 * Close DSI Host driver
 * - Free up resources and shutdown host
 * @param instance pointer to structure holding the DSI Host core information
 * @return dsih_error_t
 */
dsih_error_t mipi_dsih_close(dsih_ctrl_t * instance)
{
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}

	mipi_dsih_hal_power(instance, 0);
	return OK;
}
/**
 * Enable return to low power mode inside video periods when timing allows
 * @param instance pointer to structure holding the DSI Host core information
 * @param hfp allow to return to lp inside horizontal front porch pixels
 * @param hbp allow to return to lp inside horizontal back porch pixels
 * @param vactive allow to return to lp inside vertical active lines
 * @param vfp  allow to return to lp inside vertical front porch lines
 * @param vbp allow to return to lp inside vertical back porch lines
 * @param vsync allow to return to lp inside vertical sync lines
 */
void mipi_dsih_allow_return_to_lp(dsih_ctrl_t * instance, int hfp, int hbp, int vactive, int vfp, int vbp, int vsync)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_dpi_lp_during_hfp(instance, hfp);
			mipi_dsih_hal_dpi_lp_during_hbp(instance, hbp);
			mipi_dsih_hal_dpi_lp_during_vactive(instance, vactive);
			mipi_dsih_hal_dpi_lp_during_vfp(instance, vfp);
			mipi_dsih_hal_dpi_lp_during_vbp(instance, vbp);
			mipi_dsih_hal_dpi_lp_during_vsync(instance, vsync);
			return;
		}
	}
	if (instance->log_error != 0)
	{
		instance->log_error("invalid instance");
	}
}
/**
 * Set DCS command packet transmission to low power
 * @param instance pointer to structure holding the DSI Host core information
 * @param long_write command packets
 * @param short_write command packets with none and one parameters
 * @param short_read command packets with none parameters
 */
void mipi_dsih_dcs_cmd_lp_transmission(dsih_ctrl_t * instance, int long_write, int short_write, int short_read)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_dcs_wr_tx_type(instance, 0, short_write);
			mipi_dsih_hal_dcs_wr_tx_type(instance, 1, short_write);
			mipi_dsih_hal_dcs_wr_tx_type(instance, 3, long_write); /* long packet*/
			mipi_dsih_hal_dcs_rd_tx_type(instance, 0, short_read);
			return;
		}
	}
	if (instance->log_error != 0)
	{
		instance->log_error("invalid instance");
	}
}
/**
 * Set Generic command packet transmission to low power
 * @param instance pointer to structure holding the DSI Host core information
 * @param long_write command packets
 * @param short_write command packets with none, one and two parameters
 * @param short_read command packets with none, one and two parameters
 */
void mipi_dsih_gen_cmd_lp_transmission(dsih_ctrl_t * instance, int long_write, int short_write, int short_read)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_gen_wr_tx_type(instance, 0, short_write);
			mipi_dsih_hal_gen_wr_tx_type(instance, 1, short_write);
			mipi_dsih_hal_gen_wr_tx_type(instance, 2, short_write);
			mipi_dsih_hal_gen_wr_tx_type(instance, 3, long_write); /* long packet*/
			mipi_dsih_hal_gen_rd_tx_type(instance, 0, short_read);
			mipi_dsih_hal_gen_rd_tx_type(instance, 1, short_read);
			mipi_dsih_hal_gen_rd_tx_type(instance, 2, short_read);
			return;
		}
	}
	if (instance->log_error != 0)
	{
		instance->log_error("invalid instance");
	}
}

/* packet handling */
/**
 *  Enable all receiving activities (applying a Bus Turn Around).
 *  - Disabling using this function will stop all acknowledges by the
 *  peripherals and no interrupts from low-level protocol error reporting
 *  will be able to rise.
 *  - Enabling any receiving function (command or frame acks, ecc,
 *  tear effect ack or EoTp receiving) will enable this automatically,
 *  but it must be EXPLICITLY be disabled to disabled all kinds of
 *  receiving functionality.
 * @param instance pointer to structure holding the DSI Host core information
 * @param enable or disable
 * @return error code
 */
dsih_error_t mipi_dsih_enable_rx(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_hal_bta_en(instance, enable);
	return OK;
}
/**
 *  Enable command packet acknowledges by the peripherals
 *  - For interrupts to rise the monitored event must be registered
 *  using the event_register function
 * @param instance pointer to structure holding the DSI Host core information
 * @param enable or disable
 * @return error code
 */
dsih_error_t mipi_dsih_peripheral_ack(dsih_ctrl_t * instance, int enable)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_cmd_ack_en(instance, enable);
			if (enable)
			{
				mipi_dsih_hal_bta_en(instance, 1);
			}
			return OK;
		}
	}
	return ERR_DSI_INVALID_INSTANCE;
}
/**
 * Enable tearing effect acknowledges by the peripherals (wait for TE)
 * - It enables the following from the DSI specification
 * "Since the timing of a TE event is, by definition, unknown to the host
 * processor, the host processor shall give bus possession to the display
 * module and then wait for up to one video frame period for the TE response.
 * During this time, the host processor cannot send new commands, or requests
 * to the display module, because it does not have bus possession."
 * @param instance pointer to structure holding the DSI Host core information
 * @param enable or disable
 * @return error code
 */
dsih_error_t mipi_dsih_tear_effect_ack(dsih_ctrl_t * instance, int enable)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_tear_effect_ack_en(instance, enable);
			if (enable)
			{
				mipi_dsih_hal_bta_en(instance, 1);
			}
			return OK;
		}
	}
	return ERR_DSI_INVALID_INSTANCE;
}
/**
 * Enable the receiving of EoT packets at the end of LS transmission.
 * @param instance pointer to structure holding the DSI Host core information
 * @param enable or disable
 * @return error code
 */
dsih_error_t mipi_dsih_eotp_rx(dsih_ctrl_t * instance, int enable)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_gen_eotp_rx_en(instance, enable);
			if (enable)
			{
				mipi_dsih_hal_bta_en(instance, 1);
			}
			return OK;
		}
	}
	return ERR_DSI_INVALID_INSTANCE;
}
/**
 * Enable the listening to ECC bytes. This allows for recovering from
 * 1 bit errors. To report ECC events, the ECC events should be registered
 * @param instance pointer to structure holding the DSI Host core information
 * @param enable or disable
 * @return error code
 */
dsih_error_t mipi_dsih_ecc_rx(dsih_ctrl_t * instance, int enable)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_gen_ecc_rx_en(instance, enable);
			if (enable)
			{
				mipi_dsih_hal_bta_en(instance, 1);
			}
			return OK;
		}
	}
	return ERR_DSI_INVALID_INSTANCE;
}

/**
 * Enable the sending of EoT packets at the end of HS transmission. It was made
 * optional in the DSI spec. for retro-compatibility.
 * @param instance pointer to structure holding the DSI Host core information
 * @param enable or disable
 * @return error code
 */
dsih_error_t mipi_dsih_eotp_tx(dsih_ctrl_t * instance, int enable)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			mipi_dsih_hal_gen_eotp_tx_en(instance, enable);
		}
	}
	return ERR_DSI_INVALID_INSTANCE;
}

/**
 * Wait for D-PHY PLL Lock
 * @return error code
 */
dsih_error_t mipi_dsih_wait_pll_lock(void)
{
	int i;
	int cIdle;
	dsih_ctrl_t * instance = ginstance;
	
	for(i = 0 ; i < 10000; i ++)
	{
		if ((i % 100) == 0)
		{
			cIdle = mipi_dsih_read_part(instance, R_DSI_HOST_PHY_STATUS , 0, 1);// polling PHY LOCK
			if(cIdle)	// LOCK
			{
				if (instance->log_info != 0)
				{
					instance->log_info("D-PHY PLL spend %d ms + %d circle to lock", i/100, i%100);
				}
				break;
			}
			mdelay(1);
		}
	}
	cIdle = mipi_dsih_read_part(instance, R_DSI_HOST_PHY_STATUS , 0, 1);
	if (!cIdle){
		if (instance->log_error != 0)
		{
			instance->log_error("MIPI D-PHY NOT Lock");
		}
		return ERR_DSI_PHY_INVALID;
	}

	for (i = 0; i < 10000; i ++)
	{
		if ((i % 100) == 0){
			cIdle = mipi_dsih_read_part(instance, R_DSI_HOST_PHY_STATUS , 4, 1);	// polling STOP State of Clock Lane
			if(cIdle)	// Stop State
			{
				if (instance->log_info != 0)
				{
					instance->log_info("D-PHY PLL spend %d ms + %d circle to get in STOP State", i/100, i%100);
				}
				break;
			}
			mdelay(1);
		}
	}
	cIdle = mipi_dsih_read_part(instance, R_DSI_HOST_PHY_STATUS , 4, 1);
	if (!cIdle){
		if (instance->log_error != 0)
		{
			instance->log_error("MIPI D-PHY NOT in STOP State");
		}
		return ERR_DSI_PHY_INVALID;
	}

	return OK;
}

/**
 * Configure DPI video interface
 * - If not in burst mode, it will compute the video and null packet sizes
 * according to necessity
 * @param instance pointer to structure holding the DSI Host core information
 * @param video_params pointer to video stream-to-send information
 * @return error code
 */
dsih_error_t mipi_dsih_dpi_video(dsih_ctrl_t * instance, dsih_dpi_video_t * video_params)
{
	dsih_error_t err_code = OK;
	uint16_t bytes_per_pixel_x100 = 0;
	uint16_t video_size = 0;
	uint32_t total_line_bytes = 0;
	uint32_t no_of_chunks = 0;
	uint32_t ratio_clock_xPF = 0; /* holds dpi clock/byte clock times precision factor */
	uint16_t null_packet_size = 0;
	uint8_t video_size_step = 1;
	uint32_t hs_timeout  = 0;
	int counter = 0;

	if ((instance == 0) || (video_params == 0))
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	/* if (mipi_dsih_dphy_configure(&(instance->phy_instance), video_params->no_of_lanes, video_params->byte_clock * 8))
	{
		return ERR_DSI_PHY_POWERUP;
	} */ // sam
	ratio_clock_xPF = (video_params->byte_clock * PRECISION_FACTOR ) / (video_params->pixel_clock); /* * video_params->no_of_lanes */
	video_size = video_params->h_active_pixels;

	mipi_dsih_hal_dpi_frame_ack_en(instance, video_params->receive_ack_packets);
	if (video_params->receive_ack_packets)
	{ /* if ACK is requested, enable BTA, otherwise leave as is */
		mipi_dsih_hal_bta_en(instance, 1);
	}
	mipi_dsih_hal_gen_cmd_mode_en(instance, 0);
	mipi_dsih_hal_dpi_video_mode_en(instance, 1);

	switch (video_params->color_coding)
	{
		case COLOR_CODE_16BIT_CONFIG1:
			bytes_per_pixel_x100 = 200;
			break;
		case COLOR_CODE_16BIT_CONFIG2:
			bytes_per_pixel_x100 = 200;
			break;
		case COLOR_CODE_16BIT_CONFIG3:
			bytes_per_pixel_x100 = 200;
			break;
		case COLOR_CODE_18BIT_CONFIG1:
			mipi_dsih_hal_dpi_18_loosely_packet_en(instance, video_params->is_18_loosely);
			bytes_per_pixel_x100 = 225;
			if (!video_params->is_18_loosely)
			{ /* 18bits per pixel and NOT loosely, packets should be multiples of 4 */
				video_size_step = 4;
			}
			break;
		case COLOR_CODE_18BIT_CONFIG2:
			mipi_dsih_hal_dpi_18_loosely_packet_en(instance, video_params->is_18_loosely);
			bytes_per_pixel_x100 = 225;
			if (!video_params->is_18_loosely)
			{ /* 18bits per pixel and NOT loosely, packets should be multiples of 4 */
				video_size_step = 4;
			}
			break;
		case COLOR_CODE_24BIT:
			bytes_per_pixel_x100 = 300;
			break;
		default:
			if (instance->log_error != 0)
			{
				instance->log_error("invalid color coding");
			}
			err_code = ERR_DSI_COLOR_CODING;
			break;
	}
	if (err_code == OK)
	{
		err_code = mipi_dsih_hal_dpi_color_coding(instance, video_params->color_coding);
	}
	if (err_code != OK)
	{
		return err_code;
	}
	mipi_dsih_hal_dpi_video_mode_type(instance, video_params->video_mode);
	mipi_dsih_hal_dpi_hline(instance, (uint16_t)((video_params->h_total_pixels * ratio_clock_xPF) / PRECISION_FACTOR));
	mipi_dsih_hal_dpi_hbp(instance, ((video_params->h_back_porch_pixels * ratio_clock_xPF) / PRECISION_FACTOR));
	mipi_dsih_hal_dpi_hsa(instance, ((video_params->h_sync_pixels * ratio_clock_xPF) / PRECISION_FACTOR));
	mipi_dsih_hal_dpi_vactive(instance, video_params->v_active_lines);
	mipi_dsih_hal_dpi_vfp(instance, video_params->v_total_lines - (video_params->v_back_porch_lines + video_params->v_sync_lines + video_params->v_active_lines));
	mipi_dsih_hal_dpi_vbp(instance, video_params->v_back_porch_lines);
	mipi_dsih_hal_dpi_vsync(instance, video_params->v_sync_lines);
	mipi_dsih_hal_dpi_hsync_pol(instance, !video_params->h_polarity);
	mipi_dsih_hal_dpi_vsync_pol(instance, !video_params->v_polarity);
	mipi_dsih_hal_dpi_dataen_pol(instance, !video_params->data_en_polarity);
#if 1
	// sam
		mipi_dsih_hal_timeout_clock_division(instance, 1);
		mipi_dsih_hal_tx_escape_timeout(instance, 0x05); // fpga : 0x02; rtl : 0x0C
		//timeout transmission or reception. may be used in the event test.
#if 0	
		mipi_dsih_hal_lp_rx_timeout(instance, (IM_UINT16)0x3); // For rtl interrupt test
		mipi_dsih_hal_hs_tx_timeout(instance, (IM_UINT16)0x19); // For rtl interrupt test
#else
		mipi_dsih_hal_lp_rx_timeout(instance, (u16)0x1FFF); // For rtl test need
		mipi_dsih_hal_hs_tx_timeout(instance, (u16)0x1FFF); // For rtl test need
#endif
#else
	/* HS timeout */
	hs_timeout = ((video_params->h_total_pixels * video_params->v_active_lines) + (DSIH_PIXEL_TOLERANCE * bytes_per_pixel_x100) / 100);
	for (counter = 0xff; (counter < hs_timeout) && (counter > 2); counter--)
	{
		if ((hs_timeout % counter) == 0)
		{
			mipi_dsih_hal_timeout_clock_division(instance, counter);
			mipi_dsih_hal_lp_rx_timeout(instance, (uint16_t)(hs_timeout / counter));
			mipi_dsih_hal_hs_tx_timeout(instance, (uint16_t)(hs_timeout / counter));
			break;
		}
	}

	/* TX_ESC_CLOCK_DIV must be around 20MHz */
	for (counter = 1; ((video_params->byte_clock * PRECISION_FACTOR) / counter) > (20 * PRECISION_FACTOR); counter++)
		;
	if (((20 * PRECISION_FACTOR) - ((video_params->byte_clock * PRECISION_FACTOR) / counter)) < (((video_params->byte_clock * PRECISION_FACTOR) / (counter  - 1)) - (20 * PRECISION_FACTOR)))
	{
		mipi_dsih_hal_tx_escape_timeout(instance, counter);
	}
	else
	{
		mipi_dsih_hal_tx_escape_timeout(instance, counter - 1);
	}
#endif
	/* video packetisation */
	if (video_params->video_mode == VIDEO_BURST_WITH_SYNC_PULSES)
	{ /* BURST */
		mipi_dsih_hal_dpi_null_packet_en(instance, 0);
		mipi_dsih_hal_dpi_multi_packet_en(instance, 0);
		err_code = mipi_dsih_hal_dpi_null_packet_size(instance, 0);
		err_code |= mipi_dsih_hal_dpi_chunks_no(instance, 1);
		err_code |= mipi_dsih_hal_dpi_video_packet_size(instance, video_size);
		if (err_code != OK)
		{
			return err_code;
		}
		/* BURST by default, returns to LP during ALL empty periods - eneregy saving */
		mipi_dsih_hal_dpi_lp_during_hfp(instance, 1);
		mipi_dsih_hal_dpi_lp_during_hbp(instance, 1);
		mipi_dsih_hal_dpi_lp_during_vactive(instance, 1);
		mipi_dsih_hal_dpi_lp_during_vfp(instance, 1);
		mipi_dsih_hal_dpi_lp_during_vbp(instance, 1);
		mipi_dsih_hal_dpi_lp_during_vsync(instance, 1);
	}
	else
	{ /* NON BURST */
		if ((ratio_clock_xPF % PRECISION_FACTOR) > 1)
		{/* MULTI */
			mipi_dsih_hal_dpi_multi_packet_en(instance, 1);

			for (video_size = 1; (((video_size * ratio_clock_xPF / video_params->no_of_lanes) % PRECISION_FACTOR) != 0) && (video_size <= video_params->h_active_pixels); video_size += video_size_step)
				;

			if (((video_size * ratio_clock_xPF / video_params->no_of_lanes) % PRECISION_FACTOR) != 0)
			{ /* may get a negative overhead */
				for (video_size = 1; (((video_size * ratio_clock_xPF / video_params->no_of_lanes) % PRECISION_FACTOR) < (DSIH_PIXEL_TOLERANCE * bytes_per_pixel_x100) / 100) && (video_size <= video_params->h_active_pixels); video_size += video_size_step)
					;
			}

			no_of_chunks = video_params->h_active_pixels / video_size;
			total_line_bytes = ((video_params->h_total_pixels - video_params->h_back_porch_pixels - video_params->h_sync_pixels) * ratio_clock_xPF / PRECISION_FACTOR) * video_params->no_of_lanes;
			for (null_packet_size = 0; ((no_of_chunks * (((video_size * bytes_per_pixel_x100) / 100) + (NULL_PACKET_OVERHEAD + VIDEO_PACKET_OVERHEAD) + null_packet_size)) - 4) <= (total_line_bytes- SHORT_PACKET - BLANKING_PACKET); null_packet_size++)
				;
			if (null_packet_size == 0)
			{   /* No NULL packets needed */
				if (instance->log_info != 0)
				{
					instance->log_info("no NULL packets needed");
				}
				mipi_dsih_hal_dpi_null_packet_en(instance, 0);
				mipi_dsih_hal_dpi_null_packet_size(instance, 0);
			}
			else
			{
				null_packet_size--;
				if (instance->log_info != 0)
				{
					instance->log_info("NULL packets added");
				}
				mipi_dsih_hal_dpi_null_packet_en(instance, 1);
				err_code = mipi_dsih_hal_dpi_null_packet_size(instance, null_packet_size);
				if (err_code != OK)
				{
					instance->log_info("null_packet_size %ld", null_packet_size);
					return err_code;
				}
			}
#if DEBUG
			/*      D E B U G 		*/
			if (instance->log_info != 0)
			{
				instance->log_info("h line time %ld", (uint16_t)((video_params->h_total_pixels * ratio_clock_xPF) / PRECISION_FACTOR));
				instance->log_info("h active time %ld", (video_params->h_active_pixels * ratio_clock_xPF) / PRECISION_FACTOR);
				instance->log_info("h b p time %ld", (video_params->h_back_porch_pixels * ratio_clock_xPF) / PRECISION_FACTOR);
				instance->log_info("h sync time %ld", (video_params->h_sync_pixels * ratio_clock_xPF) / PRECISION_FACTOR);
				instance->log_info("total_line_bytes %ld", total_line_bytes);
				instance->log_info("video_size %ld", video_size);
				instance->log_info("no_of_chunks %ld", no_of_chunks);
				instance->log_info("null_packet_size %ld", null_packet_size);
			}
			/***********************/
#endif
			err_code = mipi_dsih_hal_dpi_video_packet_size(instance, video_size);
			err_code = mipi_dsih_hal_dpi_chunks_no(instance, no_of_chunks);
		}
		else
		{ /* NO MULTI NO NULL */
#if DEBUG
			/*      D E B U G 		*/
			if (instance->log_info != 0)
			{
				instance->log_info("no multi no null");
				instance->log_info("h line time %ld", (uint16_t)((video_params->h_total_pixels * ratio_clock_xPF) / PRECISION_FACTOR));
				instance->log_info("video_size %ld", video_size);
			}
#endif
			mipi_dsih_hal_dpi_null_packet_en(instance, 0);
			mipi_dsih_hal_dpi_multi_packet_en(instance, 0);
			err_code = mipi_dsih_hal_dpi_chunks_no(instance, 1);
			if ((!video_params->is_18_loosely) && ((video_params->color_coding == COLOR_CODE_18BIT_CONFIG2) || (video_params->color_coding == COLOR_CODE_18BIT_CONFIG1)))
			{	/* video size must be a multiple of 4 when not 18 loosely */
				for (video_size = video_params->h_active_pixels; (video_size % 4) != 0; video_size++)
					;
			}
			err_code |= mipi_dsih_hal_dpi_video_packet_size(instance, video_size);
		}
		if (err_code != OK)
		{
			return err_code;
		}
	}
	mipi_dsih_hal_dpi_video_vc(instance, video_params->virtual_channel);
	mipi_dsih_dphy_no_of_lanes(&(instance->phy_instance), video_params->no_of_lanes);
	return err_code;
}
/**
 * Send a DCS write command
 * @param instance pointer to structure holding the DSI Host core information
 * @param vc destination virtual channel
 * @param params byte array of command parameters, including the
 * command itself
 * @param param_length length of the above array
 * @return error code
 */
dsih_error_t mipi_dsih_dcs_wr_cmd(dsih_ctrl_t * instance, uint8_t vc, uint8_t* params, uint16_t param_length)
{
	uint8_t packet_type = 0;
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (params == 0)
	{
		return ERR_DSI_OUT_OF_BOUND;
	}
	switch (params[0])
	{
		case 0x39:
		case 0x38:
		case 0x34:
		case 0x29:
		case 0x28:
		case 0x21:
		case 0x20:
		case 0x13:
		case 0x12:
		case 0x11:
		case 0x10:
		case 0x01:
		case 0x00:
			packet_type = 0x05; /* DCS short write no param */
			break;
		case 0x3A:
		case 0x36:
		case 0x35:
		case 0x26:
			packet_type = 0x15; /* DCS short write 1 param */
			break;
		case 0x44:
		case 0x3C:
		case 0x37:
		case 0x33:
		case 0x30:
		case 0x2D:
		case 0x2C:
		case 0x2B:
		case 0x2A:
			packet_type = 0x39; /* DCS long write/write_LUT command packet */
			break;
		default:
			if (instance->log_error != 0)
			{
				instance->log_error("invalid DCS command");
			}
			return ERR_DSI_INVALID_COMMAND;
	}
	return mipi_dsih_gen_wr_cmd(instance, vc, packet_type, params, param_length);
}
/**
 * Enable command mode
 * @param instance pointer to structure holding the DSI Host core information
 * @param en enable/disable
 */
void mipi_dsih_cmd_mode(dsih_ctrl_t * instance, int en)
{
	if (instance != 0)
	{
		if (instance->status == INITIALIZED)
		{
			if (!mipi_dsih_hal_gen_is_cmd_mode(instance))
			{
				mipi_dsih_hal_dpi_video_mode_en(instance, 0);
				mipi_dsih_hal_gen_cmd_mode_en(instance, 1);
			}
			return;
		}
	}
	if (instance->log_error != 0)
	{
		instance->log_error("invalid instance");
	}
}
/**
 * Send a generic write command
 * @param instance pointer to structure holding the DSI Host core information
 * @param vc destination virtual channel
 * @param command_type type of command, inserted in first byte of header
 * @param params byte array of command parameters
 * @param param_length length of the above array
 * @return error code
 * @note this function has an active delay to wait for the buffer to clear.
 * The delay is limited to DSIH_FIFO_ACTIVE_WAIT
 */
dsih_error_t mipi_dsih_gen_wr_cmd(dsih_ctrl_t * instance, uint8_t vc, uint8_t command_type, uint8_t* params, uint16_t param_length)
{
	dsih_error_t err_code = OK;
	int timeout = 0;
	int i = 0;
	int j = 0;
	int compliment_counter = 0;
	uint8_t* payload = 0;
	uint32_t temp = 0;
	uint16_t word_count = 0;
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (params == 0) /* NULL */
	{
		return ERR_DSI_OUT_OF_BOUND;
	}
	mipi_dsih_cmd_mode(instance, 1);
	if (param_length > 2)
	{
		/* long packet - write the whole command to payload */
		payload = params + (2 * sizeof(uint8_t));
		word_count = (params[1] << 8) | params[0];
		if ((param_length - 2) < word_count)
		{
			if (instance->log_error != 0)
			{
				instance->log_error("sent > input payload. complemented with zeroes");
			}
			compliment_counter = (param_length - 2) - word_count;
		}
		if ((param_length - 2) > word_count)
		{
			if (instance->log_error != 0)
			{
				instance->log_error("Overflow - input > sent. payload truncated");
			}
		}
		for (i = 0; i < (param_length - 2); i += j)
		{
			temp = 0;
			for (j = 0; (j < 4) && ((j + i) < (param_length - 2)); j++)
			{
				/* temp = (payload[i + 3] << 24) | (payload[i + 2] << 16) | (payload[i + 1] << 8) | payload[i]; */
				temp |= payload[i + j] << (j * 8);
			}
			/* check if payload Tx fifo is not full */
			for (timeout = 0; timeout < DSIH_FIFO_ACTIVE_WAIT ; timeout++)
			{
				if (!mipi_dsih_hal_gen_packet_payload(instance, temp))
				{
					break;
				}
			}
			if (!(timeout < DSIH_FIFO_ACTIVE_WAIT))
			{
				return ERR_DSI_TIMEOUT;
			}
		}
		/* if word count entered by the user more than actual parameters received
		 * fill with zeroes - a fail safe mechanism, otherwise controller will
		 * want to send data from an empty buffer */
		for (i = 0; i < compliment_counter; i++)
		{
			/* check if payload Tx fifo is not full */
			for (timeout = 0; timeout < DSIH_FIFO_ACTIVE_WAIT ; timeout++)
			{
				if (!mipi_dsih_hal_gen_packet_payload(instance, 0x00))
				{
					break;
				}
			}
			if (!(timeout < DSIH_FIFO_ACTIVE_WAIT))
			{
				return ERR_DSI_TIMEOUT;
			}
		}
	}

	for (timeout = 0; timeout < DSIH_FIFO_ACTIVE_WAIT; timeout++)
	{
		/* check if payload Tx fifo is not full */
		if (!mipi_dsih_hal_gen_cmd_fifo_full(instance))
		{
			if (param_length == 0)
			{
				err_code |= mipi_dsih_hal_gen_packet_header(instance, vc, command_type, 0x0, 0x0);
			}
			else if (param_length == 1)
			{
				err_code |= mipi_dsih_hal_gen_packet_header(instance, vc, command_type, 0x0, params[0]);
			}
			else
			{
				err_code |= mipi_dsih_hal_gen_packet_header(instance, vc, command_type, params[1], params[0]);
			}
			break;
		}
	}
	if (!(timeout < DSIH_FIFO_ACTIVE_WAIT))
	{
		err_code = ERR_DSI_TIMEOUT;
	}
	return err_code;
}
/**
 * Send a DCS READ command to peripheral
 * @param instance pointer to structure holding the DSI Host core information
 * @param vc destination virtual channel
 * @param command DCS code
 * @param bytes_to_read no of bytes to read (expected to arrive at buffer)
 * @param read_buffer pointer to 32-bit array to hold the read buffer words
 * return read_buffer_length
 * @note this function has an active delay to wait for the buffer to clear.
 * The delay is limited to 2 x DSIH_FIFO_ACTIVE_WAIT
 * (waiting for command buffer, and waiting for receiving)
 * @note this function will enable BTA
 */
uint16_t mipi_dsih_dcs_rd_cmd(dsih_ctrl_t * instance, uint8_t vc, uint8_t command, uint8_t bytes_to_read, uint8_t* read_buffer)
{
	if (instance == 0)
	{
		return 0;
	}
	if (instance->status != INITIALIZED)
	{
		return 0;
	}
	/* COMMAND_TYPE 0x06 - DCS Read no params refer to DSI spec p.47 */
	switch (command)
	{
		case 0xA8:
		case 0xA1:
		case 0x45:
		case 0x3E:
		case 0x2E:
		case 0x0F:
		case 0x0E:
		case 0x0D:
		case 0x0C:
		case 0x0B:
		case 0x0A:
		case 0x08:
		case 0x07:
		case 0x06:
			return mipi_dsih_gen_rd_cmd(instance, vc, 0x06, 0x0, command, bytes_to_read, read_buffer);
		default:
			if (instance->log_error != 0)
			{
				instance->log_error("invalid DCS command");
			}
			return 0;
	}
	return 0;
}
/**
 * Send Generic READ command to peripheral
 * @param instance pointer to structure holding the DSI Host core information
 * @param vc destination virtual channel
 * @param command generic command type
 * @param lsb_byte first command parameter
 * @param msb_byte second command parameter
 * @param bytes_to_read no of bytes to read (expected to arrive at buffer)
 * @param read_buffer pointer to 32-bit array to hold the read buffer words
 * return read_buffer_length
 * @note this function has an active delay to wait for the buffer to clear.
 * The delay is limited to 2 x DSIH_FIFO_ACTIVE_WAIT
 * (waiting for command buffer, and waiting for receiving)
 * @note this function will enable BTA
 */
uint16_t mipi_dsih_gen_rd_cmd(dsih_ctrl_t * instance, uint8_t vc, uint8_t command, uint8_t msb_byte, uint8_t lsb_byte, uint8_t bytes_to_read, uint8_t* read_buffer)
{
	dsih_error_t err_code = OK;
	int timeout = 0;
	int counter = 0;
	int i = 0;
	uint32_t temp[1] = {0};
	if (instance == 0)
	{
		return 0;
	}
	if (instance->status != INITIALIZED)
	{
		return 0;
	}
	if (bytes_to_read < 1)
	{
		return 0;
	}
	if (read_buffer == 0)
	{
		return 0;
	}
	/* make sure command mode is on */
	mipi_dsih_cmd_mode(instance, 1);
	/* make sure receiving is enabled */
	mipi_dsih_hal_bta_en(instance, 1);
	/* listen to the same virtual channel as the one sent to */
	mipi_dsih_hal_gen_rd_vc(instance, vc);
	for (timeout = 0; timeout < DSIH_FIFO_ACTIVE_WAIT; timeout++)
	{/* check if payload Tx fifo is not full */
		if (!mipi_dsih_hal_gen_cmd_fifo_full(instance))
		{
			mipi_dsih_hal_gen_packet_header(instance, vc, command, msb_byte, lsb_byte);
			break;
		}
	}
	if (!(timeout < DSIH_FIFO_ACTIVE_WAIT))
	{
		err_code = 0;
	}
	/* loop for the number of words to be read */
	for (timeout = 0; timeout < DSIH_FIFO_ACTIVE_WAIT; timeout++)
	{	/* check if command transaction is done */
		if (!mipi_dsih_hal_gen_rd_cmd_busy(instance))
		{
			for (counter = 0; counter < bytes_to_read; counter += 4)
			{
				err_code = mipi_dsih_hal_gen_read_payload(instance, temp);
				if (err_code)
				{
					return 0;
				}
				for (i = 0; (i < 4) && ((counter + i) < bytes_to_read); i++)
				{	/* put 32 bit temp in 4 bytes of buffer passed by user*/
					read_buffer[counter + i] = (uint8_t)(temp[0] >> (i * 8));
				}
			}
		}
	}

	return bytes_to_read;
}
/**
 * Dump values stored in the DSI host core registers
 * @param instance pointer to structure holding the DSI Host core information
 * @param all whether to dump all the registers, no register_config array need
 * be provided if dump is to standard IO
 * @param config array of register_config_t type where addresses and values are
 * stored
 * @param config_length the length of the config array
 * @return the number of the registers that were read
 */
uint32_t mipi_dsih_dump_register_configuration(dsih_ctrl_t * instance, int all, register_config_t *config, uint16_t config_length)
{
	uint32_t current = 0;
	uint16_t  count = 0;
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (all)
	{ /* dump all registers*/
		for (current = R_DSI_HOST_VERSION; current <= R_DSI_HOST_ERROR_MSK1; count++, current += (R_DSI_HOST_PWR_UP - R_DSI_HOST_VERSION))
		{
			if ((config_length == 0) || (config == 0) || count >= config_length)
			{ /* no place to write - write to STD IO*/
				if (instance->log_info != 0)
				{
					instance->log_info("DSI 0x%lX:0x%lX", current, mipi_dsih_read_word(instance, current));
				}
			}
			else
			{
				config[count].addr = current;
				config[count].data = mipi_dsih_read_word(instance, current);
			}
		}
	}
	else
	{
		if(config == 0)
		{
			if (instance->log_error != 0)
			{
				instance->log_error("invalid DCS command");
			}
		}
		else
		{
			for (count = 0; count < config_length; count++)
			{
				config[count].data = mipi_dsih_read_word(instance, config[count].addr);
			}
		}
	}
	return count;
}
/**
 * Write values to DSI host core registers
 * @param instance pointer to structure holding the DSI Host core information
 * @param config array of register_config_t type where register addresses and
 * their new values are stored
 * @param config_length the length of the config array
 * @return the number of the registers that were written to
 */
uint32_t mipi_dsih_write_register_configuration(dsih_ctrl_t * instance, register_config_t *config, uint16_t config_length)
{
	uint16_t  count = 0;
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	for (count = 0; count < config_length; count++)
	{
		mipi_dsih_write_word(instance, config[count].addr, config[count].data);
	}
	return count;
}
/**
 * Register a handler for a specific event
 * - The handler will be  called when this specific event occurs
 * @param instance pointer to structure holding the DSI Host core information
 * @param event enum
 * @param handler call back to handler function to handle the event
 * @return error code
 */
dsih_error_t mipi_dsih_register_event(dsih_ctrl_t * instance, dsih_event_t event, void (*handler)(void*))
{
	uint32_t mask = 1;
	uint32_t temp = 0;
	if (event > DUMMY)
	{
		return ERR_DSI_INVALID_EVENT;
	}
	if (handler == 0)
	{
		return ERR_DSI_INVALID_HANDLE;
	}
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	instance->event_registry[event] =  handler;
	if (event < HS_CONTENTION)
	{
		temp = mipi_dsih_hal_get_error_mask_0(instance, 0xffffffff);
		temp &= ~(mask << event);
		temp |= (0 & mask) << event;
		mipi_dsih_hal_error_mask_0(instance, temp);
	}
	else
	{
		temp = mipi_dsih_hal_get_error_mask_1(instance, 0xffffffff);
		temp &= ~(mask << (event - HS_CONTENTION));
		temp |= (0 & mask) << (event - HS_CONTENTION);
		mipi_dsih_hal_error_mask_1(instance, temp);
		if (event == RX_CRC_ERR)
		{	/* automatically enable CRC reporting */
			mipi_dsih_hal_gen_crc_rx_en(instance, 1);
		}
	}
	return OK;
}
/**
 * Clear an already registered event
 * - Callback of the handler will be removed
 * @param instance pointer to structure holding the DSI Host core information
 * @param event enum
 * @return error code
 */
dsih_error_t mipi_dsih_unregister_event(dsih_ctrl_t * instance, dsih_event_t event)
{
	uint32_t mask = 1;
	uint32_t temp = 0;
	if (event > DUMMY)
	{
		return ERR_DSI_INVALID_EVENT;
	}
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	instance->event_registry[event] = 0;
	if (event < HS_CONTENTION)
	{
		temp = mipi_dsih_hal_get_error_mask_0(instance, 0xffffffff);
		temp &= ~(mask << event);
		temp |= (1 & mask) << event;
		mipi_dsih_hal_error_mask_0(instance, temp);
	}
	else
	{
		temp = mipi_dsih_hal_get_error_mask_1(instance, 0xffffffff);
		temp &= ~(mask << (event - HS_CONTENTION));
		temp |= (1 & mask) << (event - HS_CONTENTION);
		mipi_dsih_hal_error_mask_1(instance, temp);
		if (event == RX_CRC_ERR)
		{	/* automatically disable CRC reporting */
			mipi_dsih_hal_gen_crc_rx_en(instance, 0);
		}
	}
	return OK;
}
/**
 * Clear all registered events at once
 * @param instance pointer to structure holding the DSI Host core information
 * @return error code
 */
dsih_error_t mipi_dsih_unregister_all_events(dsih_ctrl_t * instance)
{
	int i = 0;
	if (instance == 0)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	if (instance->status != INITIALIZED)
	{
		return ERR_DSI_INVALID_INSTANCE;
	}
	for (i = 0; i < DUMMY; i++)
	{
		instance->event_registry[i] =  0;
	}

	mipi_dsih_hal_error_mask_0(instance, 0xffffff);
	mipi_dsih_hal_error_mask_1(instance, 0xffffff);
	/* automatically disable CRC reporting */
	mipi_dsih_hal_gen_crc_rx_en(instance, 0);
	return OK;
}
/**
 * This event handler shall be called upon receiving any event
 * it will call the specific callback (handler) registered for the invoking
 * event. Registration is done beforehand using mipi_dsih_register_event
 * its signature is void * so it could be OS agnostic (for it to be
 * registered in any OS/Interrupt controller)
 * @param param pointer to structure holding the DSI Host core information
 * @note This function must be registered with the DSI IRQs
 */
void mipi_dsih_event_handler(void * param)
{
	dsih_ctrl_t * instance = (dsih_ctrl_t *)(param);
	uint8_t i = 0;
	uint32_t status_0 = mipi_dsih_hal_error_status_0(instance, 0xffffffff);
	uint32_t status_1 = mipi_dsih_hal_error_status_1(instance, 0xffffffff);
	if (instance == 0)
	{
		return;
	}
	for (i = 0; i < DUMMY; i++)
	{
		if (instance->event_registry[i] != 0)
		{
			if (i < HS_CONTENTION)
			{
				if ((status_0 & (1 << i)) != 0)
				{
					instance->event_registry[i](&i);
				}
			}
			else
			{
				if ((status_1 & (1 << (i - HS_CONTENTION))) != 0)
				{
					instance->event_registry[i](&i);
				}
			}
		}
	}
}
/**
 * Reset the DSI Host controller
 * @param instance pointer to structure holding the DSI Host core information
 */
void mipi_dsih_reset_controller(dsih_ctrl_t * instance)
{
	mipi_dsih_hal_power(instance, 0);
	mipi_dsih_hal_power(instance, 1);
}
/**
 * Shut down the DSI Host controller
 * @param instance pointer to structure holding the DSI Host core information
 * @param shutdown (1) power up (0)
 */
void mipi_dsih_shutdown_controller(dsih_ctrl_t * instance, int shutdown)
{
	mipi_dsih_hal_power(instance, !shutdown);
}
/**
 * Reset the PHY module being controlled by the DSI Host controller
 * @param instance pointer to structure holding the DSI Host core information
 */
void mipi_dsih_reset_phy(dsih_ctrl_t * instance)
{
	mipi_dsih_dphy_reset(&(instance->phy_instance), 0);
	mipi_dsih_dphy_reset(&(instance->phy_instance), 1);
}
/**
 * Shut down the PHY module being controlled by the DSI Host controller
 * @param instance pointer to structure holding the DSI Host core information
 * @param shutdown (1) power up (0)
 */
void mipi_dsih_shutdown_phy(dsih_ctrl_t * instance, int shutdown)
{
	mipi_dsih_dphy_shutdown(&(instance->phy_instance), !shutdown);
}
