#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

struct Date {
  unsigned int sec;
  unsigned int min;
  unsigned int hour;
  unsigned int day;
  unsigned int month;
  unsigned int year;
};


#define IRQ8 8 
#define RTC_ERROR 0xFFFFFFFF
#define ADDR_REG 0x70 
#define REGISTER_C 12
#define REGISTER_B 11
#define REGISTER_A 10
#define DATA_REG 0x71 
#define UIP	BIT(7)
#define SECOND_REGISTER	0x00	
#define MINUTE_REGISTER	0x02	
#define HOUR_REGISTER 0x04	
#define DAY_REGISTER 0x07	
#define MONTH_REGISTER 0x08	
#define YEAR_REGISTER 0x09	
#define UIE BIT(4)

void rtc_ih();

int rtc_subscribe();

int rtc_unsubscribe();

int rtc_enable_update();

int rtc_disable_update();

int(read_rtc)(uint8_t reg);

int (rtc_get_date)();


#endif
