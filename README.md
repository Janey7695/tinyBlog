# 这是什么？ What is it?
![version: 1.2.0 (shields.io)](https://img.shields.io/badge/version-1.2.0-brightgreen)

`tinyBlog`是一款简单、纯粹、低占用的博客软件，正如所介绍的：
1. 简单：其只实现了一个简单的http服务器，支持主题样式自定义
2. 纯粹：个人认为Blog软件即为了Blog而生，只是纯粹的分享，记录自己所发生的所见识的，因此`tinyblog`没有华丽的功能，复杂的接口，就只是将`markdown`文件转换为`html`页面而已
3. 低占用：正常使用内存占用不超过3M

# 特性 Feature
__几乎没有哈哈！__
* `动态响应 dynamic response` 可以在运行时新增、删除、修改`markdown`源文件
* `低占用 low memory footprint` 想必是由于目前功能少，所以程序占用低...
* `纯粹 pureness`
* `简单 simple`

# 构建 How to Build
## Clone and Get submodule
```shell
git clone https://github.com/janey7695/tinyblog
cd tinyblog
git submodule update --init --recursive
```

## Build
```shell
make
```
暂无`make install`选项，需要的话拷贝可执行的二进制文件到自己需要的位置去即可
# 使用 Usage

可执行的二进制文件在`tinyblog/build/bin`中,将`tinyblog/templates`中的文件放到`可执行文件同目录下`


用法：`tinyblog -c <configure json file path>`,如
```
./tinyblog -c configure.json
```
浏览器访问:`localhost:8000`即可看到非常简陋的主页

该Blog系统是实时更新目录以及内容的，因此启动该服务后，可以直接对articles目录下的markdown文件进行修改。

# 自定义主题 How to design your own Theme
一个主题至少应该包括一个`index.html`主页，其文件夹的树形结构如下：
```shell
your-theme-floder
├── index.html
├── xx-style.css
└── xx-style.css
```
其中不同的`xx-style.css`负责不同页面的渲染，但是`xx`有规定，命名必须遵循如下：
- `home-style.css` 负责主页的样式渲染
- `mkd-style.css` 负责文章页面的渲染
- `list-style.css` 负责`Archive`即文章列表页面的渲染
使用时将主题文件夹整个放到`themes`文件夹中，再到`configure.json`文件中修改`theme`的值为主题文件夹的名字`your-theme-floder`即可

# Thanks

|项目名称|作用简介|仓库地址|
|---|---|---|
|tOO|自己写的简单的线程池|https://github.com/janey7695/too.git|
|sundown(moonrise)|作为markdown解析器|https://github.com/vmg/sundown|
|cJSON|cjson解析器，用以解析配置文件|https://github.com/DaveGamble/cJSON|
