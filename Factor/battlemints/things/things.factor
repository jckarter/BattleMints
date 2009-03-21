USING: accessors arrays assocs constructors fry generic generic.parser
kernel literals math math.affine-transforms math.functions math.vectors
sequences sets quadtrees words ;
IN: battlemints.things

! utility words
: rotate ( sequence -- equences ) 1 cut-slice prepend ;
! end utility words

TUPLE: thing spawn? label ; 

TUPLE: transform-thing < thing transform ;

TUPLE: shape < transform-thing ;
TUPLE: tile < shape ;
TUPLE: actor < transform-thing ;

TUPLE: point < transform-thing ;

TUPLE: pellet < actor ;

TUPLE: tile-shell < transform-thing vertex-start vertex-length ;
TUPLE: tile-vertices < thing vertices ;

CONSTRUCTOR: tile-shell ( vertex-start vertex-length transform -- tile-shell ) ;
CONSTRUCTOR: tile-vertices ( vertices -- tile-vertices ) ;

TUPLE: tile-vertex vertex texcoord ;

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

TUPLE: line < transform-thing endpoints ;

TUPLE: tripwire < line ;
TUPLE: alarm < tripwire ;
TUPLE: goal < tripwire next-board ;

TUPLE: wall < line ;
TUPLE: door < wall ;

TUPLE: wallpost < point ;

TUPLE: switch < transform-thing ;
TUPLE: sign < transform-thing signface ;

GENERIC: (shape-vertices) ( tile -- vertices )

M: arrow (shape-vertices)
    drop {
        {  0.5  0.0 }
        {  0.0 -0.5 }
        {  0.0 -0.3 }
        { -0.5 -0.3 }
        { -0.5  0.3 }
        {  0.0  0.3 }
        {  0.0  0.5 }
    } ;

M: tile-octagon (shape-vertices)
    drop {
        {    0.0                        0.0              }
        {    0.0                       -2.0              }
        { $[     2.0 sqrt         ] $[ -2.0 2.0 sqrt - ] }
        { $[ 2.0 2.0 sqrt       + ] $[ -2.0 2.0 sqrt - ] }
        { $[ 2.0 2.0 sqrt 2.0 * + ]    -2.0              }
        { $[ 2.0 2.0 sqrt 2.0 * + ]     0.0              }
        { $[ 2.0 2.0 sqrt       + ] $[      2.0 sqrt   ] }
        { $[     2.0 sqrt         ] $[      2.0 sqrt   ] }
    } ;

M: tile-hexagon-90 (shape-vertices)
    drop {
        {    0.0                       0.0            }
        { $[     2.0 sqrt         ] $[ 2.0 sqrt neg ] }
        { $[ 2.0 2.0 sqrt       + ] $[ 2.0 sqrt neg ] }
        { $[ 2.0 2.0 sqrt 2.0 * + ]    0.0            }
        { $[ 2.0 2.0 sqrt       + ] $[ 2.0 sqrt     ] }
        { $[     2.0 sqrt         ] $[ 2.0 sqrt     ] }
    } ;

M: tile-hexagon (shape-vertices)
    drop {
        {    0.0    0.0            }
        {    1.0 $[ 3.0 sqrt neg ] }
        {    3.0 $[ 3.0 sqrt neg ] }
        {    4.0    0.0            }
        {    3.0 $[ 3.0 sqrt     ] }
        {    1.0 $[ 3.0 sqrt     ] }
    } ;

M: tile-trapezoid (shape-vertices)
    drop {
        {    0.0    0.0            }
        {    1.0 $[ 3.0 sqrt     ] }
        {    3.0 $[ 3.0 sqrt     ] }
        {    4.0    0.0            }
        {    2.0    0.0            }
    } ;

