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

static char *rcsid = "$Header: /coda/coda.cs.cmu.edu/project/coda/cvs/coda/coda-src/rpc2/Attic/debug.c,v 4.8 1998/11/24 15:34:33 jaharkes Exp $";
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


#ifdef RPC2DEBUG
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "lwp.h"
#include "timer.h"
#include "rpc2.h"
#include "se.h"
#include "rpc2.private.h"
#include "trace.h"

/*----- Routines to aid in debugging -----*/

char *rpc2_timestring(void)
{
	struct timeval t;
	static char mytime[9];
	int rc;

	rc = gettimeofday(&t, NULL);

	if ( rc ) {
		return "BADTIME";
	} else {
		strftime(mytime, sizeof(mytime), "%H:%M:%S", 
			 localtime(&t.tv_sec));
		mytime[8] = 0;
		return mytime;
	}
}
		
	

static char *WhichMagic(int x)
{
    static char buf[20];
    switch(x)
	{
	case OBJ_PACKETBUFFER:	return("OBJ_PACKETBUFFER");
	case OBJ_CENTRY:	return("OBJ_CENTRY");
	case OBJ_SLENTRY:	return("OBJ_SLENTRY");
	case OBJ_SSENTRY:	return("OBJ_SSENTRY");
	case OBJ_HENTRY:	return("OBJ_HENTRY");
	default:		(void) sprintf(buf, "%d", x); return(buf);
	}
}

void rpc2_PrintTMElem(struct TM_Elem *tPtr, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    fprintf(tFile, "MyAddr = 0x%lx Next = 0x%lx  Prev = 0x%lx  TotalTime = %ld:%ld  TimeLeft = %ld:%ld  BackPointer = %p\n",
    	(long)tPtr, (long)tPtr->Next, (long)tPtr->Prev, tPtr->TotalTime.tv_sec, tPtr->TotalTime.tv_usec,
	tPtr->TimeLeft.tv_sec, tPtr->TimeLeft.tv_usec, tPtr->BackPointer);
    (void) fflush(tFile);
}

void rpc2_PrintFilter(RPC2_RequestFilter *fPtr, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    fprintf(tFile, "FromWhom = %s  OldOrNew = %s  ", 
	fPtr->FromWhom == ANY ? "ANY" : (fPtr->FromWhom == ONECONN ? "ONECONN" : (fPtr->FromWhom == ONESUBSYS ? "ONESUBSYS" : "??????")),
	fPtr->OldOrNew == OLD ? "OLD" : (fPtr->OldOrNew == NEW ? "NEW" : (fPtr->OldOrNew == OLDORNEW ? "OLDORNEW" : "??????")));
    switch(fPtr->FromWhom)
	{
	case ONECONN:	fprintf(tFile, "WhichConn = 0x%lx", fPtr->ConnOrSubsys.WhichConn); break;
	case ONESUBSYS: fprintf(tFile, "SubsysId = %ld", fPtr->ConnOrSubsys.SubsysId); break;
        case ANY:       break;
	}
    fprintf(tFile, "\n");
    (void) fflush(tFile);
}

void rpc2_PrintSLEntry(struct SL_Entry *slPtr, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;
    
    fprintf(tFile, "MyAddr: 0x%lx\n\tNextEntry = 0x%lx PrevEntry = 0x%lx  MagicNumber = %s  ReturnCode = %s\n\tTElem==>  ", (long)slPtr, (long)slPtr->NextEntry, (long)slPtr->PrevEntry, WhichMagic(slPtr->MagicNumber), 
	    slPtr->ReturnCode == WAITING ? "WAITING" : slPtr->ReturnCode == ARRIVED ? "ARRIVED" : slPtr->ReturnCode == TIMEOUT ? "TIMEOUT" : slPtr->ReturnCode == NAKED ? "NAKED" : "??????");
    switch(slPtr->Type)
	{
	case REPLY:
		    fprintf(tFile, "\tType = REPLY  Conn = 0x%lx\n",
			    slPtr->Conn);
		    break;
	
	case REQ:
		    fprintf(tFile, "\tElementType = REQ  Packet = 0x%lx  Filter==>  ",
			    (long)slPtr->Packet);
		    rpc2_PrintFilter(&slPtr->Filter, tFile);
		    break;
	
	case OTHER:
		    fprintf(tFile, "\tElementType = OTHER  Conn = 0x%lx  Packet = 0x%lx\n",
			    slPtr->Conn, (long)slPtr->Packet);
		    break;

	default:
		    fprintf(tFile, "\tElementType = ???????\n");
		    break;
	}
    rpc2_PrintTMElem(&slPtr->TElem, tFile);
    fprintf(tFile, "\n");
    (void) fflush(tFile);
}


