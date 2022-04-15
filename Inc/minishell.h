#ifndef INC_MINISHELL_H_
#define INC_MINISHELL_H_

/*-------------------- Includes --------------------*/
#include "stm32f4xx_hal.h"
#include "string.h"

/*-------------------- Defines --------------------*/
#define INPUT_BUFFER_SIZE 		1024
#define PATH_LENGTH 					256
#define SHELL_LENGTH 					(PATH_LENGTH + 11)
#define KEY_UP 								"\x1b\x5b\x41"
#define KEY_DOWN 							"\x1b\x5b\x42"
#define KEY_RIGHT 						"\x1b\x5b\x43"
#define KEY_LEFT 							"\x1b\x5b\x44"
#define KEY_ENTER 						'\r'
#define KEY_BACKSPACE 				'\b'
#define KEY_DEL 							0x7F
#define KEY_SPACE 						0x20
#define TRUE 									1
#define FALSE 								0

/*-------------------- Variables --------------------*/
extern uint8_t InputBuffer[INPUT_BUFFER_SIZE];	// 크기가 최대 1024byte인 입력 버퍼, 입력받은 커맨드 명령어 저장
extern int16_t InputBufferIndex;								// 현재 입력 버퍼에 있는 마지막 문자의 인덱스 저장
extern uint16_t InputBufferLength;							// 현재 입력 버퍼의 문자 개수

extern uint8_t shell_str[SHELL_LENGTH];					// 셸 표시 문자열
extern uint8_t path[PATH_LENGTH];								// 현재 디렉토리 위치

extern uint8_t rxData;													// 수신 데이터
extern uint8_t txData;													// 송신 데이터

extern UART_HandleTypeDef *huart;								// UART 수신, 송신 처리를 위한 handle type


/*-------------------- Functions --------------------*/
void uart_send(char *data);
void minishell_init(UART_HandleTypeDef *uart);
void minishell_run(void);
void command_parse(void);

#endif /* INC_MINISHELL_H_ */
