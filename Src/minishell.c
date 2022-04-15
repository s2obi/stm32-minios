#include "minishell.h"

uint8_t InputBuffer[INPUT_BUFFER_SIZE];
int16_t InputBufferIndex;
uint16_t InputBufferLength;
uint8_t shell_str[SHELL_LENGTH];
uint8_t path[PATH_LENGTH];
uint8_t rxData;
uint8_t txData;
UART_HandleTypeDef *huart;

void uart_send(char *data)
{
	int len = strlen((char *)data);
	HAL_UART_Transmit(huart, (uint8_t *)data, len, 10);
}

/**
 * @brief minishell 초기화
 *
 * @attention
 *  - UART_HandleTypeDef 정의
 *  - 입력 버퍼 초기화
 *  - 수신, 송신 데이터 초기화
 */
void minishell_init(UART_HandleTypeDef *uart)
{
	huart = uart;
	memset(InputBuffer, 0, INPUT_BUFFER_SIZE);
	memset(shell_str, 0, SHELL_LENGTH);
	memset(path, 0, PATH_LENGTH);
	strcpy((char *)shell_str, "minishell:/>");
	strcpy((char *)path, "/");
	rxData = 0;
	txData = 0;
	InputBufferIndex = -1;
	InputBufferLength = 0;
}

/**
 * @brief minishell 주 시작점
 */
void minishell_run(void)
{
	uint32_t BS = (uint32_t)"\b \b"; //
	while (1)
	{
		if (HAL_UART_Receive(huart, &rxData, 1, 10) == HAL_OK)
		{
			if (rxData == KEY_ENTER) {
				/* 입력버퍼에 데이터가 있는 경우, 명령어를 분석한다. */
				if (InputBufferLength > 0) { command_parse();	}
				/* 입력 버퍼 초기화*/
				memset(InputBuffer, 0, INPUT_BUFFER_SIZE);
				InputBufferIndex = -1;
				InputBufferLength = 0;
				/* 셸 줄바꿈 처리 */
				uart_send("\n\r");
				uart_send((char *)shell_str);
			} else {
				/* backspace 입력을 받으면 입력 버퍼의 맨 마지막 문자 제거한다. */
				if (rxData == KEY_BACKSPACE || rxData == KEY_DEL) {
					if (InputBufferLength > 0) {
						uart_send((char *)BS); // host terminal에 backspace 처리하기 위해 전송
						InputBuffer[InputBufferIndex] = 0;
						InputBufferIndex--;
						InputBufferLength--;
					}
				} else {
					/* 수신받은 데이터를 그대로 전송하고 입력 버퍼에 추가한다. */
					txData = rxData;
					uart_send((char *)&txData);
					InputBufferIndex++;
					InputBuffer[InputBufferIndex] = rxData;
					InputBufferLength++;
				}
			}
		}
	}
}

/**
 * @brief 명령어 분석
 *
 * @attention
 *  1. 지원 명령어
 *   - cd, ls, mkdir, rmdir, mk, rm, stf, program execute
 *   - 위 명령어 외에는 알 수 없는 명령어로 처리
 *  2. 명령어 구분
 *   - <command> [param1] [param2] ...
 *   - 명령어와 각 매개변수는 공백으로 구분하여 처리
 */
