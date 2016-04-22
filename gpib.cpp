#include "gpib.h"


int gpib::Dev;
char gpib::ReadBuffer[ARRAYYSIZE + 1];
char gpib::ErrorMnemonic[29][5]= {
                                  "EDVR", "ECIC", "ENOL", "EADR", "EARG",
                                  "ESAC", "EABO", "ENEB", "EDMA", "",
                                  "EOIP", "ECAP", "EFSO", "",     "EBUS",
                                  "ESTB", "ESRQ", "",     "",      "",
                                  "ETAB", "ELCK", "EARM", "EHDL",  "",
                                  "",     "EWIP", "ERST", "EPWR" };
void gpib::GPIBCleanup(int Dev,const char* ErrorMsg)
{
  printf("Error : %s/nibsta = 0x%x iberr = %d (%s)/n",
  ErrorMsg, ibsta, iberr, ErrorMnemonic[iberr]);
  if (Dev != -1)
  {
  printf("Cleanup: Returning PNA to front panel control/n");
  ibonl (Dev, 0);
  }
}



gpib::gpib(QWidget *parent) : QWidget(parent)
{

}

gpib::~gpib()
{

}

int gpib::init(){

       /*
        * ========================================================================
        *
        * INITIALIZATION SECTION
        *
        * ========================================================================
        */

       /*
        * Assign a unique identifier to the device and store in the variable
        * Dev.  If the ERR bit is set in ibsta, call GPIBCleanup with an
        * error message. Otherwise, the device handle, Dev, is returned and
        * is used in all subsequent calls to the device.
        */
       #define BDINDEX               0     // Board Index
       #define PRIMARY_ADDR_OF_DMM   22     // Primary address of device
       #define NO_SECONDARY_ADDR     0     // Secondary address of device
       #define TIMEOUT               T10s  // Timeout value = 10 seconds
       #define EOTMODE               1     // Enable the END message
       #define EOSMODE               0     // Disable the EOS mode

       Dev = ibdev(BDINDEX, PRIMARY_ADDR_OF_DMM, NO_SECONDARY_ADDR,
                   TIMEOUT, EOTMODE, EOSMODE);
       /*
       if (ThreadIbsta() & ERR)
       {
          gpib::GPIBCleanup(Dev, "Unable to open device");
          return 1;
       }
*/
       /*
        * Clear the internal or device functions of the device.  If the error
        * bit ERR is set in ibsta, call GPIBCleanup with an error message.
        */
/*
       ibclr(Dev);
       if (ThreadIbsta() & ERR)
       {
          gpib::GPIBCleanup(Dev, "Unable to clear device");
          return 1;
       }
*/
       /*
        * ========================================================================
        *
        * MAIN BODY SECTION
        *
        * In this application, the Main Body communicates with the instrument
        * by writing a command to it and reading its response. This would be
        * the right place to put other instrument communication.
        *
        * ========================================================================
        */

       /*
        * Request the identification code by sending the instruction '*IDN?'.
        * If the error bit ERR is set in ibsta, call GPIBCleanup with an error
        * message.
        */
       ibwrt(Dev, "*IDN?\n", 6L);
       if (ThreadIbsta() & ERR)
       {
          gpib::GPIBCleanup(Dev, "Unable to write to device");
          return 1;
       }
        usleep(60000);
       /*
        * Read the identification code by calling ibrd. If the ERR bit is
        * set in ibsta, call GPIBCleanup with an error message.
        */
       ibrd(Dev, ReadBuffer, ARRAYYSIZE);
       if (ThreadIbsta() & ERR)
       {
          gpib::GPIBCleanup(Dev, "Unable to read data from device");
          return 1;
       }

       /*
        * Assume that the returned string contains ASCII data. NULL terminate
        * the string using the value in ibcntl which is the number of bytes
        * read in. Use printf to display the string.
        */
       ReadBuffer[ThreadIbcntl()] = '\0';
       printf("Returned string:  %s\n", ReadBuffer);

       /*
        * ========================================================================
        *
        * CLEANUP SECTION
        *
        * ========================================================================
        */

       /*
        * Take the device offline.
        */
      // ibonl(Dev, 0);
       return 0;

}

/*
 * Write to the PNA
 */
void gpib::GPIBWrite(const char* SCPIcmd)
{
int length;
char ErrorMsg[ERRMSGSIZE + 1];
length = strlen(SCPIcmd) ;

  ibwrt (Dev, SCPIcmd, length);
  if (ibsta & ERR)
  {
  strcpy(ErrorMsg, "Unable to write this command to PNA:/n");
  strcat(ErrorMsg, SCPIcmd);

  gpib::GPIBCleanup(Dev, ErrorMsg);
  exit(1);
  }
}

/*
 * Read from the PNA
 */
char* gpib::GPIBRead(void)
{
  ibrd (Dev, ReadBuffer, ARRAYYSIZE);
  if (ibsta & ERR)
  {
  gpib::GPIBCleanup(Dev, "Unable to read from the PNA");
  exit(1);
  }
else
  return ReadBuffer;
}

/*
 * After each GPIB call, the application checks whether the call succeeded. If an NI-488.2 call fails, the GPIB driver sets the corresponding bit in the global status variable. If the call failed, this procedure prints an error message, takes the PNA offline and exits.
 */

int gpib::close(){
    if (ibsta & ERR)
    {
    gpib::GPIBCleanup(Dev, "Unable to close");
    }
    else{
        gpib::GPIBCleanup(Dev, "Closing");
    }
    return 0;
}
