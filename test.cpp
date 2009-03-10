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
    vfp_preserve_buf regs;

    vfp_preserve(regs);

    printf(
        "s0  = %10g s1  = %10g s2  = %10g s3  = %10g\n"
        "s4  = %10g s5  = %10g s6  = %10g s7  = %10g\n"
        "s8  = %10g s9  = %10g s10 = %10g s11 = %10g\n"
        "s12 = %10g s13 = %10g s14 = %10g s15 = %10g\n"
        "s16 = %10g s17 = %10g s18 = %10g s19 = %10g\n"
        "s20 = %10g s21 = %10g s22 = %10g s23 = %10g\n"
        "s24 = %10g s25 = %10g s26 = %10g s27 = %10g\n"
        "s28 = %10g s29 = %10g s30 = %10g s31 = %10g\n",
        regs.s[ 0], regs.s[ 1], regs.s[ 2], regs.s[ 3],
        regs.s[ 4], regs.s[ 5], regs.s[ 6], regs.s[ 7],
        regs.s[ 8], regs.s[ 9], regs.s[10], regs.s[11],
        regs.s[12], regs.s[13], regs.s[14], regs.s[15],
        regs.s[16], regs.s[17], regs.s[18], regs.s[19],
        regs.s[20], regs.s[21], regs.s[22], regs.s[23],
        regs.s[24], regs.s[25], regs.s[26], regs.s[27],
        regs.s[28], regs.s[29], regs.s[30], regs.s[31]
    );

    vfp_restore(regs);
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
    
    //for (sphere **s = spheres.begin(); s != spheres.end(); ++s)
    //    for (line **l = lines.begin(); l != lines.end(); ++l)
    //        std::cout << collision_time_sphere_line(**s, **l) << "\n";



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
