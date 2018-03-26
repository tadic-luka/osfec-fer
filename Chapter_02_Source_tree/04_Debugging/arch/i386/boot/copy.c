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
void copy_to_RAM ()
{
	extern char kernel_code_LMA, kernel_end_addr;
	char *ram = (char *)RAM_START;
	char *start = &kernel_code_LMA;
	char *end = &kernel_end_addr;
	while(start < end) {
		*ram= *start;
		++start;
		++ram;
	}
}