void rpc2_PrintHEntry(struct HEntry *hPtr, FILE *tFile)
{
    int head, ix;

    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */

    fprintf(tFile, "\nHost 0x%lx state is...\n\tNextEntry = 0x%lx  PrevEntry = 0x%lx  MagicNumber = %s\n",
	(long)hPtr, (long)hPtr->Next, (long)hPtr->Prev, WhichMagic(hPtr->MagicNumber));

    fprintf(tFile, "\tHost.InetAddress = %s\n", inet_ntoa(hPtr->Host));
    fprintf(tFile, "\tLastWord = %ld.%06ld\n", hPtr->LastWord.tv_sec, hPtr->LastWord.tv_usec);
    fprintf(tFile, "\tRTT = %ld.%03ld, RTTvar = %ld.%03ld\n",
	    hPtr->RTT >> RPC2_RTT_SHIFT,
	    hPtr->RTT % ((1 << RPC2_RTT_SHIFT) - 1),
	    hPtr->RTTVar >> RPC2_RTTVAR_SHIFT,
	    hPtr->RTTVar % ((1 << RPC2_RTTVAR_SHIFT) - 1));
    fprintf(tFile, "\tBW = %ld.%03ld, BWvar = %ld.%03ld\n",
	    hPtr->BW >> RPC2_BW_SHIFT,
	    hPtr->BW % ((1 << RPC2_BW_SHIFT) - 1),
	    hPtr->BWVar >> RPC2_BWVAR_SHIFT,
	    hPtr->BWVar % ((1 << RPC2_BWVAR_SHIFT) - 1));
    fprintf(tFile, "\tObservation Log Entries = %d (%d kept)\n", 
	    hPtr->NumEntries, RPC2_MAXLOGLENGTH);
    
    if (hPtr->NumEntries < RPC2_MAXLOGLENGTH) head = 0;
    else head = hPtr->NumEntries - RPC2_MAXLOGLENGTH;
    while (head < hPtr->NumEntries) {
	ix = head & (RPC2_MAXLOGLENGTH-1);
	switch(hPtr->Log[ix].Tag) 
	    {
	    case RPC2_MEASURED_NLE:
		fprintf(tFile, "\t\tentry %d: %ld.%06ld, conn 0x%lx, %ld bytes, %ld msec\n",
			ix, hPtr->Log[ix].TimeStamp.tv_sec, 
			hPtr->Log[ix].TimeStamp.tv_usec,
			hPtr->Log[ix].Value.Measured.Conn,
			hPtr->Log[ix].Value.Measured.Bytes, 
			hPtr->Log[ix].Value.Measured.ElapsedTime);
		break;
	    case RPC2_STATIC_NLE:
		fprintf(tFile, "\t\tentry %d: %ld.%06ld, static bandwidth %ld bytes/sec\n",
			ix, hPtr->Log[ix].TimeStamp.tv_sec, 
			hPtr->Log[ix].TimeStamp.tv_usec,
			hPtr->Log[ix].Value.Static.Bandwidth);
		break;
	    default:
		break;
	    }		
	head++;
    }
    (void) fflush(tFile);
}


void rpc2_PrintCEntry(struct CEntry *cPtr, FILE *tFile)
{
    long i;
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    fprintf(tFile, "MyAddr: 0x%lx\n\tNextEntry = 0x%lx  PrevEntry = 0x%lx  MagicNumber = %s  Role = %s  State = ",
	(long)cPtr, (long)cPtr->NextEntry, (long)cPtr->PrevEntry,
	WhichMagic(cPtr->MagicNumber),
	TestRole(cPtr,FREE) ? "FREE" :(TestRole(cPtr, CLIENT) ? "CLIENT" : (TestRole(cPtr, SERVER) ? "SERVER" : "?????") ));
    if (TestRole(cPtr,CLIENT))
	switch((int) (cPtr->State & 0x0000ffff))
	    {
	    case C_THINK: fprintf(tFile, "C_THINK");break;
	    case C_AWAITREPLY: fprintf(tFile, "C_AWAITREPLY");break;
	    case C_AWAITINIT2: fprintf(tFile, "C_AWAITINIT2");break;
	    case C_AWAITINIT4: fprintf(tFile, "C_AWAITINIT4");break;
	    case C_HARDERROR: fprintf(tFile, "C_HARDERROR");break;
	    default: fprintf(tFile, "???????"); break;
	    }
    if (TestRole(cPtr,SERVER))
	switch((int) (cPtr->State & 0x0000ffff))
	    {
	    case S_AWAITREQUEST: fprintf(tFile, "S_AWAITREQUEST");break;	    
	    case S_PROCESS: fprintf(tFile, "S_PROCESS");break;	    
	    case S_STARTBIND: fprintf(tFile, "S_STARTBIND");break;	    
	    case S_FINISHBIND: fprintf(tFile, "S_FINISHBIND");break;
	    case S_AWAITINIT3: fprintf(tFile, "S_AWAITINIT3");break;
	    case S_REQINQUEUE: fprintf(tFile, "S_REQINQUEUE");break;
	    case S_HARDERROR: fprintf(tFile, "S_HARDERROR");break;
	    case S_INSE: fprintf(tFile, "S_INSE");break;
	    case S_AWAITENABLE: fprintf(tFile, "S_AWAITENABLE");break;
	    default: fprintf(tFile, "??????"); break;
	    }

    fprintf(tFile, "\n\tSecurityLevel = %s", cPtr->SecurityLevel == RPC2_OPENKIMONO ? "RPC2_OPENKIMONO" : 
	(cPtr->SecurityLevel == RPC2_AUTHONLY ? "RPC2_AUTHONLY" : (cPtr->SecurityLevel == RPC2_SECURE ? "RPC2_SECURE" : 
	(cPtr->SecurityLevel == RPC2_HEADERSONLY ? "RPC2_HEADERSONLY" :"??????"))));
    fprintf(tFile, "  EncryptionType = %ld  SessionKey = 0x", cPtr->EncryptionType);
    for(i = 0; i < RPC2_KEYSIZE; i++)fprintf(tFile, "%lx", (long)cPtr->SessionKey[i]);
	
    fprintf(tFile, "\n\tUniqueCID = 0x%lx  NextSeqNumber = %ld  PeerHandle = 0x%lx\n\tPrivatePtr = 0x%lx  SideEffectPtr = 0x%lx\n",
    	cPtr->UniqueCID, cPtr->NextSeqNumber, cPtr->PeerHandle, (long)cPtr->PrivatePtr, (long)cPtr->SideEffectPtr);
	
    fprintf(tFile, "\tLowerLimit = %lu usec  %s = %ld  %s = %ld  Retries = %ld\n",
	    cPtr->LowerLimit,
	    TestRole(cPtr, CLIENT) ? "RTT" : (TestRole(cPtr, SERVER) ? "TimeEcho" : "?????"),
	    cPtr->RTT, 
	    TestRole(cPtr, CLIENT) ? "RTTVar" : (TestRole(cPtr, SERVER) ? "RequestTime" : "?????"),
	     cPtr->RTTVar,  cPtr->Retry_N);

    fprintf(tFile, "\tRetry_Beta[0] = %ld.%0ld  (timeout)\n",
	    cPtr->Retry_Beta[0].tv_sec, cPtr->Retry_Beta[0].tv_usec);
    for (i = 1; i < cPtr->Retry_N+2; i++) 
	    fprintf(tFile, "\tRetry_Beta[%ld] = %ld.%0ld\n",
		    i, cPtr->Retry_Beta[i].tv_sec, cPtr->Retry_Beta[i].tv_usec);

    fprintf(tFile, "\tHeldPacket = 0x%lx  PeerUnique = %ld\n",
    	(long)cPtr->HeldPacket, cPtr->PeerUnique);
    fprintf(tFile, "Peer==> ");    
    rpc2_PrintHostIdent(&cPtr->PeerHost, tFile);
    fprintf(tFile, "    ");
    rpc2_PrintPortIdent(&cPtr->PeerPort, tFile);
    if (cPtr->HostInfo)
	rpc2_PrintHEntry(cPtr->HostInfo, tFile);

    fprintf(tFile, "\n");
    (void) fflush(tFile);
}

