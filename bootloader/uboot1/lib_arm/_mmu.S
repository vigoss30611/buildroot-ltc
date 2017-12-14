#include <mmu_table.h>
	
#T_1ST_BASE .equ 0xA0000000
#	PRESERVE8
#	AREA    Mmucfg,CODE,READONLY
.global cfg_mmu_table
.type   cfg_mmu_table , %function
#Entry(cfg_mmu_table)	
cfg_mmu_table:

				PUSH    {r0-r12,lr}
				
				MRC			p15, 0, r0, c0, c0, 5
				UBFX		r0, r0, #0, #2
				CMP			r0, #0
				BNE			enable_mmu
__15:
				#add		r11, pc, #g_oalAddressTable -(. + 8)
				LDR			r11, =g_oalAddressTable
				LDR			r10, =PT_1ST_BASE
						#PT_1ST_BSE				
				# (r10) = 1st level page table

				MOV			r1, r11				
				# (r1) = ptr to MemoryMap array
	#----------------------------
	# Setup 1st level page table (using section descriptor)
	# Fill in first level page table entries to create "un-mapped" regions
	# from the contents of the MemoryMap array.
	#
	# (r10) = 1st level page table
	# (r11) = ptr to MemoryMap array
__20:
				MOV			r0, #0x0E					
												# (r0) = PTE for 0: 1MB cachable bufferable, C = 1, B = 1
				BIC			r0, r0, #0x7000		
												# set TEX[2:0] = b'000
				ORR			r0, r0, #0x1000		
												# set TEX[2:0] = b'001
				ORR			r0, r0, #0xC00		
												# set full access, AP[1:0] = b'11
				BIC			r0, r0, #0x8000		
												# set AP[2] = b'0
				BIC			r0, r0, #0x10			
												# set XN = b'0
				BIC			r0, r0, #0x1E0		
												# set DOMAIN[3:0] = b'0000
				BIC			r0, r0, #0x200		
												# set IMP = b'0
				BIC			r0, r0, #0x10000	
												# set shareable = b'0
				BIC			r0, r0, #0x20000	
												# set nG = b'0
				#ORR			r0, r0, #0x80000	# set none-secure map, NS = b'1
				BIC			r0, r0, #0x80000	
												# set secure map, NS = b'0
				
__25:
				#mov		r1, r11				# (r1) = ptr to MemoryMap array
				
__30:
				LDR			r2, [r1], #4			
												# (r2) = virtual address to map Bank at
				LDR			r3, [r1], #4			
												# (r3) = physical address to map from
				LDR			r4, [r1], #4			
												# (r4) = num MB to map
				LDR			r6, [r1], #4			
												#	(r6) = cacheable
				LDR			r7, [r1], #4			
												#	(r7) = shareable
				LDR			r8, [r1], #4			
												#	(r8) = memory type		
				      	
				CMP			r4, #0				
												# End of table?
				BEQ			__40
				      	
				LDR			r5, =0xFFF00000
				AND			r2, r2, r5				
												# VA needs 512MB, 1MB aligned.
				      	
				LDR			r5, =0xFFF00000
				AND			r3, r3, r5				
												# PA needs 4GB, 1MB aligned.
				      	
				ADD			r2, r10, r2, LSR #18
				ADD			r0, r0, r3				
												# (r0) = PTE for next physical page
				
__35:
				
				CMP			r6, #0x0			
												# is non-cacheable
				BICEQ		r0, r0, #0xC		
												# set C = 0, B = 0
				BICEQ		r0, r0, #0x7000		
												# set TEX[2:0] = b'000
				ORREQ		r0, r0, #0x1000		
												# set TEX[2:0] = b'001
				     		
				CMP			r7, #0x1			
												# is shareable
				ORREQ		r0, r0, #0x10000	
												# set shareable = b'0
				     		
				CMP			r8, #0x1			
												
												# is Device type
				BICEQ		r0, r0, #0xC			
				ORREQ		r0, r0, #0x4		
												# set C = 0, B = 1
				BICEQ		r0, r0, #0x7000		
												# set TEX[2:0] = b'000
				    		
				CMP			r8, #0x2			
												# is strongly-ordered type
				BICEQ		r0, r0, #0xC		
												# set C = 0, B = 0
				BICEQ		r0, r0, #0x7000		
												# set TEX[2:0] = b'000
				    		
				STR			r0, [r2], #4
				ADD			r0, r0, #0x00100000	
												# (r0) = PTE for next physical page
				SUB			r4, r4, #1			
												# Decrement number of MB left
				CMP			r4, #0
				BNE			__35				
												# Map next MB
				     
				BIC			r0, r0, #0xF0000000	
												# Clear Section Base Address Field
				BIC			r0, r0, #0x0FF00000	
												# Clear Section Base Address Field
				B			__20				
												# Get next element

