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

# How to design your own Theme
一个主题至少应该包括一个`index.html`主页，其文件夹的树形结构如下：
```shell
your-theme-floder
├── index.html
└── style.css
```
使用时将主题文件夹整个放到`themes`文件夹中，再到`configure.json`文件中修改`theme`的值为主题文件夹的名字即可