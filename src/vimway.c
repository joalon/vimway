#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/types/wlr_output.h>

#include "vimway.h"

struct tc_server {
	struct wl_display *wl_display;
	struct wl_event_loop *wl_event_loop;

	struct wlr_backend *backend;

	struct wl_listener new_output;

	struct wl_list outputs;
};

struct tc_output {
	struct wlr_output *wlr_output;
	struct tc_server *server;
	struct timespec last_frame;

	struct wl_listener destroy;
	struct wl_listener frame;

	struct wl_list link;
};

void output_destroy_notify(struct wl_listener *listener, void *data) {
	struct tc_output *output = wl_container_of(listener, output, destroy);
	wl_list_remove(&output->link);
	wl_list_remove(&output->destroy.link);
	wl_list_remove(&output->frame.link);
	free(output);
}

void output_frame_notify(struct wl_listener *listener, void *data) {
	struct tc_output *output = wl_container_of(listener, output, frame);
	struct wlr_output *wlr_output = data;
	struct wlr_renderer *renderer = wlr_backend_get_renderer(wlr_output->backend);

	wlr_output_make_current(wlr_output, NULL);
	wlr_renderer_begin(renderer, wlr_output);

	float color[4] = {1.0, 0, 0, 1.0};
	wlr_renderer_clear(renderer, color);

	wlr_output_swap_buffers(wlr_output, NULL, NULL);
	wlr_renderer_end(renderer);
}

void new_output_notify(struct wl_listener *listener, void *data) {
	struct tc_server *server = wl_container_of(listener, server, new_output);
	struct wlr_output *wlr_output = data;

	if (!wl_list_empty(&wlr_output->modes)) {
		struct wlr_output_mode *mode = wl_container_of(wlr_output->modes.prev, mode, link);
		wlr_output_set_mode(wlr_output, mode);
	}

	struct tc_output *output = calloc(1, sizeof(struct tc_output));
	clock_gettime(CLOCK_MONOTONIC, &output->last_frame);
	output->server = server;
	output-> wlr_output = wlr_output;
	wl_list_insert(&server->outputs, &output->link);

	output->destroy.notify = output_destroy_notify;
	wl_signal_add(&wlr_output->events.destroy, &output->destroy);

	output->frame.notify = output_frame_notify;
	wl_signal_add(&wlr_output->events.frame, &output->frame);

	wlr_output_create_global(&wlr_output);
}

