# system_programming

**Display Output Module**

```shell
$ sudo git clone https://github.com/goodtft/LCD-show.git

$ cd LCD-show

$ sudo ./LCD35-show
```

**ChatGPT Call Module**
```shell
gcc -o server final_version/gpt_com.c -lcurl -lcjson
```

**Keyboard Input Module**

1. get wiringPi 
```bash
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
```

2. check membrane.c, membrane.h, client.c is in same folder. then, enter the command.
```bash
gcc -Wall -o client client.c membrane.c -lwiringPi
```