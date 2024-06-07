#include "util.h"

const char* dst_ip = "127.0.0.1";
int PRIMARY_SERVER_PORT; // 주서버 포트번호
int FOLLOWER_SERVER_PORT1 = 5002; // 팔로워 서버 포트번호1
int FOLLOWER_SERVER_PORT2 = 5003; // 팔로워 서버 포트번호2

int IS_PRIMARY_SERVER = 0;
int FOLLOWER_SERVER_NUM;

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
	else if (atoi(argv[1]) == 5001) {
		IS_PRIMARY_SERVER = 1;
	}
	else if (atoi(argv[1]) == 5002) {
		FOLLOWER_SERVER_NUM = 1;
	}
	else {
		FOLLOWER_SERVER_NUM = 2;
	}

	signal(SIGINT, signal_handler); // SIGINT에 대한 핸들러 등록

	PRIMARY_SERVER_PORT = atoi(argv[1]); // 입력받은 argument를 포트번호 변수에 넣어준다.

	// primary 서버의 정보를 담을 소켓 구조체 생성 및 초기화
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(PRIMARY_SERVER_PORT);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 i.e., 자기 자신의 IP

	/* follower 서버1 구조체 설정 */
	struct sockaddr_in srv_addr1; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr1, 0, sizeof(srv_addr1)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr1.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	srv_addr1.sin_port = htons(FOLLOWER_SERVER_PORT1); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
	inet_pton(AF_INET, dst_ip, &srv_addr1.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

	/* follower 서버2 구조체 설정 */
	int SERVER_PORT = 5003;
	struct sockaddr_in srv_addr2; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr2, 0, sizeof(srv_addr2)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr2.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	srv_addr2.sin_port = htons(FOLLOWER_SERVER_PORT2); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
	inet_pton(AF_INET, dst_ip, &srv_addr2.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.


	// // follower 서버1 의 정보를 담을 소켓 구조체 생성 및 초기화
	// struct sockaddr_in srv_addr1;
	// memset(&srv_addr1, 0, sizeof(srv_addr1));
	// srv_addr1.sin_family = AF_INET;
	// srv_addr1.sin_port = htons(FOLLOWER_SERVER_PORT1);
	// srv_addr1.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 i.e., 자기 자신의 IP

	// // follower 서버2 의 정보를 담을 소켓 구조체 생성 및 초기화
	// struct sockaddr_in srv_addr2;
	// memset(&srv_addr2, 0, sizeof(srv_addr2));
	// srv_addr2.sin_family = AF_INET;
	// srv_addr2.sin_port = htons(FOLLOWER_SERVER_PORT2);
	// srv_addr2.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 i.e., 자기 자신의 IP


	// 소켓을 생성한다.
	int sock;
	int sock1;
	int sock2;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not create listen socket\n");
		exit(1);
	}
	if ((sock1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not create listen socket\n");
		exit(1);
	}
	if ((sock2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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
	int n1 = 0;
	int n2 = 0;
	struct KVS RecvMsg={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화
	struct KVS RecvMsg1={0,};
	struct KVS RecvMsg2={0,};

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  	socklen_t src_addr_len = sizeof(src_addr);
	struct sockaddr_in src_addr1; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  	socklen_t src_addr_len1 = sizeof(src_addr1);
	struct sockaddr_in src_addr2; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  	socklen_t src_addr_len2 = sizeof(src_addr2);

	size_t pkt_size = 0;
	int index = 0;
	while (!quit) {
		// 실행된 서버가 PRIMARY SERVER인 경우
		if(IS_PRIMARY_SERVER) {
			n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
			if(n > 0) { 
				// PRIMARY SERVER가 읽기 요청을 받으면 key value store를 참조해서 해당하는 value를 클라이언트에게 return
				if (RecvMsg.type == READ_REQ){
					printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg),RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
					RecvMsg.type = READ_REP;
					index = hash64(RecvMsg.key) % DATASET_SIZE;
					strcpy(RecvMsg.value, kv[index]);
					pkt_size = n + VALUE_SIZE;

					sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
				}
				// PRIMARY SERVER가 쓰기 요청을 받으면 Follower 서버 두 대에게 쓰기 요청을 보냄
				else if (RecvMsg.type == WRITE_REQ) {
					printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg),RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
		
					index = hash64(RecvMsg.key) % DATASET_SIZE;
					strcpy(kv[index], RecvMsg.value);
					pkt_size = n - VALUE_SIZE;

					sendto(sock1, &RecvMsg, pkt_size, 0, (struct sockaddr *)&srv_addr1, sizeof(srv_addr1));
					// 두 FOLLOWER SERVER로부터 쓰기를 완료했다는 답장이 올 때 까지
					while(1) {
						n1 = recvfrom(sock1, &RecvMsg1, sizeof(RecvMsg1), MSG_DONTWAIT, (struct sockaddr *)&src_addr1, &src_addr_len1);
						if(n1 > 0) {
							break;
						}
					}

					sendto(sock2, &RecvMsg, pkt_size, 0, (struct sockaddr *)&srv_addr2, sizeof(srv_addr2));
					while(1) {
						n2 = recvfrom(sock2, &RecvMsg2, sizeof(RecvMsg2), MSG_DONTWAIT, (struct sockaddr *)&src_addr2, &src_addr_len2);
						if(n2 > 0) {
							break;
						}
					}
					printf("Write is Done\n\n");
					strcpy(RecvMsg.value,"");
					RecvMsg.type=READ_REP;
					sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
				

				}
			}	
		}
		else { // 실행된 서버가 FOLLOWER SERVER인 경우
		 	while(1) {
				if (FOLLOWER_SERVER_NUM == 1) {
					n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
			
					if(n > 0) {
						printf("Type: %s Key: %s\n", get_type(RecvMsg), RecvMsg.key); // 수신한 내용을 출력한다.
						index = hash64(RecvMsg.key) % DATASET_SIZE;
						strcpy(kv[index], RecvMsg.value);

						printf("Write is Done\n\n");
						pkt_size = n;
						sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
						break;
					}
				}
				else if (FOLLOWER_SERVER_NUM == 2) {
					n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
			
					if(n > 0) {
						printf("Type: %s Key: %s\n", get_type(RecvMsg), RecvMsg.key); // 수신한 내용을 출력한다.
						index = hash64(RecvMsg.key) % DATASET_SIZE;
						strcpy(kv[index], RecvMsg.value);

						printf("Write is Done\n\n");
						pkt_size = n;
						sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
						break;
					}
				}
			} 
		}
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}
