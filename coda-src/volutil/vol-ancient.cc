#ifndef _BLURB_
#define _BLURB_
/*

            Coda: an Experimental Distributed File System
                             Release 4.0

          Copyright (c) 1987-1996 Carnegie Mellon University
                         All Rights Reserved

Permission  to  use, copy, modify and distribute this software and its
documentation is hereby granted,  provided  that  both  the  copyright
notice  and  this  permission  notice  appear  in  all  copies  of the
software, derivative works or  modified  versions,  and  any  portions
thereof, and that both notices appear in supporting documentation, and
that credit is given to Carnegie Mellon University  in  all  documents
and publicity pertaining to direct or indirect use of this code or its
derivatives.

CODA IS AN EXPERIMENTAL SOFTWARE SYSTEM AND IS  KNOWN  TO  HAVE  BUGS,
SOME  OF  WHICH MAY HAVE SERIOUS CONSEQUENCES.  CARNEGIE MELLON ALLOWS
FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION.   CARNEGIE  MELLON
DISCLAIMS  ANY  LIABILITY  OF  ANY  KIND  FOR  ANY  DAMAGES WHATSOEVER
RESULTING DIRECTLY OR INDIRECTLY FROM THE USE OF THIS SOFTWARE  OR  OF
ANY DERIVATIVE WORK.

Carnegie  Mellon  encourages  users  of  this  software  to return any
improvements or extensions that  they  make,  and  to  grant  Carnegie
Mellon the rights to redistribute these changes without encumbrance.
*/

static char *rcsid = "$Header: /afs/cs/project/coda-src/cvs/coda/coda-src/volutil/vol-ancient.cc,v 4.3 1997/12/20 23:35:35 braam Exp $";
#endif /*_BLURB_*/





#ifdef __cplusplus
extern "C" {
#endif __cplusplus

#include <sys/types.h>
#include <sys/errno.h>
#if !defined(__GLIBC__)
#include <sysent.h>
#endif

#include <lwp.h>
#include <lock.h>
#include <rpc2.h>

#ifdef __cplusplus
}
#endif __cplusplus

#include <voltypes.h>
#include <vice.h>
#include <cvnode.h>
#include <volume.h>
#include <camprivate.h>
#include <vutil.h>
#include "vvlist.h"
#include <util.h>
#include <rvmlib.h>

/*
  BEGIN_HTML
  <a name="S_VolMarkAsAncient"><strong>Mark the older dump file of a volume as ancient</strong></a> 
  END_HTML
*/
long S_VolMarkAsAncient(RPC2_Handle rpcid, VolumeId groupId, VolumeId repId)
{
    ProgramType *pt;
    int status = 0;
    int rc = 0;
    
    assert(LWP_GetRock(FSTAG, (char **)&pt) == LWP_SUCCESS);

    LogMsg(9, VolDebugLevel, stdout, "Entering S_VolMarkAsAncient: rpcid = %d, groupId = %x, repId = %x",
	rpcid, groupId, repId);

    rc = VInitVolUtil(volumeUtility);
    if (rc != 0)
	return rc;

    char listfile[MAXLISTNAME];
    getlistfilename(listfile, groupId, repId, "newlist");

    char newlistfile[MAXLISTNAME];
    getlistfilename(newlistfile, groupId, repId, "ancient");

    if (rename(listfile, newlistfile) < 0) {
#ifndef __CYGWIN32__
	LogMsg(0, VolDebugLevel, stdout, "MarkAsAncient: rename %s->%s failed, %s", listfile, newlistfile,
	    errno < sys_nerr? sys_errlist[errno]: "Cannot rename");
#else
LogMsg(0, VolDebugLevel, stdout, "MarkAsAncient: rename %s->%s failed.", listfile, newlistfile);
#endif	VDisconnectFS();
	return VFAIL;
    }

    VDisconnectFS();
    LogMsg(0, VolDebugLevel, stdout, "MarkAsAncient succeeded");

    return RPC2_SUCCESS;
}
