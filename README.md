# How to Build
## Clone and Get submodule
```shell
git clone https://github.com/janey7695/tinyblog
cd tinyblog
git submodule update --init --recursive
```

## Build
```shell
mkdir build
cd build
cmake ..
cmake --build .
```

# Usage
可执行的二进制文件在`tinyblog/build/bin`中,将`tinyblog/templates`中的文件放到`可执行文件同目录下`

用法：`tinyblog -c <configure json file path>`,如
```
./tinyblog -c configure.json
```
浏览器访问:`localhost:8000`即可看到非常简陋的主页

该Blog系统是实时更新目录以及内容的，因此启动该服务后，可以直接对articles目录下的markdown文件进行修改。