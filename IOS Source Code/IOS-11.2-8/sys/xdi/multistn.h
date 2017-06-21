/* $Id: multistn.h,v 3.2 1995/11/17 18:11:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/multistn.h,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: multistn.h,v $
 * Revision 3.2  1995/11/17  18:11:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_multistn.h xm7_v3.1.1:cset.0166:8:8 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		XLNT Manager Header File
		
		Multistation Function Call Conversion File
		
		File:			xm7_multistn.h
		Created:		12/13/91

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This header file is used by all Manager core modules. The file contains
		macros used to convert multistation interface calls made from within
		the core code into single station interface calls if the code
		is compiled for a single station target.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/


/*
*	By defining the manifest constant MULTIPLE_STATION, the compiler
*	is requesting a multistation configuration. This is the default
*	configuration for the core code. By not defining this constant
*	(which would be the default compiler command line), the core
*	code assumes that the interface routines are using the older
*	single station parameter interface. In this case, these macros
*	convert the multistation calls into the single station calls.
*/

#ifndef MULTIPLE_STATION
/*
*	CSP interface call translations.
*/
#define InitCSPInterruptsMulti(stnID)\
			InitCSPInterrupts()
#define InitCSPLocalMulti(stnID)\
			InitCSPLocal()
#define InitCSPMessageMulti(stnID)\
			InitCSPMessage()
#define CSPGetMIBAttrMulti(stnID, bufSize, bufData)\
			CSPGetMIBAttr(bufSize, bufData)
#define StationPathTestMulti(stnID)\
			StationPathTest()
#define BypassRequestMulti(stnID, bypassState)\
			BypassRequest(bypassState)
#define ConfigureMasterMulti(stnID, portPtr, cfmState)\
			ConfigureMaster(portPtr, cfmState)
#define ConfigureAttachMulti(stnID, CF_State)\
			ConfigureAttach(CF_State)
#define TransmitPMDMulti(stnID, portID, state)\
			TransmitPMD(portID, state)
#define SetPORTInterruptsMulti(stnID, portID, state)\
			SetPORTInterrupts(portID, state)
#define ReadLineStateMulti(stnID, portID)\
			ReadLineState(portID)
#define SendLineStateMulti(stnID, portID, ls)\
			SendLineState(portID, ls)
#define MACSetInterruptsMulti(stnID, macID, state)\
			MACSetInterrupts(macID, state)
#define MACResetRequestMulti(stnID, macID)\
			MACResetRequest(macID)
#define MACClaimRequestMulti(stnID, macID)\
			MACClaimRequest(macID)
#define SetBeaconMulti(stnID, macID, beaconType, DA, infoLength, beaconInfo)\
			SetBeacon(macID, beaconType, DA, infoLength, beaconInfo)
#define BeaconRequestMulti(stnID, macID)\
			BeaconRequest(macID)
#define ChangeMACAddressMulti(stnID, macID, newAddress)\
			ChangeMACAddress(macID, newAddress)
#define ChangeMACTBidMulti(stnID, macID, newTBid)\
			ChangeMACTBid(macID, newTBid)
#define SetMACInputMulti(stnID, macID, state)\
			SetMACInput(macID, state)
#define ConfigureMACMulti(stnID, macID, state, pathID, TVXLB, T_MaxLB, MaxT_Req)\
			ConfigureMAC(macID, state, pathID, TVXLB, T_MaxLB, MaxT_Req)
#define ReadMACR_FlagMulti(stnID, macID)\
			ReadMACR_Flag(macID)
#define SetLEMCounterMulti(stnID, portID, threshold)\
			SetLEMCounter(portID, threshold)
#define ReadLEMCounterMulti(stnID, portID)\
			ReadLEMCounter(portID)
#define SetLEMStateMulti(stnID, portID, state)\
			SetLEMState(portID, state)
#define SetupMACLCTMulti(stnID, portID)\
			SetupMACLCT(portID)
#define FinishMACLCTMulti(stnID, portID)\
			FinishMACLCT(portID)
