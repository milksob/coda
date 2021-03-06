/* BLURB lgpl

                           Coda File System
                              Release 7

          Copyright (c) 1987-2019 Carnegie Mellon University
                  Additional copyrights listed below

This  code  is  distributed "AS IS" without warranty of any kind under
the  terms of the  GNU  Library General Public Licence  Version 2,  as
shown in the file LICENSE. The technical and financial contributors to
Coda are listed in the file CREDITS.

                        Additional copyrights
                           none currently

#*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rvm/rvm.h>
#include <rvm/rvm_segment.h>

/* from rvm_private.h */
rvm_bool_t rvm_register_page(char *vmaddr, rvm_length_t length);
rvm_bool_t rvm_unregister_page(char *vmaddr, rvm_length_t length);

#ifdef __CYGWIN32__
#include <windows.h>
#endif

/* Routine to check if regions will overlap in memory. */

int overlap(unsigned long nregions, rvm_region_def_t regionDefs[])
{
    int i, j;
    rvm_region_def_t temp;

    /* sort array */
    for (i = 0; i < (nregions - 1); i++) {
        for (j = i + 1; j < nregions; j++) {
            if (regionDefs[j].vmaddr < regionDefs[i].vmaddr) {
                temp.vmaddr = regionDefs[i].vmaddr;
                temp.length = regionDefs[i].length;
                temp.offset = regionDefs[i].offset;

                regionDefs[i].vmaddr = regionDefs[j].vmaddr;
                regionDefs[i].length = regionDefs[j].length;
                regionDefs[i].offset = regionDefs[j].offset;

                regionDefs[j].vmaddr = temp.vmaddr;
                regionDefs[j].length = temp.length;
                regionDefs[j].offset = temp.offset;
            }
        }
    }

    for (i = 0; i < (nregions - 1); i++) {
        if (regionDefs[i].vmaddr + regionDefs[i].length >
            regionDefs[i + 1].vmaddr)
            return (TRUE);
    }

    return FALSE;
}

/* BSD44 memory allocation; uses mmap as an allocator.  Any mmap-aware
   system should be able to use this code */

#include <sys/types.h>
#include <sys/mman.h>
#include "coda_mmap_anon.h"
#include <errno.h>
#define ALLOCATE_VM_DEFINED

rvm_return_t allocate_vm(char **addr, unsigned long length)
{
    rvm_return_t ret     = RVM_SUCCESS;
    char *requested_addr = *addr; /* save this so that we can check it
				     against the address location
				     returned by mmap. this is
				     important because if it isn't 0,
				     it's a location that we HAVE to
				     be able to map to. */

#ifdef HAVE_MMAP
    mmap_anon(*addr, *addr, length, (PROT_READ | PROT_WRITE));
#else
    {
        HANDLE hMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL,
                                        PAGE_READWRITE, 0, length, NULL);
        if (hMap == NULL)
            return (RVM_EINTERNAL);
        *addr = MapViewOfFileEx(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0,
                                *addr);
        if (*addr == NULL) {
#if 0
	  DWORD errnum;
	  errnum = GetLastError();
	  printf ("allocate_vm: errnum = %d\n", errnum);
#endif
            *addr = (char *)-1;
        }
        CloseHandle(hMap);
    }
#endif

    if (*addr == (char *)-1) {
        if (errno == ENOMEM) {
            ret = RVM_ENO_MEMORY;
        } else {
            ret = RVM_EINTERNAL;
        }
    }

    if (requested_addr != 0 && *addr != requested_addr) {
        ret = RVM_EINTERNAL; /* couldn't allocated requested memory. */
    }

    /* modified by tilt, Nov 19 1996.
       When we allocate a page (or range of pages) we register
       it in an internal table we're keeping around to keep
       track of pages. (The previous solution was to try to
       re-allocate the page, and see if it fails, which is
       not only wrong [since we don't if it's allocated, or
       actually allocated in the RVM heap!!], but doesn't
       work with mmap()). */
    if (rvm_register_page(*addr, length) == rvm_false) {
        ret = RVM_EINTERNAL;
    }

    return ret;
}

rvm_return_t deallocate_vm(char *addr, unsigned long length)
{
    rvm_return_t ret = RVM_SUCCESS;

#ifdef HAVE_MMAP
    if (munmap(addr, length)) {
        ret = RVM_EINTERNAL;
    }
#else
    UnmapViewOfFile(addr);
#endif

    return ret;
}
