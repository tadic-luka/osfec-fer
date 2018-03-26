/*
 * =====================================================================================
 *
 *       Filename:  copy.c
 *
 *    Description:  Copies kernel code to RAM
 *
 *        Version:  1.0
 *        Created:  03/26/2018 10:21:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: luka.tadic@fer.hr 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <lib/string.h>
void copy_to_RAM ()
{
	extern char program_code_LMA, program_end_addr, program_code_VMA;
	char *ram = &program_code_VMA;
	char *start = &program_code_LMA;
	char *end = &program_end_addr;
	memcpy(ram, start, end - start); 
	/*while(start < end) {*/
		/**ram++= *start++;*/
	/*}*/
}
