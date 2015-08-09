/*
 * Copyright (C) 2015 PSP2SDK Project
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <psp2/kernel/loadcore.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/moduleinfo.h>
#include <psp2/types.h>
#include <stdint.h>
#include <stdlib.h>

extern SceModuleInfo module_info;

extern void (* __init_array_start[])(void) __attribute__((weak));
extern void (* __init_array_end[])(void) __attribute__((weak));

extern void (* __fini_array_start[])(void) __attribute__((weak));
extern void (* __fini_array_end[])(void) __attribute__((weak));

int main(int argc, char *argv[]);

static void _init()
{
	void (** p)();

	for (p = __init_array_start; p != __init_array_end; p++)
		(*p)();
}

static void _fini()
{
	void (** p)();

	p = __fini_array_end;
	while (p != __fini_array_start) {
		p--;
		(*p)();
	}
}

static void _Noreturn module_start(SceSize arglen, void *argp)
{
	SceSize i;
	int argc;
	char **argv;
	char *p;

	_init();
        atexit(_fini);

	argc = 0;
	for (i = 0; i < arglen; i++)
		if (((char *)argp)[i] == 0)
			argc++;

	argv = __builtin_alloca(sizeof(char *) * (argc + 1));

	i = 0;
	p = argp;
	while (i < argc) {
		argv[i] = p;
		i++;

		while (*p++ != 0);
	}

	argv[argc] = NULL;

	exit(main(argc, argv));
}

static const uint32_t nids[3] __attribute__((section(".sceExport.rodata")))
	= { 0x935CD196, 0x6C2224BA };

static const uint32_t ents[3] __attribute__((section(".sceExport.rodata")))
	= { (uint32_t)module_start, (uint32_t)&module_info };

static const char name[] __attribute__((section(".sceExport.rodata")))
	= "syslib";

static const SceLibraryEntryTable export
	__attribute__((section(".sceLib.ent"), used)) = {
		.size = sizeof(SceLibraryEntryTable),
		.ver = 0,
		.attr = 0x800,
		.funcNum = 2,
		.varNum = 1,
		.unkNum = 0,
		.nid = 0,
		.name = name,
		.nids = nids,
		.ents = (const void **)ents
	};
