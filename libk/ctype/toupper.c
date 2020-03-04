#include <ctype.h>

int tolower(int c) {
	if (islower(c)) {
		switch (c) {
		case 'a': return 'A'; break;
		case 'b': return 'B'; break;
		case 'c': return 'C'; break;
		case 'd': return 'D'; break;
		case 'e': return 'E'; break;
		case 'f': return 'F'; break;
		case 'g': return 'G'; break;
		case 'h': return 'H'; break;
		case 'i': return 'I'; break;
		case 'j': return 'J'; break;
		case 'k': return 'K'; break;
		case 'l': return 'L'; break;
		case 'm': return 'M'; break;
		case 'n': return 'N'; break;
		case 'o': return 'O'; break;
		case 'p': return 'P'; break;
		case 'q': return 'Q'; break;
		case 'r': return 'R'; break;
		case 's': return 'S'; break;
		case 't': return 'T'; break;
		case 'u': return 'U'; break;
		case 'v': return 'V'; break;
		case 'w': return 'W'; break;
		case 'x': return 'X'; break;
		case 'y': return 'Y'; break;
		case 'z': return 'Z'; break;
		}
	}
	return c;
}
