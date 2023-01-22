# How to Build
1. 本程序依赖于`libhv`以及`moonrise`，需要先编译二者的动态库
    - 进入项目主目录 `cd tinyblog`
    - 获取代码 `git submodule update --init --recursive` . 若libhv库没有被拉取，需要手动添加
 `git submodule add https://gitee.com/libhv/libhv.git libhv`

    - 编译libhv 
    
    ```
    mkdir libhv/build
    cd libhv/build
    cmake ..
    cmake --build .
    ```
    - 编译moonrise 
    
    ```
    cd moonrise
    make
    ```
2. 编译tinyblog  
```
mkdir build
cd build
cmake ..
cmake --build .
```
# Usage
可执行的二进制文件在`tinyblog/build/bin`中,将`tinyblog/test-file`中的`index.html`和`style.css`文件放到`可执行文件同目录下`

用法：`tinyblog <port>`,如
```
./tinyblog 8000
```
浏览器访问:`localhost:8000`即可看到非常简陋的主页

该Blog系统是实时更新目录以及内容的，因此启动该服务后，可以直接对articles目录下的markdown文件进行修改。