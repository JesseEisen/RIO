RIO
====

这里面一个有五个函数，两个是直接读写的，没有缓冲区的。`rio_readn`保证所有的数都能读到，返回值只有EOF的时候返回一个不足值，`rio_writen`不会有不足值返回。

其他的三个，`rio_readinitb`用来初始化的。`rio_readlineb`提供一个有缓冲区的版本。`rio_readnb`这是对上面那个版本的提供提供的一个读版本的代缓冲区的。


