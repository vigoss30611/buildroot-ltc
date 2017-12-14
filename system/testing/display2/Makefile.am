EXTRA_CFLAGS=  -I$(srcdir)/include -Iinclude -Wall -Werror

bin_PROGRAMS=buf2fb bmp2fb ck2fb demo_ck demo_vsync fben set_vic select_peripheral
buf2fb_SOURCES = buf2fb.c
buf2fb_CFLAGS= $(EXTRA_CFLAGS)
buf2fb_LDFLAGS= -version-info 1:0:0

bmp2fb_SOURCES = bmp2fb.c
bmp2fb_CFLAGS= $(EXTRA_CFLAGS)
bmp2fb_LDFLAGS= -version-info 1:0:0	\
			$(LIBFR_LIBS)

ck2fb_SOURCES = ck2fb.c
ck2fb_CFLAGS= $(EXTRA_CFLAGS)
ck2fb_LDFLAGS= -version-info 1:0:0

demo_ck_SOURCES = demo_ck.c
demo_ck_CFLAGS= $(EXTRA_CFLAGS)
demo_ck_LDFLAGS= -version-info 1:0:0

demo_vsync_SOURCES = demo_vsync.c
demo_vsync_CFLAGS= $(EXTRA_CFLAGS)
demo_vsync_LDFLAGS= -version-info 1:0:0	\
			$(LIBFR_LIBS)

fben_SOURCES = fben.c
fben_CFLAGS= $(EXTRA_CFLAGS)
fben_LDFLAGS= -version-info 1:0:0

set_vic_SOURCES = set_vic.c
set_vic_CFLAGS= $(EXTRA_CFLAGS)
set_vic_LDFLAGS= -version-info 1:0:0

select_peripheral_SOURCES = select_peripheral.c
select_peripheral_CFLAGS= $(EXTRA_CFLAGS)
select_peripheral_LDFLAGS= -version-info 1:0:0
