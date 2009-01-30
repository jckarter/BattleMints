USING: accessors arrays assocs kernel literals math math.affine-transforms
math.functions math.vectors sequences sets ;
IN: battlemints.things

! utility words
: vmax* ( array -- vmax ) { -1.0/0.0 -1.0/0.0 } [ vmax ] reduce ; 
: vmin* ( array -- vmax ) {  1.0/0.0  1.0/0.0 } [ vmin ] reduce ; 
: rotate ( sequence -- equences ) 1 cut-slice prepend ;

TUPLE: thing transform ; 
TUPLE: shape < thing ;
TUPLE: tile < shape ;
TUPLE: actor < thing ;

TUPLE: arrow < shape ;

TUPLE: tile-octagon < tile ;
TUPLE: tile-hexagon-90 < tile ;
TUPLE: tile-hexagon < tile ;
TUPLE: tile-trapezoid < tile ;
TUPLE: tile-square < tile ;
TUPLE: tile-rhombus-60 < tile ;
TUPLE: tile-rhombus-45 < tile ;
TUPLE: tile-rhombus-30 < tile ;
TUPLE: tile-triangle < tile ;

TUPLE: mini < actor ;
TUPLE: mega < actor ;
TUPLE: bumper < actor ;
TUPLE: player < actor ;
TUPLE: powerup < actor powerup-kind ;

TUPLE: line endpoints ;

TUPLE: tripwire < line ;
TUPLE: goal < tripwire next-board ;

TUPLE: wall < line ;

TUPLE: wallpost center ;

GENERIC: (shape-vertices) ( tile -- vertices )

M: arrow (shape-vertices)
    drop {
        {  0.0 -0.3 }
        { -0.5 -0.3 }
        { -0.5  0.3 }
        {  0.0  0.3 }
        {  0.0  0.5 }
        {  0.5  0.0 }
        {  0.0 -0.5 }
        {  0.0 -0.3 }
    } ;

M: tile-octagon (shape-vertices)
    drop {
        {    0.0                        0.0              }
        { $[     2.0 sqrt         ] $[      2.0 sqrt   ] }
        { $[ 2.0 2.0 sqrt       + ] $[      2.0 sqrt   ] }
        { $[ 2.0 2.0 sqrt 2.0 * + ]     0.0              }
        { $[ 2.0 2.0 sqrt 2.0 * + ]    -2.0              }
        { $[ 2.0 2.0 sqrt       + ] $[ -2.0 2.0 sqrt - ] }
        { $[     2.0 sqrt         ] $[ -2.0 2.0 sqrt - ] }
        {    0.0                       -2.0              }
    } ;

M: tile-hexagon-90 (shape-vertices)
    drop {
        {    0.0                       0.0            }
        { $[     2.0 sqrt         ] $[ 2.0 sqrt     ] }
        { $[ 2.0 2.0 sqrt       + ] $[ 2.0 sqrt     ] }
        { $[ 2.0 2.0 sqrt 2.0 * + ]    0.0            }
        { $[ 2.0 2.0 sqrt       + ] $[ 2.0 sqrt neg ] }
        { $[     2.0 sqrt         ] $[ 2.0 sqrt neg ] }
    } ;

M: tile-hexagon (shape-vertices)
    drop {
        {    0.0    0.0            }
        {    1.0 $[ 3.0 sqrt     ] }
        {    3.0 $[ 3.0 sqrt     ] }
        {    4.0    0.0            }
        {    3.0 $[ 3.0 sqrt neg ] }
        {    1.0 $[ 3.0 sqrt neg ] }
    } ;

M: tile-trapezoid (shape-vertices)
    drop {
        {    0.0    0.0        }
        {    1.0 $[ 3.0 sqrt ] }
        {    3.0 $[ 3.0 sqrt ] }
        {    4.0    0.0        }
        {    2.0    0.0        }
    } ;

M: tile-square (shape-vertices)
    drop {
        { 0.0 0.0 }
        { 0.0 2.0 }
        { 2.0 2.0 }
        { 2.0 0.0 }
    } ;

M: tile-rhombus-60 (shape-vertices)
    drop {
        {    0.0    0.0        }
        {    1.0 $[ 3.0 sqrt ] }
        {    3.0 $[ 3.0 sqrt ] }
        {    2.0    0.0        }
    } ;

M: tile-rhombus-45 (shape-vertices)
    drop {
        {    0.0                 0.0        }
        { $[     2.0 sqrt   ] $[ 2.0 sqrt ] }
        { $[ 2.0 2.0 sqrt + ] $[ 2.0 sqrt ] }
        {    2.0                 0.0        }
    } ;

M: tile-rhombus-30 (shape-vertices)
    drop {
        { 0.0    0.0              }
        { 0.0    2.0              }
        { 1.0 $[ 2.0 3.0 sqrt + ] }
        { 1.0 $[     3.0 sqrt   ] }
    } ;

M: tile-triangle (shape-vertices)
    drop {
        {    0.0    0.0        }
        {    1.0 $[ 3.0 sqrt ] }
        {    2.0    0.0        }
    } ;

: shape-vertices ( shape -- vertices )
    [ transform>> ] [ (shape-vertices) ] bi
    [ a.v ] with map ;

GENERIC: actor-radius ( thing -- radius )

M: mini actor-radius drop 0.35 ;
M: mega actor-radius drop 2.6 ;
M: bumper actor-radius drop 0.5 ;
M: player actor-radius drop 0.5 ;
M: powerup actor-radius drop 0.5 ;

GENERIC: thing-extents ( thing -- min max )

M: shape thing-extents
    shape-vertices [ vmin* ] [ vmax* ] bi ;
M: actor thing-extents
    [ transform>> origin>> ] [ actor-radius dup 2array ] bi
    [ v- ] [ v+ ] 2bi ;
M: line thing-extents
    endpoints>> [ vmin* ] [ vmax* ] bi ;
M: wallpost thing-extents
    center>> dup ;

: vertices>edges ( vertices -- edges )
    dup rotate zip ;

: edges>vertices ( edges -- vertices )
    concat prune ; 
