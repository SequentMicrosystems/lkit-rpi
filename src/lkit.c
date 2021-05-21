/*	<https://www.sequentmicrosystems.com>
 ***********************************************************************
 *	Author: Sequent Microsystems
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "lkit.h"
#include "comm.h"
#include "gpio.h"
#include "cli.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)0

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define CMD_ARRAY_SIZE	8

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
	if (ERROR == i2cMem8Read(dev, I2C_MEM_REVISION_MAJOR_ADD, buff, 1))
	{
		printf("Learning Kit card not detected!\n");
		return ERROR;
	}
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
	if (ERROR == i2cMem8Read(dev, I2C_MEM_REVISION_MAJOR_ADD, buff, 1))
	{

		return ERROR;
	}
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
	"\tUsage:		lkit  -board\n",
	"",
	"\tExample:		lkit -board  Display vcc, temperature, firmware version \n"};

int doBoard(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[4];
	int resp = 0;
	int temperature = 25;
	float voltage = 3.3;
	
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
		"Hardware %02d.%02d, Firmware %02d.%02d, CPU temperature %d C, voltage %0.2f V\n",
		(int)buff[0], (int)buff[1], (int)buff[2], (int)buff[3], temperature,
		voltage);
	return OK;
}

const CliCmdType *gCmdArray[] =
{
	&CMD_VERSION,
	&CMD_HELP,
	&CMD_WAR,
	&CMD_LIST,
	&CMD_BOARD,
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
				return ret;
			}
		}
		i++;
	}
	printf("Invalid command option\n");
	usage();

	return -1;
}
