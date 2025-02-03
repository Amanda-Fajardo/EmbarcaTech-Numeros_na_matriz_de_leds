#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h" 
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

#define LED_R 13
#define LED_G 11
#define LED_B 12

#define BT_A 5
#define BT_B 6 

// Número atual exibido na matriz de LEDs
volatile int num_atual = 0; 
volatile bool atualizar_display = false; // Flag para atualizar display

// Variáveis para debouncing
static absolute_time_t last_time_bt_a = 0;
static absolute_time_t last_time_bt_b = 0;
#define DEBOUNCE_DELAY_MS 50 // Tempo de debouncing em milissegundos

// Números de 0 a 9 representados na matriz 5x5
bool numeros[10][NUM_PIXELS] = {
       { 
        0, 1, 1, 1, 0, // 0
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //1
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0
    },
    { 
        0, 1, 1, 1, 0, //2
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //3
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 0, 0, 0, //4
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //5
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    },
    {
        0, 1, 1, 1, 0, //6
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 0, 0, 0, //7
        0, 0, 0, 1, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //8
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    {
        0, 1, 1, 1, 0, //9
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    }

};

// Acende o LED na cor especificada
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Converte valores RGB para um número de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Debounce para os botões      
bool debounce(uint gpio) {
    static absolute_time_t last_time = 0;                   //armazena o último momento da leitura
    absolute_time_t now = get_absolute_time();              //obtém o tempo atual
    if (absolute_time_diff_us(last_time, now) < 50000) {    //calcula a diferença de tempo entre a última leitura e o tempo atual
        return false;                                       //se a diferença for menor que 50.000 µs (50 ms), a função retorna false
    }                                                       //a leitura atual

    last_time = now; //atualiza o tempo para a última leitura válida
    return !gpio_get(gpio);
}

// Exibe o número na matriz de LEDs
void exibir_numero(int numero) {
    if (numero < 0 || numero > 9) return; // Evita números inválidos

    for (int i = 0; i < NUM_PIXELS; i++) {     
        if (numeros[numero][i]) {             
            put_pixel(urgb_u32(255, 0, 255)); // Acende em roxo
        } else {
            put_pixel(urgb_u32(0, 0, 0)); // Apaga o LED
        }
    }
    sleep_ms(10); 
}

// Interrupção dos botões
void gpio_irq(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();

    if (gpio == BT_A) {
        if (absolute_time_diff_us(last_time_bt_a, now) > DEBOUNCE_DELAY_MS * 1000) {
            if (!gpio_get(BT_A)) {
                num_atual = (num_atual + 1) % 10;
                atualizar_display = true;
            }
            last_time_bt_a = now;
        }
    } else if (gpio == BT_B) {
        if (absolute_time_diff_us(last_time_bt_b, now) > DEBOUNCE_DELAY_MS * 1000) {
            if (!gpio_get(BT_B)) {
                num_atual = (num_atual + 9) % 10;
                atualizar_display = true;
            }
            last_time_bt_b = now;
        }
    }
}

int main() {
    stdio_init_all();

    // Configuração dos LEDs RGB
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);

    // Configuração dos botões
    gpio_init(BT_A);
    gpio_init(BT_B);
    gpio_set_dir(BT_A, GPIO_IN);
    gpio_set_dir(BT_B, GPIO_IN);
    gpio_pull_up(BT_A);
    gpio_pull_up(BT_B);
    
    // Interrupção dos botões
    gpio_set_irq_enabled_with_callback(BT_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq);
    gpio_set_irq_enabled_with_callback(BT_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq);
    
    // Inicializa o PIO para WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Exibe o primeiro número na inicialização
    exibir_numero(num_atual);

    while (1) {
        if (atualizar_display) {
            exibir_numero(num_atual);
            atualizar_display = false;
        }

        // Pisca o LED vermelho
        gpio_put(LED_R, 1);
        sleep_ms(100);
        gpio_put(LED_R, 0);
        sleep_ms(100);
    }

    return 0;
}