#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"
#include "i8042.h"
#include "timer.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
    uint8_t bit_no;
    int ipc_status;
    message msg;
    uint32_t packets_processed = 0;

    // Subscrever interrupções do rato
    if (mouse_subscribe_int(&bit_no) != 0) {
        printf("Erro ao subscrever interrupções do rato\n");
        return 1;
    }

    // Ativar data reporting (stream mode)
    if (mouse_write_cmd(MOUSE_ENABLE_DATA_REPORTING) != 0) {
        printf("Erro ao ativar data reporting\n");
        mouse_unsubscribe_int();
        return 1;
    }

    // Processar interrupções até receber o número de pacotes especificado
    while (packets_processed < cnt) {
        // Obter uma mensagem do driver
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            printf("Erro ao receber driver\n");
            continue;
        }

        if (is_ipc_notify(ipc_status)) { // Recebeu notificação
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: // Interrupção de hardware
                    if (msg.m_notify.interrupts & bit_no) { // Interrupção do rato
                        mouse_ih(); // Chama o interrupt handler do rato
                        mouse_sync_bytes(); // Sincroniza os bytes do pacote

                        if (byte_index == 0) { // Pacote completo foi processado
                            packets_processed++;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Desativar data reporting
    if (mouse_write_cmd(MOUSE_DISABLE_DATA_REPORTING) != 0) {
        printf("Erro ao desativar data reporting\n");
        mouse_unsubscribe_int();
        return 1;
    }

    // Cancelar subscrição de interrupções
    if (mouse_unsubscribe_int() != 0) {
        printf("Erro ao cancelar subscrição de interrupções do rato\n");
        return 1;
    }

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    uint8_t mouse_bit_no, timer_bit_no;
    int ipc_status;
    message msg;
    uint32_t seconds_elapsed = 0;

    // Converter tempo de idle para interrupções do timer (60Hz)
    uint8_t idle_ticks = idle_time * sys_hz();

    // Subscrever interrupções do rato
    if (mouse_subscribe_int(&mouse_bit_no) != 0) {
        printf("Erro ao subscrever interrupções do rato\n");
        return 1;
    }

    // Subscrever interrupções do timer
    if (timer_subscribe_int(&timer_bit_no) != 0) {
        printf("Erro ao subscrever interrupções do timer\n");
        mouse_unsubscribe_int();
        return 1;
    }

    // Ativar data reporting (stream mode)
    if (mouse_write_cmd(MOUSE_ENABLE_DATA_REPORTING) != 0) {
        printf("Erro ao ativar data reporting\n");
        timer_unsubscribe_int();
        mouse_unsubscribe_int();
        return 1;
    }

    // Inicializar contador para verificar tempo de idle
    counter = 0;

    // Processar interrupções até ultrapassar o tempo de idle
    while (counter < idle_ticks) {
        // Obter uma mensagem do driver
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            printf("Erro ao receber driver\n");
            continue;
        }

        if (is_ipc_notify(ipc_status)) { // Recebeu notificação
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: // Interrupção de hardware
                    if (msg.m_notify.interrupts & mouse_bit_no) { // Interrupção do rato
                        mouse_ih(); // Chama o interrupt handler do rato
                        mouse_sync_bytes(); // Sincroniza os bytes do pacote

                        // Resetar contador ao receber pacote completo
                        if (byte_index == 0) {
                            counter = 0;
                        }
                    }

                    if (msg.m_notify.interrupts & timer_bit_no) { // Interrupção do timer
                        timer_int_handler(); // Incrementa o contador global

                        // Verifica se um segundo passou
                        if (counter % sys_hz() == 0 && counter > 0) {
                            seconds_elapsed++;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Desativar data reporting
    if (mouse_write_cmd(MOUSE_DISABLE_DATA_REPORTING) != 0) {
        printf("Erro ao desativar data reporting\n");
        timer_unsubscribe_int();
        mouse_unsubscribe_int();
        return 1;
    }

    // Cancelar subscrição de interrupções do timer
    if (timer_unsubscribe_int() != 0) {
        printf("Erro ao cancelar subscrição de interrupções do timer\n");
        mouse_unsubscribe_int();
        return 1;
    }

    // Cancelar subscrição de interrupções do rato
    if (mouse_unsubscribe_int() != 0) {
        printf("Erro ao cancelar subscrição de interrupções do rato\n");
        return 1;
    }

    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
	uint8_t bit_no;
  	int ipc_status;
  	message msg;

 	 // Estados para a máquina de estados
	typedef enum {
    	INIT,       // Estado inicial
    	DRAW_UP,    // Desenhando a primeira linha (botão esquerdo)
    	VERTEX,     // Esperando no vértice
    	DRAW_DOWN,  // Desenhando a segunda linha (botão direito)
    	DETECTED    // Gesto detectado
  	} GestureState;

	GestureState state = INIT;
  	struct packet pp;
  	int x_displacement = 0;

    // Subscrever interrupções do rato
    if (mouse_subscribe_int(&bit_no) != 0) {
        printf("Erro ao subscrever interrupções do rato\n");
        return 1;
    }

    // Ativar data reporting (stream mode)
    if (mouse_write_cmd(MOUSE_ENABLE_DATA_REPORTING) != 0) {
        printf("Erro ao desativar data reporting\n");
        mouse_unsubscribe_int();
        return 1;
    }

    // Processar interrupções até reconhecer o gesto
    while (state != DETECTED) {
        // Obter uma mensagem do driver
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            printf("Erro ao receber driver\n");
            break;
        }

        if (is_ipc_notify(ipc_status)) { // Recebeu notificação
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: // Interrupção de hardware
                    if (msg.m_notify.interrupts & bit_no) { // Interrupção do rato
                        mouse_ih(); // Chama o interrupt handler do rato

						// Atualizar o índice e o pacote
            			if (byte_index == 0 && (current_byte & BIT(3))) {
              				packet[byte_index] = current_byte;
              				byte_index++;
           				} else if (byte_index > 0 && byte_index < 3) {
              				packet[byte_index] = current_byte;
              				byte_index++;
            			}

        			    // Se completou um pacote, processar e atualizar a máquina de estados
            			if (byte_index == 3) {
              				// Preencher a estrutura do pacote
              				pp.bytes[0] = packet[0];
      				        pp.bytes[1] = packet[1];
              				pp.bytes[2] = packet[2];

				            pp.lb = pp.bytes[0] & MOUSE_LB;
              				pp.rb = pp.bytes[0] & MOUSE_RB;
              				pp.mb = pp.bytes[0] & MOUSE_MB;

			             	// Calcular os deslocamentos
              				if (pp.bytes[0] & MOUSE_X_SIGN) {
                				pp.delta_x = pp.bytes[1] | 0xFF00; // Complemento de 2 para valores negativos
              				} else {
                				pp.delta_x = pp.bytes[1];
              				}

            				if (pp.bytes[0] & MOUSE_Y_SIGN) {
                				pp.delta_y = pp.bytes[2] | 0xFF00; // Complemento de 2 para valores negativos
              				} else {
                				pp.delta_y = pp.bytes[2];
              				}

				            pp.x_ov = pp.bytes[0] & MOUSE_X_OVERFLOW;
              				pp.y_ov = pp.bytes[0] & MOUSE_Y_OVERFLOW;

							// Exibir o pacote
                            mouse_print_packet(&pp);

                            // Máquina de estados para reconhecer o gesto
                            switch (state) {
                                case INIT:
                                    // Verifica se o botão esquerdo foi pressionado (sem outros botões)
                                    if (pp.lb && !pp.rb && !pp.mb) {
                                        state = DRAW_UP;
                                        x_displacement = 0;
                                    }
                                    break;

                                case DRAW_UP:
                                    // Verifica se ainda está a desenhar
                                    if (pp.lb && !pp.rb && !pp.mb) {
               						    // Verificar se o movimento tem inclinação positiva (slope > 1)
                  						if (pp.delta_x > -tolerance && pp.delta_y > -tolerance) {
                      						// Aceitar movimento se a inclinação for maior que 1
                      						if (abs(pp.delta_y) > abs(pp.delta_x)) {
                        						x_displacement += pp.delta_x;
                      						} else {
                        						// Reset se a inclinação não for adequada
                        						state = INIT;
                        						x_displacement = 0;
                      						}
                    					} else {
                      						// Reset se houver um movimento negativo maior que a tolerância
                      						state = INIT;
                      						x_displacement = 0;
                   						}
                  					}
                  					// Transição para VERTEX quando LB é disponibilizado
                  					else if (!pp.lb && !pp.rb && !pp.mb) {
                    					// Verificar se o deslocamento horizontal é suficiente
                    					if (x_displacement >= x_len) {
                      						state = VERTEX;
                    					} else {
                      						state = INIT;
                     						x_displacement = 0;
                    					}
                  					} else {
                    					// Reset se outros botões forem pressionados
                    					state = INIT;
                    					x_displacement = 0;
                  					}
                  					break;

                                case VERTEX:
                                    // Verificar se está no vértice (sem botões pressionados)
                 					if (!pp.lb && !pp.rb && !pp.mb) {
                 				   		// Verificar se há movimento residual
                    					if (abs(pp.delta_x) <= tolerance && abs(pp.delta_y) <= tolerance) {
                      						// Continuar no estado VERTEX
                    					} else {
											// Reset se o movimento for maior que a tolerância
                                        	state = INIT;
											x_displacement=0;
                                 		}
									}
									// Transição para DRAW_DOWN quando RB é pressionado e outros não
                 					else if (!pp.lb && pp.rb && !pp.mb) {
                    					state = DRAW_DOWN;
                    					x_displacement = 0;
                  					} else {
                    					// Reset se outros botões forem pressionados
                    					state = INIT;
                    					x_displacement = 0;
                  					}
                                    break;
								case DRAW_DOWN:
             				    	// Verificar se ainda está desenhando a segunda linha
                  					if (!pp.lb && pp.rb && !pp.mb) {
                    					// Verificar se o movimento tem inclinação negativa (slope < -1)
                    					if (pp.delta_x > -tolerance && pp.delta_y < tolerance) {
                      						// Aceitar movimento se a inclinação for menor que -1
                      						if (abs(pp.delta_y) > abs(pp.delta_x)) {
                        						x_displacement += pp.delta_x;
                      						} else {
                        						// Reset se a inclinação não for adequada
                        						state = INIT;
                        						x_displacement = 0;
                      						}
                    					} else {
                      						// Reset se houver um movimento inadequado
                      						state = INIT;
                      						x_displacement = 0;
                      					}
                    				}
                  					// Transição para DETECTED quando RB é liberado
                    				else if (!pp.lb && !pp.rb && !pp.mb) {
                      					// Verificar se o deslocamento horizontal é suficiente
                      					if (x_displacement >= x_len) {
                        					state = DETECTED;
                      					} else {
                        					state = INIT;
                        					x_displacement = 0;
                      					}
                    				} else {
                      					// Reset se outros botões forem pressionados
                      					state = INIT;
                  					    x_displacement = 0;
                    				}
                  					break;
				                default:
            				    	break;
              				}

					   		// Resetar o índice para o próximo pacote
					        byte_index = 0;
            			}
          			}
                   	break;
                default:
                  	break;
           	}
        }
	}

    // Desativar data reporting
    if (mouse_write_cmd(MOUSE_DISABLE_DATA_REPORTING) != 0) {
        printf("Erro ao desativar data reporting\n");
        mouse_unsubscribe_int();
        return 1;
    }

    // Cancelar subscrição de interrupções
    if (mouse_unsubscribe_int() != 0) {
        printf("Erro ao cancelar subscrição de interrupções do rato\n");
        return 1;
    }

    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
