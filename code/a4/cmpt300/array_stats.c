#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include "array_stats.h"

asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size);
SYSCALL_DEFINE3(array_stats,struct array_stats*,stats, long* ,data, long, size){
	struct array_stats result;
	int i = 0;

	printk("size = %ld\n", size);

	if(size <= 0){
		return -EINVAL;
	}
	printk("passed size<=0 test\n");

	if(copy_from_user(&result.min, &data[0], sizeof(result.min))){
		return -EFAULT;
	}

	printk("passed result.min\n");

	if(copy_from_user(&result.max, &data[0], sizeof(result.max))){
		return -EFAULT;
	}
	printk("passed result.max\n");

	result.sum = 0;
	for(;i<size;i++){
		long temp;
		if(copy_from_user(&temp, &data[i], sizeof(temp))){
			return -EFAULT;
		}
		if(temp < result.min){
			result.min = temp;
		}
		if(temp > result.max){
			result.max = temp;
		}
		result.sum += temp;
	}

	printk("make min, max, total\n");

	if(copy_to_user(stats, &result, sizeof(result))){
		return -EFAULT;
	}

	printk("copy done---\n");

	return 0;
}