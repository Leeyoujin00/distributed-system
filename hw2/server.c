#include "util.h"

int SERVER_PORT; // 서버 포트번호
char* kv[DATASET_SIZE]; // 정적 key value stores
void init_kvs(){
  for(int i =0;i<DATASET_SIZE;i++){
		kv[i] = malloc(VALUE_SIZE);
		strcpy(kv[i], "DDDCCCCBBBBAAAA");
		//printf("%s\n",kv[i]);
	}
}

static volatile int quit = 0; // Trigger conditions for SIGINT
void signal_handler(int signum) {
	if(signum == SIGINT){  // Functions for Ctrl+C (SIGINT)
		quit = 1;
	}
}

int main(int argc, char *argv[]) {
	// 프로그램 시작시 입력받은 매개변수를 parsing한다.
	if ( argc < 2 ){
		printf("Input : %s port number\n", argv[0]);
	 	return 1;
	}

	signal(SIGINT, signal_handler); // SIGINT에 대한 핸들러 등록

	SERVER_PORT = atoi(argv[1]); // 입력받은 argument를 포트번호 변수에 넣어준다.

	// 서버의 정보를 담을 소켓 구조체 생성 및 초기화
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SERVER_PORT);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 i.e., 자기 자신의 IP

	// 소켓을 생성한다.
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not create listen socket\n");
		exit(1);
	}

	// 생성한 소켓에 소켓구조체를 bind시킨다.
	if ((bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr))) < 0) {
		printf("Could not bind socket\n");
		exit(1);
	}

	init_kvs(); // key-value store 초기화

	int n = 0;
    struct KVS RecvMsg = {0,}; // 수신용으로 쓸 메시지 구조체 생성

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체

  	socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;

	while (!quit) {
    // Your codes should be here
	// client로부터 요청을 수신한다.
		struct KVS RecvMsg = {0,}; // 수신용 메시지 담을 구조체 재선언
		struct KVS SendMsg = {0,}; // 송신용 메시지 담을 구조체 재선언

		n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len);

		if (n > 0) { // 만약 수신한 데이터가 0바이트를 초과한다면 (즉, 1바이트라도 수신했다면)
			
			printf("Type: %s Key: %s Value: %s\n", get_type(RecvMsg), RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력해본다.
			

			strcpy(SendMsg.key, RecvMsg.key); // 수신용 메시지 구조체에 key 복사

			// key값에 주어진 hash64 해시 함수를 적용, DATASET_SIZE 와 modulo 연산을 수행해 index 구함
			int index = hash64(RecvMsg.key) % DATASET_SIZE;

			if (RecvMsg.type == READ_REQ) { // 클라이언트가 보낸 요청이 읽기 요청임
				// 읽기 답장으로 메시지 type 변경
				SendMsg.type = READ_REP;

				// 정적 KVS의 value 값을 value 필드로 값을 복사
				strcpy(SendMsg.value, kv[index]);

				// client에 이를 전송
				sendto(sock, &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&src_addr, src_addr_len); // 생성한 메시지를 client로 송신한다.
			}

			else if(RecvMsg.type == WRITE_REQ) { // 클라이언트가 보낸 요청이 쓰기 요청임
				// 쓰기 답장으로 메시지 type 변경
				SendMsg.type = WRITE_REP;
				
				// 패킷의 value 값을 정적 KVS에 복사해 넣어줌
				strcpy(kv[index], RecvMsg.value);

				// client에 이를 전송
				sendto(sock, &SendMsg, 17, 0, (struct sockaddr *)&src_addr, src_addr_len); // 생성한 메시지를 client로 송신한다.
			}

		
		}
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}
