#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_LENGTH (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static const char* directory = "./watch";

static void DisplayInotifyEvent( struct inotify_event *event)
{
  printf("len = %u\n", event->len);

  if(event->len > 0)
  {
    printf("name = %s\n", event->name);

    char filename[100];

    snprintf(filename, 100, "%s/%s", directory, event->name);

    FILE *fp = fopen(filename, "r");

    char contents[100];
    fscanf(fp, "%s", contents);
    printf("%s\n", contents);
    
    fclose(fp);
  }
}

int main(void)
{
  int inotifyFileDescriptor = inotify_init();

  if(inotifyFileDescriptor == -1)
  {
    return -1;
  }

  // File must exist for this to work!
  int wd = inotify_add_watch(inotifyFileDescriptor, directory, IN_CLOSE_WRITE);

  if(wd == -1)
  {
    return -2;
  }

  for(;;)
  {
    char buf[BUFFER_LENGTH] __attribute__((aligned(8)));

    ssize_t numRead = read(inotifyFileDescriptor, buf, BUFFER_LENGTH); 

    if(numRead == 0)
    {
      return -3;
    }

    if(numRead == -1)
    {
      return -4;
    }

    for(char *p = buf; p < buf + numRead;)
    {
      struct inotify_event *event = (struct inotify_event *)p;

      DisplayInotifyEvent(event);

      p += sizeof(struct inotify_event) + event->len;
    }
  }

  return 0;
}