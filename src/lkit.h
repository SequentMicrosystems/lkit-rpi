#ifndef LKIT_H_
#define LKIT_H_

#include <stdint.h>

#define ERROR	-1
#define OK		0
#define FAIL	-1
#define ARG_ERR -2
#define ARG_CNT_ERR -3

#define SLAVE_OWN_ADDRESS_BASE	0x05
#define ADC_CH_NO	2
#define ADC_RAW_VAL_SIZE	2

typedef uint8_t u8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;

typedef enum
{
	OFF = 0,
	ON,
	STATE_COUNT
} OutStateEnumType;


enum
{
	I2C_MEM_ADC_VAL_RAW_ADD = 0,
	I2C_MEM_ADC_VAL_MV_ADD = I2C_MEM_ADC_VAL_RAW_ADD + ADC_CH_NO * ADC_RAW_VAL_SIZE,

	I2C_MEM_DIAG_TEMPERATURE_ADD,
	I2C_MEM_DIAG_3V3_MV_ADD,
	I2C_MEM_DIAG_3V3_MV_ADD1,
	I2C_MEM_CALIB_VALUE,
	I2C_MEM_CALIB_CHANNEL = I2C_MEM_CALIB_VALUE + 2,//ADC channels [1,8]; DAC channels [9, 12]
	I2C_MEM_CALIB_KEY, //set calib point 0xaa; reset calibration on the channel 0x55
	I2C_MEM_CALIB_STATUS,

	I2C_MEM_REVISION_HW_MAJOR_ADD = 0x78,
	I2C_MEM_REVISION_HW_MINOR_ADD,
	I2C_MEM_REVISION_MAJOR_ADD,
	I2C_MEM_REVISION_MINOR_ADD,

	SLAVE_BUFF_SIZE
}I2C_MEM_ADD;

#endif //LKIT_H_
