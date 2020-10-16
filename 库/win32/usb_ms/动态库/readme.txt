1. skf主要分为*_with_log.*，*_fix_log.*，*_logcat.*和*_no_log.*版本，其中：
    （1）*_with_log.*：是带日志的版本，产生日志的方法可参考2；
    （2）*_fix_log.*：也是带日志版本。该版本在固定位置生成skflog.txt日志，不依赖skf.txt文件；
        （2.1）windows平台：生成的日志路径为c:\log\skflog.txt；
        （2.2）linux平台：生成的日志路径为/tmp/skflog.txt；
        （2.3）android平台：生成的日志路径为/data/local/tmp/skflog.txt；
    （3）*_logcat.*：该版本只在Android平台生效，过滤关键字为CLOG_SO；
    （4）*_no_log.*：该版本是无日志版；

2. *_with_log.*版本库日志产生的方法如下：
    windows平台：
        （1）在c:\log目录下创建skf.txt文件；
        （2）如果skf.txt文件中指定了日志文件（绝对路径，如c:\log\skflog_test.txt），则日志输出到该路径；
        （3）如果skf.txt为空文件，则日志默认输出到c:\log\skflog.txt文件；
    linux平台：
        （1）在/tmp目录下创建skf.txt文件；
        （2）如果skf.txt文件中指定了日志文件（绝对路径，如/tmp/skflog_test.txt），则日志输出到该路径；
        （3）如果skf.txt为空文件，则日志默认输出到/tmp/skflog.txt文件；
    android平台：
        （1）在可执行程序运行目录下创建skf.txt文件；
        （2）如果skf.txt文件中指定了日志文件（绝对路径，如/tmp/skflog_test.txt），则日志输出到该路径；
        （3）如果skf.txt为空文件，则日志默认输出到可执行程序运行目录下skflog.txt文件；
    
    如无对应skf.txt文件，则不会产生日志；

3. 如果使用静态库版本，则静态库的链接顺序为skf core cdrv clog cutils pthread dl；
    
