/*	<https://www.sequentmicrosystems.com>
 ***********************************************************************
 *	Author: Sequent Microsystems
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "lkit.h"
#include "comm.h"
#include "gpio.h"
#include "cli.h"
#include "gpio.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)1

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define IN_CH_NR	4
#define RELAY_NR 	2
#define CMD_ARRAY_SIZE	8
#define SCALE_FACTOR 1000

char *warranty =
	"	       Copyright (c) 2016-2021 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";

const int gInPins[IN_CH_NR] =
{
	4,
	17,
	27,
	22};
const int gRelayPins[RELAY_NR] =
{
	19,
	16};

typedef struct {
	float min;
	float max;
}	CalibLimitsStruct;

const CalibLimitsStruct calLimits[CALIB_CH_COUNT] = {{0, 0}, {0, 10}, {4, 20}, {4, 20}, {0, 10}, {4, 20}};
int gServoEnable = 0;

void usage(void)
{
	int i = 0;
	while (gCmdArray[i] != NULL)
	{
		if (gCmdArray[i]->name != NULL)
		{
			if (strlen(gCmdArray[i]->usage1) > 2)
			{
				printf("%s", gCmdArray[i]->usage1);
			}
			if (strlen(gCmdArray[i]->usage2) > 2)
			{
				printf("%s", gCmdArray[i]->usage2);
			}
		}
		i++;
	}
	printf("Type lkit -h <command> for more help\n");
}

int writeU16(int dev, int add, int val)
{
	u8 buff[2];
	u16 v = 0;

	if(add < 0 || add > SLAVE_BUFF_SIZE)
	{
		return ARG_ERR;
	}
	v = (u16)val;
	memcpy(buff, &v, 2);

	return i2cMem8Write(dev, add, buff, 2);
}

int calib(int dev, float val, u8 channel, u8 reset)
{
	int rawVal = 0;
	u8 buff[2];
	int ret = 0;

	if( channel <= CALIB_CH_NONE || channel >= CALIB_CH_COUNT)
	{
		printf("Invalid calibration channel %d \n", (int)channel);
		return ARG_ERR;
	}

	if(reset)
	{
		buff[0] = channel;
		buff[1] = RESET_CALIBRATION_KEY;
		return i2cMem8Write(dev, I2C_MEM_CALIB_CHANNEL, buff, 2);
	}

	if( val < calLimits[channel].min || val > calLimits[channel].max)
	{
		printf("Invalid calibration value, most be [%f, %f]!\n",calLimits[channel].min, calLimits[channel].max);
		return ARG_ERR;
	}
	rawVal = (int)round(val * SCALE_FACTOR);
	ret = writeU16(dev, I2C_MEM_CALIB_VALUE, rawVal);
	if (ret)
	{
		return ret;
	}
	buff[0] = channel;
	buff[1] = CALIBRATION_KEY;
	return i2cMem8Write(dev, I2C_MEM_CALIB_CHANNEL, buff, 2);
}

int doBoardInit(void)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	add = SLAVE_OWN_ADDRESS_BASE;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, I2C_MEM_SERVO_ENABLE, buff, 1))
	{
		printf("Learning Kit card not detected!\n");
		return ERROR;
	}
	gServoEnable = buff[0];
	return dev;
}

int boardCheck(void)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	add = SLAVE_OWN_ADDRESS_BASE;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, I2C_MEM_SERVO_ENABLE, buff, 1))
	{

		return ERROR;
	}
	gServoEnable = buff[0];
	return OK;
}

int doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
{
	"-h",
	1,
	&doHelp,
	"\t-h		Display the list of command options or one command option details\n",
	"\tUsage:		lkit -h    Display command options list\n",
	"\tUsage:		lkit -h <param>   Display help for <param> command option\n",
	"\tExample:		lkit -h board    Display help for \"board\" command option\n"};

int doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				if (strcasecmp(argv[2], gCmdArray[i]->name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i]->help, gCmdArray[i]->usage1,
						gCmdArray[i]->usage2, gCmdArray[i]->example);
					break;
				}
			}
			i++;
		}
		if (NULL == gCmdArray[i])
		{
			printf("Option \"%s\" not found\n", argv[2]);
			i = 0;
			while (NULL != gCmdArray[i])
			{
				if (gCmdArray[i]->name != NULL)
				{
					printf("%s", gCmdArray[i]->help);
					break;
				}
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				printf("%s", gCmdArray[i]->help);
			}
			i++;
		}
	}
	return OK;
}

int doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
{
	"-v",
	1,
	&doVersion,
	"\t-v		Display the lkit command version number\n",
	"\tUsage:		lkit -v\n",
	"",
	"\tExample:		lkit -v  Display the version number\n"};

int doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("lkit v%d.%d.%d Copyright (c) 2016 - 2021 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: lkit -warranty\n");
	return OK;
}

int doWarranty(int argc, char *argv[]);
const CliCmdType CMD_WAR =
{
	"-warranty",
	1,
	&doWarranty,
	"\t-warranty	Display the warranty\n",
	"\tUsage:		lkit -warranty\n",
	"",
	"\tExample:		lkit -warranty  Display the warranty text\n"};

int doWarranty(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("%s\n", warranty);
	return OK;
}

int doList(int argc, char *argv[]);
const CliCmdType CMD_LIST =
	{
		"-list",
		1,
		&doList,
		"\t-list:		List all lkit boards connected,return the # of boards and stack level for every board\n",
		"\tUsage:		lkit -list\n",
		"",
		"\tExample:		lkit -list display: one \n"};

int doList(int argc, char *argv[])
{

	UNUSED(argc);
	UNUSED(argv);

	if (boardCheck() == OK)
	{
		printf("One card detected\n");
	}
	else
	{
		printf("No card detected !!!\n");
	}
	return OK;
}

int doBoard(int argc, char *argv[]);
const CliCmdType CMD_BOARD =
{
	"-board",
	1,
	&doBoard,
	"\t-board		Display the board status and firmware version number\n",
	"\tUsage:		lkit -board\n",
	"",
	"\tExample:		lkit -board  Display vcc, temperature, firmware version \n"};

int doBoard(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[4];
	int resp = 0;
	int temperature = 25;
	float voltage = 0;

	UNUSED(argc);
	UNUSED(argv);

	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	resp = i2cMem8Read(dev, I2C_MEM_DIAG_TEMPERATURE_ADD, buff, 3);
	if (FAIL == resp)
	{
		printf("Fail to read board info!\n");
		exit(1);
	}
	temperature = buff[0];
	memcpy(&resp, &buff[1], 2);
	voltage = (float)resp / 1000; //read in milivolts

	resp = i2cMem8Read(dev, I2C_MEM_REVISION_HW_MAJOR_ADD, buff, 4);
	if (FAIL == resp)
	{
		printf("Fail to read board info!\n");
		exit(1);
	}
	printf(
		"Hardware %02d.%02d, Firmware %02d.%02d, CPU temperature %d C, voltage %0.3f V\n",
		(int)buff[0], (int)buff[1], (int)buff[2], (int)buff[3], temperature,
		voltage);
	return OK;
}

int doRelayWrite(int argc, char *argv[]);
const CliCmdType CMD_RELAY_WRITE =
{
	"relwr",
	1,
	&doRelayWrite,
	"\trelwr:		Set relays On/Off\n",
	"\tUsage:		lkit relwr <channel> <0/1>\n",
	"",
	"\tExample:		lkit relwr 2 1; Turn ON Relay #2\n"};

int doRelayWrite(int argc, char *argv[])
{
	int ch = 0;
	int state = 0;
	int ret = 0;

	if (argc != 4)
	{
		return ARG_CNT_ERR;
	}
	ch = atoi(argv[2]);
	if (ch < 1 || ch > 2)
	{
		printf("Invalid relay number [1..2]!\n");
		return ARG_ERR;
	}
	state = atoi(argv[3]);
	if (state != 0)
	{
		ret = GPIOWrite(gRelayPins[ch - 1], 1);
	}
	else
	{
		ret = GPIOWrite(gRelayPins[ch - 1], 0);
	}
	return ret;
}

int doOptoRead(int argc, char *argv[]);
const CliCmdType CMD_OPTO_READ =
{
	"optrd",
	1,
	&doOptoRead,
	"\toptrd:		Read optocoupled inputs status\n",
	"\tUsage:		lkit optrd <channel>\n",
	"\tUsage:		lkit optrd\n",
	"\tExample:		lkit optrd 2; Read Status of Optocoupled input ch #2\n"};

int doOptoRead(int argc, char *argv[])
{
	int ch = 0;
	int state = 0;
	int ret = OK;

	if (argc == 2)
	{
		for (ch = 0; ch < IN_CH_NR; ch++)
		{
			if (0 == GPIORead(gInPins[ch]))
			{
				state += 1 << ch;
			}
		}
		printf("%d\n", state);
	}
	else if (argc == 3)
	{
		ch = atoi(argv[2]);
		if (ch < 1 || ch > IN_CH_NR)
		{
			printf("Invalid input channel number [1..4]!\n");
			ret = ARG_ERR;
		}
		else
		{
			if (0 == GPIORead(gInPins[ch - 1]))
			{
				state = 1;
			}
			printf("%d\n", state);
		}
	}
	else
	{
		ret = ARG_CNT_ERR;
	}
	return ret;
}

int doVinRead(int argc, char *argv[]);
const CliCmdType CMD_VIN_READ =
{
	"vinrd",
	1,
	&doVinRead,
	"\tvinrd		Display the 0-10V input port readings\n",
	"\tUsage:		lkit vinrd\n",
	"",
	"\tExample:		lkit vinrd  Display the voltage on 0-10v in port\n"};

int doVinRead(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;

	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	resp = i2cMem8Read(dev, I2C_MEM_V_IN_ADD, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		return FAIL;
	}

	memcpy(&resp, buff, 2);
	printf("%.3f\n", (float)resp / 1000);
	return OK;
}


int doVinCal(int argc, char *argv[]);
const CliCmdType CMD_VIN_CAL =
{
	"vincal",
	1,
	&doVinCal,
	"\tvincal		Calibrate 0-10V input port (Calibration must be done in 2 points closer to the full scale)\n",
	"\tUsage:		lkit vincal <val>\n",
	"",
	"\tExample:		lkit vincal 1.23  Calibrate the 0-10V input port at 1.23V\n"};

int doVinCal(int argc, char *argv[])
{
	int dev = -1;
	float value = 0;

	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	value = atof(argv[2]);
	return calib(dev, value, CALIB_CH_V_IN, 0);
}

int doVinCalRst(int argc, char *argv[]);
const CliCmdType CMD_VIN_CAL_RST =
{
	"vincalrst",
	1,
	&doVinCalRst,
	"\tvincalrst		Reset the 0-10V input port calibration\n",
	"\tUsage:		lkit vincalrst \n",
	"",
	"\tExample:		lkit vincalrst Reset the 0-10V input port calibration\n"};

int doVinCalRst(int argc, char *argv[])
{
	int dev = -1;
	
	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	
	return calib(dev, 0, CALIB_CH_V_IN, 1);
}



int doVoutRead(int argc, char *argv[]);
const CliCmdType CMD_VOUT_READ =
{
	"voutrd",
	1,
	&doVoutRead,
	"\tvoutrd		Display the 0-10V output port value\n",
	"\tUsage:		lkit voutrd\n",
	"",
	"\tExample:		lkit voutrd  Display the voltage on 0-10V output port\n"};

int doVoutRead(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;

	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	resp = i2cMem8Read(dev, I2C_MEM_V_OUT_ADD, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		return FAIL;
	}

	memcpy(&resp, buff, 2);
	printf("%.3f\n", (float)resp / 1000);
	return OK;
}

int doVoutWrite(int argc, char *argv[]);
const CliCmdType CMD_VOUT_WRITE =
	{
		"voutwr",
		1,
		&doVoutWrite,
		"\tvoutwr		Set the 0-10V output port value\n",
		"\tUsage:		lkit voutwr <value(V)>\n",
		"",
		"\tExample:		lkit voutwr 5.52  Set the voltage on 0-10V output port at 5.52V\n"};

int doVoutWrite(int argc, char *argv[])
{
	int dev = -1;
	int resp = 0;
	float value = 0;
	int val = 0;
	u8 buff[2];

	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	if(0 != gServoEnable)
	{
		buff[0] = 0;
		resp = i2cMem8Write(dev,I2C_MEM_SERVO_ENABLE, buff, 1);
		if( FAIL == resp)
		{
			printf("Fail to write!\n");
			return FAIL;
		}
	}
	value = atof(argv[2]);
	if( value > 10 || value < 0)
	{
		return ARG_ERR;
	}
	val = (u16)round(value * 1000);
	resp = writeU16(dev, I2C_MEM_V_OUT_ADD, val);
	if (OK != resp)
	{
		printf("Fail to write!\n");
		return FAIL;
	}
	return OK;
}

int doIinRead(int argc, char *argv[]);
const CliCmdType CMD_IIN_READ =
{
	"iinrd",
	1,
	&doIinRead,
	"\tiinrd		Display the 4-20mA input port readings\n",
	"\tUsage:		lkit iinrd\n",
	"",
	"\tExample:		lkit iinrd  Display the voltage on 4-20mA in port\n"};

int doIinRead(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;

	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	resp = i2cMem8Read(dev, I2C_MEM_I_IN_ADD, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		return FAIL;
	}

	memcpy(&resp, buff, 2);
	printf("%.3f\n", (float)resp / 1000);
	return OK;
}


int doIinCal(int argc, char *argv[]);
const CliCmdType CMD_IIN_CAL =
{
	"iincal",
	1,
	&doIinCal,
	"\tiincal		Calibrate 4-20mA input port (Calibration must be done in 2 points closer to the full scale)\n",
	"\tUsage:		lkit iincal <val>\n",
	"",
	"\tExample:		lkit iincal 4.23  Calibrate the 4-20mA input port at 4.23mA\n"};

int doIinCal(int argc, char *argv[])
{
	int dev = -1;
	float value = 0;

	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	value = atof(argv[2]);
	return calib(dev, value, CALIB_CH_I_IN, 0);
}

int doIinCalRst(int argc, char *argv[]);
const CliCmdType CMD_IIN_CAL_RST =
{
	"iincalrst",
	1,
	&doIinCalRst,
	"\tiincalrst		Reset the 4-20mA input port calibration\n",
	"\tUsage:		lkit iincalrst \n",
	"",
	"\tExample:		lkit iincalrst Reset the 4-20mA input port calibration\n"};

int doIinCalRst(int argc, char *argv[])
{
	int dev = -1;
	
	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	
	return calib(dev, 0, CALIB_CH_I_IN, 1);
}

int doIoutRead(int argc, char *argv[]);
const CliCmdType CMD_IOUT_READ =
{
	"ioutrd",
	1,
	&doIoutRead,
	"\tioutrd		Display the 4-20mA output port value\n",
	"\tUsage:		lkit ioutrd\n",
	"",
	"\tExample:		lkit ioutrd  Display the voltage on 4_20mA output port\n"};

int doIoutRead(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;

	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	resp = i2cMem8Read(dev, I2C_MEM_I_OUT_ADD, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		return FAIL;
	}

	memcpy(&resp, buff, 2);
	printf("%.3f\n", (float)resp / 1000);
	return OK;
}

int doIoutWrite(int argc, char *argv[]);
const CliCmdType CMD_IOUT_WRITE =
	{
		"ioutwr",
		1,
		&doIoutWrite,
		"\tioutwr		Set the 4-20mA output port value\n",
		"\tUsage:		lkit ioutwr <value(mA)>\n",
		"",
		"\tExample:		lkit ioutwr 5.52  Set the current on 4-20mA output port at 5.52mA\n"};

int doIoutWrite(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	float value = 0;
	u16 aux = 0;
	
	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	if(0 != gServoEnable)
	{
		buff[0] = 0;
		resp = i2cMem8Write(dev,I2C_MEM_SERVO_ENABLE, buff, 1);
		if( FAIL == resp)
		{
			printf("Fail to write!\n");
			return FAIL;
		}
	}
	value = atof(argv[2]);
	if (value > 20 || value < 4)
	{
		return ARG_ERR;
	}

	aux = (u16)round(value * 1000);
	memcpy(buff, &aux, 2);
	resp = i2cMem8Write(dev, I2C_MEM_I_OUT_ADD, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		return FAIL;
	}
	return OK;
}

int doMotRead(int argc, char *argv[]);
const CliCmdType CMD_MOT_READ =
{
	"motrd",
	1,
	&doMotRead,
	"\tmotrd		Display the motor power value (-100..100) in %\n",
	"\tUsage:		lkit motrd\n",
	"",
	"\tExample:		lkit motrd  Display the motor power value \n"};

int doMotRead(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	s16 val = 0;

	UNUSED(argv);
	if (argc != 2)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	resp = i2cMem8Read(dev, I2C_MEM_MOT_VAL, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		return FAIL;
	}

	memcpy(&val, buff, 2);
	printf("%.1f\n", (float)val / 10);
	return OK;
}

int doMotWrite(int argc, char *argv[]);
const CliCmdType CMD_MOT_WRITE =
{
	"motwr",
	1,
	&doMotWrite,
	"\tmotwr		Set the motor port output power (-100..100)\n",
	"\tUsage:		lkit motwr <value(%)>\n",
	"",
	"\tExample:		lkit motwr -25.2  Set the motor power to 25.2% in reverse \n"};

int doMotWrite(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	float value = 0;
	s16 aux = 0;

	UNUSED(argv);
	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	value = atof(argv[2]);
	if (value > 100 || value < -100)
	{
		return ARG_ERR;
	}
	aux = (s16)round(value * 10);
	memcpy(buff, &aux, 2);
	resp = i2cMem8Write(dev, I2C_MEM_MOT_VAL, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		return FAIL;
	}
	return OK;
}

int doServoRead(int argc, char *argv[]);
const CliCmdType CMD_SERVO_READ =
{
	"servord",
	1,
	&doServoRead,
	"\tservord		Display the servo position value in %\n",
	"\tUsage:		lkit servord <channel>\n",
	"",
	"\tExample:		lkit servord 1  Display the servo 1 position \n"};

int doServoRead(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	int channel = 0;
	s16 val = 0;

	UNUSED(argv);
	if (argc != 3)
	{
		return ARG_CNT_ERR;
	}
	channel = atoi(argv[2]);
	if(1 != channel && 2 != channel)
	{
		return ARG_ERR;
	}

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	resp = i2cMem8Read(dev, I2C_MEM_SERVO_VAL1 + 2 * (channel - 1), buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		return FAIL;
	}

	memcpy(&val, buff, 2);
	printf("%.1f\n", (float)val / 10);
	return OK;
}

int doServoWrite(int argc, char *argv[]);
const CliCmdType CMD_SERVO_WRITE =
{
	"servowr",
	1,
	&doServoWrite,
	"\tservowr		Set the servo position (-100..100) for standard (-140..140) for extended range servo's \n",
	"\tUsage:		lkit servowr <channel> <value(%)>\n",
	"",
	"\tExample:		lkit servowr 1 25.2  Set the servo 1 position to 25.2%  \n"};

int doServoWrite(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	float value = 0;
	int channel = 0;
	s16 aux = 0;

	if (argc != 4)
	{
		return ARG_CNT_ERR;
	}
	channel = atoi(argv[2]);
	if(1 != channel && 2 != channel)
	{
		return ARG_ERR;
	}
	value = atof(argv[3]);
	if (value > 140 || value < -140)
	{
		return ARG_ERR;
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	if(0 == gServoEnable)
	{
		buff[0] = 1;
		resp = i2cMem8Write(dev,I2C_MEM_SERVO_ENABLE, buff, 1);
		if( FAIL == resp)
		{
			printf("Fail to write!\n");
			return FAIL;
		}
	}
	aux = (s16)round(value * 10);
	memcpy(buff, &aux, 2);
	
	resp = i2cMem8Write(dev, I2C_MEM_SERVO_VAL1 + 2 * (channel - 1), buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		return FAIL;
	}
	return OK;
}

int doLedWrite(int argc, char *argv[]);
const CliCmdType CMD_LED_WRITE =
{
	"ledwr",
	1,
	&doLedWrite,
	"\tledwr		Set the state of general purpose LEDS on the card\n",
	"\tUsage:		lkit ledwr <led[1..4]> <state(0/1)>\n",
	"\tUsage:		lkit ledwr <value[0..15]>",
	"\tExample:		lkit ledwr 1 1  Turn ON the LED #1\n"};

int doLedWrite(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	int led = 0;
	int state = 0;

	dev = doBoardInit();
	if (dev <= 0)
	{
		return FAIL;
	}
	if (argc == 3)
	{
		state = atoi(argv[2]);
		if (state < 0 || state > 15)
		{
			return ARG_ERR;
		}

		buff[0] = 0xff & state;
		resp = i2cMem8Write(dev, I2C_MEM_LED_VAL_ADD, buff, 1);
		if (FAIL == resp)
		{
			printf("Fail to write!\n");
			return FAIL;
		}
	}
	else if (argc == 4)
	{
		led = atoi(argv[2]);
		if (led < 1 || led > 4)
		{
			return ARG_ERR;
		}
		state = atoi(argv[3]);
		buff[0] = 0xff & led;
		if (state > 0)
		{
			resp = i2cMem8Write(dev, I2C_MEM_LED_SET_ADD, buff, 1);
		}
		else
		{
			resp = i2cMem8Write(dev, I2C_MEM_LED_CLR_ADD, buff, 1);
		}
		if (FAIL == resp)
		{
			printf("Fail to write!\n");
			return FAIL;
		}
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}

const CliCmdType *gCmdArray[] =
{
	&CMD_VERSION,
	&CMD_HELP,
	&CMD_WAR,
	&CMD_LIST,
	&CMD_BOARD,
	&CMD_RELAY_WRITE,
	&CMD_OPTO_READ,
	&CMD_VIN_READ,
	&CMD_VIN_CAL,
	&CMD_VIN_CAL_RST,
	&CMD_VOUT_READ,
	&CMD_VOUT_WRITE,
	&CMD_IIN_READ,
	&CMD_IIN_CAL,
	&CMD_IIN_CAL_RST,
	&CMD_IOUT_READ,
	&CMD_IOUT_WRITE,
	&CMD_MOT_READ,
	&CMD_MOT_WRITE,
	&CMD_SERVO_READ,
	&CMD_SERVO_WRITE,
	&CMD_LED_WRITE,
	NULL};

int main(int argc, char *argv[])
{
	int i = 0;
	int ret = OK;

	if (argc == 1)
	{
		usage();
		return -1;
	}
	while (NULL != gCmdArray[i])
	{
		if ( (gCmdArray[i]->name != NULL) && (gCmdArray[i]->namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i]->namePos], gCmdArray[i]->name) == 0)
			{
				ret = gCmdArray[i]->pFunc(argc, argv);
				if (ret == ARG_CNT_ERR)
				{
					printf("Invalid parameters number!\n");
					printf("%s", gCmdArray[i]->usage1);
					if (strlen(gCmdArray[i]->usage2) > 2)
					{
						printf("%s", gCmdArray[i]->usage2);
					}
				}
				else if(ret == ARG_ERR)
				{
					printf("Invalid parameters!\n");
					printf("%s", gCmdArray[i]->usage1);
					if (strlen(gCmdArray[i]->usage2) > 2)
					{
						printf("%s", gCmdArray[i]->usage2);
					}
				}
				return ret;
			}
		}
		i++;
	}
	printf("Invalid command option\n");
	usage();

	return -1;
}