#define StartScrubbingMulti(stnID, pathID)\
			StartScrubbing(pathID)

/*
*	Hardware-assisted PCM Interface Call Translations
*/
#define	ReadPHYRegMulti(stnID,sigEntity,reg)\
			ReadPHYReg(sigEntity,reg)
#define	WritePHYRegMulti(stnID,sigEntity,reg,regval)\
			WritePHYReg(sigEntity,reg,regval)


/*
*	FSP interface call translations
*/

#define LocalESFAnnouncementMulti(stnID, memPtr, frameLen, MACNum, EACbits)\
			LocalESFAnnouncement(memPtr, frameLen, MACNum, EACbits)
#define LocalESFRequestMulti(stnID, memPtr, frameLen, MACNum, EACbits)\
			LocalESFRequest(memPtr, frameLen, MACNum, EACbits)
#define LocalESFResponseMulti(stnID, memPtr, frameLen, MACNum, EACbits)\
			LocalESFResponse(memPtr, frameLen, MACNum, EACbits)
#define InitESFLocalMulti(stnID)\
			InitESFLocal()
#define SendSMTFrameMulti(stnID, frameBuffer, frameLength, MACNum)\
			SendSMTFrame(frameBuffer, frameLength, MACNum)
#define InitFBMFrameMulti(stnID)\
			InitFBMFrame()
#define InitFBMInterruptsMulti(stnID)\
			InitFBMInterrupts()
#define LocalFBMNotificationMulti(stnID, mibPtr, ecPtr)\
			LocalFBMNotification(mibPtr, ecPtr)
#define LocalFBMFramesMulti(stnID, memPtr, frameLen, MACNum, EACbits)\
			LocalFBMFrames(memPtr, frameLen, MACNum, EACbits)
#define ProcessLocalTickMulti(stnID)\
			ProcessLocalTick()
#define InitFBMLocalMulti(stnID)\
			InitFBMLocal()
#define InitFBMMessageMulti(stnID)\
			InitFBMMessage()
#define FBMGetMIBAttrMulti(stnID, bufSize, bufData)\
			FBMGetMIBAttr(bufSize, bufData)
#define FBMMultiGetMIBAttrMulti(stnID, bufSize, bufData, setCount)\
			FBMMultiGetMIBAttr(bufSize, bufData, setCount)
#define FBMChangeMIBAttrMulti(stnID, bufSize, bufData, setCount, stationID)\
			FBMChangeMIBAttr(bufSize, bufData, setCount, stationID)
#define LocalRAFRequestMulti(stnID, memPtr, frameLen, MACNum, EACbits)\
			LocalRAFRequest(memPtr, frameLen, MACNum, EACbits)
#define LocalRAFResponseMulti(stnID, memPtr, frameLen, MACNum, EACbits)\
			LocalRAFResponse(memPtr, frameLen, MACNum, EACbits)
#define InitRAFLocalMulti(stnID)\
			InitRAFLocal()
#define InitFBMTimerMulti(stnID)\
			InitFBMTimer()
#define VerifyAuthorizationMulti(stnID, aBuffer, aLen, memPtr, frameLen, macID)\
			VerifyAuthorization(aBuffer, aLen, memPtr, frameLen, macID)
#define BuildAuthorizationMulti(stnID, aBuffer, frameHdr, maxLen, macID)\
			BuildAuthorization(aBuffer, frameHdr, maxLen, macID)

/*
*	MSP Interface Call Translations
*/
#define CreateMIBMulti(stnID)\
			CreateMIB()
#define BuildSMTMIBMulti(stnID, mib)\
			BuildSMTMIB(mib)
#define BuildMACMIBMulti(stnID)\
			BuildMACMIB()
#define BuildPATHMIBMulti(stnID)\
			BuildPATHMIB()
#define BuildPORTMIBMulti(stnID)\
			BuildPORTMIB()
#define BuildConfigMulti(stnID)\
			BuildConfig()
#define MIBInterruptHandler(stnID)\
			MIBInterruptHandler()
