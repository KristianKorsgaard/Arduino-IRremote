
//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
// Edited by Mitra to add new controller SANYO
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
//******************************************************************************

#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
// The ISR header contains several useful macros the user may wish to use
//
#include "IRremoteInt.h"

//------------------------------------------------------------------------------
// Supported IR protocols
// Each protocol you include costs memory and, during decode, costs time
// Disable (set to 0) all the protocols you do not need/want!

#ifndef EXCLUDE_ALL

#define DECODE_RC5
#define SEND_RC5
// #define EMULATE_RC5          // NOT IMPLEMENTED YET

#define DECODE_RC6
#define SEND_RC6
// #define EMULATE_RC6          // NOT IMPLEMENTED YET

#define DECODE_NEC
#define SEND_NEC
#define EMULATE_NEC

#define DECODE_SONY
#define SEND_SONY
// #define EMULATE_SONY         // NOT IMPLEMENTED YET

#define DECODE_PANASONIC
#define SEND_PANASONIC
// #define EMULATE_PANASONIC    // NOT IMPLEMENTED YET

#define DECODE_JVC
#define SEND_JVC
// #define EMULATE_JVC          // NOT IMPLEMENTED YET

#define DECODE_SAMSUNG
#define SEND_SAMSUNG
// #define EMULATE_SAMSUNG      // NOT IMPLEMENTED YET

#define DECODE_WHYNTER
#define SEND_WHYNTER
// #define EMULATE_WHYNTER      // NOT IMPLEMENTED YET

#define DECODE_AIWA_RC_T501
#define SEND_AIWA_RC_T501
// #define EMULATE_AIWA_RC_T501 // NOT IMPLEMENTED YET

#define DECODE_LG
#define SEND_LG
// #define EMULATE_LG           // NOT IMPLEMENTED YET

#define DECODE_SANYO
// #define SEND_SANYO           // NOT WRITTEN
// #define EMULATE_SANYO        // NOT WRITTEN

#define DECODE_MITSUBISHI
// #define SEND_MITSUBISHI      // NOT WRITTEN
// #define EMULATE_MITSUBISHI   // NOT WRITTEN

// #define DECODE_DISH          // NOT WRITTEN
#define SEND_DISH
// #define EMULATE_DISH         // NOT IMPLEMENTED YET

// #define DECODE_SHARP         // NOT WRITTEN
#define SEND_SHARP
// #define EMULATE_SHARP        // NOT IMPLEMENTED YET

#define DECODE_DENON
#define SEND_DENON
// #define EMULATE_DENON        // NOT IMPLEMENTED YET

// #define DECODE_PRONTO        // This function doe not logically make sense
#define SEND_PRONTO
// #define EMULATE_PRONTO       // NOT IMPLEMENTED YET

// #define DECODE_LEGO_PF       // NOT WRITTEN
#define SEND_LEGO_PF
// #define EMULATE_LEGO_PF      // NOT IMPLEMENTED YET

#endif

//------------------------------------------------------------------------------
// When sending a Pronto code we request to send either the "once" code
//                                                   or the "repeat" code
// If the code requested does not exist we can request to fallback on the
// other code (the one we did not explicitly request)
//
// I would suggest that "fallback" will be the standard calling method
// The last paragraph on this page discusses the rationale of this idea:
//   http://www.remotecentral.com/features/irdisp2.htm
//
// #define PRONTO_ONCE
#define PRONTO_REPEAT
#define PRONTO_FALLBACK
// #define PRONTO_NOFALLBACK

//------------------------------------------------------------------------------
// An enumerated list of all supported formats
// You do NOT need to remove entries from this list when disabling protocols!
//
typedef
	enum {
		UNKNOWN      = -1,
		UNUSED       =  0,
		RC5,
		RC6,
		NEC,
		SONY,
		PANASONIC,
		JVC,
		SAMSUNG,
		WHYNTER,
		AIWA_RC_T501,
		LG,
		SANYO,
		MITSUBISHI,
		DISH,
		SHARP,
		DENON,
		PRONTO,
		LEGO_PF,
	}
