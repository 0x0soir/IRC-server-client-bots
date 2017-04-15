#include "../includes/G-2313-06-P1_thread_pool.h"

thread_pool_t *thread_pool_create(int size)
{
    thread_pool_t *p;
    thread_t *master_thread;

    p = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if(p == NULL)
    {
        return NULL;
    }

    master_thread = (thread_t *)malloc(sizeof(thread_t));
    if(master_thread == NULL)
    {
        free(p);
        return NULL;
    }

    p->master_thread = master_thread;
    p->size = size;
    pthread_mutex_init(&p->global, NULL);
    return p;
}

void thread_pool_delete(thread_pool_t *p){
  thread_t *t = NULL;
  pthread_mutex_lock(&p->global);
  list_for_each_entry(t, (&(p->worker_queue)), worker_entry){
    if(p->size>0){
      syslog(LOG_ERR, "Eliminando hilo");
      pthread_kill(t->tid, SIGINT);
      pthread_detach(t->tid);
      thread_del_internal(p, t, 0);
      syslog(LOG_ERR, "Fuera de lista");
    }
  }
  syslog(LOG_ERR, "Termina hijos");
  /*list_for_each_entry(t, (&(p->idle_queue)), idle_entry){
    syslog(LOG_ERR, "Eliminando hilo");
    pthread_exit(&t->tid);
    syslog(LOG_ERR, "Eliminado 1");
    pthread_detach(t->tid);
    syslog(LOG_ERR, "Eliminado 2");
    thread_del_internal(p, t, 0);
    syslog(LOG_ERR, "Fuera de lista");
  }*/
  pthread_mutex_unlock(&p->global);
  if(p->master_thread){
    free(p->master_thread);
    p->master_thread = NULL;
  }
  syslog(LOG_INFO, "POOL: Termina de liberar");
  return;
}

int thread_pool_init(thread_pool_t *p)
{
    return thread_pool_init_internal(p, THREAD_IDLE_LEVEL,
                                     THREAD_BUSY_LEVEL, MASTER_INTERVAL);
}

int thread_pool_init_internal(thread_pool_t *p, double low_level,
                              double high_level, int master_interval)
{
    int i;
    int size;
    int ret;
    thread_t *t = NULL;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(ret != 0)
    {
        return -1;
    }

    pthread_mutex_init(&p->global, NULL);
    p->max_size = THREAD_POOL_MAX_THREADS;
    p->min_size = THREAD_POOL_MIN_THREADS;
    p->low_level = low_level;
    p->high_level = high_level;
    p->master_interval = master_interval;
    INIT_LIST_HEAD(&p->worker_queue);
    INIT_LIST_HEAD(&p->idle_queue);

    size = p->size;
    p->size = 0;
    for(i = 0; i < size; i++)
    {

        t = thread_create();
        if(t == NULL)
        {
            return -1;
        }
        thread_add(p, t);

    }
    /*pthread_create(&p->master_thread->tid, &attr,
                   master_callback, (void *)p);*/
    pthread_attr_destroy(&attr);
    p->task_next = list_first_entry((&p->worker_queue), thread_t, worker_entry);
    return 0;
}

thread_t *thread_create(void)
{
    thread_t *t = NULL;

    t = (thread_t *)calloc(1, sizeof(thread_t));
    if(t == NULL)
    {
      return NULL;
    }
    return t;
}

int thread_add(thread_pool_t *p, thread_t *t)
{
  int ret;
  pthread_attr_t attr;
  struct thread_args *args;

  pthread_attr_init(&attr);
  ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if(ret != 0){
    pthread_attr_destroy(&attr);
    return -1;
  }
  INIT_LIST_HEAD(&t->task_queue);
  list_add_tail(&(t->worker_entry), &(p->worker_queue));
  list_add_tail(&(t->idle_entry), &(p->idle_queue));
  p->size++;
  t->state = THREAD_STATE_IDLE;
  t->state = THREAD_RUNNING;
  t->queue_size = 0;
  pthread_mutex_init(&t->mutex, NULL);
  pthread_cond_init(&t->cond, NULL);

  args = malloc(sizeof *args);
  args->pool = p;
  args->current = t;
  pthread_create(&t->tid, &attr, worker_callback, (void *)args);
  pthread_attr_destroy(&attr);

  return 0;
}