void command_parse(void)
{
	char buf_copy[INPUT_BUFFER_SIZE];		// 입력 버퍼 복사본
	char *s_ptr;												// 문자열 처리를 위한 포인터
	char command[INPUT_BUFFER_SIZE];		// 명렁어 저장 배열
	char param[INPUT_BUFFER_SIZE];			// 매개변수 저장 배열

	/* 배열 초기화 */
	memset(buf_copy, 0, INPUT_BUFFER_SIZE);
	memset(command, 0, INPUT_BUFFER_SIZE);
	memset(param, 0, INPUT_BUFFER_SIZE);
	/* 명령어 저장 */
	strcpy(buf_copy, (char *)InputBuffer);
	s_ptr = strtok((char *)buf_copy, " ");
	strcpy(command, s_ptr);
	if (strcmp(command, "ls") == 0) { // 현재 위치에 있는 디렉토리와 파일 목록 출력
		uart_send("\n\r");
		uart_send("====type=====size=====name=====\n\r");
		uart_send("|   dir   |        |  home    |\n\r");
		uart_send("|   dir   |        |  usr     |\n\r");
		uart_send("|   dir   |        |  bin     |\n\r");
		uart_send("|   file  | 423kb  |  a.out   |\n\r");
		uart_send("|   file  | 88kb   |  b.out   |\n\r");
	} else if (strcmp(command, "cd") == 0) { // 디렉토리 이동
		/* 매개변수 저장 */
		memset(buf_copy, 0, INPUT_BUFFER_SIZE);
		strcpy(buf_copy, (char *)InputBuffer); // 입력 버퍼 복사
		strncpy(param, (buf_copy + strlen(command) + 1), strlen((char *)buf_copy) - strlen(command));

		/* 현재 디렉토리 위치 업데이트 */
		/* 절대경로인 경우 */
		memset(path, 0, PATH_LENGTH);
		strcpy((char *)path, param);
		char *tmp = strcat((char *)path, ">");
		memset(shell_str, 0, SHELL_LENGTH);
		strcpy((char *)shell_str, "minishell:");
		strcpy((char *)shell_str, strcat((char *)shell_str, tmp));
		/* 상대경로인 경우*/
		uart_send("\n\r");
		uart_send("command: change directory(cd)");
		uart_send("\n\r");
		uart_send("param(path): ");
		if (param != NULL) { uart_send(param); }
	} else if (strcmp(command, "mkdir") == 0) { // 디렉토리 생성
		/* 매개변수 저장 */
		memset(buf_copy, 0, INPUT_BUFFER_SIZE);
		strcpy(buf_copy, (char *)InputBuffer); // 입력 버퍼 복사
		strncpy(param, (buf_copy + strlen(command) + 1), strlen((char *)buf_copy) - strlen(command));
		uart_send("\n\r");
		uart_send("command: make directory(mkdir)");
		uart_send("\n\r");
		uart_send("param: ");
		uart_send(param);
	} else if (strcmp(command, "rmdir") == 0) {
		/* 매개변수 저장 */
		memset(buf_copy, 0, INPUT_BUFFER_SIZE);
		strcpy(buf_copy, (char *)InputBuffer); // 입력 버퍼 복사
		strncpy(param, (buf_copy + strlen(command) + 1), strlen((char *)buf_copy) - strlen(command));
		uart_send("\n\r");
		uart_send("command: remove directory(rmdir)");
		uart_send("\n\r");
		uart_send("param: ");
		uart_send(param);
	} else if (strcmp(command, "rm") == 0) {
		/* 매개변수 저장 */
		memset(buf_copy, 0, INPUT_BUFFER_SIZE);
		strcpy(buf_copy, (char *)InputBuffer); // 입력 버퍼 복사
		strncpy(param, (buf_copy + strlen(command) + 1), strlen((char *)buf_copy) - strlen(command));
		uart_send("\n\r");
		uart_send("command: remove");
		uart_send("\n\r");
		uart_send("param: ");
		uart_send(param);
	} else if (strcmp(command, "stf") == 0) {
		/* 매개변수 저장 */
		memset(buf_copy, 0, INPUT_BUFFER_SIZE);
		strcpy(buf_copy, (char *)InputBuffer); // 입력 버퍼 복사
		strncpy(param, (buf_copy + strlen(command) + 1), strlen((char *)buf_copy) - strlen(command));
		uart_send("\n\r");
		uart_send("command: store file");
		uart_send("\n\r");
		uart_send("param: ");
		uart_send(param);
	} else {
		/* 실행파일인 경우, 프로그램 실행하고 그렇지 않으면 알 수 없는 명령어로 처리 */
		if (strstr(command, ".bin") != NULL) {
			uart_send("\n\r");
			uart_send("program execute: ");
			uart_send(command);
		} else {
			char s[INPUT_BUFFER_SIZE + 23] = "'";
			char *str_error = strcat(s, (char *)InputBuffer);
			str_error = strcat(str_error, "' is unknown command.");
			uart_send("\n\r");
			uart_send(str_error);
		}
	}
}
