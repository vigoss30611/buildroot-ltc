#include <fr/libfr.h>
#include <audiobox2.h>
#include <alsa/asoundlib.h>
#include <list.h>
#include <base.h>
#include <stdio.h>
#include <log.h>
#include <ctype.h>
#include <math.h>
#include <audio_ctl.h>

#define convert_prange1(val, min, max) \
	ceil((val) * ((max) - (min)) * 0.01 + (min))

#define check_range(val, min, max) \
	((val < min) ? (min) : (val > max) ? (max) : (val))

static long get_integer(char **ptr, long min, long max)
{
	long val = min;
	char *p = *ptr, *s;

	if (*p == ':')
		p++;
	if (*p == '\0' || (!isdigit(*p) && *p != '-'))
		goto out;

	s = p;
	val = strtol(s, &p, 10);
	if (*p == '.') {
		p++;
		strtol(p, &p, 10);
	}
	if (*p == '%') {
		val = (long)convert_prange1(strtod(s, NULL), min, max);
		p++;
	}
	val = check_range(val, min, max);
	if (*p == ',')
		p++;
 out:
	*ptr = p;
	return val;
}

static long get_integer64(char **ptr, long long min, long long max)
{
	long long val = min;
	char *p = *ptr, *s;

	if (*p == ':')
		p++;
	if (*p == '\0' || (!isdigit(*p) && *p != '-'))
		goto out;

	s = p;
	val = strtol(s, &p, 10);
	if (*p == '.') {
		p++;
		strtol(p, &p, 10);
	}
	if (*p == '%') {
		val = (long long)convert_prange1(strtod(s, NULL), min, max);
		p++;
	}
	val = check_range(val, min, max);
	if (*p == ',')
		p++;
 out:
	*ptr = p;
	return val;
}

static int get_ctl_enum_item_index(snd_ctl_t *handle, snd_ctl_elem_info_t *info,
				   char **ptrp)
{
	char *ptr = *ptrp;
	int items, i, len;
	const char *name;
	
	items = snd_ctl_elem_info_get_items(info);
	if (items <= 0)
		return -1;

	for (i = 0; i < items; i++) {
		snd_ctl_elem_info_set_item(info, i);
		if (snd_ctl_elem_info(handle, info) < 0)
			return -1;
		name = snd_ctl_elem_info_get_item_name(info);
		len = strlen(name);
		if (! strncmp(name, ptr, len)) {
			if (! ptr[len] || ptr[len] == ',' || ptr[len] == '\n') {
				ptr += len;
				*ptrp = ptr;
				return i;
			}
		}
	}
	return -1;
}

static int parse_control_id(const char *str, snd_ctl_elem_id_t *id)
{
	int c, size, numid;
	char *ptr;

	while (*str == ' ' || *str == '\t')
		str++;
	if (!(*str))
		return -EINVAL;
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);	/* default */
	while (*str) {
		if (!strncasecmp(str, "numid=", 6)) {
			str += 6;
			numid = atoi(str);
			if (numid <= 0) {
				fprintf(stderr, "amixer: Invalid numid %d\n", numid);
				return -EINVAL;
			}
			snd_ctl_elem_id_set_numid(id, atoi(str));
			while (isdigit(*str))
				str++;
		} else if (!strncasecmp(str, "iface=", 6)) {
			str += 6;
			if (!strncasecmp(str, "card", 4)) {
				snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_CARD);
				str += 4;
			} else if (!strncasecmp(str, "mixer", 5)) {
				snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
				str += 5;
			} else if (!strncasecmp(str, "pcm", 3)) {
				snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_PCM);
				str += 3;
			} else if (!strncasecmp(str, "rawmidi", 7)) {
				snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_RAWMIDI);
				str += 7;
			} else if (!strncasecmp(str, "timer", 5)) {
				snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_TIMER);
				str += 5;
			} else if (!strncasecmp(str, "sequencer", 9)) {
				snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_SEQUENCER);
				str += 9;
			} else {
				return -EINVAL;
			}
		} else if (!strncasecmp(str, "name=", 5)) {
			char buf[64];
			str += 5;
			ptr = buf;
			size = 0;
			if (*str == '\'' || *str == '\"') {
				c = *str++;
				while (*str && *str != c) {
					if (size < (int)sizeof(buf)) {
						*ptr++ = *str;
						size++;
					}
					str++;
				}
				if (*str == c)
					str++;
			} else {
				while (*str && *str != ',') {
					if (size < (int)sizeof(buf)) {
						*ptr++ = *str;
						size++;
					}
					str++;
				}
				*ptr = '\0';
			}
			snd_ctl_elem_id_set_name(id, buf);
		} else if (!strncasecmp(str, "index=", 6)) {
			str += 6;
			snd_ctl_elem_id_set_index(id, atoi(str));
			while (isdigit(*str))
				str++;
		} else if (!strncasecmp(str, "device=", 7)) {
			str += 7;
			snd_ctl_elem_id_set_device(id, atoi(str));
			while (isdigit(*str))
				str++;
		} else if (!strncasecmp(str, "subdevice=", 10)) {
			str += 10;
			snd_ctl_elem_id_set_subdevice(id, atoi(str));
			while (isdigit(*str))
				str++;
		}
		if (*str == ',') {
			str++;
		} else {
			if (*str)
				return -EINVAL;
		}
	}			
	return 0;
}

