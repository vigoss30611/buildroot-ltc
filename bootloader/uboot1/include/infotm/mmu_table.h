# Export Definition
#  	AREA    MMUTABLE,CODE,READONLY
  
#EXPORT  g_oalAddressTable[DATA]

#------------------------------------------------------------------------------
#
# TABLE FORMAT
#       cached address, physical address, size
#------------------------------------------------------------------------------
#T_1ST_BASE:
#			.word	0xa0000000
g_oalAddressTable:
			# VA, PA, SIZE(MB), cacheable, shareable, memory type(0: normal Memory, 1: device, 2: strongly ordered)	
			.word 0x00000000
			.word 0x00000000
			.word 1
			.word 1
			.word 1
			.word 0 
			
			.word 0x20000000
			.word 0x20000000 
			.word 192
			.word 0
			.word 0
			.word 1 		
				# 192MB REG
			
			.word 0x3C000000
			.word 0x3C000000
			.word 1
			.word 1
			.word 1
			.word 0     
				# 1MB SRAM 
			
			.word 0x40000000
			.word 0x40000000
			.word 1024
			.word 1
			.word 1
			.word 0     
				# 1024MB cacheable, DRAM
			
			.word 0x80000000
			.word 0x80000000
			.word 1024
			.word 1
			.word 1
			.word 0     
				# 1024MB cacheable, DRAM
			
			.word 0xA0000000
			.word 0x40000000
			.word 1024
			.word 0
			.word 0
			.word 0     
				# 1024MB no_cacheable, DRAM	
			
			.word 0x00000000
			.word 0x85700000
			.word 1
			.word 1
			.word 1
			.word 0     
				# 1024MB no_cacheable, DRAM
			
			.word 0x00000000
			.word 0x00000000
			.word 0
			.word 0
			.word 0
			.word 0     
				# end of table
#------------------------------------------------------------------------------
#define PT_1ST_BASE 0xA0000000
#define DCACHE_ENABLE 1
#define ICACHE_ENABLE 1

