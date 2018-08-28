#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/types/wlr_output.h>

#include "vimway.h"

int main(int argc, char* argv[]) {
	struct vw_server server;

	server.wl_display = wl_display_create();
	assert(server.wl_display);
	server.wl_event_loop = wl_display_get_event_loop(server.wl_display);
	assert(server.wl_event_loop);

	server.backend = wlr_backend_autocreate(server.wl_display, NULL);
	assert(server.backend);

	wl_list_init(&server.outputs);

	server.new_output.notify = new_output_notify;
	wl_signal_add(&server.backend->events.new_output, &server.new_output);

	const char *socket = wl_display_add_socket_auto(server.wl_display);
	assert(socket);

	if (!wlr_backend_start(server.backend)) {
		fprintf(stderr, "Failed to start backend\n");
		wl_display_destroy(server.wl_display);
		return 1;
	}

	printf("Running compositor on wayland display '%s'\n", socket);
	setenv("WAYLAND_DISPLAY", socket, true);

	wl_display_init_shm(server.wl_display);

	wl_display_run(server.wl_display);
	wl_display_destroy(server.wl_display);

	printf("Stopping test-comp...\n");

	return 0;
}
