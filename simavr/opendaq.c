/* Simulate an openDAQ board using simavr.  */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <simavr/sim_avr.h>
#include <simavr/avr_ioport.h>
#include <simavr/sim_elf.h>
#include <simavr/sim_gdb.h>
#include <simavr/sim_vcd_file.h>
#include "uart_pty.h"

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define FIRM "opendaq.elf"
#define MMCU "atmega644p"
#define FREQ 16000000


int main(int argc, char *argv[])
{
	char * mmcu = MMCU;
	uint32_t freq = FREQ;
	int debug = 0;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-d"))
			debug++;
		else {
			fprintf(stderr, "%s: invalid argument %s\n", argv[0], argv[i]);
			exit(1);
		}
	}

	avr_t *avr = avr_make_mcu_by_name(mmcu);
	if (!avr) {
		fprintf(stderr, "%s: Error creating the AVR core\n", argv[0]);
		exit(1);
	}
	avr_init(avr);
	avr->frequency = freq;

	// load the firmware
	elf_firmware_t fw;
	if (elf_read_firmware(FIRM, &fw) != 0) {
		fprintf(stderr, "%s: Unable to load %s\n", argv[0], FIRM);
		exit(1);
	}
	avr_load_firmware(avr, &fw);

	// even if not setup at startup, activate gdb if crashing
	avr->gdb_port = 1234;
	if (debug) {
		avr->state = cpu_Stopped;
		avr_gdb_init(avr);
	}

	// create a virtual serial port
	uart_pty_t uart_pty;
	uart_pty_init(avr, &uart_pty);
	uart_pty_connect(&uart_pty, '0');

	int state = cpu_Running;
	while ((state != cpu_Done) && (state != cpu_Crashed)) {
		state = avr_run(avr);
	}
}
