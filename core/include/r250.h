// source code taken from: http://www.taygeta.com/random.xml
//                     and http://www.taygeta.com/random/gaussian.html

/* r250.h	prototypes for r250 random number generator,

		Kirkpatrick, S., and E. Stoll, 1981; "A Very Fast
		Shift-Register Sequence Random Number Generator",
		Journal of Computational Physics, V.40

		also:

		see W.L. Maier, DDJ May 1991
*/

#ifndef _R250_H_
#define _R250_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NO_PROTO
void         r250_init();
unsigned int r250();
double      dr250();

#else
void         r250_init(int seed);
unsigned int r250( void );
double       dr250( void );
#endif

#ifdef NO_PROTO
long         set_seed();
long         get_seed();
unsigned long int randlcg();

#else
long         set_seed(long);
long         get_seed(long);
unsigned long int randlcg();

#endif

double dr250_box_muller(double m, double s);

#ifdef __cplusplus
}
#endif

#endif
