#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include "process_ancestors.h"


asmlinkage long sys_process_ancestors(struct process_info *info_array, long size, long *num_filled);
SYSCALL_DEFINE3(process_ancestors,struct process_info*,info_array, long,size, long*,num_filled){
    long temp_numFilled = 0;
	int children_count = 0;
	int sibling_count = 0;
	struct process_info temp_process;
	struct task_struct* curr_task = current;
	struct list_head* list_head = NULL; 
	
	//check parameters
	if (size <= 0){
		return -EINVAL;
	}
	if ((info_array == NULL) || (num_filled == NULL)){
		return -EFAULT;
	}
	
	while ((temp_numFilled < size) && ((curr_task->parent) != curr_task)){
		temp_process.pid = curr_task->pid;
		strncpy(temp_process.name, curr_task->comm, ANCESTOR_NAME_LEN);
		temp_process.uid = (curr_task->cred)->uid.val;
		temp_process.nvcsw = curr_task-> nvcsw;
		temp_process.nivcsw = curr_task->nivcsw;
		temp_process.state = curr_task->state;

		list_for_each(list_head, &(curr_task->children)){//count children
			children_count++;
		}
		temp_process.num_children = children_count;
        
		list_for_each(list_head, &(curr_task->sibling)){//count silbling
			sibling_count++;
		}
		temp_process.num_siblings = sibling_count;
        
		if((copy_to_user(&(info_array[temp_numFilled]), &temp_process, sizeof(struct process_info)))){
			return -EFAULT;
		}

		temp_numFilled++;
		curr_task = curr_task->parent;
	}
    
	if (copy_to_user(num_filled, &temp_numFilled, sizeof(long))){
		return -EFAULT;
	}

	return 0;
}