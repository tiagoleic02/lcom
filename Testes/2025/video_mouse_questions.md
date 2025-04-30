# Questões de Teste: Placa de Vídeo e Mouse no Minix 3

## Modos de Vídeo e Gráficos

1. Quais são os dois modos principais de funcionamento de vídeo no Minix 3?

A) Modo Texto e Modo Gráfico
B) Modo Linear e Modo Indexado
C) Modo VGA e Modo SVGA
D) Modo Direto e Modo Indireto

✅ Resposta correta: A

2. Qual é a system call utilizada para configurar o modo gráfico através da Video BIOS Extension (VBE)?

A) sys_videocall()
B) sys_graphic_mode()
C) sys_int86()
D) sys_vbe_config()

✅ Resposta correta: C

3. O que significa o parâmetro BIT(14) quando configurado no registrador BX ao definir o modo gráfico?

A) Ativa o suporte a 14 bits por pixel
B) Define a resolução máxima como 2^14 pixels
C) Configura o uso de uma paleta com 14 cores
D) Torna o mapeamento da memória linear

✅ Resposta correta: D

4. Como se retorna ao modo de texto após utilizar o modo gráfico no Minix 3?

A) Reiniciando o sistema
B) Chamando a função reset_video_mode()
C) Chamando a função vg_exit() da LCF
D) Chamando sys_int86() com parâmetros específicos

✅ Resposta correta: C

5. O que é VRAM no contexto da programação gráfica?

A) Virtual Random Access Memory, usada para armazenar dados temporários
B) Video Random Access Memory, onde são armazenadas as informações de cor dos píxeis
C) Variable RAM, usada apenas para modos gráficos avançados
D) Vector RAM, utilizada para processamento de gráficos vetoriais

✅ Resposta correta: B

6. Qual estrutura armazena as informações sobre o modo gráfico após a chamada a vbe_get_mode_info()?

A) video_info_t
B) graphic_info_t
C) vbe_mode_info_t
D) display_info_t

✅ Resposta correta: C

7. Como se calcula o índice de um pixel na memória linear do frame buffer?

A) x + y
B) x * y
C) y * largura_horizontal + x
D) x * altura_vertical + y

✅ Resposta correta: C

8. Quais são os dois modelos de cores principais mencionados para o modo gráfico?

A) RGB e CMYK
B) YCbCr e RGB
C) HSL e YUV
D) Modelo Direto e Modelo Indexado

✅ Resposta correta: D

9. Qual é o propósito do parâmetro "PhysBasePtr" na estrutura vbe_mode_info_t?

A) Armazenar o endereço da tabela de paleta de cores
B) Determinar a resolução máxima suportada
C) Especificar o endereço físico do início do frame buffer
D) Indicar o número máximo de cores disponíveis

✅ Resposta correta: C

10. No modo indexado 0x105, quantos bits são utilizados para representar cada pixel?

A) 4 bits
B) 8 bits
C) 15 bits
D) 16 bits

✅ Resposta correta: B

## Manipulação de Memória e Pixels

11. Como se mapeia a memória física do frame buffer para memória virtual no Minix 3?

A) Usando a função map_phys_memory()
B) Usando a função vm_map_phys()
C) Usando a função sys_map_vram()
D) Usando a função sys_phys_copy()

✅ Resposta correta: B

12. O que acontece se tentarmos escrever em um pixel fora dos limites da resolução atual?

A) O sistema operativo corrige automaticamente as coordenadas
B) A operação é ignorada e retorna um erro
C) O sistema pode apresentar comportamento imprevisível ou falhar
D) A imagem é redimensionada automaticamente

✅ Resposta correta: C

13. Por que é necessário arredondar o número de bits por pixel para calcular o número de bytes por pixel?

A) Para otimizar o desempenho da placa gráfica
B) Porque a memória é alocada em bytes completos
C) Para compatibilidade com monitores antigos
D) Para suportar cores transparentes

✅ Resposta correta: B
💡 Explicação: A memória é alocada em bytes completos, então mesmo que um modo use 15 bits por pixel, serão necessários 2 bytes completos.

