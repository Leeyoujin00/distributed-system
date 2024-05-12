#include "util.h"

const char* dst_ip = "127.0.0.1"; // 하나의 host안에서 통신할 것이므로 서버주소는 localhost(i.e., 127.0.0.1)임
#define NUM_SRV 2

// 임의의 key를 생성해서 반환해줌
void generate_key(char* key) {
    uint64_t number = rand() % DATASET_SIZE;
    for (int i = 0; i < 5; ++i) number = ((number << 3) - number + 7) & 0xFFFFFFFFFFFFFFFF;
    key[KEY_SIZE - 1] = '\0';
    for (int i = KEY_SIZE - 2; i >= 0; i--) {
        int index = number % SET_SIZE;
        key[i] = SET[index];
        number /= SET_SIZE;
    }
}

int main(int argc, char *argv[]) {

  srand((unsigned int)time(NULL));  // 난수 발생기 초기화
  /* 서버 구조체 설정 */
	int SERVER_PORT = 5001; // 입력받은 argument를 포트번호 변수에 넣어준다.
	struct sockaddr_in srv_addr; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr, 0, sizeof(srv_addr)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	srv_addr.sin_port = htons(SERVER_PORT); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
	inet_pton(AF_INET, dst_ip, &srv_addr.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

  /* 소켓 생성 */
	int sock; // 소켓 디스크립터(socket descriptor)를 생성한다.
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n"); // sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
	}

	int n = 0;

  struct KVS SendMsg={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화

  struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr); // 수신한 패킷의 소켓 구조체 크기를 저장함. IPv4를 사용하므로 sockaddr_in 크기인 16바이트가 저장됨.
  int cnt = 0; // 패킷 5개를 전송한다.
  size_t pkt_size = 0;

	while(cnt < 5){
    printf("Request ID: %d\n",cnt++);
    // Your codes should be here

    struct KVS SendMsg={0,}; // 송신용으로 쓸 메시지 구조체 재선언 -> value 필드 처음에 비워야하므로
    struct KVS RecvMsg={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화 -> value 필드 처음에 비워야하므로

    // 임의의 key를 생성한다.
    generate_key(SendMsg.key);

    // Partition 번호를 구한다.
    int PARTITION = hash64(SendMsg.key);
    PARTITION = abs(PARTITION) % NUM_SRV; //음수가 나오는 경우가 있으므로 PARTITTION에 절댓값을 취한다.
    printf("Partition: %d\n", PARTITION);


    SERVER_PORT = 5000 + PARTITION + 1;
    srv_addr.sin_port = htons(SERVER_PORT); // 서버의 포트번호를 넣어준다.
    inet_pton(AF_INET, dst_ip, &srv_addr.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

    // 보낼 요청의 타입(읽기/쓰기)을 랜덤으로 지정한다.
    // 타입이 0 이면 읽기, 1 이면 쓰기 요청을 보내도록 한다.
    int req_type = rand() % 2;

    if (req_type == 0) { // 읽기 요청을 서버로 보낸다.
      SendMsg.type = READ_REQ;
      sendto(sock, &SendMsg, 17, 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr)); // 생성한 메시지를 서버로 송신한다.
      // 보낸 메시지의 byte 크기를 출력
      printf("Sent bytes: %d\n", 17);
    }

    else if (req_type == 1) { // 쓰기 요청을 서버로 보낸다.
      SendMsg.type = WRITE_REQ; // 서버로 보낼 요청의 타입을 WRITE로 변경
      strcpy(SendMsg.value, "AAAABBBBCCCCDDD"); // 쓰기 요청의 value 데이터를 담는다.
      sendto(sock, &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr)); // 생성한 메시지를 서버로 송신한다.
      // 보낸 메시지의 byte 크기를 출력
      printf("Sent bytes: %lu\n", sizeof(SendMsg));
    }

    printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg), SendMsg.key, SendMsg.value); // 송신한 내용을 출력해본다.

    n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len); // 서버로부터 답장을 수신한다.

		if (n > 0) { // 만약 송신한 데이터가 0바이트를 초과한다면 (즉, 1바이트라도 수신했다면)
      printf("Received bytes: %d\n", n);
      printf("Type: %s Key: %s Value: %s\n\n", get_type(RecvMsg), RecvMsg.key, RecvMsg.value); // 수신한 내용을 출력한다.
		}

	}

	close(sock); // 소켓을 닫아준다.
	return 0;
}
