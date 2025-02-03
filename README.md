# EmbarcaTech-Numeros_na_matriz_de_leds

Este projeto utiliza a placa BitDogLab com um Raspberry Pi Pico para controlar uma matriz de leds WS2812, leds RGB e dois botões, permitindo com que seja exibido uma sequência de números de 0 a 9 de forma interativa. A interação ocorre por meio dos dois botões presentes na placa, que utilizam **interrupções(IRQ)** e **debounce via software**.

# Componentes utilizados

• Matriz 5x5 de LEDs (endereçáveis) WS2812, conectada à GPIO 7;
• LED RGB, com os pinos conectados às GPIOs (11, 12 e 13);
• Botão A conectado à GPIO 5;
• Botão B conectado à GPIO 6.

# Funcionalidades

1. **LED RGB**

   O LED vermelho pisca a cada 100 ms, totalizando 5 vezes por segundo.

2. **Matriz de LEDs**

   Os números de 0 a 9 são exibidos em um formato fixo na matriz de LEDs.

3. **Controle com Botões**

   Botão A: Incrementa o número exibido.

   Botão B: Decrementa o número exibido.

   O tratamento dos botões é feito via interrupções (IRQ) com debouncing para evitar leituras incorretas.

5. **Debouncing**

   Implementado via software para garantir que apenas uma leitura válida seja considerada por pressionamento.

# Como compilar e executar

  Clone o repositório do projeto através da URL do repositório (ou copie o código para o seu ambiente de desenvolvimento).

  Configure o ambiente para compilar projetos para o Raspberry Pi Pico.

Compile o código usando o comando:

```cmake
mkdir build
cd build
cmake ..
make
```
Conecte o Raspberry Pi Pico ao computador no modo de bootloader (segure o botão BOOTSEL ao conectar o USB).

Copie o arquivo `.uf2` gerado para o Pico.
