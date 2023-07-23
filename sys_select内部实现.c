select(fd2 + 1, &rfds, NULL, NULL, NULL);
--------------------------------------------------------------------
vi -t sys_select

SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
        fd_set __user *, exp, struct __kernel_old_timeval __user *, tvp) 
{
    return kern_select(n, inp, outp, exp, tvp);
} 
--------------------------------------------------------------------

int sys_select(int n,fd_set __user * inp, fd_set __user * outp,
        fd_set __user * exp, struct __kernel_old_timeval __user * tvp)
{
	return kern_select(n, inp, outp, exp, tvp);
} 


static int kern_select(int n, fd_set __user *inp, fd_set __user *outp,
		       fd_set __user *exp, struct __kernel_old_timeval __user *tvp)
{
	int ret;
	
	ret = core_sys_select(n, inp, outp, exp, to);
	
	return poll_select_finish(&end_time, tvp, PT_TIMEVAL, ret);
}

int core_sys_select(int n, fd_set __user *inp, fd_set __user *outp,
			   fd_set __user *exp, struct timespec64 *end_time)
{
	fd_set_bits fds;
	void *bits;
	int ret, max_fds;
	size_t size, alloc_size;
	struct fdtable *fdt;
	/* Allocate small arguments on the stack to save memory and be faster */
	long stack_fds[SELECT_STACK_ALLOC/sizeof(long)];

	ret = -EINVAL;

	//1.如果用户写的最大文件描述符的值小于0，直接报错返回
	if (n < 0)
		goto out_nofds;
	

	//2.从当前进程中获取最大文件描述符个数和用户传递的n做比较
	//如果用户传递的文件描述符的值不合法，强制更正。
	//current--->task_struct功能?
	//#define current get_current()
	//#define get_current() (current_thread_info()->task)  获取了当前进程的结构体
	fdt = files_fdtable(current->files);
	max_fds = fdt->max_fds;
	if (n > max_fds)
		n = max_fds;

	
	//3.分配6张表的内存，前3张表存用户的文件描述符集合
	//后3张表存放就绪的文件描述符
	alloc_size = 6 * size;
	bits = kvmalloc(alloc_size, GFP_KERNEL);

	fds.in      = bits;
	fds.out     = bits +   size;
	fds.ex      = bits + 2*size;
	fds.res_in  = bits + 3*size;
	fds.res_out = bits + 4*size;
	fds.res_ex  = bits + 5*size;

	//将用户空间表中的数据拷贝到内核空间中
	if ((ret = get_fd_set(n, inp, fds.in)) ||
	    (ret = get_fd_set(n, outp, fds.out)) ||
	    (ret = get_fd_set(n, exp, fds.ex)))
		goto out;
	//将就绪的三张表清零
	zero_fd_set(n, fds.res_in);
	zero_fd_set(n, fds.res_out);
	zero_fd_set(n, fds.res_ex);


	//4.文件描述符遍历工作
	ret = do_select(n, &fds, end_time);

	if (ret < 0)
		goto out;
	if (!ret) {
		ret = -ERESTARTNOHAND;
		if (signal_pending(current))
			goto out;
		ret = 0;
	}

	//5.将就绪的文件描述符拷贝到用户空间
	if (set_fd_set(n, inp, fds.res_in) ||
	    set_fd_set(n, outp, fds.res_out) ||
	    set_fd_set(n, exp, fds.res_ex))
		ret = -EFAULT;

out:
	if (bits != stack_fds)
		kvfree(bits);
out_nofds:
	return ret;
}


static int do_select(int n, fd_set_bits *fds, struct timespec64 *end_time)
{
	ktime_t expire, *to = NULL;
	struct poll_wqueues table;
	poll_table *wait;
	int retval, i, timed_out = 0;
	u64 slack = 0;
	__poll_t busy_flag = net_busy_loop_on() ? POLL_BUSY_LOOP : 0;
	unsigned long busy_start = 0;


	//1.对最大的文件描述符的值做校验工作
	n = retval;

	//2.初始化了一个构造等待队列的函数(这个函数没有被调用)
	poll_initwait(&table);
	wait = &table.pt;

	retval = 0;
	for (;;) {
		unsigned long *rinp, *routp, *rexp, *inp, *outp, *exp;
		bool can_busy_loop = false;

		//分配6个unsigned long的指针用于从6张表中获取第一个unsigned long的整数
		inp = fds->in; outp = fds->out; exp = fds->ex;
		rinp = fds->res_in; routp = fds->res_out; rexp = fds->res_ex;

		for (i = 0; i < n; ++rinp, ++routp, ++rexp) {
			unsigned long in, out, ex, all_bits, bit = 1, j;
			unsigned long res_in = 0, res_out = 0, res_ex = 0;
			__poll_t mask;

			//从表中取出第一个unsigned long的整数，如果这个值不为0，说明
			//里面有要监听的文件描述符，如果为0说明没有监听的文件描述符，
			//执行continue，判断下一个unsigned long是否有数据
			in = *inp++; out = *outp++; ex = *exp++;
			all_bits = in | out | ex;
			if (all_bits == 0) {
				i += BITS_PER_LONG;
				continue;
			}

			//如果all_bits不为0 ，说明里面有bit为是1，找出bit位为1
			//的文件描述符
			for (j = 0; j < BITS_PER_LONG; ++j, ++i, bit <<= 1) {
				struct fd f;
				if (i >= n)
					break;
				if (!(bit & all_bits))
					continue;

				//这里的i就是找到的文件描述，比如i=3或者i=4
				//i==fd-->fd_array[fd]--->file-->f_op-->poll
				f = fdget(i);
				if (f.file) {
					//调用驱动的poll函数
					mask = file->f_op->poll(f.file, wait);

					if ((mask & POLLIN_SET) && (in & bit)) {
						res_in |= bit;
						retval++;
						wait->_qproc = NULL;
					}
					if ((mask & POLLOUT_SET) && (out & bit)) {
						res_out |= bit;
						retval++;
						wait->_qproc = NULL;
					}
					if ((mask & POLLEX_SET) && (ex & bit)) {
						res_ex |= bit;
						retval++;
						wait->_qproc = NULL;
					}
				

				}
			}
			
		}
		
		//select退出的三个条件:1.数据准备好了，2.超时时间到了，3.收到了信号
		if (retval || timed_out || signal_pending(current))
			break;
		
		//进程休眠
		if (!poll_schedule_timeout(&table, TASK_INTERRUPTIBLE,
					   to, slack))
			timed_out = 1;
	}

	poll_freewait(&table);

	return retval;
}