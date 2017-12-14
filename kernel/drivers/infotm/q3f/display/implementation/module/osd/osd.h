#ifndef OSD_Q3F_H
#define OSD_Q3F_H

int osd_n_enable(int idsx, int osd_num, int enable);
int osd_dma_addr(int widsx, dma_addr_t addr, int frame_size, int fbx);
int osd_swap(int idsx, int fbx, unsigned int fb_addr, int win_id);
int swap_buffer(u32 phy_addr);

int set_vic_osd_ui(int idsx,int vic, int window, int iformat, int alpha, int colorkey);

int scaler_config_size(unsigned int window, unsigned int in_width,unsigned int in_height,
		unsigned int in_virt_width, unsigned int in_virt_height,
		unsigned int out_width,unsigned int out_height,
		unsigned int osd0_pos_x,unsigned int osd0_pos_y,
		unsigned int scaler_left_x, unsigned int scaler_top_y,
		dma_addr_t scaler_buff, int format, int interlaced);

int set_vic_osd_prescaler_par(unsigned int window,
			unsigned int in_width, unsigned int in_height,
			unsigned int virt_in_width, unsigned int virt_in_height,
			unsigned int out_width,unsigned int out_height, unsigned int osd0_pos_x,
			unsigned int osd0_pos_y, dma_addr_t scaler_buff, int format, int interlaced);

/*Note: for LCD test */
int osd_set_buffer_addr(u32 idsx,u32 win, u32 phy_addr, u32 bsel);

#endif
