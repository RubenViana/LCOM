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

#define RTC_ADDR_REG 0x70 
#define RTC_REGISTER_C 12
#define RTC_REGISTER_B 11
#define RTC_REGISTER_A 10
#define RTC_DATA_REG 0x71 

#define RTC_SECOND_REG	0x00	
#define RTC_SECOND_ALARM_REG	0x01
#define RTC_MINUTE_REG	0x02	
#define RTC_MINUTE_ALARM_REG	0x03
#define RTC_HOUR_REG 0x04	
#define RTC_HOUR_ALARM_REG	0x05
#define RTC_DAY_REG 0x07	
#define RTC_MONTH_REG 0x08	
#define RTC_YEAR_REG 0x09

#define RTC_UIP	BIT(7)	
#define RTC_UIE BIT(4)
#define RTC_AIE BIT(5)

void rtc_ih();

int rtc_subscribe();

int rtc_unsubscribe();

int rtc_enable_update_alarm();

int rtc_disable_update_alarm();

int(read_from_rtc)(uint8_t reg);

int (rtc_get_date)();


#endif