14. Como se calcula o tamanho total do frame buffer em bytes?

A) Soma da resolução horizontal e vertical
B) Largura * Altura
C) Largura * Altura * Bytes por Pixel
D) 2^(Bits por Pixel) * Largura * Altura

✅ Resposta correta: C

15. Ao pintar um pixel no frame buffer, por que utilizamos a função memcpy() em vez de atribuição direta?

A) Para garantir a sincronização com o hardware
B) Para evitar alterações em bits adjacentes quando a cor não preenche bytes completos
C) Por questões de desempenho
D) Para suportar transparência

✅ Resposta correta: B

16. Qual operação permite desenhar um pixel utilizando o modelo de cores direto no modo 0x115 (24 bits)?

A) frame_buffer[index] = color
B) memcpy(&frame_buffer[index], &color, BytesPerPixel)
C) set_pixel(x, y, RGB(r, g, b))
D) frame_buffer[y][x] = color

✅ Resposta correta: B

17. No modo 0x11A (16 bits, 5:6:5), como se cria uma cor verde pura?

A) 0x00FF00
B) (0 << 11) | (63 << 5) | 0
C) (0 << 11) | (31 << 5) | 0
D) 0x0000FF00

✅ Resposta correta: B
💡 Explicação: No formato 5:6:5, o verde ocupa 6 bits na posição central com valor máximo de 63.

18. Por que é importante voltar ao modo de texto antes de encerrar um programa que usa modo gráfico?

A) Para economizar bateria
B) Para evitar problemas de memória
C) Para manter a integridade do sistema
D) Apenas por questões estéticas

✅ Resposta correta: C

19. Qual função da LCF é usada para carregar uma imagem XPM?

A) xpm_init()
B) xpm_create()
C) xpm_load()
D) xpm_read()

✅ Resposta correta: C

20. Como se determina a posição física na memória onde um pixel deve ser escrito?

A) Usando a função get_pixel_pos()
B) Multiplicando as coordenadas pelo número de bits por pixel
C) Calculando o índice do pixel e multiplicando pelo número de bytes por pixel
D) Consultando a tabela de mapeamento de pixels

✅ Resposta correta: C

## Mouse e Interação com o KBC

21. Qual é a porta utilizada para ler os dados do mouse PS/2?

A) 0x60
B) 0x64
C) 0x20
D) 0x68

✅ Resposta correta: A

22. Quantos bytes compõem um pacote completo de dados do mouse PS/2 padrão?

A) 1 byte
B) 2 bytes
C) 3 bytes
D) 4 bytes

✅ Resposta correta: C

23. Como se pode ativar o mouse PS/2 no Minix 3?

A) Escrevendo o comando 0xA8 para a porta 0x64
B) Enviando uma interrupção IRQ12
C) Chamando a função enable_mouse()
D) Ativando o bit do mouse no registo de controlo do teclado

✅ Resposta correta: A

24. O que significa o bit 0 do primeiro byte de um pacote do mouse?

A) Botão direito pressionado
B) Botão esquerdo pressionado
C) Botão do meio pressionado
D) Movimento vertical

✅ Resposta correta: B

25. Como se envia um comando para o mouse através do KBC (Keyboard Controller)?

A) Escrevendo diretamente na porta 0x60
B) Escrevendo o comando na porta 0x64 e depois o argumento na porta 0x60
C) Escrevendo o comando 0xD4 na porta 0x64, seguido do comando do mouse na porta 0x60
D) Utilizando a função sys_mouse_cmd()

✅ Resposta correta: C

26. Qual é a IRQ associada ao mouse PS/2 no Minix 3?

A) IRQ0
B) IRQ1
C) IRQ8
D) IRQ12

✅ Resposta correta: D

27. O que indica o bit 3 do primeiro byte de um pacote do mouse?

A) O valor X é negativo
B) O valor Y é negativo
C) O movimento foi excessivo (overflow)
D) Foi recebido um pacote de dados válido

✅ Resposta correta: A

28. O que significa quando o bit 4 do primeiro byte de um pacote do mouse está ativo?

