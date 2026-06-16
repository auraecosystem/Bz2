/*-----------------------------------------------------------*/
/*--- Demonstration / regression test for BZ2_allocSize.  ---*/
/*---                                 alloc_overflow_test.c ---*/
/*-----------------------------------------------------------*/

/* Demonstrates the allocator hardening: BZ2_allocSize() rejects an
   element-count * element-size product that does not fit in a signed
   32-bit int, instead of letting it wrap around to a small (or zero)
   value -- which is what the old `malloc(items * size)` in
   default_bzalloc() / the old `n * sizeof(...)` call sites would do,
   yielding an undersized buffer and a subsequent heap overflow.

   Build against the static library, e.g.:

     cc -I. -c bzlib.c blocksort.c huffman.c crctable.c \
           randtable.c compress.c decompress.c
     ar rcs libbz2.a *.o
     cc -I. alloc_overflow_test.c libbz2.a -o alloc_overflow_test
     ./alloc_overflow_test

   (BZ2_allocSize is an internal symbol, so link the static archive.)
*/

#include <stdio.h>

typedef int Int32;

extern Int32 BZ2_allocSize ( Int32 nelems, Int32 elemSize );

static int failures = 0;

static void expect ( const char* what, Int32 got, Int32 want )
{
   if (got == want) {
      printf ( "  ok:   %-40s = %d\n", what, got );
   } else {
      printf ( "  FAIL: %-40s = %d (expected %d)\n", what, got, want );
      failures++;
   }
}

int main ( void )
{
   printf ( "BZ2_allocSize overflow-check demonstration:\n" );

   /* Sizes libbzip2 actually requests (blockSize100k == 9) must pass. */
   expect ( "BZ2_allocSize(900000, 4)",   BZ2_allocSize(900000, 4),   3600000 );
   expect ( "BZ2_allocSize(65537, 4)",    BZ2_allocSize(65537, 4),    262148  );

   /* Products that overflow Int32 must be rejected (-1), NOT wrapped.
      0x40000000 * 4 == 0x100000000, which truncates to 0 in 32 bits:
      the old code would have called malloc(0). */
   expect ( "BZ2_allocSize(0x40000000, 4)", BZ2_allocSize(0x40000000, 4), -1 );
   expect ( "BZ2_allocSize(0x7FFFFFFF, 2)", BZ2_allocSize(0x7FFFFFFF, 2), -1 );

   /* Negative inputs (e.g. a chained failed result) are rejected. */
   expect ( "BZ2_allocSize(-1, 4)",        BZ2_allocSize(-1, 4),        -1 );

   if (failures == 0) {
      printf ( "ALL PASS\n" );
      return 0;
   }
   printf ( "%d FAILURE(S)\n", failures );
   return 1;
}

/*-----------------------------------------------------------*/
/*--- end                            alloc_overflow_test.c ---*/
/*-----------------------------------------------------------*/
