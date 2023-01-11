#include <am.h>
#include <nemu.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t rtc_time[2] ;

  // pTimeUs = (uint32_t *)uptime->us;

  rtc_time[0] = inl(RTC_ADDR) ;
  rtc_time[1] = inl(RTC_ADDR + 4) ;
  uptime->us = (uint64_t)rtc_time[1] << 32 | rtc_time[0] ;

}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
