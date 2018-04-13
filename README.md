SYSCALL_DEFINE5(mysetnice,pid_t,pid,int,flag ,int,nicevalue,void __user *,prio,void __user *,nice)
{
	if(pid < 0 || (flag != 1 && flag != 0)){
		return EFAULT;
	    }
	    struct task_struct *p;
	    int my_nice;
	    int my_prio;
	    for_each_process(p) {
		if(p->pid == pid){
		    if(flag == 1){
		        set_user_nice(p, nicevalue);
		    }
		    //内核空间的nice和prio
		    my_nice = task_nice(p);
		    my_prio = task_prio(p);
		    // 用户空间与内核空间数据拷贝
		    copy_to_user(prio, &my_prio, sizeof(my_prio));
		    copy_to_user(nice, &my_nice, sizeof(my_nice));
		    return 0;
		}
	    }
	    return EFAULT;
}
