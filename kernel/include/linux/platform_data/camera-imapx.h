
/*                                                                        
 * camera-imapx.h                 
 *                         
 */                                                                       

#ifndef __ASM_ARCH_CAMERA_H_                                              
#define __ASM_ARCH_CAMERA_H_                                              
                                                                           
/*                                                                       
 * strutc sensor_info -SOC camera sensor information              
 * @exsit		-	is it really on board
 * @model_name	-	like "gc2035"
 * @ctrl_bus	-	like "i2c"
 * @interface		-	like "isp" or "camif"
 * @face		-	like "front" or "rear"
 * @power_iovdd -	power supply by pmu,most time mean 1.8v
 * @power_dvdd  -	power supply by pmu,most time mean 2.8v
 * @bus_chn		-	ctrl bus channel
 * @pdown_pin	-	power down gpio pin		
 * @reset_pin	-	sensor reset gpio pin
 * @rotation	-	hardware rotation,depend on IC layout 
 * @mclk		-	sensor input clock frequency,usally 24MHZ
 */                                                                       


struct imapx_cam_board {

	int (*init)(void);
	int (*deinit)(void);
};                                                                        

#endif /* __ASM_ARCH_CAMERA_H_ */                                         

