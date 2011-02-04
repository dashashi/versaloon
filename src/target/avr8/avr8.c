/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_cfg.h"
#include "app_type.h"
#include "app_err.h"
#include "app_log.h"
#include "prog_interface.h"

#include "vsprog.h"
#include "programmer.h"
#include "target.h"
#include "scripts.h"

#include "avr8.h"
#include "avr8_internal.h"

#define CUR_TARGET_STRING			AVR8_STRING

const struct program_area_map_t avr8_program_area_map[] = 
{
	{APPLICATION_CHAR, 1, 0, 0, 0, AREA_ATTR_EWR},
	{EEPROM_CHAR, 1, 2, 0, 256, AREA_ATTR_EWR},
	{FUSE_CHAR, 0, 0, 0, 0, AREA_ATTR_WR},
	{LOCK_CHAR, 0, 0, 0, 0, AREA_ATTR_WR},
	{CALIBRATION_CHAR, 0, 0, 0, 0, AREA_ATTR_R},
	{0, 0, 0, 0, 0, 0}
};

const struct program_mode_t avr8_program_mode[] = 
{
	{'i', SET_FREQUENCY, SPI | GPIO},
	{'j', SET_FREQUENCY, JTAG_HL},
	{'p', "", HV},
	{'s', "", HV},
	{0, NULL, 0}
};

struct program_functions_t avr8_program_functions;

VSS_HANDLER(avr8_help)
{
	VSS_CHECK_ARGC(1);
	printf("\
Usage of %s:\n\
  -F,  --frequency <FREQUENCY>              set ISP frequency, in KHz\n\
  -m,  --mode <MODE>                        set mode<b|p>\n\n", 
			CUR_TARGET_STRING);
	return ERROR_OK;
}

VSS_HANDLER(avr8_mode)
{
	uint8_t mode;
	
	VSS_CHECK_ARGC(2);
	mode = (uint8_t)strtoul(argv[1], NULL,0);
	switch (mode)
	{
	case AVR8_ISP:
		memcpy(&avr8_program_functions, &avr8isp_program_functions, 
				sizeof(avr8_program_functions));
		break;
	case AVR8_JTAG:
		memcpy(&avr8_program_functions, &avr8jtag_program_functions, 
				sizeof(avr8_program_functions));
		break;
	case AVR8_HVPP:
	case AVR8_HVSP:
		return ERROR_FAIL;
		break;
	}
	return ERROR_OK;
}

const struct vss_cmd_t avr8_notifier[] = 
{
	VSS_CMD(	"help",
				"print help information of current target for internal call",
				avr8_help),
	VSS_CMD(	"mode",
				"set programming mode of target for internal call",
				avr8_mode),
	VSS_CMD_END
};