A) O valor X é negativo
B) O valor Y é negativo
C) Mouse está em modo de stream
D) Sempre deve ser 1 em pacotes válidos

✅ Resposta correta: D
💡 Explicação: O bit 4 do primeiro byte deve sempre ser 1 para indicar que é um pacote válido do mouse.

29. Qual é a principal diferença entre o modo de operação "Remote" e "Stream" do mouse?

A) O modo Remote envia dados apenas quando solicitado, o Stream envia continuamente quando há movimento
B) O modo Remote está limitado a 2 bytes por pacote
C) O modo Stream não suporta a roda de rolagem (scroll wheel)
D) O modo Remote só funciona em modo gráfico

✅ Resposta correta: A

30. Como se pode desativar o relatório de eventos do mouse?

A) Enviando o comando 0xF5 para o mouse
B) Desativando a IRQ12
C) Chamando a função disable_mouse()
D) Retornando ao modo de texto

✅ Resposta correta: A

31. Qual é o comando para solicitar que o mouse envie o seu ID de dispositivo?

A) 0xE8
B) 0xF2
C) 0xF3
D) 0xF6

✅ Resposta correta: B

32. O que significa quando o mouse retorna o ID 0x03?

A) Mouse com dois botões
B) Mouse com três botões
C) Mouse com roda de rolagem
D) Mouse sem fio

✅ Resposta correta: C

33. Como se pode definir a taxa de amostragem (sample rate) do mouse?

A) Usando o comando 0xF3 seguido do valor da taxa
B) Configurando o Timer para a frequência desejada
C) Escrevendo diretamente no registo de controle do mouse
D) Alterando o divisor da frequência do sistema

✅ Resposta correta: A

34. Qual operação bitwise permite verificar se o botão direito do mouse está pressionado no primeiro byte de um pacote?

A) if (byte & BIT(0))
B) if (byte & BIT(1))
C) if (byte & BIT(2))
D) if (byte & BIT(3))

✅ Resposta correta: B

35. Como converter o valor de deslocamento do mouse de 8 bits (com sinal) para um valor de 16 bits com sinal?

A) (int16_t)((delta & 0x80) ? (0xFF00 | delta) : delta)
B) (int16_t)(delta << 8)
C) (int16_t)(delta & 0x00FF)
D) (int16_t)(delta | 0xFF00)

✅ Resposta correta: A
💡 Explicação: É necessário estender o sinal, preenchendo com 1s os bits mais significativos se o bit 7 estiver ativo.

36. Qual comando permite ativar o relatório de eventos do mouse?

A) 0xF4
B) 0xF6
C) 0xEA
D) 0xEC

✅ Resposta correta: A

37. O que acontece se não se processar um pacote completo do mouse antes da chegada do próximo?

A) O sistema gera uma exceção
B) Os dados são sobrescritos no buffer
C) O mouse para de enviar dados até ser reiniciado
D) O KBC gera uma interrupção de erro

✅ Resposta correta: B

38. Qual é a importância do bit 6 do primeiro byte de um pacote do mouse?

A) Indica se o deslocamento X é positivo ou negativo
B) Indica se o deslocamento Y é positivo ou negativo
C) Indica se houve overflow no deslocamento Y
D) Indica se o pacote contém informações sobre a roda de rolagem

✅ Resposta correta: C

39. Como se pode combinar o modo gráfico e a interação com o mouse para criar uma interface gráfica?

A) Usando apenas funções predefinidas do Minix
B) Mapeando a posição do cursor para coordenadas do frame buffer e desenhando-o
C) O Minix já fornece uma API completa para interfaces gráficas
D) Não é possível usar o mouse em modo gráfico no Minix 3

✅ Resposta correta: B

40. Por que é necessário subscrever as interrupções tanto do teclado quanto do mouse ao criar uma aplicação gráfica interativa?

A) Para economizar recursos do sistema
B) Para permitir a comunicação entre dispositivos
C) Para capturar eventos de ambos os dispositivos de entrada
D) Porque o mouse usa o controlador do teclado para comunicação

✅ Resposta correta: C
