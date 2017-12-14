struct simple_cache {
	char *data;
	loff_t start;
	size_t size;
	int valid;
};

#define sc_hit(_s, f, l) ((_s)->valid && (f >= (_s)->start)  \
			   && (f + l <= (_s)->start + (_s)->size))
#define sc_overlap(_s, t, l) ((_s)->start < t + l && t < (_s)->start + (_s)->size)
//#define sc_fit(_s, f, l) ((f & ~((_s)->size - 1)) + (_s)->size >= f + l)
#define sc_fit(_s, f, l) (!(l & 0x3ff) && (f & ~((_s)->size - 1)) + (_s)->size >= f + l)
#define sc_inv(_s) ((_s)->valid = 0)
static inline loff_t
sc_set(struct simple_cache *sc, loff_t from, size_t len)
{
	sc->start = from & ~(sc->size - 1);
	sc->valid = 1;
	return sc->start;
}
static inline int
sc_get(struct simple_cache *sc, u_char *buf, loff_t from, size_t len)
{
	memcpy(buf, sc->data + from - sc->start, len);
	return len;
}
static inline int
sc_init(struct simple_cache *sc, int size)
{
	sc->size = size;
	sc->data = kzalloc(sc->size, GFP_KERNEL);
	sc->start = sc->valid = 0;
	return 0;
}

static long long __getns(void) {
	ktime_t a = ktime_get_boottime();
	return a.tv64;
}

