
#ifndef _VCRCOMMON_
#define _VCRCOMMON_

/* DO NOT EDIT: generated by rp2gen from /home/braam/ss/coda-src/vicedep/vcrcommon.rpc2 */
#ifdef __cplusplus
extern "C" {
#endif __cplusplus
#include "rpc2.h"
#include "se.h"
#include "errors.h"
#ifdef __cplusplus
}
#endif __cplusplus

#ifndef _BLURB_
#define _BLURB_
/*

            Coda: an Experimental Distributed File System
                             Release 3.1

          Copyright (c) 1987-1995 Carnegie Mellon University
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

static char *rcsid = "$Header: /coda/coda.cs.cmu.edu/project/coda/cvs/coda/coda-src/vicedep/Attic/vcrcommon.h,v 4.1 1998/12/04 22:26:45 braam Exp $";
#endif /*_BLURB_*/

/*

                         IBM COPYRIGHT NOTICE

                          Copyright (C) 1986
             International Business Machines Corporation
                         All Rights Reserved

This  file  contains  some  code identical to or derived from the 1986
version of the Andrew File System ("AFS"), which is owned by  the  IBM
Corporation.    This  code is provded "AS IS" and IBM does not warrant
that it is free of infringement of  any  intellectual  rights  of  any
third  party.    IBM  disclaims  liability of any kind for any damages
whatsoever resulting directly or indirectly from use of this  software
or  of  any  derivative work.  Carnegie Mellon University has obtained
permission to distribute this code, which is based on Version 2 of AFS
and  does  not  contain the features and enhancements that are part of
Version 3 of AFS.  Version 3 of  AFS  is  commercially  available  and
supported by Transarc Corporation, Pittsburgh, PA.

*/


#ifndef _FID_T_
#define _FID_T_


typedef RPC2_Unsigned VolumeId;

typedef RPC2_Unsigned VnodeId;

typedef VolumeId VolId;

typedef RPC2_Unsigned Unique_t;

typedef RPC2_Unsigned FileVersion;
 
#endif


#ifndef _VUID_T_
#define _VUID_T_
typedef unsigned int vuid_t;
typedef unsigned int vgid_t;
#endif _VUID_T_


#ifndef _VICEFID_T_
#define _VICEFID_T_


typedef struct ViceFid {
    RPC2_Unsigned Volume;
    RPC2_Unsigned Vnode;
    RPC2_Unsigned Unique;
} ViceFid;
 
#endif


typedef struct ViceStoreId {
    RPC2_Unsigned Host;
    RPC2_Unsigned Uniquifier;
} ViceStoreId;

typedef struct ViceVersionArray {
    RPC2_Integer Site0;
    RPC2_Integer Site1;
    RPC2_Integer Site2;
    RPC2_Integer Site3;
    RPC2_Integer Site4;
    RPC2_Integer Site5;
    RPC2_Integer Site6;
    RPC2_Integer Site7;
} ViceVersionArray;

typedef struct ViceVersionVector {
    ViceVersionArray Versions;
    ViceStoreId StoreId;
    RPC2_Unsigned Flags;
} ViceVersionVector;

typedef RPC2_Unsigned UserId;

typedef RPC2_Unsigned Date_t;

typedef RPC2_Integer Rights;

typedef enum{ Invalid=0, File=1, Directory=2, SymbolicLink=3 } ViceDataType;

typedef enum{ NoCallBack=0, CallBackSet=1 } CallBackStatus;

typedef struct ViceStatus {
    RPC2_Unsigned InterfaceVersion;
    ViceDataType VnodeType;
    RPC2_Integer LinkCount;
    RPC2_Unsigned Length;
    FileVersion DataVersion;
    ViceVersionVector VV;
    Date_t Date;
    UserId Author;
    UserId Owner;
    CallBackStatus CallBack;
    Rights MyAccess;
    Rights AnyAccess;
    RPC2_Unsigned Mode;
    VnodeId vparent;
    Unique_t uparent;
} ViceStatus;

#ifndef _STUB_PREDEFINED_
#define _STUB_PREDEFINED_

typedef struct CallCountEntry {
    RPC2_String name;
    RPC2_Integer countent;
    RPC2_Integer countexit;
    RPC2_Integer tsec;
    RPC2_Integer tusec;
    RPC2_Integer counttime;
} CallCountEntry;

typedef struct MultiCallEntry {
    RPC2_String name;
    RPC2_Integer countent;
    RPC2_Integer countexit;
    RPC2_Integer tsec;
    RPC2_Integer tusec;
    RPC2_Integer counttime;
    RPC2_Integer counthost;
} MultiCallEntry;

typedef struct MultiStubWork {
    RPC2_Integer opengate;
    RPC2_Integer tsec;
    RPC2_Integer tusec;
} MultiStubWork;
#endif _STUB_PREDEFINED_

/* Op codes and definitions */

#ifdef __cplusplus
extern "C"{
#endif
#ifdef __cplusplus
}
#endif

#endif _VCRCOMMON_