void rpc2_PrintMEntry(struct MEntry *mPtr, FILE *tFile)
{
    long i;
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    fprintf(tFile, "MyAddr: 0x%lx\n\tNextEntry = 0x%lx  PrevEntry = 0x%lx  MagicNumber = %s  Role = %s  State = ",
	(long)mPtr, (long)mPtr->Next, (long)mPtr->Prev,
	WhichMagic(mPtr->MagicNumber),
	TestRole(mPtr,FREE) ? "FREE" :(TestRole(mPtr, CLIENT) ? "CLIENT" : (TestRole(mPtr, SERVER) ? "SERVER" : "?????") ));
    if (TestRole(mPtr,CLIENT))
	switch((int) (mPtr->State & 0x0000ffff))
	    {
	    case C_THINK: fprintf(tFile, "C_THINK");break;
	    case C_AWAITREPLY: fprintf(tFile, "C_AWAITREPLY");break;
	    case C_HARDERROR: fprintf(tFile, "C_HARDERROR");break;
	    default: fprintf(tFile, "???????"); break;
	    }
    if (TestRole(mPtr,SERVER))
	switch((int) (mPtr->State & 0x0000ffff))
	    {
	    case S_AWAITREQUEST: fprintf(tFile, "S_AWAITREQUEST");break;	    
	    case S_PROCESS: fprintf(tFile, "S_PROCESS");break;	    
	    case S_REQINQUEUE: fprintf(tFile, "S_REQINQUEUE");break;
	    case S_HARDERROR: fprintf(tFile, "S_HARDERROR");break;
	    case S_INSE: fprintf(tFile, "S_INSE");break;
	    case S_AWAITENABLE: fprintf(tFile, "S_AWAITENABLE");break;
	    default: fprintf(tFile, "??????"); break;
	    }

    fprintf(tFile, "\n\tSecurityLevel = %s", mPtr->SecurityLevel == RPC2_OPENKIMONO ? "RPC2_OPENKIMONO" : 
	(mPtr->SecurityLevel == RPC2_AUTHONLY ? "RPC2_AUTHONLY" : (mPtr->SecurityLevel == RPC2_SECURE ? "RPC2_SECURE" : 
	(mPtr->SecurityLevel == RPC2_HEADERSONLY ? "RPC2_HEADERSONLY" :"??????"))));
    fprintf(tFile, "  EncryptionType = %ld  SessionKey = 0x", mPtr->EncryptionType);
    for(i = 0; i < RPC2_KEYSIZE; i++)fprintf(tFile, "%lx", (long)mPtr->SessionKey[i]);

    fprintf(tFile, "\n\tMgrpID = %ld  NextSeqNumber = %ld  SubsysID = %ld\n",
    	mPtr->MgroupID, mPtr->NextSeqNumber, mPtr->SubsysId);
	
    fprintf(tFile, "Client Host Ident:\n");
    rpc2_PrintHostIdent(&mPtr->ClientHost, tFile);
    fprintf(tFile, "Client PortIdent:\n");
    rpc2_PrintPortIdent(&mPtr->ClientPort, tFile);

    if (TestRole(mPtr,CLIENT)) {
	fprintf(tFile, "\n\tMaxlisteners = %ld  Listeners = %ld\n",
	    mPtr->me_conns.me_client.mec_maxlisteners, mPtr->me_conns.me_client.mec_howmanylisteners);
	fprintf(tFile, "IP Multicast Host Address:\n");
	rpc2_PrintHostIdent(&mPtr->IPMHost, tFile);
	fprintf(tFile, "IP Multicast Port Number:\n");
	rpc2_PrintPortIdent(&mPtr->IPMPort, tFile);
	fprintf(tFile, "Current multicast packet:\n");
	rpc2_PrintPacketHeader(mPtr->CurrentPacket, tFile);
    }
    else {
	fprintf(tFile, "Client CEntry:\n");
	rpc2_PrintCEntry(mPtr->me_conns.mes_conn, tFile);
    }
	
    fprintf(tFile, "\n");
    (void) fflush(tFile);
}


