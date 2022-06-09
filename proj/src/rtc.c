#include <lcom/lcf.h>
#include "rtc.h"

struct Date date;

int hook_id_rtc = 3;

bool TIME_ENDED = false;

int rtc_subscribe(){
    if (sys_irqsetpolicy(IRQ8, IRQ_REENABLE, &hook_id_rtc)) return 1;
    return 0;
}
int rtc_unsubscribe(){
  if (sys_irqrmpolicy(&hook_id_rtc) != 0) return 1;
  return 0;
}

int rtc_enable_update_alarm(){
    uint32_t data;
    data = read_from_rtc(RTC_REGISTER_B);
    
    data |= RTC_UIE | RTC_AIE;

    sys_outb(RTC_ADDR_REG, RTC_REGISTER_B);
    sys_outb(RTC_DATA_REG, data);
    
    return 0;
    
}

int rtc_disable_update_alarm(){
    uint32_t data;
    data = read_from_rtc(RTC_REGISTER_B);
    
    data &= ~RTC_UIE;
    data &= ~RTC_AIE;

    sys_outb(RTC_ADDR_REG, RTC_REGISTER_B);
    sys_outb(RTC_DATA_REG, data);
    
    return 0;
}

void rtc_ih() {
	uint32_t data;
    data = read_from_rtc(RTC_REGISTER_C);
    if (data & RTC_AIE)
      TIME_ENDED = true;
}

int(read_from_rtc)(uint8_t reg) {
  uint32_t data;
  if (sys_outb(RTC_ADDR_REG, reg))
    return 1;

  if (sys_inb(RTC_DATA_REG, &data))
    return 1;

  return data;
}

int (rtc_get_date)() {

	uint32_t reg;

	do {
		reg = read_from_rtc(RTC_REGISTER_A);
		if(reg == RTC_ERROR) {
			return -1;
		}
	} while((reg & RTC_UIP) != 0);

    date.year = read_from_rtc(RTC_YEAR_REG);
    date.month = read_from_rtc(RTC_MONTH_REG);
    date.day = read_from_rtc(RTC_DAY_REG);
    date.hour = read_from_rtc(RTC_HOUR_REG);
    date.min = read_from_rtc(RTC_MINUTE_REG);
    date.sec = read_from_rtc(RTC_SECOND_REG);
    
	if( date.year == RTC_ERROR ||
   date.month == RTC_ERROR || 
   date.day == RTC_ERROR || 
   date.hour == RTC_ERROR || 
   date.min == RTC_ERROR || 
   date.sec == RTC_ERROR) {
		return -1;
	}

	return 0;
}

