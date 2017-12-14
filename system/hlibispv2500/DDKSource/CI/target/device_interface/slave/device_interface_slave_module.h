/*! 
******************************************************************************
 @file   : device_interface_slave.cpp

 @brief  

 @Author Imagination Technologies

 @date   17/10/2007
 
         <b>Copyright 2005 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
        This file provides an implementation of a systemC module that attaches to a device_interface_slave to driver the rest of the system.


 \n<b>Platform:</b>\n
	     All 

******************************************************************************/
#ifndef DEVIF_SLAVE_MODULE_H
#define DEVIF_SLAVE_MODULE_H

#include "systemc.h"
#include "device_interface_slave.h"
class DevIfSlaveModule: public sc_module
{
	DeviceInterfaceSlave &slaveModule;

	SC_HAS_PROCESS(DevIfSlaveModule);
public:
	DevIfSlaveModule(const sc_module_name& name, DeviceInterfaceSlave &slaveModule)
		: sc_module(name)
		, slaveModule(slaveModule)
	{
		SC_THREAD(main);
	}
	
	void main(){
		slaveModule.main();
		sc_stop();
	}
};
#endif