M: tile-square (shape-vertices)
    drop {
        { 0.0  0.0 }
        { 0.0  2.0 }
        { 2.0  2.0 }
        { 2.0  0.0 }
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

GENERIC: shape-texcoords ( shape -- texcoords )

M: arrow shape-texcoords
    drop {
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
    } ;

M: tile-octagon shape-texcoords
    drop {
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
    } ;

M: tile-hexagon-90 shape-texcoords
    drop {
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
        { 0.0 0.0 }
    } ;

M: tile-hexagon shape-texcoords
    drop {
        { 0.1016 0.7497 }
        { 0.2461 1.0000 }
        { 0.5352 1.0000 }
        { 0.6797 0.7497 }
        { 0.5352 0.4993 }
        { 0.2461 0.4993 }
    } ;

M: tile-trapezoid shape-texcoords
    drop {
        { 0.8359 0.6484 }
        { 0.5859 0.5041 }
        { 0.5859 0.2154 }
        { 0.8359 0.0711 }
        { 0.8359 0.3598 }
    } ;

M: tile-square shape-texcoords
    drop {
        { 0.2500 0.4922 }
        { 0.5391 0.4922 }
        { 0.5391 0.2031 }
        { 0.2500 0.4922 }
    } ;

M: tile-rhombus-60 shape-texcoords
    drop {
        { 0.5426 1.0000 }
        { 0.6872 0.7495 }
        { 0.9766 0.7495 }
        { 0.8319 1.0000 }
    } ;

M: tile-rhombus-45 shape-texcoords
    drop {
        { 0.2518 0.2031 }
        { 0.4549 0.0000 }
        { 0.7422 0.0000 }
        { 0.5391 0.2031 }
    } ;

M: tile-rhombus-30 shape-texcoords
    drop {
        { 0.8438 0.6484 }
        { 0.8438 0.3516 }
        { 0.9922 0.0945 }
        { 0.9922 0.3913 }
    } ;

! M 0,144.33123 L 249.9964,1.4108293e-13 L 250,288.6688 L 0,144.33123 z
M: tile-triangle shape-texcoords
    drop {
        { 0.0000 0.1443 }
        { 0.2500 0.0000 }
        { 0.2500 0.2887 }
    } ;

GENERIC: (shape-center) ( tile -- center )

M: arrow (shape-center)
    drop { 0.0 0.0 } ;
M: tile-octagon (shape-center)
    drop { $[ 1.0 2.0 sqrt + ] -1.0 } ;
M: tile-hexagon-90 (shape-center)
    drop { $[ 1.0 2.0 sqrt + ] 0.0 } ;
M: tile-hexagon (shape-center)
    drop { 2.0 0.0 } ;
M: tile-trapezoid (shape-center)
    drop { 2.0 $[ 3.0 sqrt 0.5 * ] } ;
M: tile-square (shape-center)
    drop { 1.0 -1.0 } ;
M: tile-rhombus-60 (shape-center)
    drop { 1.5 $[ 3.0 sqrt 0.5 * ] } ;
M: tile-rhombus-45 (shape-center)
    drop { $[ 1.0 0.5 sqrt + ] $[ 0.5 sqrt ] } ;
M: tile-rhombus-30 (shape-center)
    drop { 0.5 $[ 1.0 0.75 sqrt + ] } ;
M: tile-triangle (shape-center)
    drop { 1.0 $[ 1.0 3.0 sqrt / ] } ;

: shape-center ( shape -- center )
    [ transform>> ] [ (shape-center) ] bi a.v ;

GENERIC: shape-color ( thing -- color )

M: arrow shape-color
    drop { 0.28 0.24 0.22 0.5 } ;
M: tile-octagon shape-color
    drop { 0.96 0.84 0.93 1.0 } ;
M: tile-hexagon-90 shape-color
    drop { 0.78 0.71 0.75 1.0 } ;
M: tile-hexagon shape-color
    drop { 1.0  0.96 0.84 1.0 } ;
M: tile-trapezoid shape-color
    drop { 1.0  0.84 0.84 1.0 } ;
M: tile-square shape-color
    drop { 1.0  0.90 0.84 1.0 } ;
M: tile-rhombus-60 shape-color
    drop { 0.84 0.84 1.0  1.0 } ;
M: tile-rhombus-45 shape-color
    drop { 1.0  0.66 0.8  1.0 } ;
M: tile-rhombus-30 shape-color
    drop { 0.96 0.89 0.84 1.0 } ;
M: tile-triangle shape-color
    drop { 0.84 1.0  0.84 1.0 } ;

GENERIC: actor-radius ( thing -- radius )
    
M: mini actor-radius drop 0.35 ;
M: mega actor-radius drop 2.6 ;
M: bumper actor-radius drop 0.5 ;
M: player actor-radius drop 0.5 ;
M: powerup actor-radius drop 0.5 ;
M: pellet actor-radius drop 0.0 ;

GENERIC: thing-extents ( thing -- min max )

M: object thing-extents
    transform>> origin>> dup ;

M: shape thing-extents
    shape-vertices [ vinfimum ] [ vsupremum ] bi ;
M: actor thing-extents
    [ transform>> origin>> ] [ actor-radius dup 2array ] bi
    [ v- ] [ v+ ] 2bi ;
M: line thing-extents
    endpoints>> [ vinfimum ] [ vsupremum ] bi ;
M: point thing-extents
    transform>> origin>> dup ;
M: tile-shell thing-extents
    transform>> origin>> dup ;
M: tile-vertices thing-extents
    vertices>> [ vertex>> ] map [ vinfimum ] [ vsupremum ] bi ;

M: sign thing-extents
    transform>> origin>> { 0.7 0.7 }
    [ v- ] [ v+ ] 2bi ;

: vertices>edges ( vertices -- edges )
    dup rotate zip ;

: edges>vertices ( edges -- vertices )
    concat prune ;

