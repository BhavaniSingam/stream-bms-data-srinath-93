#include <stdio.h>
#include "dataBmsTx.h"

/* global variable declaration */
batteryReading_st bmsTempSocData;

/*
 **********************************************************************************************************************
 * Service name         : txBmsData
 * Syntax               : void txBmsData(void)
 * param[in]            : None
 * return               : None
 * Description          : Program to print the read battery temperature and soc data to the console
 **********************************************************************************************************************
 */
void txBmsData(void)
{
  int loopCntr = 0;
  printf("Recorded BMS Data \nBattery Temperature \t\t\t Battery StateOfCharge\n");
  while(loopCntr < bmsTempSocData.numOfData)
  {
    printf("Battery Temperature[%d] -> %f \t\t Battery StateOfCharge[%d] -> %f\n",loopCntr,bmsTempSocData.batteryTempearature[loopCntr],loopCntr,bmsTempSocData.batterySoc[loopCntr]);
    loopCntr++;
  }
}

/*
 **********************************************************************************************************************
 * Service name         : readBmsData
 * Syntax               : retBmsStatus_en readBmsData(void)
 * param[in]            : None
 * return               : OK_STATUS, ERROR_STATUS
 * Description          : Program to read battery health that includes temperature, state of charge from a recorded 
                          file
 **********************************************************************************************************************
 */
retBmsStatus_en readBmsData(int runTimeIpNum)
{
  retBmsStatus_en bmsStatusRet = ERROR_STATUS;
  float dataTemperature = 0.0;
  float dataSoc = 0.0;
  int cntrLoop = 0;
  int tempRunTimeIp = runTimeIpNum;
  bmsTempSocData.numOfData = 0; 
  char runTimeIp = 'n';
  
  FILE * fileToBeRead= fopen("./Tx/dataBms.txt","r");
  if (fileToBeRead)
  {
    for(;fscanf(fileToBeRead, "%f %f\n", &dataTemperature,&dataSoc)!=EOF;cntrLoop++)
    {
      bmsTempSocData.batteryTempearature[cntrLoop] = dataTemperature;
      bmsTempSocData.batterySoc[cntrLoop] = dataSoc;
      runTimeIp = checkHaltRead(runTimeIpNum, cntrLoop);
      if(runTimeIp == 'y')
      {
        break;
      }
      else
      { 
        runTimeIpNum += tempRunTimeIp;
      }
    }
    bmsTempSocData.numOfData = cntrLoop;
    bmsStatusRet= OK_STATUS;
  }
  fclose(fileToBeRead);
  return bmsStatusRet;
}

char checkHaltRead(int runTimeIpNum, int loopCounter)
{
  char chInput ='n';
/*
  char runTimeIp;
  char runTimeIpTemp = 'n';
*/
  FILE * fileCheckHalt= fopen("./haltBmsRead.txt","r");
  if((fileCheckHalt)&&(loopCounter == runTimeIpNum))
  {
    /* provide input to the file as 'y' to stop the data read else press 'n' */
/*
    runTimeIp = getc(stdin);
    runTimeIpTemp = scanf(" %c ",&runTimeIp);
*/
    while(fscanf(fileCheckHalt, "%c \n", &chInput)!=EOF)
    {
      break;
    }
  }
  fclose(fileCheckHalt);
  return chInput;
}
/*
 **********************************************************************************************************************
 * Service name         : dataBmsMain
 * Syntax               : retBmsStatus_en txBmsData(void)
 * param[in]            : None
 * return               : OK_STATUS, ERROR_STATUS
 * Description          : Program to invoke reading of required bms datas and send them to console or reciever
 **********************************************************************************************************************
 */
retBmsStatus_en dataBmsMain(int runTimeIpNum)
{
  retBmsStatus_en retBmsStatus = ERROR_STATUS;
  retBmsStatus = readBmsData(runTimeIpNum);
  if(retBmsStatus != ERROR_STATUS)
  {
    txBmsData();
  }
  return retBmsStatus;
}
