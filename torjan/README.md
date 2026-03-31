# Torjan
    简单的跨平台木马

## 编译

### linux
```
g++ server.cpp -o server
g++ client.cpp -o client
```

### windows
```
g++ server.cpp -o server -lws2_32
g++ client.cpp -o client -lws2_32 -mwindows
```

## 运行
```
./server
./client ["服务器ip" "服务器端口"]
```
