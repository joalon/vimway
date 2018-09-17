#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/types/wlr_list.h>
#include <wlr/types/wlr_matrix.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_box.h>
#include <wlr/types/wlr_surface.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_input_device.h>
#include <xkbcommon/xkbcommon.h>

#include "vimway.h"

void keyboard_handle_key(struct wl_listener *listener, void *data) {
	printf("Key pressed!\n");

	return;
}

void keyboard_handle_modifiers(struct wl_listener *listener, void *data) {
	printf("Handling keyboard modifiers\n");

	return;
}

void new_input_notify(struct wl_listener *listener, void *data) {
	struct vw_server *server = wl_container_of(listener, server, new_input);
	struct wlr_input_device *wlr_input_device = data;
	printf("In new_input_notify!\n");

	struct wlr_keyboard *keyboard;
//	struct wlr_pointer *pointer;

	if (wlr_input_device->type == WLR_INPUT_DEVICE_KEYBOARD) {
		printf("Got a keyboard!\n");
		keyboard = wlr_input_device->keyboard;
		printf("num_keycodes: %d\n", keyboard->num_keycodes);

		// Create a new xkb keymap
		struct xkb_rule_names rules = { 0 };
		struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		struct xkb_keymap *keymap = xkb_map_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);

		// Wire up keyboard
		wlr_keyboard_set_keymap(keyboard, keymap);
		xkb_keymap_unref(keymap);
		xkb_context_unref(context);
		wlr_keyboard_set_repeat_info(keyboard, 25, 600);


		// Set callbacks for keyboard events
//		keyboard->modifiers.notify = keyboard_handle_modifiers;
//		wl_signal_add(&keyboard->events.modifiers, NULL);
//		keyboard->key.notify = keyboard_handle_key;

		struct wl_listener *new_listener = calloc(1, sizeof(struct wl_listener));
		new_listener->notify = keyboard_handle_key;
		wl_signal_add(&wlr_input_device->keyboard->events.key, new_listener);

		wlr_seat_set_keyboard(server->seat, wlr_input_device);

		wlr_seat_set_capabilities(server->seat, server->seat->capabilities |= WL_SEAT_CAPABILITY_KEYBOARD); 


	} else if (wlr_input_device->type == WLR_INPUT_DEVICE_POINTER) {
		printf("Got a pointer!\n");
//		pointer = wlr_input_device->pointer;
	}

	return;
}

void output_destroy_notify(struct wl_listener *listener, void *data) {
	struct vw_output *output = wl_container_of(listener, output, destroy);
	wl_list_remove(&output->link);
	wl_list_remove(&output->destroy.link);
	wl_list_remove(&output->frame.link);
	free(output);
}

void output_frame_notify(struct wl_listener *listener, void *data) {
	struct vw_output *output = wl_container_of(listener, output, frame);
	struct vw_server *server = output->server;
	struct wlr_output *wlr_output = data;
	struct wlr_renderer *renderer = wlr_backend_get_renderer(wlr_output->backend);

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	wlr_output_make_current(wlr_output, NULL);
	wlr_renderer_begin(renderer, wlr_output->width, wlr_output->height);

	float color[4] = { 1.0f, 0.2f, 0.2f, 1.0f };
	wlr_renderer_clear(renderer, color);

	struct wl_resource *_surface;
	wl_resource_for_each(_surface, &server->compositor->surface_resources) {
		struct wlr_surface *surface = wlr_surface_from_resource(_surface);
		if (!wlr_surface_has_buffer(surface)) {
			continue;
		}

		struct wlr_box render_box = {
                        .x = 20, .y = 20,
                        .width = surface->current.width,
                        .height = surface->current.height
                };
                float matrix[16];

		wlr_render_rect(renderer, &render_box, color, wlr_output->transform_matrix); 
		wlr_surface_send_frame_done(surface, &now);
	}

	wlr_output_swap_buffers(wlr_output, NULL, NULL);
	wlr_renderer_end(renderer);
}

void new_output_notify(struct wl_listener *listener, void *data) {
	struct vw_server *server = wl_container_of(listener, server, new_output);
	struct wlr_output *wlr_output = data;

	printf("In new_output_notify\n");

	if (!wl_list_empty(&wlr_output->modes)) {
		struct wlr_output_mode *mode = wl_container_of(wlr_output->modes.prev, mode, link);
		wlr_output_set_mode(wlr_output, mode);
	}

	struct vw_output *output = calloc(1, sizeof(struct vw_output));
	clock_gettime(CLOCK_MONOTONIC, &output->last_frame);
	output->server = server;
	output-> wlr_output = wlr_output;
	wl_list_insert(&server->outputs, &output->link);

	output->destroy.notify = output_destroy_notify;
	wl_signal_add(&wlr_output->events.destroy, &output->destroy);

	output->frame.notify = output_frame_notify;
	wl_signal_add(&wlr_output->events.frame, &output->frame);

	wlr_output_create_global(wlr_output);
}

