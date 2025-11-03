bin_PROGRAMS=adjimg
adjimg_SOURCES=adjimg.c xdg-shell-protocol.c xdg-shell-client-protocol.h
adjimg_CFLAGS=$(WAYLAND_CLIENT_CFLAGS) $(WAYAND_PROTOCOLS_CFLAGS) \
	      $(XKBCOMMON_CFLAGS)
adjimg_LDFLAGS=$(WAYLAND_CLIENT_LIBS) $(XKBCOMMON_LIBS)

xdg-shell-protocol.c: @XDG_SHELL_PATH@
	wayland-scanner private-code < $< > $@

xdg-shell-client-protocol.h: @XDG_SHELL_PATH@
	wayland-scanner client-header < $< > $@