decode_type_t;

//------------------------------------------------------------------------------
// Uncomment DEBUG for lots of lovely debug output
//
// #define DEBUG

//------------------------------------------------------------------------------
// Debug directives
//
#ifdef DEBUG
#	define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#	define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
#	define DBG_PRINT(...)
#	define DBG_PRINTLN(...)
#endif

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int  MATCH       (int measured, int desired) ;
int  MATCH_MARK  (int measured_ticks, int desired_us) ;
int  MATCH_SPACE (int measured_ticks, int desired_us) ;

//------------------------------------------------------------------------------
// Results returned from the decoder
//
class decode_results
{
	public:
		decode_type_t          decode_type;  // UNKNOWN, NEC, SONY, RC5, ...
		unsigned int           address;      // Used by Panasonic & Sharp [16-bits]
		unsigned long          value;        // Decoded value [max 32-bits]
		int                    bits;         // Number of bits in decoded value
		volatile unsigned int  *rawbuf;      // Raw intervals in 50uS ticks
		int                    rawlen;       // Number of records in rawbuf
		int                    overflow;     // true iff IR raw code too long
};

//------------------------------------------------------------------------------
// Decoded value for NEC when a repeat code is received
//
#ifdef DECODE_NEC
	#define REPEAT 0xFFFFFFFF
#endif

//------------------------------------------------------------------------------
// Main class for receiving IR
//
class IRrecv
{
	public:
		IRrecv (int recvpin) ;
		IRrecv (int recvpin, int blinkpin);

		void  blink13    (int blinkflag) ;
		int   decode     (decode_results *results) ;
		void  enableIRIn ( ) ;
		bool  isIdle     ( ) ;
		void  resume     ( ) ;

	private:
		long  decodeHash (decode_results *results) ;
		int   compare    (unsigned int oldval, unsigned int newval) ;