static int audio_get_control(snd_hctl_elem_t *elem, char *value)
{
	int err;
	unsigned int idx, count;
	snd_ctl_elem_type_t type;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_value_t *control;
	snd_aes_iec958_t iec958;
	unsigned long enumid;

	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_value_alloca(&control);

	if ((err = snd_hctl_elem_info(elem, info)) < 0) {
		AUD_ERR("Control snd_hctl_elem_info error: %s\n", snd_strerror(err));
		return err;
	}

	count = snd_ctl_elem_info_get_count(info);
	type = snd_ctl_elem_info_get_type(info);

	if ((err = snd_hctl_elem_read(elem, control)) < 0) {
		AUD_ERR("Control element read error: %s\n", snd_strerror(err));
		return err;
	}

	for (idx = 0; idx < count; idx++) {
		switch (type) {
			case SND_CTL_ELEM_TYPE_BOOLEAN:
				sprintf(value, "%s", snd_ctl_elem_value_get_boolean(control, idx) ? "on" : "off");
				AUD_INFO("BOOLEAN: %s\n", value);
				break;
			case SND_CTL_ELEM_TYPE_INTEGER:
				*(long *)value = snd_ctl_elem_value_get_integer(control, idx);
				AUD_INFO("INTEGER: %li\n", snd_ctl_elem_value_get_integer(control, idx));
				break;
			case SND_CTL_ELEM_TYPE_INTEGER64:
				*(long long *)value = snd_ctl_elem_value_get_integer64(control, idx);
				AUD_INFO("INTEGER64: %Li\n", snd_ctl_elem_value_get_integer64(control, idx));
				break;
			case SND_CTL_ELEM_TYPE_ENUMERATED:
				enumid = snd_ctl_elem_value_get_enumerated(control, idx);
				snd_ctl_elem_info_set_item(info, enumid);
				if ((err = snd_hctl_elem_info(elem, info)) < 0)
					return -1;

				sprintf(value, "%s", snd_ctl_elem_info_get_item_name(info));
				AUD_INFO("ENUMERATED: %s\n", value);
				break;
			case SND_CTL_ELEM_TYPE_BYTES:
				*(unsigned char *)value = snd_ctl_elem_value_get_byte(control, idx);
				AUD_INFO("BYTES: 0x%02x\n", snd_ctl_elem_value_get_byte(control, idx));
				break;
			case SND_CTL_ELEM_TYPE_IEC958:
				snd_ctl_elem_value_get_iec958(control, &iec958);
				memcpy(value, &iec958, sizeof(snd_aes_iec958_t));
				AUD_INFO("[AES0=0x%02x AES1=0x%02x AES2=0x%02x AES3=0x%02x]\n",
						iec958.status[0], iec958.status[1],
						iec958.status[2], iec958.status[3]);
				break;
			default:
				AUD_ERR("Unknow Audio CTL type\n");
				break;
		}
	}

	return 0;
}

