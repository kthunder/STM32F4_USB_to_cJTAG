#include <stdint.h>
#include <stdio.h>

// #include "log.h"
#include "tools.h"

#include "hal_gpio.h"
#include "stm32f4xx.h"

// #include "tx_api.h"

// uint32_t my_thread_counter = 0;
// TX_THREAD my_thread;

// void my_thread_entry(ULONG thread_input)
// {
//     /* Enter into a forever loop. */
//     (void)(thread_input);
//     while (1)
//     {
//         /* Increment thread counter. */
//         my_thread_counter++;
//         GPIO_TogglePin(GPIOC, GPIO_Pin_13);
//         /* Sleep for 1 tick. */
//         tx_thread_sleep(1000);
//     }
// }

// void tx_application_define(void *first_unused_memory)
// {
//     /* Create my_thread! */
//     tx_thread_create(&my_thread, "My Thread",
//                      my_thread_entry, 0x1234, first_unused_memory, 1024,
//                      3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
// }

extern int misc_init();

extern void vTaskStartScheduler();

int main()
{
    misc_init();
    // vTaskStartScheduler();
    for (size_t i = 0; i < 0xFFFFFF; i++)
            ;
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    while (1)
    {
        // GPIO_TogglePin(GPIOC, GPIO_Pin_13);
        
        // GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        // log_info("heart beat!");
        printf("heart beat!\n");

        for (size_t i = 0; i < 0xFFFFFF; i++)
            ;
    }
    // tx_kernel_enter();
}