void rpc2_PrintHostIdent(RPC2_HostIdent *hPtr, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    switch (hPtr->Tag)
	{
	case RPC2_HOSTBYINETADDR:
	case RPC2_MGRPBYINETADDR:
		{
		fprintf(tFile, "Host.InetAddress = %s",
			inet_ntoa(hPtr->Value.InetAddress));
		break;	
		}
	
	case RPC2_MGRPBYNAME:
	case RPC2_HOSTBYNAME:
		fprintf(tFile, "Host.Name = \"%s\"", hPtr->Value.Name);
		break;
	
	default:	fprintf(tFile, "Host = ??????\n");
	}

    (void) fflush(tFile);
}

void rpc2_PrintPortIdent(RPC2_PortIdent *pPtr, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    switch (pPtr->Tag)
	{
	case RPC2_PORTBYINETNUMBER:
		fprintf(tFile, "Port.InetPortNumber = %u", ntohs(pPtr->Value.InetPortNumber));
		break;	
	
	case RPC2_PORTBYNAME:
		fprintf(tFile, "Port.Name = \"%s\"", pPtr->Value.Name);
		break;
	
	default:	fprintf(tFile, "Port = ??????");
	}


    (void) fflush(tFile);
}


void rpc2_PrintSubsysIdent(RPC2_SubsysIdent *Subsys, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */
    switch(Subsys->Tag) {
	case RPC2_SUBSYSBYNAME:
		say(-1, RPC2_DebugLevel, "Someone is still trying to use obsoleted RPC2_SUBSYSBYNAME\n");
		CODA_ASSERT(0);
		break;
			
	case RPC2_SUBSYSBYID:
		fprintf(tFile, "Subsys:    Tag = RPC2_SUBSYSBYID    Name = %ld\n", Subsys->Value.SubsysId);
		break;
			
	default:
		say(-1, RPC2_DebugLevel, "BOGUS Tag value in Subsys!\n");
		CODA_ASSERT(0);
    }
}

/*
 *	The packet should be in *network* byte order.
 */

void rpc2_PrintPacketHeader(RPC2_PacketBuffer *pb, FILE *tFile)
{
    if (tFile == NULL) tFile = rpc2_logfile;	/* it's ok, call-by-value */

    fprintf(tFile, "\tPrefix: BufferSize = %ld  LengthOfPacket = %ld  ",
	    pb->Prefix.BufferSize, pb->Prefix.LengthOfPacket);
    fprintf(tFile, "MagicNumber = %ld\n", (long) pb->Prefix.MagicNumber);
    fprintf(tFile, "Q = %p, RecvStamp = %ld.%06ld\n", pb->Prefix.Qname,
	    pb->Prefix.RecvStamp.tv_sec, pb->Prefix.RecvStamp.tv_usec);
    fprintf(tFile, "\tHeader: ProtoVersion = 0x%lx  RemoteHandle = 0x%lx  ",
	    (unsigned long)ntohl(pb->Header.ProtoVersion),
	    (unsigned long)ntohl(pb->Header.RemoteHandle));
    fprintf(tFile, "LocalHandle = 0x%lx  BodyLength = %lu  SeqNumber = %lu\n",
	    (unsigned long)ntohl(pb->Header.LocalHandle),
	    (unsigned long)ntohl(pb->Header.BodyLength),
	    (unsigned long)ntohl(pb->Header.SeqNumber));

    switch((int) ntohl(pb->Header.Opcode)) {
	case RPC2_INIT1OPENKIMONO:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT1OPENKIMONO");
		break;

	case RPC2_INIT1AUTHONLY:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT1AUTHONLY");
		break;

	case RPC2_INIT1SECURE:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT1SECURE");
		break;

	case RPC2_INIT1HEADERSONLY:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT1HEADERSONLY");
		break;

	case RPC2_LASTACK:
		fprintf(tFile, "\t\tOpcode = RPC2_LASTACK");
		break;

	case RPC2_REPLY:
		fprintf(tFile, "\t\tOpcode = RPC2_REPLY");
		break;

	case RPC2_BUSY:
		fprintf(tFile, "\t\tOpcode = RPC2_BUSY");
		break;

	case RPC2_INIT2:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT2");
		break;

	case RPC2_INIT3:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT3");
		break;

	case RPC2_INIT4:
		fprintf(tFile, "\t\tOpcode = RPC2_INIT4");
		break;

	case RPC2_NEWCONNECTION:
		fprintf(tFile, "\t\tOpcode = RPC2_NEWCONNECTION");
		break;

	default:
		fprintf(tFile, "\t\tOpcode = %lu",
			(unsigned long)ntohl(pb->Header.Opcode));
		break;
	}

    fprintf(tFile, "  SEFlags = 0x%lx  SEDataOffset = %lu  ",
	    (unsigned long)ntohl(pb->Header.SEFlags),
	    (unsigned long)ntohl(pb->Header.SEDataOffset));
    fprintf(tFile, "SubsysId = %lu  ReturnCode = %lu\n",
	    (unsigned long)ntohl(pb->Header.SubsysId),
	    (unsigned long)ntohl(pb->Header.ReturnCode));
    fprintf(tFile, "\t\tFlags = 0x%lx  Uniquefier = %lu  Lamport = %lu\n",
	    (unsigned long)ntohl(pb->Header.Flags),
	    (unsigned long)ntohl(pb->Header.Uniquefier),
	    (unsigned long)ntohl(pb->Header.Lamport));
    fprintf(tFile, "\t\tTimeStamp = %lu  BindTime = %lu\n",
	    (unsigned long)ntohl(pb->Header.TimeStamp),
	    (unsigned long)ntohl(pb->Header.BindTime));
    fprintf(tFile, "\n");

    (void) fflush(tFile);
}

