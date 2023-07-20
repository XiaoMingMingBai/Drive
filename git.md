# 拉取
1. 安装git
2. 建立文件夹
3. 初始化仓库, 在文件夹下执行
    ```bash
    git init
    ```
4. 添加个人信息
    ```bash
    git config --global user.name "Your Name"
    git config --global user.email "Your Email"
    ```
5. 生成ssh key
    ```bash
    ssh-keygen -t rsa -C " Your Email"
    ```
6. 联系我，把 id_rsa.pub 文件内容发给我添加到仓库
7. 复制 ssh 地址, 在仓库添加
    ```bash 
    git remote add origin "ssh地址"
    ```
8. 拉取代码
    ```bash
    git pull origin master
    ```
9. 提交代码
    ```bash
    git add .
    git commit -m "提交信息"
    git push origin master
    ```
## 其他操作
1. 查看状态
    ```bash
    git status
    ```
2. 查看提交记录
    ```bash
    git log
    ```
3. 查看分支
    ```bash
    git branch
    ```
4. 创建分支
    ```bash
    git branch 分支名
    ```
5. 切换分支
    ```bash
    git checkout 分支名
    ```
