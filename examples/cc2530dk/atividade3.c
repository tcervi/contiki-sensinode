/* This is a very simple hello_world program.
 * It aims to demonstrate the co-existence of two processes:
 * One of them prints a hello world message and the other blinks the LEDs
 *
 * It is largely based on hello_world in $(CONTIKI)/examples/sensinode
 *
 * Author: George Oikonomou - <oikonomou@users.sourceforge.net>
 * Aletrado para a disciplina de Rede de Sensores Sem Fio
 */

#include "contiki.h"
#include "dev/leds.h"

#include <stdio.h> /* For printf() */
# define LED_PING_EVENT 44
# define LED_PONG_EVENT 45
/*---------------------------------------------------------------------------*/
static struct etimer et_hello;
static struct etimer et_blink;
static struct etimer et_proc3;
static struct timer  t_pong;
static uint16_t count, count2, countPong;
static uint8_t blinks;
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(blink_process, "LED blink process");
PROCESS(proc3_process, "Additional process");
PROCESS(pong_process, "Pong process");
AUTOSTART_PROCESSES(&blink_process, &hello_world_process, &proc3_process, &pong_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&et_hello, CLOCK_SECOND * 8);
  count = 0;

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER) {
		printf("Hello world: pinging...\n");
		process_post(&pong_process, LED_PING_EVENT,(void*)(&hello_world_process));
		PROCESS_WAIT_EVENT();
		if(ev == LED_PONG_EVENT){
			leds_toggle(LEDS_GREEN);
			etimer_reset(&et_hello);
			count++;
			printf("Hello...#%u: LED Pong!\n", count);
		}
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  PROCESS_BEGIN();

  blinks = 0;
  etimer_set(&et_blink, CLOCK_SECOND * 2);

  while(1){
	PROCESS_WAIT_EVENT();
	if(ev == PROCESS_EVENT_TIMER){
		printf("Blink: pinging...\n");
		process_post(&pong_process, LED_PING_EVENT,(void*)(&blink_process));
		PROCESS_WAIT_EVENT();
		if(ev == LED_PONG_EVENT){
			leds_toggle(LEDS_BLUE);
			blinks++;
			printf("Blink...#%u: LED Pong!\n", blinks);
			etimer_reset(&et_blink);
		}
	}
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(proc3_process, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&et_proc3, CLOCK_SECOND * 4);
  count2 = 0;

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER) {
		printf("Proc3: pinging...\n");
		process_post(&pong_process, LED_PING_EVENT,(void*)(&proc3_process));
		PROCESS_WAIT_EVENT();
		if(ev == LED_PONG_EVENT){
			leds_toggle(LEDS_RED);
			count2++;
			printf("Proc3... #%u: LED Pong! \n", count2);
			etimer_reset(&et_proc3);
		}
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pong_process, ev, data)
{
  PROCESS_BEGIN();

  countPong = 0;
  timer_set(&t_pong, (CLOCK_SECOND * 0.2));

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == LED_PING_EVENT) {
    	leds_on(LEDS_YELLOW);
    	printf("Pong esperando 200ms ...\n", count);
    	timer_reset(&t_pong);
    	while(!(timer_expired(&t_pong))){}
		leds_off(LEDS_YELLOW);
		countPong++;
		printf("Pong: LED Ping! Ponging num.: #%u! ...\n", countPong);
		process_post((struct process*)data, LED_PONG_EVENT, NULL);
    }
 }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
