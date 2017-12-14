AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES= source/math_acosf.c \
					source/math_ldexpf.c \
					source/math_powf.c \
					source/math_sqrtfv.c \
      				source/math_asinf.c \
					source/math_expf.c \
					source/math_log10f.c \
					source/math_runfast.c \
					source/math_tanf.c \
       				source/math_atan2f.c \
					source/math_fabsf.c \
					source/math_logf.c \
					source/math_sincosf.c \
					source/math_tanhf.c \
        			source/math_atanf.c \
					source/math_floorf.c \
					source/math_mat2.c \
					source/math_sinf.c \
					source/math_vec2.c \
         			source/math_ceilf.c \
					source/math_fmodf.c \
				   	source/math_mat3.c \
				   	source/math_sinfv.c \
					source/math_vec3.c \
	      			source/math_cosf.c \
					source/math_frexpf.c \
					source/math_mat4.c \
					source/math_sinhf.c \
					source/math_vec4.c \
	       			source/math_coshf.c \
					source/math_invsqrtf.c \
					source/math_modf.c \
					source/math_sqrtf.c \
					source/math_debug.c


#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS = -O2 -ggdb -march=armv7-a -mcpu=cortex-a5 -mthumb -mfpu=neon -mfloat-abi=hard -marm -ansi -std=gnu99 -pedantic \
			   -Wall -Wextra -Wno-unused-parameter -Wmissing-prototypes \
			   -Wa,-mimplicit-it=thumb -lm -D__arm__


EXTRA_LDLAGS=  -avoid-version


#get shared lib
lib_LTLIBRARIES=libmneon.la
libmneon_la_CPPFLAGS= $(EXTRA_CFLAGS)
libmneon_la_LDFLAGS=  $(EXTRA_LDFLAGS)
libmneon_la_SOURCES=  $(lib_common_SOURCES)
