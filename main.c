/* mehPL:
 *    This is Open Source, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 ************************************************************************
 **  MICROCHIP
 **   This took quite a bit of time and effort on my part, and I'm broke.
 **   PLEASE send me some swag...
 **   Or, yahknow, a remote/telecommuting/contracting gig would be awesome.
 ************************************************************************
 */




// This test is to confirm math errors with the xc32-gcc optimizer
//
// In general, the problem exists when using int8_t
//   (it may also exist with other small integer types, this is untested)
//
// e.g. 
//  uint8_t power = 127;
//  int8_t direction = -1;
//  < some code that doesn't always explicitly modify power or direction >
//  int16_t signedPower = (int16_t)power * (int16_t)direction;
//
//  Depending on the circumstances, the int8_t may act as though it
//  is of a larger type that hasn't been properly casted.
//
//  e.g. signedPower = power * direction    may result in:
//       signedPower = 127 * 255 (0xff) = 32385
//  when it should be:
//       signedPower = 127 * -1 (0xff) = -127
//
// This program has determined that the error 
//   (int8_t being treated as a larger integer type) 
//  also occurs in other circumstances, 
//  e.g. in a simple "printf()" statement.
//
// This was discovered on a PIC32MX230F064
// and is now being tested on a PIC32MX170F256


// RA0 is used as an indicator LED
// RB15 is the UART1 TX output, used as stdout at 9600bps







//#######   TEST CONFIGURATION OPTIONS   ########


//####### PRINT_IT_FUNCTION #########
//
//  When PRINT_IT_FUNCTION is defined (and TRUE),
//   PRINT_IT() is a function.
//  When PRINT_IT_FUNCTION is NOT defined,
//   PRINT_IT() is a macro.
//
// WHEN CALLED AS A MACRO:
//  Printout shows several bugs:
//   Bug1: PRINTOUT: 'PRId8' of (int8_t)dir=-1 results in '255'
//         (in certain cases, as seen).
//   Bug2: MATH: (power*dir)=(127 * -1) results in 32385
//         This is not just a printout error, this math is *used*
//
// WHEN CALLED AS A FUNCTION:
//  Printout is different
//  (Likely due to loading into new registers)
//  BUG2, from above, still occurs, showing that MATH IS WRONG,
//   And, *used* as-such.
//
// COMMENT OR UNCOMMENT AS DESIRED:
//#define PRINT_IT_FUNCTION 1


//#######  INCLUDE_RAND  #######
//
//  When INCLUDE_RAND is defined (and TRUE),
//   the variable 'dir' is assigned a new value on a random basis
//   This Intentionally SELDOMLY occurs (almost never).
//   The point is to demonstrate that the math-error does not present
//   itself unless the value of the variable 'dir' *might be* changed.
//  When INCLUDE_RAND is NOT defined,
//   The variable 'dir' is never reassigned a different value.
//   Printout and values are as-expected.
//
//  THUS: Presumably, the error does not present itself without this piece
//   of code DUE TO the optimizer's ability to recognize that it could work
//   with constants and never actually assign the (int8_t) value to a 
//   register.
//
//  Then, Presumably: Once the register is used, the value appears to be 
//  treated as the larger-type. Likely the registers' native 32-bit type.
//  FINE (right?) EXCEPT: the value -1 should then be assigned to the 
//  register as 0xffffffff. Instead, it seems to be assigned as 0x000000ff.
//
//NOTE: IF INCLUDE_RAND is not defined (and true), 
//      THEN MATH AND PRINTOUTS WORK AS EXPECTED
// COMMENT OR UNCCOMENT AS DESIRED:
#define INCLUDE_RAND 1




#ifndef DESKTOP_TEST
#include <xc.h>
#endif


#include <stdio.h>
#include <stdlib.h> //for rand()
#include <inttypes.h>
//#include <plib.h> //DNE on my system




