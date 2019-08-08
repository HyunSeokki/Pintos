#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/malloc.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
/* prj1 Add */
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "filesys/file.h"
/* Add end */


static void syscall_handler (struct intr_frame *);

/****************************************************************/
/* @name: int where_vaddr													 */
/*	@param: void *vaddr - 메모리 주소 값								 */
/* @return: 1 or 0															 */
/*																					 */
/* 현재 참조하는 주소가 유저영역인지 커널영역인지 확인함			 */
/* 1: 유저 0: 커널				 											 */
/*	is_user_vaddr(), is_kernel_vaddr()이라는 함수가 이미 있음 킥 */
/****************************************************************/
int where_vaddr( void* vaddr ){
	return vaddr < PHYS_BASE;
}

/****************************************************************/
/* @name: void syscall_halt												 */
/* @param: none																 */
/*	@return: none 																 */
/*																					 */
/*	빡종																			 */
/****************************************************************/
void syscall_halt(void){
	shutdown_power_off();
}

/****************************************************************/
/* @name: void syscall_exit												 */
/* @param: int exitstatus - 쓰레드 종료 상태 값						 */
/*	@return: none 																 */
/*																					 */
/* 어떤 쓰레드가 어떤 상태값으로 종료하는지 출력하고 쓰레드를	 */
/* 종료함																		 */
/****************************************************************/
void syscall_exit(int exitstatus){
	struct thread *cur = thread_current();
	cur->exit_status = exitstatus;
//	printf(" thr tid: %d\n", cur->tid);
	printf("%s: exit(%d)\n", cur->name, exitstatus);
	
	struct list_elem* begin = list_begin( &cur->file_list );	
	struct list_elem* end = list_end( &cur->file_list );

	if( !list_empty( &cur->file_list ) ){
		while( begin != end ){
			struct file_desc* fd = list_entry( begin, struct file_desc, elem );
			file_close( fd->file );
			begin = list_next( begin );
			free( fd );
		}
	}
	thread_exit();
}

/****************************************************************/
/* @name: int syscall_exec													 */
/* @param: const char *cmd_line - 실행시킬 명령어					 */
/*	@return: int pid - process_execute함수에서 리턴된 쓰레드 id	 */
/*																					 */
/* 명령어를 실행시킬 쓰레드를 만들고 그 id값을 리턴함				 */
/****************************************************************/
int syscall_exec(const char *cmd_line){
	int pid = process_execute(cmd_line);

	return pid;
}

/****************************************************************/
/* @name: int syscall_wait													 */
/* @param: int pid - 쓰레드 id											 */
/*	@return: int - process_wait함수 실행 결과 값						 */
/****************************************************************/
int syscall_wait(int pid){
	return process_wait(pid);
}

/****************************************************************/
/* @name: int syscall_read													 */
/* @param: int fd - file descriptor										 */
/* @param: void *buffer - buffer										 	 */
/* @param: unsigned size - buffer size									 */
/*	@return: unsigned size													 */
/****************************************************************/
int syscall_read(int fd, void *buffer, unsigned size){
	unsigned i = 0;
	struct thread *t_cur = thread_current();
	struct file_desc *fdesc;
	struct list_elem *e;
	int cnt;

	if(t_cur == NULL || fd == 1)
		return -1;
	if(buffer == NULL || !is_user_vaddr(buffer))
		syscall_exit(-1);
	if(size == 0)
		return 0;
	//fd = 0;
	if( fd == 0 ){
		for( i=0; i<size; i++)
			((char*)buffer)[i] = input_getc();
		return size;
	}
	else
	{
//		if(t_cur->fd == 2) return -1;
		if(list_empty(&t_cur->file_list))
			return -1;
//		printf("%d %d\n",fd, size);	
		//lock_acquire(&fs_lock);

		e = get_fd(&t_cur->file_list,fd);
		fdesc  = list_entry(e,struct file_desc, elem);
		if(fdesc == NULL)
			return -1;
		if(fdesc->fd == fd)
		{
//			if(fdesc == NULL)
//				return -1;
			cnt = (int)file_read(fdesc->file,buffer,(off_t)size);
//			printf("%d\n",cnt);
			return cnt;
		}
		//lock_release(&fs_lock);
	}
	return -1;
}

