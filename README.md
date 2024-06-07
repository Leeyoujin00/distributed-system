# distributed-system
분산시스템 과제 repository

### hw1
**과제명**  UDP Echo 프로그램 만들기

**문제 설명**

본 과제는 POSIX Socket API를 사용해 클라이언트-서버 프로그램을 실제 구현하여, 분산 시스템의 실제 동작
원리를 이해하는 것을 목표로 한다.

<br>

### hw2

**과제명**  Distributed key-value stores 구현하기

**문제 설명**

본 과제에서는 여러 서버에 데이터가 나뉘어 저장되어 있는 분산 key-value store 서버와 해시 함수 기반으로 파티션을 고려해서 요청을 전송하는 클라이언트를 직접 구현함으로써, 파티셔닝과 분산 데이터 저장소에 대해 이해를 높인다.

<br>

### hw3

**과제명**  Look-aside cache 구현하기

**문제 설명**

본 과제에서는 look-aside cache를 구현해서 캐시 의 작동 원리에 대해 이해하는 것을 목표로 한다. 

<br>

### hw4

**과제명**  Power of two choices 로드 밸런싱 구현하기

**문제 설명**

본 과제에서는 power of two choices라는 새로운 알고리즘을 직접 구현해서 배움을 심화시켜 본다. Power of two choices는 random 로드밸런싱과 least connection을 혼합한 것과 유사하다. 방법은, 임의로 두 개의 서버를 선택을 한 후, 선택된 두 개의 서버 중에서 현재 전송중인 요청의 수 혹은 연결되어 있는 커넥션의 수가 적은 쪽을 선택한다.

<br>

### hw4

**과제명**  Primary-backup 구현하기

**문제 설명**

본 과제에서는 간단한 Primary-backup 프로토콜을 구현하여 복제 프로토콜에 대한 이해를 높인다. 

클라이언트는 임의의 key를 생성하여 읽기 요청(Read request) 혹은 쓰기 요청(Write request)을 보낸다. 클라이언트는 primary 서버의 포트 번호만을 알고 있습니다. 클라이언트는 모든 요청을 primary 서버에게 보낸다.

서버로부터 답장이 도착하면 해당 내용을 출력한다. 총 5개의 요청을 보내며, 읽기 요청 혹은 쓰기 요청인지는 50% 확률로 결정된다. 포트 번호 5001을 사용하면 primary 서버 역할을 수행합니다. 읽기 요청을 받으면 key value store를 참조해서 해당하는 value를 클라이언트에게 return 한다. 쓰기 요청을 받으면 자신의 key value store에 해당 값을 요청의 value 필드에 있는 값으로 갱신해주고, 포트 번호 5002와 5003을 사용하는 follower 서버 두 대에게 쓰기 요청을 보낸다. follower 서버 두 대로부터 쓰기가 완료되었다는 답장을 수신하면, 클라이언트에게 쓰기 답장을 return 한다.