__40:
enable_mmu:
				ldr		r10, =PT_1ST_BASE	
												# (r10) = restore address of 1st level page table
				
				MOV		r0,#0
				
				MCR 		p15, 0, r0, c7, c5, 0 
													# Invalidate ICache
				MCR 		p15, 0, r0, c8, c7, 0 
													# Invalidate entire Unified TLB
				
				# The page tables and exception vectors are setup.
				# Initialize the MMU and turn it on.
				MRC 		p15, 0, r0, c2, c0, 0
													# Read Translation Table Base Register 0TTBR0
				BIC 		r0, r0, #1
													# IRGN[1] = b'0
				ORR 		r0, r0, #2
													# S = b'1
				MOV			r1, #3
				BIC 		r0, r0, r1, LSL #3
													# RGN = b'00
				BIC 		r0, r0, #0x40
													# IRGN[0] = b'0
				LDR			r1, =0xffffc000
				BIC 		r0, r0, r1
				ORR 		r0, r0, r10
				MCR 		p15, 0, r0, c2, c0, 0
													# Write Translation Table Base Register 0TTBR0
				MRC 		p15, 0, r0, c2, c0, 0
													# Read Translation Table Base Register 0TTBR0
				ISB
				
				MRC 		p15, 0, r0, c2, c0, 1 
													# Read Translation Table Base Register 0TTBR1
				    		
				MRC 		p15, 0, r0, c2, c0, 2 
													# Read TTBCR
				MOV			r1, #0x7
				BIC 		r0, r0, r1	
													#set N = 0
				MCR 		p15, 0, r0, c2, c0, 2 
													# Write TTBCR    (Translation table base control register)
				MRC 		p15, 0, r0, c2, c0, 2 
													# Read TTBCR
				    		
				MRC 		p15, 0, r0, c3, c0, 0 
													# Read DACR
				MOV 		r1, #0x3
				BIC 		r0, r0, r1
				MOV			r1, #0x1
				ORR 		r0, r0, r1
													# set domain 0 as client
				MCR 		p15, 0, r0, c3, c0, 0 
													# Write DACR   (Domain access control register)
				MRC 		p15, 0, r0, c3, c0, 0 
													# Read DACR
				    		
				MCR 		p15, 0, r0, c8, c7, 0 
													# Invalidate entire Unified TLB
				    		
				MRC 		p15, 0, r1, c1, c0, 0 
													# Read SCTLR
				ORR			r1, r1, #0x1
													#			
													#Enables the MMU
				BIC			r1, r1, #0x2
													#			
#ifdef DCACHE_ENABLE
				ORR			r1, r1, #0x0004		
													# Enable the Data Cache
#else
							BIC			r1, r1, #0x0004		
													# Disable the Data Cache
#endif
			
#ifdef ICACHE_ENABLE
				ORR			r1, r1, #0x1000		
													# Enable the I Cache
#else
				BIC			r1, r1, #0x1000		
													# Disable the I Cache
#endif
				#]
				BIC 		r1, r1, #0x10000000	
													# TEX remap disabled.
				BIC 		r1, r1, #0x20000000	
													# Access Flag disabled
				MCR 		p15, 0, r1, c1, c0, 0 
													# Write SCTLR
				MRC 		p15, 0, r1, c1, c0, 0 
													# Read (SCTLR  system control register)
				ISB
				
				NOP 
				POP			{r0-r12,pc}