static char *CallName(int x)
{
    switch(x)
	{
	case INIT:		return("RPC2_Init");
	case EXPORT:		return("RPC2_Export");
	case DEEXPORT:		return("RPC2_DeExport");
	case ALLOCBUFFER:	return("RPC2_AllocBuffer");
	case FREEBUFFER:	return("RPC2_FreeBuffer");
	case SENDRESPONSE:	return("RPC2_SendResponse");
	case GETREQUEST:	return("RPC2_GetRequest");
	case MAKERPC:		return("RPC2_MakeRPC");
	case BIND:		return("RPC2_NewBinding");
	case INITSIDEEFFECT:	return("RPC2_InitSideEffect");
	case CHECKSIDEEFFECT:	return("RPC2_CheckSideEffect");
	case UNBIND:		return("RPC2_Unbind");
	case GETPRIVATEPOINTER:	return("RPC2_GetPrivatePointer");
	case SETPRIVATEPOINTER:	return("RPC2_SetPrivatePointer");
	case GETSEPOINTER:	return("RPC2_GetSEPointer");
	case SETSEPOINTER:	return("RPC2_SetSEPointer");
	case GETPEERINFO:	return("RPC2_GetPeerInfo");
	case SLNEWPACKET:	return("Packet Received");
	case SENDRELIABLY:      return("rpc2_SendReliably");
	case XMITPACKET:	return("rpc2_XmitPacket");
	case CLOCKTICK:		return("Clock Tick");
	case MULTIRPC:		return("RPC2_MultiRPC");
	case MSENDPACKETSRELIABLY: return("rpc2_MSendPacketsReliably");
	case ADDTOMGRP:		return("RPC2_AddToMgrp");
	case CREATEMGRP:	return("RPC2_CreateMgrp");
	case REMOVEFROMMGRP:	return("rpc2_RemoveFromMgrp");
	case XLATEMCASTPACKET:	return("XlateMcastPacket");
	}
    return("?????");
}


