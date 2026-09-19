#include "system.h"

void system_init(void) 
{
    sysctl_pll_set_freq(SYSCTL_PLL0, SYSTEM_PLL0_FREQ); 
    sysctl_pll_set_freq(SYSCTL_PLL1, SYSTEM_PLL1_FREQ); 
    sysctl_pll_set_freq(SYSCTL_PLL2, SYSTEM_PLL2_FREQ);  

    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18); 
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18); 

    plic_init();
    dmac_init();

    sysctl_enable_irq();
}