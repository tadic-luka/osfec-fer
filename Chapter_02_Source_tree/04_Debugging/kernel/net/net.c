#include <kernel/kprint.h>
#include <kernel/net.h>
#include <lib/string.h>

static char *p = 0;
void net_init(char *prefix) {
	p = prefix;
	int len = strlen(prefix);
	if ( len > 20 ) {
		p[20] = 0; 
	}
}

void net_print(char *msg) {
	kprintf("%s: %s\n", p, msg);
}
