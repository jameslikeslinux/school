#ifndef BASE64_H
#define BASE64_H

/* GPL Code from the VLC project */
static inline char *vlc_b64_encode(unsigned char *src) {
	static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	char *dst = malloc(strlen(src) * 4 / 3 + 12);
	char *ret = dst;
	unsigned i_bits = 0;
	unsigned i_shift = 0;
	
	while (1) {
		if (*src) {
			i_bits = (i_bits << 8) | (*src++);
			i_shift += 8;
		} else if (i_shift > 0) {
			i_bits <<= 6 - i_shift;
			i_shift = 6;
		} else {
			*dst++ = '=';
			break;
		}

		while (i_shift >= 6) {
			i_shift -= 6;
			*dst++ = b64[(i_bits >> i_shift) & 0x3f];
		}
	}

	*dst++ = '\0';

	return ret;
}

#endif
