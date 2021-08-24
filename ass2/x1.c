// Test Bits ADT

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"

int main(int argc, char **argv)
{
	Bits b = newBits(60);
	printf("t=0: "); showBits(b); printf("\n");
	setBit(b, 5);
	printf("t=1: "); showBits(b); printf("\n");
	setBit(b, 0);
	setBit(b, 50);
	setBit(b, 59);
	printf("t=2: "); showBits(b); printf("\n");
	if (bitIsSet(b,5)) printf("Bit 5 is set\n");
	if (bitIsSet(b,10)) printf("Bit 10 is set\n");
	setAllBits(b);
	printf("t=3: "); showBits(b); printf("\n");
	unsetBit(b, 40);
	printf("t=4: "); showBits(b); printf("\n");
	if (bitIsSet(b,20)) printf("Bit 20 is set\n");
	if (bitIsSet(b,40)) printf("Bit 40 is set\n");
	if (bitIsSet(b,50)) printf("Bit 50 is set\n");
	setBit(b, 59);

	// unsetAllBits(b);
	// printf("t=?: "); showBits(b); printf("\n");
	// setBit(b, 1);
	// setBit(b, 9);
	// setBit(b, 17);
	// setBit(b, 25);
	// setBit(b, 33);
	// setBit(b, 41);
	// setBit(b, 49);
	// setBit(b, 57);
	// setBit(b, 59);
	// printf("t=???: "); showBits(b); printf("\n");
	// shiftBits(b, -50);
	// printf("t=???: "); showBits(b); printf("\n");

	// unsetAllBits(b);

	Bits b1 = newBits(25);
	printf("t=???: "); showBits(b1); printf("\n");
	setBit(b1, 1);
	setBit(b1, 9);
	printf("t=???: "); showBits(b1); printf("\n");
	unsetAllBits(b1);
	printf("t=???: "); showBits(b1); printf("\n");
	setAllBits(b1);
	printf("t=???: "); showBits(b1); printf("\n");
	printf("\n");
	// setBit(b1, 17);
	// setBit(b1, 2);
	// setBit(b1, 6);
	// setBit(b1, 41);
	// setBit(b1, 49);
	// printf("\n%d\n", isSubset(b1, b));
	printf("\n");

	return 0;
}
