obj-$(CONFIG_VIDEO_AR330) += ar330mipi.o
obj-$(CONFIG_VIDEO_GC0308) += gc0308.o
obj-$(CONFIG_VIDEO_GC2035) += gc2035.o
obj-$(CONFIG_VIDEO_GC0329) += gc0329_config.o gc0329_front.o gc0329_common.o gc0329_rear.o
obj-$(CONFIG_DDK_SENSOR) += ddk_sensor.o
#obj-$(CONFIG_XC9080) += xc9080_driver.o
obj-$(CONFIG_XC9080_V4L2) +=xc9080_V4L2_drv.o xc9080_common.o 
obj-$(CONFIG_XC9080_ISP) += xc9080_isp.o

obj-$(CONFIG_SENSOR_POWER_UP) += power_up.o
obj-$(CONFIG_SENSOR_ITEM_UTILS) += items_utils.o

ifdef CONFIG_ARCH_Q3F
obj-$(CONFIG_SENSOR_DVP_WRAPPER) += dvp_wrapper.o
else ifdef CONFIG_ARCH_APOLLO3
obj-$(CONFIG_SENSOR_DVP_WRAPPER) += dvp_wrapper.o
endif

obj-$(CONFIG_VIDEO_STK3855) += stk3855.o

ddk_sensor-objs:= ddk_sensor_driver.o
#xc9080-objs:= xc9080_driver.o