		//......................................................................
#		if defined(DECODE_RC5) || defined(DECODE_RC6)
			// This helper function is shared by RC5 and RC6
			int  getRClevel (decode_results *results,  int *offset,  int *used,  int t1) ;
#		endif
#		ifdef DECODE_RC5
			bool  decodeRC5        (decode_results *results) ;
#		endif
#		ifdef DECODE_RC6
			bool  decodeRC6        (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_NEC
			bool  decodeNEC        (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_SONY
			bool  decodeSony       (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_PANASONIC
			bool  decodePanasonic  (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_JVC
			bool  decodeJVC        (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_SAMSUNG
			bool  decodeSAMSUNG    (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_WHYNTER
			bool  decodeWhynter    (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_AIWA_RC_T501
			bool  decodeAiwaRCT501 (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_LG
			bool  decodeLG         (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_SANYO
			bool  decodeSanyo      (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_MITSUBISHI
			bool  decodeMitsubishi (decode_results *results) ;
#		endif
		//......................................................................
#		ifdef DECODE_DISH
			bool  decodeDish (decode_results *results) ; // NOT WRITTEN
#		endif
		//......................................................................
#		ifdef DECODE_SHARP
			bool  decodeSharp (decode_results *results) ; // NOT WRITTEN
#		endif
		//......................................................................
#		ifdef DECODE_DENON
			bool  decodeDenon (decode_results *results) ;
#		endif
//......................................................................
#		ifdef DECODE_LEGO_PF
			bool  decodeLegoPowerFunctions (decode_results *results) ;
#		endif
} ;

//------------------------------------------------------------------------------
// Main class for sending IR
//
class IRsend
{
	public:
		IRsend () { }

		void  custom_delay_usec (unsigned long uSecs);
		void  enableIROut 		(int khz) ;
		void  mark        		(unsigned int usec) ;
		void  space       		(unsigned int usec) ;
		void  sendRaw     		(const unsigned int buf[],  unsigned int len,  unsigned int hz) ;

		//......................................................................
#		ifdef SEND_RC5
			void  sendRC5        (unsigned long data,  int nbits) ;
#		endif
#		ifdef SEND_RC6
			void  sendRC6        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_NEC
			void  sendNEC        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_SONY
			void  sendSony       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_PANASONIC
			void  sendPanasonic  (unsigned int address,  unsigned long data) ;
#		endif
		//......................................................................
#		ifdef SEND_JVC
			// JVC does NOT repeat by sending a separate code (like NEC does).
			// The JVC protocol repeats by skipping the header.
			// To send a JVC repeat signal, send the original code value
			//   and set 'repeat' to true
			void  sendJVC        (unsigned long data,  int nbits,  bool repeat) ;
#		endif
		//......................................................................
#		ifdef SEND_SAMSUNG
			void  sendSAMSUNG    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_WHYNTER
			void  sendWhynter    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_AIWA_RC_T501
			void  sendAiwaRCT501 (int code) ;
#		endif
		//......................................................................
#		ifdef SEND_LG
			void  sendLG         (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_SANYO
			void  sendSanyo      ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		ifdef SEND_MISUBISHI
			void  sendMitsubishi ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		ifdef SEND_DISH
			void  sendDISH       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_SHARP
			void  sendSharpRaw   (unsigned long data,  int nbits) ;
			void  sendSharp      (unsigned int address,  unsigned int command) ;
#		endif
		//......................................................................
#		ifdef SEND_DENON
			void  sendDenon      (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef SEND_PRONTO
			void  sendPronto     (char* code,  bool repeat,  bool fallback) ;
#		endif
//......................................................................
#		ifdef SEND_LEGO_PF
			void  sendLegoPowerFunctions (uint16_t data, bool repeat = true) ;
#		endif
} ;

//------------------------------------------------------------------------------
// Main class for emulating IR receiver
//
class IRemulate
{
	public:
		IRemulate (int emulatepin, bool solesource = true);

		void  custom_delay_usec (unsigned long uSecs);
		void  enableIROut 		() ;
		void  disableIROut 		() ;
		void  mark        		(unsigned int usec) ;
		void  space       		(unsigned int usec) ;
		void  emulateRaw     		(const unsigned int buf[],  unsigned int len) ;

		//......................................................................
#		ifdef EMULATE_RC5
			// void  emulateRC5        (unsigned long data,  int nbits) ;
#		endif
#		ifdef EMULATE_RC6
			// void  emulateRC6        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_NEC
			void  emulateNEC        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_SONY
			// void  emulateSony       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_PANASONIC
			// void  emulatePanasonic  (unsigned int address,  unsigned long data) ;
#		endif
		//......................................................................
#		ifdef EMULATE_JVC
			// JVC does NOT repeat by sending a separate code (like NEC does).
			// The JVC protocol repeats by skipping the header.
			// To send a JVC repeat signal, send the original code value
			//   and set 'repeat' to true
			// void  emulateJVC        (unsigned long data,  int nbits,  bool repeat) ;
#		endif
		//......................................................................
#		ifdef EMULATE_SAMSUNG
			// void  emulateSAMSUNG    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_WHYNTER
			// void  emulateWhynter    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_AIWA_RC_T501
			// void  emulateAiwaRCT501 (int code) ;
#		endif
		//......................................................................
#		ifdef EMULATE_LG
			// void  emulateLG         (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_SANYO
			// void  emulateSanyo      ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		ifdef EMULATE_MISUBISHI
			// void  emulateMitsubishi ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		ifdef EMULATE_DISH
			// void  emulateDISH       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_SHARP
			// void  emulateSharpRaw   (unsigned long data,  int nbits) ;
			// void  emulateSharp      (unsigned int address,  unsigned int command) ;
#		endif
		//......................................................................
#		ifdef EMULATE_DENON
			// void  emulateDenon      (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		ifdef EMULATE_PRONTO
			// void  emulatePronto     (char* code,  bool repeat,  bool fallback) ;
#		endif
//......................................................................
#		ifdef EMULATE_LEGO_PF
			// void  emulateLegoPowerFunctions (uint16_t data, bool repeat = true) ;
#		endif

	private:
		byte emulatepin_;
		bool solesource_; // Used for specifying if emulator is sole signal source
		                  // or if it runs on top of an actual IR receiver
} ;

#endif
