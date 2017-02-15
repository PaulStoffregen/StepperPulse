// https://forum.pjrc.com/threads/41857-Options-for-bare-metal-development/page2

#include "DMAChannel.h"

// FTM3 ch0 = pin 2
// FTM3 ch1 = pin 14
// FTM3 ch2 = pin 7
// FTM3 ch3 = pin 8
// FTM3 ch4 = pin 35
// FTM3 ch5 = pin 36
// FTM3 ch6 = pin 37
// FTM3 ch7 = pin 38

DMAChannel dma;

uint32_t output[] = {
	(10000u << 16) | 0xDD, // set on match
	(30000u << 16) | 0xD9, // clear on match
	(40000u << 16) | 0xDD, // set on match
	(45000u << 16) | 0xD9, // clear on match
	(25000u << 16) | 0xD9, // clear on match
};

void setup()
{
	while (!Serial) ;
	Serial.println("fast stepper test");
	FTM3_SC = 0;    // halt timer
	FTM3_C0V = 19000;
	FTM3_CNT = 0;   // reset timer
	FTM3_MOD = 0xFFFF;
	FTM3_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0); // start timer

	FTM3_C2SC = 0x28;  // 50% duty reference waveform on pin 7
	FTM3_C2V = 0x7FFF;

	CORE_PIN2_CONFIG =  PORT_PCR_MUX(4) | PORT_PCR_SRE; // ch0, pin 2 = output
	CORE_PIN7_CONFIG =  PORT_PCR_MUX(4) | PORT_PCR_SRE; // ch2, pin 7 = reference

	// use DMA
	dma.begin();
	dma.TCD->SADDR = output;
	dma.TCD->SOFF = 4;
	dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(1);
	dma.TCD->NBYTES_MLNO = DMA_TCD_NBYTES_DMLOE | DMA_TCD_NBYTES_MLOFFYES_MLOFF(-8)
		| DMA_TCD_NBYTES_MLOFFYES_NBYTES(4);
	dma.TCD->SLAST = -sizeof(output);
	dma.TCD->DADDR = &FTM3_C0SC;
	dma.TCD->DOFF = 4;
	dma.TCD->CITER_ELINKNO = sizeof(output) / 4;
	dma.TCD->DLASTSGA = -8;
	dma.TCD->BITER_ELINKNO = sizeof(output) / 4;
	dma.TCD->CSR = 0;
	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_FTM3_CH0);
	dma.enable();
	FTM3_C0SC = 0x59; // clear on match, use DMA
	FTM3_C0SC = 0x5D; // set on match, use DMA

	Serial.println("done");
}


void loop()
{



}
