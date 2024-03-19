#include<mpd/status.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<mpd/client.h>

struct mpd_connection* conn = NULL;

void sig_handler(int signum){
	switch (signum){
		case SIGINT:
			printf("Caught interrupt, exiting...\n");
			mpd_send_noidle(conn);
			mpd_response_finish(conn);
			mpd_connection_free(conn);
			exit(0);
	}
}


int main() 
{
	const char * title = NULL, * artist = NULL, * uri = NULL;
	struct mpd_status * status;
	struct mpd_song *song = NULL;
	enum mpd_state state;
	bool status_code;
	char status_icon;

	// register signal handler
	signal(SIGINT, sig_handler);

	// open connection
	conn = mpd_connection_new(NULL, 0, 0);

	// main loop
	do {
		status_code = mpd_send_status(conn);
		if (status_code == false) { 
			printf("status request failed.\n");
			fflush(stdout);
			sleep(60);
			conn = mpd_connection_new(NULL, 0, 0);
			continue;
		}

		status = mpd_recv_status(conn);
		state = mpd_status_get_state(status);

		switch (state) {
			case MPD_STATE_PLAY:
				status_icon = '>';
				break;
			case MPD_STATE_PAUSE:
				status_icon = '=';
				break;
			case MPD_STATE_STOP:
				status_icon = 'x';
				break;
			case MPD_STATE_UNKNOWN:
				status_icon = '?';
				break;
		}

		if (state == MPD_STATE_PLAY || state == MPD_STATE_PAUSE) {
				mpd_send_current_song(conn);
				song = mpd_recv_song(conn);
				artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
				title = mpd_song_get_tag(song,MPD_TAG_TITLE, 0);

				// if tags are empty, print URI instead
				if (artist == NULL && title == NULL){
					uri = mpd_song_get_uri(song);
					printf("[%c] %s\n", status_icon, uri);
				} else {
					printf("[%c] %s - %s\n", status_icon, artist, title);
				}
				fflush(stdout);
				mpd_song_free(song);
		} else {
				printf("\n");
				fflush(stdout);
		}

		mpd_status_free(status);
		mpd_response_finish(conn);
		
	} while (mpd_run_idle_mask(conn, MPD_IDLE_PLAYER));

	/* Cleanup */
	mpd_connection_free(conn);

	return 0;
}
