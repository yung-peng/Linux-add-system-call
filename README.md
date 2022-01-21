# Linux-add a new system call

## Problem
1. [寫一個 system call](https://blog.kaibro.tw/2016/11/07/Linux-Kernel%E7%B7%A8%E8%AD%AF-Ubuntu/) 從 kernel space 找出指定 process(task) 的 code(text) 段的位址

```warning
每個版本編譯 kernel 方法不盡相同，請找適合自己版本的方法
```

2. user 段程式利用 system call 傳入 pid 並取得結果放至 user space 的 buffer 上，再輸出至 terminal 上

## 編譯Kernel過程
* Linux 版本：ubuntu 18.04
* Kernel 版本：Kernel 4.15
```
$ cd linux-4.15/
```
1. 我們要編寫的system call放在這裡
```
$ mkdir mycall
$ cd mycall
```
2. 將system call的程式編寫進去
```
$ vim helloworld.c
$ vim Makefile
obj-y := helloworld.o
```
3. 回到上層目錄，修改Makefile
```
$ cd ..
$ vim Makefile
core-y += kernel/ mm/ fs/ ipc/ security/ crypto/ block/
找到這行，在最後面新增 mycall/
core-y += kernel/ mm/ fs/ ipc/ security/ crypto/ block/ mycall/
```
這是為了告訴它，我們新的system call的source files在mycall資料夾裡

4. 再來我們要新增我們的system call到system call table裡
```
$ vim arch/x86/entry/syscalls/syscall_64.tbl
在64位元最後新增一行，原本system call只到332號，所以這邊就填333號
333   common  helloworld  sys_helloworld
```
5. 再來修改system call header
```
$ vim include/linux/syscalls.h
在最下面(#endif前)新增一行
asmlinkage int helloworld(void);
這定義了我們system call的prototype
asmlinkage代表我們的參數都可以在stack裡取用
```
(以上改前綴的方式其實算是"偷吃步"，詳情請見 macro [SYSCALL_DEFINEn](https://elixir.bootlin.com/linux/v4.15/source/include/linux/syscalls.h#L192))

6. 接著編譯kernel前要裝一些套件

```
$ sudo apt install build-essential libncurses-dev libssl-dev libelf-dev bison flex -y
$ sudo apt-get install libncurses5-dev
$ sudo apt install gcc-multilib
```
7. 還有設定檔 (先把terminal文字縮小再執行這行）
```
$ sudo make menuconfig
// 這邊我都用預設的
$ sudo make oldconfig
```

8. 開始Compile
```
$ sudo make -j8
```

9. 編完之後，安裝到作業系統上
```
$ sudo make modules_install install
```

10. 更改grub設定重開時可以選擇我們的kernel
```
$ sudo vim /etc/default/grub
像我一樣找到下面這兩行，前面加#，把他們註解掉
#GRUB_HIDDEN_TIMEOUT=0
#GRUB_HIDDEN_TIMEOUT_QUIET=true
然後更新
$ sudo update-grub
之後重開機就能看到選單了
```
**若沒有修改到tbl file（沒有新增新的system call）直接執行8和9即可，會加速compile的時間**

## 程式執行畫面
### User Space 端
![](https://i.imgur.com/zThOCYf.jpg)
### Kernel Message
![](https://i.imgur.com/Acr324c.png)
![](https://i.imgur.com/Whply37.jpg)

## Q&A
1. asmlinkage的功用是什麼？ 為何system call傳遞參數要用stack？
    * [答] asmlinkage的功用是告訴compiler只能從stack上來取值。 因為32-bit跟64-bit的calling convention不一樣，所以在進入到syscall前，避免掉取值的問題，在**entry_64.S**裡面將參數一律先丟到stack上。進入syscall後，統一從stack拿值。
2. 在新增一個system call時，asmlinkage long sys_XXX(**args)算是“偷吃步”，應該定義為SYSCALL DEFINEn(XXX, **args)。為什麼？
    * [答] 使用"sys_"來定義可能隨著kernel版本而有不同的prefix，舉例：kernel 5.x版可能變成__x64_sys_. 而syscall.h下有定義SYSCALL DEFINEn的macro，呼叫相應的asmlinkage long sys_XXX(**args)，我們寫新的syscall只要定義為SYSCALL DEFINEn(XXX, **args)，好處是通用且安全(CVE漏洞)
3. 為什麼使用copy_to_user？ 不能直接使用memcpy嗎？ 為什麼？
    * [答] Kernel可以寫進所有記憶體位置。用copy_to_user會檢查destination是否是目前process可讀且可寫的，所以不能只使用Memcpy
4. 簡述vm_area_struct與mm_struct的結構差異與區別。
    * [答] mm_struct會包含一個linked list的vm_area_struct指標，一個process有多個vm_area_struct負責去管理記憶體的分頁(page)

## Reference
1. [Stack Overflow](https://stackoverflow.com/questions/25440319/system-call-uses-registers-or-stack-to-pass-the-parameters-to-kernel)
2. [entry_64.S](https://elixir.bootlin.com/linux/latest/source/arch/x86/entry/entry_64.S#L87)
3. [lwn.net](https://lwn.net/Articles/604287/)
4. [Write a Syscall Stack Overflow](https://stackoverflow.com/questions/17751216/writing-a-new-system-call)
5. [Syscall Define csdn](https://blog.csdn.net/hxmhyp/article/details/22699669)
6. [System call factorial](https://hackmd.io/@combo-tw/Linux-讀書會/%2F%40a29654068%2FHyD4Lu_Dr)
7. 第四組員：黃建鴻

## Contributor
Yung-Peng Hsu、Ruei-Yuan Wang、Cheng-Kai Wang
