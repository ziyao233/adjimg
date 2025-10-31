#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/mman.h>
#include <unistd.h>

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

#define SHM_BUFFER_SIZE		(1024 * 1024 * 16)

#define do_if(cond, stmt) do { \
	if (cond) {							\
		stmt;							\
	}								\
} while (0);

#define assert_msg(cond, ...) \
	do_if(!(cond), {						\
		fprintf(stderr, __VA_ARGS__);				\
		exit(-1);						\
	});								\

struct wl_compositor *gWLCompositor;
struct wl_shm *gWLShm;
struct xdg_wm_base *gXdgWmBase;

struct wl_buffer *gWLBuffer;
struct wl_surface *gWLSurface;

static void
registry_on_global(void *data, struct wl_registry *registry, uint32_t name,
		   const char *interface, uint32_t version)
{
	(void)data;

	if (!strcmp(interface, wl_compositor_interface.name)) {
		gWLCompositor = wl_registry_bind(registry, name,
						 &wl_compositor_interface,
						 version);
		assert_msg(gWLCompositor, "failed to bind wl_compositor\n");
	} else if (!strcmp(interface, wl_shm_interface.name)) {
		gWLShm = wl_registry_bind(registry, name,
					  &wl_shm_interface, 1);
		assert_msg(gWLShm, "failed to bind wl_shm\n");
	} else if (!strcmp(interface, xdg_wm_base_interface.name)) {
		gXdgWmBase = wl_registry_bind(registry, name,
					      &xdg_wm_base_interface, version);
		assert_msg(gXdgWmBase, "failed to bind xdg_wm_base");
	}
}

static struct wl_registry_listener wlRegistryListener = {
	.global		= registry_on_global,
};

static void
wm_base_on_ping(void *data, struct xdg_wm_base *xdgWmBase, uint32_t serial)
{
	xdg_wm_base_pong(xdgWmBase, serial);
}

static struct xdg_wm_base_listener xdgWmBaseListener = {
	.ping		= wm_base_on_ping,
};

static void
xdg_surface_on_configure(void *data, struct xdg_surface *xdgSurface,
			 uint32_t serial)
{
	(void)data;
	xdg_surface_ack_configure(xdgSurface, serial);

	wl_surface_attach(gWLSurface, gWLBuffer, 0, 0);
	wl_surface_damage(gWLSurface, 0, 0, 640, 480);
	wl_surface_commit(gWLSurface);
}

static struct xdg_surface_listener xdgSurfaceListener = {
	.configure	= xdg_surface_on_configure,
};

int
main(int argc, const char *argv[])
{
	struct wl_display *display = wl_display_connect(NULL);
	assert_msg(display, "failed to open wayland display\n");

	struct wl_registry *registry = wl_display_get_registry(display);
	assert_msg(registry, "failed to get wayland registry\n");

	int ret = wl_registry_add_listener(registry, &wlRegistryListener, NULL);
	assert_msg(!ret, "failed to listen registry event\n");

	wl_display_roundtrip(display);

	ret = xdg_wm_base_add_listener(gXdgWmBase, &xdgWmBaseListener, NULL);
	assert_msg(!ret, "failed to listen xdg_wm_base\n");

	int shmfd = memfd_create("wl_buffer_shm", 0);
	assert_msg(shmfd >= 0, "failed to create shared memory object: %s\n",
		   strerror(errno));

	ret = ftruncate(shmfd, SHM_BUFFER_SIZE);
	assert_msg(!ret, "failed to resize shared memory object: %s\n",
		   strerror(errno));

	void *shmBuffer = mmap(NULL, SHM_BUFFER_SIZE, PROT_READ | PROT_WRITE,
			       MAP_SHARED, shmfd, 0);
	assert_msg(shmBuffer != MAP_FAILED,
		   "failed to mmap shared memory object: %s\n",
		   strerror(errno));

	struct wl_shm_pool *shmPool = wl_shm_create_pool(gWLShm, shmfd,
							 SHM_BUFFER_SIZE);
	assert_msg(shmPool, "failed to create wl_shm_pool\n");

	gWLBuffer = wl_shm_pool_create_buffer(shmPool, 0, 640, 480, 2560,
					      WL_SHM_FORMAT_XRGB8888);
	assert_msg(gWLBuffer, "failed to create wl_buffer\n");

	uint32_t *p = shmBuffer;
	for (size_t i = 0; i < 640 * 480; i++)
		p[i] = rand() * rand() * rand();

	gWLSurface = wl_compositor_create_surface(gWLCompositor);
	assert_msg(gWLSurface, "failed to create wl_surface\n");

	struct xdg_surface *xdgSurface;
	xdgSurface = xdg_wm_base_get_xdg_surface(gXdgWmBase, gWLSurface);
	assert_msg(xdgSurface, "failed to create xdg_surface\n");

	ret = xdg_surface_add_listener(xdgSurface, &xdgSurfaceListener, NULL);
	assert_msg(!ret, "failed to listen xdg_surface\n");

	struct xdg_toplevel *toplevel = xdg_surface_get_toplevel(xdgSurface);
	assert_msg(toplevel, "failed to create xdg_toplevel\n");

	xdg_toplevel_set_title(toplevel, "adjimg");

	wl_surface_commit(gWLSurface);

	while (wl_display_dispatch(display))
		;

	wl_display_disconnect(display);

	return 0;
}