#define InitMIBInterruptsMulti(stnID)\
			InitMIBInterrupts()
#define InitMIBLocalMulti(stnID)\
			InitMIBLocal()
#define DetectVendorNotificationsMulti(stnID, mibPtr)\
			DetectVendorNotifications(mibPtr)
#define	LocalMIBEventMulti(stnID, mibPtr, paramType, xdiIndex, vendorID)\
			LocalMIBEvent(mibPtr, paramType, xdiIndex, vendorID)
#define InitMIBMessageMulti(stnID)\
			InitMIBMessage()
#define InitTimeStampMulti(stnID)\
			InitTimeStamp()
#define GetTimeStampMulti(stnID, buf)\
			GetTimeStamp(buf)
#define SetSMTDataMulti(stnID, attr, mibPtr, buffer)\
			SetSMTData(attr, mibPtr, buffer)
#define GetMACDataMulti(stnID, attr, macPtr, buffer)\
			GetMACData(attr, macPtr, buffer)
#define GetMACAddressesMulti(stnID, macPtr, bufSize, paramData)\
			GetMACAddresses(macPtr, bufSize, paramData)
#define SetMACDataMulti(stnID, attr, macPtr, buffer)\
			SetMACData(attr, macPtr, buffer)
#define GetPATHDataMulti(stnID, attr, pathPtr, buffer)\
			GetPATHData(attr, pathPtr, buffer)
#define SetPATHDataMulti(stnID, attr, pathPtr, buffer)\
			SetPATHData(attr, pathPtr, buffer)
#define GetPORTDataMulti(stnID, attr, portPtr, buffer)\
			GetPORTData(attr, portPtr, buffer)
#define SetPORTDataMulti(stnID, attr, portPtr, buffer)\
			SetPORTData(attr, portPtr, buffer)
#define GetVendorSMTAttrMulti(stnID, bufSize, p, smtPtr)\
			GetVendorSMTAttr(bufSize, p, smtPtr)
#define GetVendorMACAttrMulti(stnID, bufSize, p, macPtr)\
			GetVendorMACAttr(bufSize, p, macPtr)
#define GetVendorPATHAttrMulti(stnID, bufSize, p, pathPtr)\
			GetVendorPATHAttr(bufSize, p, pathPtr)
#define GetVendorPORTAttrMulti(stnID, bufSize, p, portPtr)\
			GetVendorPORTAttr(bufSize, p, portPtr)
#define ChangeVendorSMTAttrMulti(stnID, p, smtPtr)\
			ChangeVendorSMTAttr(p, smtPtr)
#define ChangeVendorMACAttrMulti(stnID, p, macPtr)\
			ChangeVendorMACAttr(p, macPtr)
#define ChangeVendorPATHAttrMulti(stnID, p, pathPtr)\
			ChangeVendorPATHAttr(p, pathPtr)
#define ChangeVendorPORTAttrMulti(stnID, p, portPtr)\
			ChangeVendorPORTAttr(p, portPtr)

#ifdef	XM_USE_VSO

/*
 * Definitions for MASIC translations
 */
#ifdef	XM_USE_VSO_MASIC

#define InitMASICObjectMulti(stnID)\
			InitMASICObject()
#define BuildMASICMIBMulti(stnID)\
			BuildMASICMIB()
#define GetMASICDataMulti(stnID, attr, masicPtr, buffer)\
			GetMASICData(attr, masicPtr, buffer)
#define SetMASICDataMulti(stnID, attr, masicPtr, p)\
			SetMASICData (attr, masicPtr, p)

#endif							/* XM_USE_VSO_MASIC */

/*
 * Definitions for other vendor-specific object translations
 */
#ifdef	XM_USE_VSO_OTHER

#define BuildVSOMIBMulti(stnID)\
			BuildVSOMIB()
#define InitVSOMulti(stnID)\
			InitVSO()

#endif							/* XM_USE_VSO_OTHER */

#endif							/* XM_USE_VSO */

#endif							/* MULTIPLE_STATION */
