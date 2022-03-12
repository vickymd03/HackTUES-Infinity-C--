#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>


/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

#define Data_OK 0

////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  RTC Time structure definition
  */
typedef struct{
	uint8_t RTC_Hours;  /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */
	uint8_t RTC_Minutes;/*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
	uint8_t RTC_Seconds;/*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */
	uint8_t RTC_H12;    /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef;

/**
  * @brief  RTC Date structure definition
  */
typedef struct{
	uint8_t RTC_WeekDay;/*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */
	uint8_t RTC_Month;  /*!< Specifies the RTC Date Month (in BCD format).
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */
	uint8_t RTC_Date;   /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */
	uint8_t RTC_Year;   /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

enum command{
	ov7725 = 0x77,
	mlx90640 = 0x90,
	SEND_MEASUREMENT = 0,
    SEND_SHOT = 1
};

enum ack{
	OK_ACK = 0x06,
    NAK = 0x15
};

////////////////////////////////////////////////////////////////////////////////

/*
void init_programs_data (programData_t *programs);
int make_file_data (char *buf, uint8_t mode);
char* convert_uint8_charchar(char *buf, uint8_t *number);
char* convert_hex_charchar(char *buf, uint8_t *number);
*/

////////////////////////////////////////////////////////////////////////////////
#endif /* ! _MAIN_H_ */
