#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

unsigned int uptime = 0;

void pretty_time(unsigned int seconds) 
{
	int hours = seconds / 3600;
	int minutes = ( seconds - hours * 3600 ) / 60;
	printf("up %d:%02d\n", hours, minutes);
	fflush(stdout);
}

void reset_clock(int signum)
{
	uptime = 60;
	pretty_time(uptime);
}

void add_time(int signum)
{
	// add 1 hour to clock
	uptime += 60 * 60;
	pretty_time(uptime);
}

int main()
{
	FILE *stream;
	char *line = NULL;
	char delim = ' ';
	size_t len = 0;
  
	// dwmblocks will eat USR1
	signal(SIGHUP, reset_clock);
	signal(SIGUSR2, add_time);

	stream = fopen("/proc/uptime", "r");
	getdelim(&line, &len, delim, stream);
	fclose(stream);
	uptime = (int) strtol(line, (char **)NULL, 10);

	while (1) 
	{
		pretty_time(uptime);
		sleep(60);
		uptime = uptime + 60;
	}
	return 0;
}