void thread_del(thread_t *t){
    pthread_mutex_lock(&t->mutex);
    t->stop = THREAD_STOPPING;
    pthread_cond_signal(&t->cond);
    pthread_mutex_unlock(&t->mutex);
}

void thread_del_internal(thread_pool_t *p, thread_t *t, int force_lock){
  if (force_lock){
    pthread_mutex_lock(&p->global);
  }
  list_del(&t->worker_entry);
  list_del(&t->idle_entry);
  p->size--;
  if (force_lock){
    pthread_mutex_unlock(&p->global);
  }
}

void *worker_callback(void *arg)
{
    struct thread_args *args = arg;
    thread_t *curr = (thread_t *) args->current;
    thread_pool_t *tp = (thread_pool_t *) args->pool;
    task_t *t;

    while(1)
    {
        pthread_mutex_lock(&curr->mutex);
        while(curr->queue_size == 0 && curr->stop != THREAD_STOPPING)
        {
            pthread_cond_wait(&curr->cond, &curr->mutex);
        }
        if(curr->stop == THREAD_STOPPING)
        {
            if(curr->queue_size == 0)
            {
                pthread_mutex_unlock(&curr->mutex);
                thread_del_internal(tp, curr, 1);
                break;
            }
            else
            {
                pthread_mutex_unlock(&curr->mutex);
                continue;
            }
        }
        else
        {
            t = list_entry(curr->task_queue.next, task_t, entry);
            if(t)
            {
                t->task_callback(t->arg);
            }
            list_del(&t->entry);
            free(t);
            curr->queue_size--;
        }
        if(curr->queue_size == 0)
        {
            pthread_mutex_lock(&tp->global);
            list_add_tail(&(curr->idle_entry), &(tp->idle_queue));
            pthread_mutex_unlock(&tp->global);
            curr->state = THREAD_STATE_IDLE;
        }
        pthread_mutex_unlock(&curr->mutex);
    }
    return NULL;
}

void *master_callback(void *arg)
{
    thread_pool_t *p = (thread_pool_t *)arg;
    thread_t *t;
    int busy = 0;
    int idle = 0;
    int add_num = 0;
    list_for_each_entry(t, (&(p->worker_queue)), worker_entry)
    {
      if(t->state == THREAD_STATE_IDLE)
      {
        idle++;
      }
      else if(t->state == THREAD_STATE_BUSY)
      {
        busy++;
      }
    }
    if(idle<5){
      add_num = 5;
      syslog(LOG_ERR, "POOL: Falta de espacio, incrementa pool en %d hilos", add_num);
      while(add_num--){
        t = thread_create();
        syslog(LOG_ERR, "POOL: Crea nuevo hilo");
        if(t == NULL){
          break;
        }
        else{
          thread_add(p, t);
        }
        syslog(LOG_ERR, "POOL: Add hilo al pool");
      }
    }
    syslog(LOG_ERR, "POOL: Busy: %d | Idle: %d", busy, idle);
    return NULL;
}

task_t *task_create(void)
{
    task_t *t;

    t = (task_t *)malloc(sizeof(task_t));
    if(t == NULL)
    {
        return NULL;
    }
    return t;
}

void task_init(task_t *t, void* (*task_callback)(int), int arg)
{
    t->task_callback = task_callback;
    t->arg = arg;
}

void task_add(thread_pool_t *p, task_t *t)
{
    thread_t *th = NULL;
    thread_t *last = NULL;

    pthread_mutex_lock(&p->global);
    master_callback((void *)p);
    th = p->task_next;
    last = list_entry(p->worker_queue.prev, thread_t, worker_entry);
    if(th == last)
    {
      p->task_next = list_first_entry((&p->worker_queue), thread_t, worker_entry);
    } else {
      p->task_next = list_next_entry(th, worker_entry);
    }
    pthread_mutex_unlock(&p->global);
    pthread_mutex_trylock(&th->mutex);
    list_add_tail(&(t->entry), &(th->task_queue));
    th->queue_size++;
    if(th->queue_size == 1)
    {
        th->state = THREAD_STATE_BUSY;
        pthread_mutex_lock(&p->global);
        list_del(&(th->idle_entry));
        pthread_mutex_unlock(&p->global);
    }

    pthread_cond_signal(&th->cond);
    pthread_mutex_unlock(&th->mutex);
}
