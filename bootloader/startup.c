/* 
    startup = kod, który MCU wykonuje jako PIERWSZY po resecie.
    robi 2 rzeczy: kopiuje zmienne globalne do RAM i zeruje niezainicjalizowane.
    potem woła main().
*/

extern int main(void);

extern unsigned int _estack; // wierzcholek stosu
extern unsigned int _sdata; // poczatek .data w ram s[tart]data
extern unsigned int _edata; // koniec .data w ram e[nd]data
extern unsigned int _sidata; // poczatek .data w flash
extern unsigned int _sbss; // poczatek .bss
extern unsigned int _ebss; // koniec .bss

void Reset_Handler(void){
    unsigned int *src = &_sidata;
    unsigned int *dst = &_sdata;

    while(dst < &_edata){
        *dst++ = 0;
    }

    main();

    while(1);
}


/* 
    tablica wektorów przerwań.
    cortex-M po resecie czyta pierwsze dwa słowa z adresu 0x00000000:
    - [0] = wskaźnik stosu (MSP)
    - [1] = adres Reset_Handler
*/

__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))&_estack,
    Reset_Handler,
};