CFLAGS := -O2 -ggdb -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon -ansi -std=gnu99 -pedantic
WARNINGS := -Wall -Wextra -Wno-unused-parameter -Wmissing-prototypes
ASSEMBLER := -Wa,-mimplicit-it=thumb

override CFLAGS += $(WARNINGS) $(ASSEMBLER)
LIBS := -lm

all: math_test

libmathneon.a: math_acosf.o math_ldexpf.o math_powf.o math_sqrtfv.o \
	math_asinf.o math_expf.o math_log10f.o math_runfast.o math_tanf.o \
	math_atan2f.o  math_fabsf.o math_logf.o math_sincosf.o math_tanhf.o \
	math_atanf.o math_floorf.o math_mat2.o math_sinf.o math_vec2.o \
	math_ceilf.o math_fmodf.o math_mat3.o math_sinfv.o math_vec3.o \
	math_cosf.o math_frexpf.o math_mat4.o math_sinhf.o math_vec4.o \
	math_coshf.o math_invsqrtf.o math_modf.o math_sqrtf.o

math_test: math_test.o libmathneon.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o:: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.a::
	$(AR) rcs $@ $^

clean:
	$(RM) -v math_test *.o *.a