/****************************************************************/
/* @name: int syscall_wirte												 */
/* @param: int fd - file descriptor										 */
/* @param: void *buffer - buffer										 	 */
/* @param: unsigned size - buffer size									 */
/*	@return: unsigned size													 */
/****************************************************************/
int syscall_write(int fd, const void *buffer, unsigned size){
	struct thread *t_cur = thread_current();
	struct file_desc *fdesc;
	struct list_elem *e;
	int cnt=-1;

	if(!is_user_vaddr(buffer) || buffer == NULL)
		return -1;

	if( fd == 1 ){
		//fd = 1;
		putbuf((char*)buffer, size);
		return size;
	}
	else
	{
		//lock_acquire(&fs_lock);
		e = get_fd(&t_cur->file_list,fd);

		fdesc = list_entry(e,struct file_desc, elem);
		//iprintf("\n\n%d\n\n", fdesc->file->deny_write);
		//if( fdesc->file->deny_write == true )
			//file_deny_write(fdesc->file);
		
		//if(fdesc->fd == fd)
		if( fdesc != NULL )
		{
			//if( (unsigned)fdesc->file->pos < size ){ 
				//printf("\n\n%d %d\n", fdesc->file->pos, size);
				cnt = file_write(fdesc->file, buffer, size);
			//}
			//return cnt;
		}
		//lock_release(&fs_lock);
		return cnt;
	}
	return -1;
}

/****************************************************************/
/* @name: int syscall_fibo													 */
/* @param: int n - n번째 피보나치 수를 찾는데 사용					 */
/*	@return: int result - n번째 피보나치 수							 */
/****************************************************************/
int syscall_fibo(int n){
	int a = 1, b = 1, result = 0;
	
	if( n == 1 || n == 2 )
		return 1;
	else{
		int i = 0;
		for( i=3; i<=n; i++ ){
			result = a + b;
			a = b;
			b = result;
		}
	}

	return result;
}

/****************************************************************/
/* @name: int syscall_4sum													 */
/* @param: int a - 1번째 정수												 */
/* @param: int b - 2번째 정수												 */
/* @param: int c - 3번째 정수												 */
/* @param: int d - 4번째 정수												 */
/*	@return: int - a+b+c+d													 */
/****************************************************************/
int syscall_4sum(int a, int b, int c, int d){
	return a+b+c+d;
}

void
syscall_init (void) 
{
	lock_init(&fs_lock);
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int *esp = (int*)(f->esp);
	int syscall_enum = esp[0];
	void *vaddr = f->esp+4;
	struct thread *t_cur = thread_current();
	//if( !where_vaddr( vaddr ) )
		//syscall_exit(-1);

	if( syscall_enum == SYS_HALT ){
		syscall_halt();
	}
	else if( syscall_enum == SYS_EXIT ){
		if( where_vaddr( vaddr ) ){
			f->eax = esp[1];
			syscall_exit( esp[1] );
		}
		else
			syscall_exit(-1);
		
	}	
	else if( syscall_enum == SYS_EXEC ){
		//if( !where_vaddr( vaddr ) ){
		if( is_user_vaddr( vaddr ) ){
			f->eax = syscall_exec( (char*)esp[1]);
		}
		else{
			syscall_exit(-1);
		}

	}
	else if( syscall_enum == SYS_WAIT ){
		if( where_vaddr( vaddr ) ){
			f->eax = syscall_wait( esp[1] );
		}
		else
			syscall_exit(-1);
	}
	else if( syscall_enum == SYS_READ ){
		lock_acquire(&fs_lock);
		f->eax = syscall_read( esp[1], (void*)esp[2], (unsigned)esp[3]);
		lock_release(&fs_lock);
	}
	else if( syscall_enum == SYS_WRITE ){
		lock_acquire(&fs_lock);
		f->eax = syscall_write( esp[1], (void*)esp[2], (unsigned)esp[3] );
		lock_release(&fs_lock);
	}
	else if( syscall_enum == SYS_FIBO ){
		f->eax = syscall_fibo( esp[1] );
	}
	else if( syscall_enum == SYS_4SUM ){
		f->eax = syscall_4sum( esp[1], esp[2], esp[3], esp[4] );
	}
	else if( syscall_enum == SYS_CREATE ){
		if((const char*)esp[1] != NULL){
			
			//lock_acquire(&fs_lock);
			f->eax = filesys_create((const char*)esp[1], (unsigned)esp[2]);
			//lock_release(&fs_lock);
		}
		else
			syscall_exit(-1);
	}
	else if( syscall_enum == SYS_REMOVE ){
		if((const char*)esp[1] != NULL){
			//lock_acquire(&fs_lock);
			f->eax = filesys_remove((const char*) esp[1]);		
			//lock_release(&fs_lock);
		}
		else
			syscall_exit(-1);
	}
	else if( syscall_enum == SYS_OPEN ){

		if( where_vaddr( vaddr ))
			f->eax = syscall_open ((const char*) esp[1]);
		else
		{
			t_cur->parents->exit_status = -1;
			f->eax = -1;
			syscall_exit(-1);
		}
	}
	else if( syscall_enum == SYS_CLOSE )
	{
		if(esp[1] == 0 || esp[1] == 1)
		{
			f->eax = -1;
			t_cur->parents->exit_status = -1;			

			syscall_exit(-1);
		}

		if(!is_user_vaddr((const void*)esp[1]))
		{
			f->eax = -1;
			t_cur->parents->exit_status = -1;

			syscall_exit(-1);
		}

		syscall_close( esp[1] );
	}
	else if( syscall_enum == SYS_FILESIZE ){
		f->eax = syscall_filesize( esp[1] );
	}
	else if( syscall_enum == SYS_SEEK ){
		syscall_seek( esp[1], (unsigned)esp[2] );
	}
	else if( syscall_enum == SYS_TELL ){
		f->eax = syscall_tell( esp[1] );
	}
}

