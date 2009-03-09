#include "thing.hpp"
#include "collision.hpp"
#include <boost/array.hpp>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#ifdef __arm__
extern "C" void _dumpvfps(void)
    __attribute__((noinline));

extern "C"
void _dumpvfps(void)
{
    float regs[32];

    __asm__ volatile ("fstmias %[regs], {s0-s31}\n\t" : : [regs] "r" (regs));

    printf(
        "s0  = %10g s1  = %10g s2  = %10g s3  = %10g\n"
        "s4  = %10g s5  = %10g s6  = %10g s7  = %10g\n"
        "s8  = %10g s9  = %10g s10 = %10g s11 = %10g\n"
        "s12 = %10g s13 = %10g s14 = %10g s15 = %10g\n"
        "s16 = %10g s17 = %10g s18 = %10g s19 = %10g\n"
        "s20 = %10g s21 = %10g s22 = %10g s23 = %10g\n"
        "s24 = %10g s25 = %10g s26 = %10g s27 = %10g\n"
        "s28 = %10g s29 = %10g s30 = %10g s31 = %10g\n",
        regs[ 0], regs[ 1], regs[ 2], regs[ 3],
        regs[ 4], regs[ 5], regs[ 6], regs[ 7],
        regs[ 8], regs[ 9], regs[10], regs[11],
        regs[12], regs[13], regs[14], regs[15],
        regs[16], regs[17], regs[18], regs[19],
        regs[20], regs[21], regs[22], regs[23],
        regs[24], regs[25], regs[26], regs[27],
        regs[28], regs[29], regs[30], regs[31]
    );
}
#endif

extern "C" void
test_sphere_line()
{
    using namespace battlemints;

    _dumpvfps();

    boost::array<sphere*, 7> spheres = {
        new sphere(1.0, make_vec2( 5.0, 0.0), 1.0, 0.0),
        new sphere(1.0, make_vec2(-5.0, 0.0), 1.0, 0.0),
        new sphere(1.0, make_vec2( 5.0, 0.0), 2.0, 0.0),
        new sphere(1.0, make_vec2( 5.0, 0.0), 1.0, 0.0),
        new sphere(1.0, make_vec2(-5.0, 0.0), 1.0, 0.0),
        new sphere(1.0, make_vec2( 5.0, 0.0), 1.0, 0.0),
        new sphere(1.0, make_vec2( 5.0, 0.0), 1.0, 0.0)
    };

    spheres[0]->velocity = make_vec2(-1.0,  0.0);
    spheres[1]->velocity = make_vec2( 1.0,  0.0);
    spheres[2]->velocity = make_vec2(-1.0,  0.0);
    spheres[3]->velocity = make_vec2( 1.0,  0.0);
    spheres[4]->velocity = make_vec2(-1.0,  0.0);
    spheres[5]->velocity = make_vec2(-1.0,  1.0);
    spheres[6]->velocity = make_vec2(-1.0, -1.0);

    boost::array<line*, 3> lines = {
        new line(make_vec2( 0.0, -1.0), make_vec2(0.0, 1.0)),
        new line(make_vec2(-1.0,  0.0), make_vec2(1.0, 0.0)),
        new line(make_vec2(-1.0, -1.0), make_vec2(1.0, 1.0))
    };
    
    for (sphere **s = spheres.begin(); s != spheres.end(); ++s)
        for (line **l = lines.begin(); l != lines.end(); ++l)
            std::cout << collision_time_sphere_line(**s, **l) << "\n";

    struct timeval start, end;
    gettimeofday(&start, NULL);

    float sink = 0;
    for (int i = 0; i < 100000; ++i)
        for (sphere **s = spheres.begin(); s != spheres.end(); ++s)
            for (line **l = lines.begin(); l != lines.end(); ++l)
                sink += collision_time_sphere_line(**s, **l);

    std::cout << "sink: " << sink << "\n";

    gettimeofday(&end, NULL);
    std::cout << "100,000 iterations: "
              << (int)((end.tv_sec - start.tv_sec)*1000000)
               + (int)((int)end.tv_usec - (int)start.tv_usec)
              << "\n";

}