#ifndef DESKTOP_TEST
//Use the internal R/C oscillator:
//This should be 48MHz... (FRC+PLL = 4MHz, * MUL_24 / DIV2)
#pragma config FPLLMUL = MUL_24,\
               FPLLODIV = DIV_2,\
               FWDTEN = OFF
#pragma config POSCMOD = OFF,\
               FNOSC = FRCPLL,\
               FPBDIV = DIV_2, \
               OSCIOFNC = ON


//Make sure we don't have difficulty reconfiguring the PPS for the UART
#pragma config IOL1WAY = OFF // Allow multiple reconfigurations

#endif






#if PRINT_IT_FUNCTION
  void PRINT_IT(int ID, uint8_t power, int8_t dir, int16_t signedPower)
  {

    printf("  %d: power(u8) = %" PRIu8 
           "   dir(i8) = %" PRId8 
           "   signedPower(i16) = %" PRIi16 "\n\r", 
           ID, 
           power, 
           dir, 
           signedPower 
          ); 
  }

#else //! PRINT_IT_FUNCTION


  #define PRINT_IT(ID, power, dir, signedPower) \
  ({ \
    printf("  %d: power(u8) = %" PRIu8 \
           "   dir(i8) = %" PRId8 \
           "   signedPower(i16) = %" PRIi16 "\n\r", \
           ID, \
           power, \
           dir, \
           signedPower \
          ); \
   {}; \
  })

#endif //PRINT_IT_FUNCTION (vs. Macro)















//This function *should* Print the following, in all cases:
//  1: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  2: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  3: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//
//THE FOLLOWING RESULTS are with INCLUDE_RAND defined (and TRUE)
//  in xc32-gcc 1.40 (linux):
//
//When PRINT_IT_FUNCTION is defined (and TRUE), it prints:
//  1: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  2: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  3: power(u8) = 127   dir(i8) = -1   signedPower(i16) = 32385
//
//When PRINT_IT_FUNCTION is NOT defined (PRINT_IT() is a macro), it prints:
//  1: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  2: power(u8) = 127   dir(i8) = 255   signedPower(i16) = -127
//  3: power(u8) = 127   dir(i8) = 255   signedPower(i16) = 32385

//NOTE: Many tests have been performed (no longer included in this code)
// Including:
//  dir = (int8_t)dir;
//  printf(...PRIi8..., (int8_t)dir, ...);
//  dir = (dir & 0x000000ff);
//  various castings in the assignment of signedPower, including:
//   int32_t signedPower = (int32_t)dir * (int32_t)power;
//   int16_t signedPower = (int16_t)dir * (int32_t)power;
// None of these clear up the bug(s).
void testBrokenMathAndPrintout(void)
{
   uint8_t power = 127;
   int8_t dir = -1; 


   int16_t signedPower = (int16_t)power * (int16_t)dir;

   
   PRINT_IT(1, power, dir, signedPower);


#if INCLUDE_RAND
   //This case will SELDOMLY be *executed*
   // but without some potential reassignment to the variable 'dir' 
   // (somewhere)
   // the bug does not appear
   if(rand() == 0)
   {
      printf("!!! rand() == 0, else{} executed!\n\r");
      
      //THIS VALUE IS NOT EXPECTED TO BE USED
      // SEE NOTES FOR 'INCLUDE_RAND'
      dir = ((int8_t)(0));
   }
#endif //INCLUDE_RAND


   PRINT_IT(2, power, dir, signedPower);


   signedPower = (int16_t)power * (int16_t)dir;

   PRINT_IT(3, power, dir, signedPower);
}


