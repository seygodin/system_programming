# Let’s Cheat Final Exam using ChatGPT

This is usage for using this program. You need two rasberry pi(server and client). And we used one display for rasberry pi*. Following paragraph introduces the usage of of our project

**Demo Video**

[![Video Label](http://img.youtube.com/vi/zA1R5BKPsq0/0.jpg)](https://youtu.be/zA1R5BKPsq0)


*https://www.devicemart.co.kr/goods/view?no=1376592

**1. Copy this repository**

Run this code on server and client rasberry pi.
```shell
git clone https://github.com/seygodin/system_programming.git
```

**2. Compile Display Output Module**

Run this code on client rasberry pi.
```shell
$ sudo git clone https://github.com/goodtft/LCD-show.git

$ cd LCD-show

$ sudo ./LCD35-show
```

**3. Compile Keyboard Input Module**

Run this code on client rasberry pi.

*A. get wiringPi*
```bash
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
```

*B. check membrane.c, membrane.h, client.c is in same folder. then, enter the command.*

```bash
gcc -Wall -o client client.c membrane.c -lwiringPi
```

**4. Compile ChatGPT Call Module**

Run this code on server rasberry pi. You must put your own GPT api key to YOUR_API_KEY of final_version/gpt_com.c (line 97).
```shell
gcc -o server final_version/gpt_com.c -lcurl -lcjson
```

**5. Run program**

Server Rasberry Pi

```shell
./server
```

Client Rasberry Pi

```shell
./client
```
