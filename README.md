## 在linux环境编译源码并编写测试程序
### 1. 安装环境
```shell
sudo apt-get install perl g++ make automake libtool unzip
```
### 2. 安装并编译zlib
```shell
tar -xvf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure
sudo make && make install
```

### 3. 安装并编译openssl
```shell
tar -xvf openssl-1.1.1k.tar.gz
./config
sudo make && make install
```

### 4. 安装并编译libevent
```shell
tar -xvf libevent-2.1.12-stable.tar.gz
cd libevent-2.1.12-stable
./configure
make && sudo make install
```
> 接着执行
```shell
sudo vim /etc/ld.so.conf
```
> 在"include ld.so.conf.d/*.conf"下方增加"/usr/local/lib"

> 接着执行
```shell
sudo /sbin/ldconfig -v