#ifndef DESKTOP_TEST
void init_uart1_for_stdout(void)
{
   // /opt/microchip/xc32/v1.40/docs/README.html
   /* UART1 or UART2 default for stdout/stderr - By default the stdout and
      stderr streams (used by functions such as printf) output to UART2.
      This is useful on many Microchip demo boards such as the Explorer 16.
      Often times, however, you may wish to use UART1. The MPLAB® XC32
      compiler provides a __XC_UART variable that you may use to switch the
      default to UART 1. After including the xc.h header file, just add the
      following statement to your code in one of your functions such as
      main(): __XC_UART = 1; Currently, only UART1 and UART2 are supported
      by this mechanism. To write to another peripheral, provide a custom
      void _mon_putc (char c) implementation in your project as described
      in the 32-bit Libraries Guide.
   */

   //Configure UART1 for stdout...
   U1MODEbits.UARTEN = 0;
   U1STAbits.UTXEN = 1;
   U1MODEbits.BRGH = 1;
   U1BRG = 24000000/(4*9600) - 1;
   U1MODEbits.UARTEN = 1;


   __XC_UART = 1; 

   // U1TX and U2TX are located on Peripheral Pin Select...
   //U1TX: RPA0, RPB3,4, 15, 7, RPC7,0,5
   //U2TX: RPA3, RPB14, 0, 10, 9, RPC9, 2, 4

   //Let's use RB15 (U1TX)

   //"Register values can only be changed if the Configuration bit, IOLOCK
   // (CFGCON<13>), = 0."
   
   //THIS PROCEDURE IS REALLY CONFUSING:
   //
   // What does the OSCCON register have to do with the PPS registers?!
   // Why is this unlock-procedure listed for OSCCON but makes no reference
   // to unlocking for other registers (PPS, in this case).

   //  OLD NOTES in the process of trying to figure out where to find the
   //    /PPS/ unlock-procedure:
   //  According to the MX1xx/2xx Family Data Sheet:
   //  The peripheral Pin Selection cannot be changed without UNLOCKING it
   //  first... See 11.3.6.1
   //  Don't believe what it says about checking Section 6 in the other
   //  datasheet... check 25.4 in this one... 
   //  I dunno the whole thing's weird


   //Family Reference Manual - Section 6
   // Example 6-1:
   SYSKEY = 0x0; //ensure OSCCON is locked
   SYSKEY = 0xAA996655; //Write Key1 to SYSKEY 
   SYSKEY = 0x556699AA; //Write Key2 to SYSKEY
   // it claims that OSCCON is now unlocked, and it's time to make the
   // change... we're not working with OSCCON, but this seems to be the
   // only reference to an unlock-sequence
   CFGCON = CFGCON & ~(1<<13);
   
   //Set PPS for RPB15 to be the UART1 Tx output.
   RPB15R = 0x1; //RPB15R<3:0> = 0001 = U1TX

   //And relock it...
   SYSKEY = 0x0; 
}
#endif





int main(void)
{
#ifndef DESKTOP_TEST
   //Configure LED output:
    //Unnecessary for *output*
    //ANSELA = 0;
   CNPUACLR = 0x01;
   TRISACLR = 0x01;
   LATA = 0x00;


   init_uart1_for_stdout();
#endif


   int loopNum = 0;

   while(1)
   {
#ifndef DESKTOP_TEST
      //Blink the LED on RA0
      LATAINV = 0x01; 
#endif
      //Print the loop number...
      printf("loopNum %d\n", loopNum);
      loopNum++;
      
      //RUN THE MATH-BUG TEST
      testBrokenMathAndPrintout();
      
      //Pause for a bit...
      int i;
      for(i=0; i<0xffffff; i++)
      {
         //Make sure the otherwise-empty for-loop doesn't get optimized-out
         asm("nop");
      }
   }

   return 0;
}


//NOTES RE: 'PRIi8', etc:
// DESPITE printout bug (which is just an annoyance): 
// This became an issue because the MATH was wrong, according to
//  the functioning of the motor, not the printout.
//
//PRINTOUT notes:
// http://stackoverflow.com/questions/1893490 ...
//           ... /difference-between-format-specifiers-i-and-d-in-printf
// PRId# and PRIi# are identical in printf (not scanf).
//
// As expected, switching between PRId8 vs PRIi8 seems to have no effect.

