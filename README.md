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