void rpc2_PrintTraceElem(struct TraceElem *whichTE, long whichIndex,
			 FILE *outFile)
{
    long i;
    fprintf(outFile, "\nTrace Entry %ld:	<<<<<< %s: %s", whichIndex, whichTE->ActiveLWP, CallName(whichTE->CallCode));
    switch(whichTE->CallCode)
	{
	case SLNEWPACKET:
	case CLOCKTICK:
	    fprintf(outFile, " >>>>>>\n");
	    break;
	    
	default:
	    fprintf(outFile, "() >>>>>>\n");
	    break;
	}

    switch(whichTE->CallCode)
	{
	case INIT: break;

	case EXPORT:
		{
		struct te_EXPORT *tea;
		tea = &whichTE->Args.ExportEntry;
		if (tea->Subsys.Tag == RPC2_SUBSYSBYID)
		    fprintf(outFile, "Subsys:	Tag = RPC2_SUBSYSBYID    SubsysId = %ld\n", tea->Subsys.Value.SubsysId);
		else
		    fprintf(outFile, "Subsys:	Tag = RPC2_SUBSYSBYNAME  Name = \"%s\"\n", tea->Subsys.Value.Name);
		break;	/* switch */
		}

	case DEEXPORT:
		{
		struct te_DEEXPORT *tea;
		tea = &whichTE->Args.DeExportEntry;
		if (tea->Subsys.Tag == RPC2_SUBSYSBYID)
		    fprintf(outFile, "Subsys:	Tag = RPC2_SUBSYSBYID    SubsysId = %ld\n", tea->Subsys.Value.SubsysId);
		else
		    fprintf(outFile, "Subsys:	Tag = RPC2_SUBSYSBYNAME  Name = \"%s\"\n", tea->Subsys.Value.Name);
		break;	/* switch */
		}

	case ALLOCBUFFER:
		{
		struct te_ALLOCBUFFER *tea;
		tea = &whichTE->Args.AllocBufferEntry;
		fprintf(outFile, "MinBodySize:  %d\n", tea->MinBodySize);
		break;	/* switch */
		}

	case FREEBUFFER:
		{
		struct te_FREEBUFFER *tea;
		tea = &whichTE->Args.FreeBufferEntry;
		fprintf(outFile, "*BuffPtr:  0x%lx\n", (long)tea->BuffPtr);
		break;	/* switch */
		}

	case SENDRESPONSE:
		{
		struct te_SENDRESPONSE *tea;
		tea = &whichTE->Args.SendResponseEntry;
		fprintf(outFile, "ConnHandle: 0x%lx\n", tea->ConnHandle);
		break;	/* switch */
		}

	case GETREQUEST:
		{
		struct te_GETREQUEST *tea;
		tea = &whichTE->Args.GetRequestEntry;
		fprintf(outFile, "Filter: "); rpc2_PrintFilter(&tea->Filter, outFile);
		if (tea->IsNullBreathOfLife) fprintf(outFile, "BreathOfLife:  NULL\n");
		else fprintf(outFile, "BreathOfLife:	%ld.%ld\n", tea->BreathOfLife.tv_sec, tea->BreathOfLife.tv_usec);
		fprintf(outFile, "GetKeys: 0x%lx    EncryptionTypeMask: 0x%x\n", (long)tea->GetKeys, tea->EncryptionTypeMask);
		break;	/* switch */
		}

	case MAKERPC:
		{
		struct te_MAKERPC *tea;
		tea = &whichTE->Args.MakeRPCEntry;
		fprintf(outFile, "Conn: 0x%lx  ", tea->ConnHandle);
		fprintf(outFile, "Enqueue: %d  ", tea->EnqueueRequest);
		if (tea->IsNullBreathOfLife) fprintf(outFile, "BreathOfLife: NULL  ");
		else fprintf(outFile, "BreathOfLife: %ld.%ld  ", tea->BreathOfLife.tv_sec,
			tea->BreathOfLife.tv_usec);
		if (tea->IsNullSDesc) fprintf(outFile, "SDesc: NULL\n");
		else {fprintf(outFile, "\nSDesc: "); rpc2_PrintSEDesc(&tea->SDesc, outFile);}
		break;	/* switch */
		}

	case MULTIRPC:
		{
		struct te_MULTIRPC *tea;
		tea = &whichTE->Args.MultiRPCEntry;
		fprintf(outFile, "ConnHandle: 0x%lx\n",
			(unsigned long)tea->ConnHandle);
		fprintf(outFile, "Request:    OriginalAddress = %p    ",
			tea->Request_Address);
		rpc2_PrintPacketHeader(&tea->Request, outFile);
		if (tea->IsNullSDesc) fprintf(outFile, "SDesc:    NULL\n");
		else {fprintf(outFile, "SDesc: "); rpc2_PrintSEDesc(&tea->SDesc, outFile);}
		fprintf(outFile, "HandleResult: %p\n", tea->HandleResult);
		if (tea->IsNullBreathOfLife) fprintf(outFile, "BreathOfLife:  NULL\n");
		else fprintf(outFile, "BreathOfLife:	%ld.%ld\n", tea->BreathOfLife.tv_sec, tea->BreathOfLife.tv_usec);
		break;	/* switch */
		}

	case BIND:
		{
		struct te_BIND *tea;
		tea = &whichTE->Args.BindEntry;
		fprintf(outFile, "SecurityLevel:   %s    EncryptionType: %d\n", (tea->SecurityLevel == RPC2_OPENKIMONO) ? "RPC2_OPENKIMONO" :
			(tea->SecurityLevel == RPC2_SECURE) ? "RPC2_SECURE" : (tea->SecurityLevel == RPC2_AUTHONLY) ?
			"RPC2_ONLYAUTHENTICATE" : (tea->SecurityLevel == RPC2_HEADERSONLY) ? "RPC2_HEADERSONLY" : "????????", 
			tea->EncryptionType);
		switch (tea->Host.Tag)
		    {
		    case RPC2_HOSTBYNAME:
			fprintf(outFile, "Host:	Tag = RPC2_HOSTBYNAME    Name = \"%s\"\n", tea->Host.Value.Name);
			break;
		    
		    case RPC2_HOSTBYINETADDR:
			fprintf(outFile, "Host:     Tag = RPC2_HOSTBYINETADDR	InetAddress = %s\n",
				inet_ntoa(tea->Host.Value.InetAddress));
			break;

		    default:
			fprintf(outFile, "Host:   ?????????\n");
			break;
		    }

		switch (tea->Port.Tag)
		    {
		    case RPC2_PORTBYNAME:
			fprintf(outFile, "Port:    Tag = RPC2_PORTBYNAME    Name = \"%s\"\n", tea->Port.Value.Name);
			break;
			
		    case RPC2_PORTBYINETNUMBER:
			fprintf(outFile, "Port:    Tag = RPC2_PORTBYINETNUMBER    InetNumber = \"%u\"\n", (unsigned) tea->Port.Value.InetPortNumber);		    
			break;
			
		    default:
			fprintf(outFile, "Port:    ??????\n");
			break;
		    }


		switch(tea->Subsys.Tag)
		    {
		    case RPC2_SUBSYSBYNAME:
			fprintf(outFile, "Subsys:    Tag = RPC2_SUBSYSBYNAME    Name = \"%s\"\n", tea->Subsys.Value.Name);
			break;
			
		    case RPC2_SUBSYSBYID:
			fprintf(outFile, "Subsys:    Tag = RPC2_SUBSYSBYID    Name = %ld\n", tea->Subsys.Value.SubsysId);
			break;
			
		    default:
			fprintf(outFile, "Subsys:    ??????\n");
		    }
		    
		fprintf(outFile, "SideEffectType = %d\n", tea->SideEffectType);
		if (tea->IsNullClientIdent) fprintf(outFile, "ClientIdent:    NULL\n");
		else
		    {
		    long max;
		    fprintf(outFile, "ClientIdent:    SeqLen = %ld   SeqBody\"", tea->ClientIdent.SeqLen);
		    max = (tea->ClientIdent.SeqLen < sizeof(tea->ClientIdent_Value)) ? tea->ClientIdent.SeqLen :
		    	sizeof(tea->ClientIdent_Value);
		    for (i = 0; i < max; i++) fprintf(outFile, "%c", (tea->ClientIdent_Value)[i]);
		    if (max < tea->ClientIdent.SeqLen) fprintf(outFile, ".....");
		    fprintf(outFile, "\"\n");
		    }
		if (tea->IsNullSharedSecret) fprintf(outFile, "SharedSecret:    NULL\n");		
		else
		    {
		    fprintf(outFile, "SharedSecret:    0x");
			for (i = 0; i < sizeof(RPC2_EncryptionKey); i++) fprintf(outFile, "%lx", (long)(tea->SharedSecret)[i]);
		    fprintf(outFile, "\n");
		    }
		
		break;	/* switch */
		}

	case INITSIDEEFFECT:
		{
		struct te_INITSIDEEFFECT *tea;
		tea = &whichTE->Args.InitSideEffectEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", tea->ConnHandle);
		if (tea->IsNullSDesc) fprintf(outFile, "SDesc:    NULL\n");
		else  {fprintf(outFile, "SDesc:    "); rpc2_PrintSEDesc(&tea->SDesc, outFile); }
		break;	/* switch */
		}

	case CHECKSIDEEFFECT:
		{
		struct te_CHECKSIDEEFFECT *tea;
		tea = &whichTE->Args.CheckSideEffectEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", tea->ConnHandle);
		if (tea->IsNullSDesc) fprintf(outFile, "SDesc:    NULL\n");
		else  {fprintf(outFile, "SDesc:    ");  rpc2_PrintSEDesc(&tea->SDesc, outFile);}
		fprintf(outFile, "Flags:  { ");
		if (tea->Flags & SE_AWAITLOCALSTATUS) fprintf(outFile, "SE_AWAITLOCALSTATUS  ");
		if (tea->Flags & SE_AWAITREMOTESTATUS) fprintf(outFile, "SE_AWAITREMOTESTATUS  ");
		fprintf(outFile, "}\n");
		break;	/* switch */
		}

	case UNBIND:
		{
		struct te_UNBIND *tea;
		tea = &whichTE->Args.UnbindEntry;
		fprintf(outFile, "whichConn:    0x%lx\n", tea->whichConn);
		break;	/* switch */
		}

	case GETPRIVATEPOINTER:
		{
		struct te_GETPRIVATEPOINTER *tea;
		tea = &whichTE->Args.GetPrivatePointerEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", tea->ConnHandle);
		break;	/* switch */
		}

	case SETPRIVATEPOINTER:
		{
		struct te_SETPRIVATEPOINTER *tea;
		tea = &whichTE->Args.SetPrivatePointerEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", tea->ConnHandle);
		fprintf(outFile, "PrivatePtr:    0x%lx\n", (long)tea->PrivatePtr);
		break;	/* switch */
		}

	case GETSEPOINTER:
		{
		struct te_GETSEPOINTER *tea;
		tea = &whichTE->Args.GetSEPointerEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", tea->ConnHandle);
		break;	/* switch */
		}

	case SETSEPOINTER:
		{
		struct te_SETSEPOINTER *tea;
		tea = &whichTE->Args.SetSEPointerEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", tea->ConnHandle);
		fprintf(outFile, "SEPtr:    0x%lx\n", (long)tea->SEPtr);
		break;	/* switch */
		}

	case GETPEERINFO:
		{
		struct te_GETPEERINFO *tea;
		tea = &whichTE->Args.GetPeerInfoEntry;
		fprintf(outFile, "ConnHandle:    0x%lx\n", (long)tea->ConnHandle);
		break;	/* switch */
		}

	case SLNEWPACKET:
		{
		struct te_SLNEWPACKET *tea;
		tea = &whichTE->Args.SLNewPacketEntry;
		rpc2_PrintPacketHeader(&tea->pb, outFile);
		break;	/* switch */
		}

	case SENDRELIABLY:
		{
		struct te_SENDRELIABLY *tea;
		tea = &whichTE->Args.SendReliablyEntry;
		fprintf(outFile, "Conn.UniqueCID = 0x%x    ",tea->Conn_UniqueCID);
		if (tea->IsNullTimeout) fprintf(outFile, "TimeOut:    NULL\n");
		else fprintf(outFile, "TimeOut:	%ld.%ld\n", tea->Timeout.tv_sec, tea->Timeout.tv_usec);
		break;	/* switch */
		}

	case MSENDPACKETSRELIABLY:
		{
		struct te_MSENDPACKETSRELIABLY *tea;
		tea = &whichTE->Args.MSendPacketsReliablyEntry;
		fprintf(outFile, "HowMany:    %d    ConnArray[0]:    %p    ConnArray[0].UniqueCID = 0x%x\n",
			tea->HowMany, tea->ConnArray0, tea->ConnArray0_UniqueCID);
		fprintf(outFile, "PacketArray[0]:    OriginalAddress = 0x%lx    ", (long)tea->PacketArray0_Address);
		rpc2_PrintPacketHeader(&tea->PacketArray0, outFile);
		if (tea->IsNullTimeout) fprintf(outFile, "TimeOut:    NULL\n");
		else fprintf(outFile, "TimeOut:	%ld.%ld\n", tea->Timeout.tv_sec, tea->Timeout.tv_usec);
		break;	/* switch */
		}

	case XMITPACKET:
		{
		struct te_XMITPACKET *tea;
		tea = &whichTE->Args.XmitPacketEntry;
		fprintf(outFile, "whichSocket = %ld\n", tea->whichSocket);
		fprintf(outFile, "whichHost:    "); rpc2_PrintHostIdent(&tea->whichHost, outFile);
		fprintf(outFile, "    ");
		fprintf(outFile, "whichPort:    "); rpc2_PrintPortIdent(&tea->whichPort, outFile);
		fprintf(outFile,"\n");
		rpc2_PrintPacketHeader(&tea->whichPB, outFile);
		break;	/* switch */
		}

	case CLOCKTICK:
		{
		struct te_CLOCKTICK *tea;
		tea = &whichTE->Args.ClockTickEntry;
		fprintf(outFile, "TimeNow:    %d\n", tea->TimeNow);
		break;	/* switch */
		}

	case CREATEMGRP:
		{
		struct te_CREATEMGRP *tea;
		tea = &whichTE->Args.CreateMgrpEntry;
		fprintf(outFile, "MgroupHandle: %ld\n", tea->MgroupHandle);
		fprintf(outFile, "McastHost:      ");
		rpc2_PrintHostIdent((RPC2_HostIdent *)&(tea->McastHost), outFile);
		fprintf(outFile, "           ");
		fprintf(outFile, "McastPort:      ");
		rpc2_PrintPortIdent(&(tea->Port), outFile);
		fprintf(outFile, "           ");
		fprintf(outFile, "Subsystem:        ");
		rpc2_PrintSubsysIdent(&(tea->Subsys), outFile);
		fprintf(outFile, "           ");
		fprintf(outFile, "SecurityLevel = %s", tea->SecurityLevel == RPC2_OPENKIMONO ? "RPC2_OPENKIMONO" : (tea->SecurityLevel == RPC2_AUTHONLY ? "RPC2_AUTHONLY" : (tea->SecurityLevel == RPC2_SECURE ? "RPC2_SECURE" : (tea->SecurityLevel == RPC2_HEADERSONLY ? "RPC2_HEADERSONLY" :"??????"))));
		fprintf(outFile, "  IsEncrypted = %s  ", (tea->IsEncrypted) ? "TRUE" : "FALSE");
		fprintf(outFile, "  EncryptionType = %ld  SessionKey = 0x", tea->EncryptionType);
		for(i = 0; i < RPC2_KEYSIZE; i++)fprintf(outFile, "%lx", (long)tea->SessionKey[i]);
		fprintf(outFile, "\n");
		break; /* switch */
		}

	case ADDTOMGRP:
		{
		struct te_ADDTOMGRP *tea;
		tea = &whichTE->Args.AddToMgrpEntry;
		fprintf(outFile, "MgroupHandle:   %ld     ConnHandle:   %ld\n", tea->MgroupHandle,
			tea->ConnHandle);
		break; /* switch */
		}

	case REMOVEFROMMGRP:
		{
		struct te_REMOVEFROMMGRP *tea;
		tea = &whichTE->Args.RemoveFromMgrpEntry;
		fprintf(outFile, "MEntry:      "); rpc2_PrintMEntry(&tea->me, outFile);
		fprintf(outFile, "        ");
		fprintf(outFile, "CEntry:      "); rpc2_PrintCEntry(&tea->ce, outFile);
		fprintf(outFile, "\n");
		break; /* switch */
		}

	case XLATEMCASTPACKET:
		{
		struct te_XLATEMCASTPACKET *tea;
		tea = &whichTE->Args.XlateMcastPacketEntry;
		fprintf(outFile, "PacketBuffer Address:  0x%lx      PacketHeader:     ",
			tea->pb_address);
		rpc2_PrintPacketHeader(&tea->pb, outFile);
		fprintf(outFile, "         ClientHost:      ");
		rpc2_PrintHostIdent((RPC2_HostIdent *)&tea->ThisHost, outFile);
		fprintf(outFile, "         ClientPort:     ");
		rpc2_PrintPortIdent(&tea->ThisPort, outFile);
		fprintf(outFile, "\n");
		break; /* switch */
		}

	}
    
}

void rpc2_PrintSEDesc(SE_Descriptor *whichSDesc, FILE *whichFile)
{
    long i;
    if (whichFile == NULL) whichFile = rpc2_logfile;	/* it's ok, call by value */
    for (i = 0; i < SE_DefCount; i++)
	if (SE_DefSpecs[i].SideEffectType == whichSDesc->Tag) break;
    if (i >= SE_DefCount) return; /* Bogus side effect */
    (*SE_DefSpecs[i].SE_PrintSEDescriptor)(whichSDesc, whichFile);
}
#endif RPC2DEBUG