int audio_route_cget(audio_dev_t dev, char *route, char *value)
{
	int err;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_hctl_t *hctl;
	snd_hctl_elem_t *elem;

	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_id_alloca(&id);

	if (parse_control_id(route, id)) {
		AUD_ERR("Wrong control identifier: %s\n", route);
		return -1;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(dev->ctl, info)) < 0) {
		AUD_ERR("Cannot find the given element from control %s\n", dev->devname);
		return err;
	}
	snd_ctl_elem_info_get_id(info, id);	/* FIXME: Remove it when hctl find works ok !!! */
	
	err = snd_hctl_open_ctl(&hctl, dev->ctl);
	if (err < 0) {
		AUD_ERR("open hctl err: %d\n", err);
		return err;
	}

	if ((err = snd_hctl_load(hctl)) < 0) {
		AUD_ERR("Control %s load error: %s\n", dev->devname, snd_strerror(err));
		return err;
	}

	elem = snd_hctl_find_elem(hctl, id);
	if (!elem) {
		AUD_ERR("Could not find the specified element\n");
		goto done;
	}

	err = audio_get_control(elem, value);
	if (err < 0) {
		AUD_ERR("Audio get control err\n");
		goto done;
	}

	return 0;
done:
	snd_hctl_free(hctl);
	free(hctl);
	return -1;
}

int audio_route_cset(audio_dev_t dev, char *route, char *value)
{
	int err;
	static snd_ctl_t *handle = NULL;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	char *ptr;
	unsigned int idx, count;
	long tmp;
	snd_ctl_elem_type_t type;
	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_value_alloca(&control);

	if (parse_control_id(route, id)) {
		AUD_ERR("Wrong control identifier: %s\n", route);
		return -1;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(dev->ctl, info)) < 0) {
		AUD_ERR("Cannot find the given element from control %s\n", dev->devname);
		return err;
	}

	snd_ctl_elem_info_get_id(info, id);	/* FIXME: Remove it when hctl find works ok !!! */
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	ptr = value;
	for (idx = 0; idx < count && idx < 128 && ptr && *ptr; idx++) {
		switch (type) {
			case SND_CTL_ELEM_TYPE_BOOLEAN:
				tmp = 0;
				if (!strncasecmp(ptr, "on", 2) || !strncasecmp(ptr, "up", 2)) {
					tmp = 1;
					ptr += 2;
				} else if (!strncasecmp(ptr, "yes", 3)) {
					tmp = 1;
					ptr += 3;
				} else if (!strncasecmp(ptr, "toggle", 6)) {
					tmp = snd_ctl_elem_value_get_boolean(control, idx);
					tmp = tmp > 0 ? 0 : 1;
					ptr += 6;
				} else if (isdigit(*ptr)) {
					tmp = atoi(ptr) > 0 ? 1 : 0;
					while (isdigit(*ptr))
						ptr++;
				} else {
					while (*ptr && *ptr != ',')
						ptr++;
				}
				snd_ctl_elem_value_set_boolean(control, idx, tmp);
				break;
			case SND_CTL_ELEM_TYPE_INTEGER:
				tmp = get_integer(&ptr,
						snd_ctl_elem_info_get_min(info),
						snd_ctl_elem_info_get_max(info));
				snd_ctl_elem_value_set_integer(control, idx, tmp);
				break;
			case SND_CTL_ELEM_TYPE_INTEGER64:
				tmp = get_integer64(&ptr,
						snd_ctl_elem_info_get_min64(info),
						snd_ctl_elem_info_get_max64(info));
				snd_ctl_elem_value_set_integer64(control, idx, tmp);
				break;
			case SND_CTL_ELEM_TYPE_ENUMERATED:
				tmp = get_ctl_enum_item_index(handle, info, &ptr);
				if (tmp < 0)
					tmp = get_integer(&ptr, 0, snd_ctl_elem_info_get_items(info) - 1);
				snd_ctl_elem_value_set_enumerated(control, idx, tmp);
				break;
			case SND_CTL_ELEM_TYPE_BYTES:
				tmp = get_integer(&ptr, 0, 255);
				snd_ctl_elem_value_set_byte(control, idx, tmp);
				break;
			default:
				break;
		}
		if (!strchr(value, ','))
			ptr = value;
		else if (*ptr == ',')
			ptr++;
	}

	return snd_ctl_elem_write(dev->ctl, control);
}

int audio_open_ctl(audio_dev_t dev)
{
	int ret;

	ret = snd_ctl_open(&dev->ctl, dev->devname, 0);
	if (ret < 0)
		return snd_ctl_open(&dev->ctl, "default", 0);

	return ret;
}

void audio_close_ctl(audio_dev_t dev)
{
	snd_ctl_close(dev->ctl);
}