/* xc32/v1.40/pic32mx/include/machine/int_fmtio.h:
    #define  PRIi8       "i"   // int8_t 
    #define  PRIi16      "i"   // int16_t 
    #define  PRIi32      "i"   // int32_t
*/

/* xc32/v1.40/pic32mx/include/stdint.h:
    #include <machine/int_types.h>
    
    #ifndef  int8_t
    typedef  __int8_t int8_t;
    #define  int8_t      __int8_t
    #endif
*/    
    
/* xc32/v1.40/pic32mx/include/machine/int_types.h:
    typedef  __signed char      __int8_t;
    typedef  unsigned char     __uint8_t;
*/

/* POTENTIALLY INTERESTING TIDBITS:
   xc32/v1.40/pic32-libs/include/lega-c/mips/int_limits.h:
     #define INT8_MIN  (-0x7f-1)         // int8_t
   xc32/v1.40/pic32-libs/include/lega-c/mips/int_limits.h:
     #define INT8_MAX  0x7f           // int8_t   
*/


/* WTF???
   xc32/v1.40/pic32-libs/include/lega-c/mips/types.h:
      typedef   __int8_t   u_int8_t;
   xc32/v1.40/pic32-libs/include/lega-c/mips/types.h:
      #define   u_int8_t   __uint8_t
*/


/* mehPL:
 *    I would love to believe in a world where licensing shouldn't be
 *    necessary; where people would respect others' work and wishes, 
 *    and give credit where it's due. 
 *    A world where those who find people's work useful would at least 
 *    send positive vibes--if not an email.
 *    A world where we wouldn't have to think about the potential
 *    legal-loopholes that others may take advantage of.
 *
 *    Until that world exists:
 *
 *    This software and associated hardware design is free to use,
 *    modify, and even redistribute, etc. with only a few exceptions
 *    I've thought-up as-yet (this list may be appended-to, hopefully it
 *    doesn't have to be):
 * 
 *    1) Please do not change/remove this licensing info.
 *    2) Please do not change/remove others' credit/licensing/copyright 
 *         info, where noted. 
 *    3) If you find yourself profiting from my work, please send me a
 *         beer, a trinket, or cash is always handy as well.
 *         (Please be considerate. E.G. if you've reposted my work on a
 *          revenue-making (ad-based) website, please think of the
 *          years and years of hard work that went into this!)
 *    4) If you *intend* to profit from my work, you must get my
 *         permission, first. 
 *    5) No permission is given for my work to be used in Military, NSA,
 *         or other creepy-ass purposes. No exceptions. And if there's 
 *         any question in your mind as to whether your project qualifies
 *         under this category, you must get my explicit permission.
 *
 *    The open-sourced project this originated from is ~98% the work of
 *    the original author, except where otherwise noted.
 *    That includes the "commonCode" and makefiles.
 *    Thanks, of course, should be given to those who worked on the tools
 *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
 *    I'm certain many others. 
 *    And, as well, to the countless coders who've taken time to post
 *    solutions to issues I couldn't solve, all over the internets.
 *
 *
 *    I'd love to hear of how this is being used, suggestions for
 *    improvements, etc!
 *         
 *    The creator of the original code and original hardware can be
 *    contacted at:
 *
 *        EricWazHung At Gmail Dotcom
 *
 *    The site associated with the original open-sourced project is at:
 *
 *        https://sites.google.com/site/geekattempts/
 *
 * This license added to the original file located at:
 * /home/meh/_pic32Projects/printfTest/6-revisit_nochange/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 *
 ************************************************************************
 **  MICROCHIP
 **   This took quite a bit of time and effort on my part, and I'm broke.
 **   PLEASE send me some swag...
 **   Or, yahknow, a remote/telecommuting/contracting gig would be awesome.
 ************************************************************************
 */
