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



//###############################################################
// THIS FILE IS ONLY FOR SIMPLIFIED-VIEWING
//  THIS WILL RESULT IN THE "BUG"
// SEE main.c (in the parent-directory)
//  which contains explanations of the tests/errors
//  and options for locating the bug
//###############################################################


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





#include <xc.h>
#include <stdlib.h> //for rand()
#include <stdio.h>
#include <inttypes.h>


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






//This function *should* Print the following, in all cases:
//  1: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  2: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  3: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//
//WITH OPTIMIZATION it prints:
//  1: power(u8) = 127   dir(i8) = -1   signedPower(i16) = -127
//  2: power(u8) = 127   dir(i8) = 255   signedPower(i16) = -127
//  3: power(u8) = 127   dir(i8) = 255   signedPower(i16) = 32385

//NOTE: Many tests have been performed (no longer included in this code)
// SEE main.c in the parent directory!!!
void testBrokenMathAndPrintout(void)
{
   uint8_t power = 127;
   int8_t dir = -1; 


   int16_t signedPower = (int16_t)power * (int16_t)dir;

   //"THE BUG" appears differently depending on how PRINT_IT is implemented
   // IN THIS FILE:
   // PRINT_IT() is merely a macro.
   //  Identical to including the printf() statement RIGHT HERE
   //  AND has the IDENTICAL result.
   PRINT_IT(1, power, dir, signedPower);


   //This case will SELDOMLY be *executed*
   // but without some *potential* reassignment to the variable 'dir' 
   // (somewhere)
   // the bug does not appear
   if(rand() == 0)
   {
      printf("!!! rand() == 0, else{} executed!\n\r");
      
      //THIS VALUE IS NOT EXPECTED TO BE USED
      // SEE NOTES FOR 'INCLUDE_RAND'
      // in main.c in the parent directory
      dir = ((int8_t)(0));
   }


   PRINT_IT(2, power, dir, signedPower);


   signedPower = (int16_t)power * (int16_t)dir;

   PRINT_IT(3, power, dir, signedPower);
}


void init_uart1_for_stdout(void)
{
   //Configure UART1 for stdout...
   U1MODEbits.UARTEN = 0;
   U1STAbits.UTXEN = 1;
   U1MODEbits.BRGH = 1;
   U1BRG = 24000000/(4*9600) - 1;
   U1MODEbits.UARTEN = 1;


   __XC_UART = 1; 

   SYSKEY = 0x0; //ensure OSCCON is locked
   SYSKEY = 0xAA996655; //Write Key1 to SYSKEY 
   SYSKEY = 0x556699AA; //Write Key2 to SYSKEY
   CFGCON = CFGCON & ~(1<<13);
   
   //Set PPS for RPB15 to be the UART1 Tx output.
   RPB15R = 0x1; //RPB15R<3:0> = 0001 = U1TX

   //And relock it...
   SYSKEY = 0x0; 
}





int main(void)
{
   //Configure LED output:
    //Unnecessary for *output*
    //ANSELA = 0;
   CNPUACLR = 0x01;
   TRISACLR = 0x01;
   LATA = 0x00;


   init_uart1_for_stdout();


   int loopNum = 0;

   while(1)
   {
      //Blink the LED on RA0
      LATAINV = 0x01; 
      
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
 * /home/meh/_pic32Projects/printfTest/6-revisit_nochange/errorOnly/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 ************************************************************************
 **  MICROCHIP
 **   This took quite a bit of time and effort on my part, and I'm broke.
 **   PLEASE send me some swag...
 **   Or, yahknow, a remote/telecommuting/contracting gig would be awesome.
 ************************************************************************
 */
