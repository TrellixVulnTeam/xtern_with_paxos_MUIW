/* Author: Junfeng Yang (junfeng@cs.columbia.edu) -*- Mode: C++ -*- */

/* This is the central table that lists the synchronization functions and
 * blocking system calls that tern hooks, as well as tern builtin synch
 * functions. */

/* intentionally no include guard */

/* format:
 *   DEF(func, kind, ret_type, arg0, arg1, ...)
 *   DEFTERN(func)  for tern builtin sync events
 */

/* pthread synchronization operations */
DEF(pthread_create,         Synchronization, int, pthread_t *thread, pthread_attr_t *attr, void* (*start_routine)(void *), void *arg)
DEF(pthread_join,           Synchronization, int, pthread_t th, void **thread_return)
DEF(pthread_exit,           Synchronization, void, void *retval)
// DEF(pthread_mutex_init,     Synchronization, int, pthread_mutex_t *mutex, const  pthread_mutexattr_t *mutexattr)
DEF(pthread_mutex_lock,     Synchronization, int, pthread_mutex_t *mutex)
DEF(pthread_mutex_unlock,   Synchronization, int, pthread_mutex_t *mutex)
DEF(pthread_mutex_trylock,  Synchronization, int, pthread_mutex_t *mutex)
DEF(pthread_mutex_timedlock,Synchronization, int, pthread_mutex_t *mutex, const struct timespec *abstime)
// DEF(pthread_mutex_destroy,  Synchronization, int, pthread_mutex_t *mutex)
// DEF(pthread_cond_init,      Synchronization, int, pthread_cond_t *cond, pthread_condattr_t*attr)
DEF(pthread_cond_wait,      Synchronization, int, pthread_cond_t *cond, pthread_mutex_t *mutex)
DEF(pthread_cond_timedwait, Synchronization, int, pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
DEF(pthread_cond_broadcast, Synchronization, int, pthread_cond_t *cond)
DEF(pthread_cond_signal,    Synchronization, int, pthread_cond_t *cond)
// DEF(pthread_cond_destroy,   Synchronization, int, pthread_cond_t *cond)
DEF(pthread_barrier_init,   Synchronization, int, pthread_barrier_t *barrier, const pthread_barrierattr_t* attr, unsigned count)
DEF(pthread_barrier_wait,   Synchronization, int, pthread_barrier_t *barrier)
DEF(pthread_barrier_destroy,Synchronization, int, pthread_barrier_t *barrier)
// DEF(sem_init,               Synchronization, int, sem_t *sem, int pshared, unsigned int value)
DEF(sem_post,               Synchronization, int, sem_t *sem)
DEF(sem_wait,               Synchronization, int, sem_t *sem)
DEF(sem_trywait,            Synchronization, int, sem_t *sem)
DEF(sem_timedwait,          Synchronization, int, sem_t *sem, const struct timespec *abs_timeout)
// DEF(sem_destroy,            Synchronization, int, sem_t *sem)

/* socket functions and file functions */
//	blockings: accept, connect, recv, recvfrom, recvmsg, read, select 
DEF(socket, Synchronization, int, int domain, int type, int protocol)
DEF(listen, Synchronization, int, int sockfd, int backlog)
DEF(accept, Synchronization, int, int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen)
DEF(connect, Synchronization, int, int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen)
//DEF(gethostbyname, Synchronization, struct hostent*, const char *name)
//DEF(gethostbyaddr, Synchronization, struct hostent*, const void *addr, int len, int type)
DEF(send, Synchronization, ssize_t, int sockfd, const void *buf, size_t len, int flags)
DEF(sendto, Synchronization, ssize_t, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
DEF(sendmsg, Synchronization, ssize_t, int sockfd, const struct msghdr *msg, int flags)
DEF(recv, Synchronization, ssize_t, int sockfd, void *buf, size_t len, int flags)
DEF(recvfrom, Synchronization, ssize_t, int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
DEF(recvmsg, Synchronization, ssize_t, int sockfd, struct msghdr *msg, int flags)
DEF(shutdown, Synchronization, int, int sockfd, int how)
DEF(getpeername, Synchronization, int, int sockfd, struct sockaddr *addr, socklen_t *addrlen)  
DEF(getsockopt, Synchronization, int, int sockfd, int level, int optname, void *optval, socklen_t *optlen)
DEF(setsockopt, Synchronization, int, int sockfd, int level, int optname, const void *optval, socklen_t optlen)

DEF(close, Synchronization, int, int fd)
DEF(read, Synchronization, ssize_t, int fd, void *buf, size_t count)
DEF(write, Synchronization, ssize_t, int fd, const void *buf, size_t count)
DEF(select, Synchronization, int, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
//DEF(poll, Synchronization, int, struct pollfd *fds, nfds_t nfds, int timeout)

// file operations not handled not. 
//DEF(open, Synchronization, int, const char *pathname, int flags)
//DEF(open, Synchronization, int, const char *pathname, int flags, mode_t mode)
//DEF(creat, Synchronization, int, const char *pathname, mode_t mode)
//DEF(ppoll, Synchronization, int, struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask)
//DEF(pselect, Synchronization, int, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask)
//DEF(FD_CLR, Synchronization, void, int fd, fd_set *set)
//DEF(FD_ISSET, Synchronization, int, int fd, fd_set *set)
//DEF(FD_SET, Synchronization, void, int fd, fd_set *set)
//DEF(FD_ZERO, Synchronization, void, fd_set *set)

// seems like apis in different arch.
//DEF(sockatmark, Synchronization, int, int fd)
//DEF(isfdtype, Synchronization, int, int fd, int fdtype)

/* blocking system calls */
DEF(sleep,                  BlockingSyscall, unsigned int, unsigned int seconds)
DEF(usleep,                 BlockingSyscall, int, useconds_t usec)
DEF(nanosleep,              BlockingSyscall, int, const struct timespec *req, struct timespec *rem)
//socket DEF(accept,                 BlockingSyscall, int, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
//socket DEF(select,                 BlockingSyscall, int, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
DEF(epoll_wait,             BlockingSyscall, int, int epfd, struct epoll_event *events, int maxevents, int timeout)
DEF(sigwait,                BlockingSyscall, int, const sigset_t *set, int *sig)
/* should include sched_yield */

/* DEF(exit,                   BlockingSyscall, void, int status) */
/* DEF(syscall,                BlockingSyscall, tern_, int, int) */ /* FIXME: why include generic syscall entry point? */
/* DEF(ap_mpm_pod_check,       BlockingSyscall, tern_) */ /* FIXME: ap_mpm_pod_check is not a real lib call; needed for apache */

DEFTERNUSER(tern_symbolic)
DEFTERNAUTO(tern_thread_begin)
DEFTERNAUTO(tern_thread_end)
DEFTERNUSER(tern_task_begin)
DEFTERNUSER(tern_task_end)
DEFTERNAUTO(tern_fix_up)
DEFTERNAUTO(tern_fix_down)