int syscall_open(const char *filename)
{
	struct file *fp;
	struct file_desc *fdesc;
	struct thread *t_cur = thread_current();
	
	if(filename == NULL) return -1;


	fp = filesys_open(filename);

	if( strcmp(t_cur->name, filename) == 0 ){
	//	printf("\n\nfuckfuck %d\n\n", t_cur->fd);
		file_deny_write(fp);
	//	printf("\n\n %d \n\n", fp->deny_write);
		//file_close(fp);
		//return -1;
	}


	if(fp != NULL)
	{
		fdesc =(struct file_desc*)malloc(sizeof(struct file_desc));

		fdesc->fd = t_cur->fd;
		t_cur->fd ++;
		fdesc->file = fp;
		list_push_front(&t_cur->file_list, &fdesc->elem);

	//	printf("fdfd::: %d\n\n\n", t_cur->fd);

		return t_cur->fd-1; //?
	} 
	else return -1;	
}

void syscall_close(int fd)
{
	struct file_desc *fdesc;
	struct list_elem *e;
	struct thread *t_cur = thread_current();

	//printf("\n\nfuckfuck\n\n");
	//lock_acquire(&fs_lock);
	e = get_fd(&t_cur->file_list,fd);
	fdesc = list_entry(e, struct file_desc, elem);

	if(fdesc->fd == fd)
	{
		file_close(fdesc->file);
		list_remove(e);
		t_cur->fd--;
	}
	//lock_release(&fs_lock);
	return ;
}

int syscall_filesize(int fd)
{
	struct file_desc *fdesc;
	struct list_elem *e;
	struct thread *t_cur = thread_current();

	e = get_fd(&t_cur->file_list,fd);
	fdesc = list_entry(e, struct file_desc, elem);
	
	if(fdesc->fd == fd)
	{
		return file_length(fdesc->file);
	}
	return 0;
}

void syscall_seek(int fd, unsigned new_pos)
{
	struct file_desc *fdesc;
	struct list_elem *e;
	struct thread *t_cur = thread_current();

	e = get_fd(&t_cur->file_list,fd);
	fdesc = list_entry(e, struct file_desc, elem);

	file_seek(fdesc->file, (off_t)new_pos);
}

unsigned syscall_tell(int fd)
{
	struct file_desc *fdesc;
	struct list_elem *e;
	struct thread *t_cur = thread_current();

	e = get_fd(&t_cur->file_list,fd);
	fdesc = list_entry(e, struct file_desc, elem);
	
	if(fdesc->fd == fd)
	{
		return 	file_tell(fdesc->file);
	}

	return 0;
}
