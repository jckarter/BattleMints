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
TUPLE: shield-mini < actor ;
TUPLE: mega < actor ;
TUPLE: bumper < actor ;
TUPLE: bomb < actor ;
TUPLE: player < actor ;
TUPLE: durian < actor ;
TUPLE: powerup < actor powerup-kind ;

TUPLE: line < transform-thing endpoints ;

TUPLE: tripwire < line ;
TUPLE: alarm < tripwire ;
TUPLE: goal < tripwire ;
TUPLE: stage-exit < tripwire stage-number ;
TUPLE: world-exit < tripwire world-number ;
TUPLE: loader < tripwire universe-name ;

TUPLE: wall < line ;
TUPLE: door < wall ;

TUPLE: wallpost < point ;

TUPLE: protip < transform-thing text ;

TUPLE: switch < transform-thing ;
TUPLE: heavy-switch < switch ;
TUPLE: eraser < switch universe-name ;
TUPLE: sign < transform-thing signface ;
TUPLE: start-banner < transform-thing ;
TUPLE: battlemints-flag < transform-thing ;

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
        { 0.0078125 0.7421875 }
        { 0.1484375 0.4921875 }
        { 0.4453125 0.4921875 }
        { 0.5859375 0.7421875 }
        { 0.4453125 0.9921875 }
        { 0.1484375 0.9921875 }
    } ;

M: tile-trapezoid shape-texcoords
    drop {
        { 0.0703125 0.46875 }
        { 0.2109375 0.21875 }
        { 0.5078125 0.21875 }
        { 0.6484375 0.46875 }
    } ;

M: tile-square shape-texcoords
    drop {
        { 0.703125 0.9921875 }
        { 0.9921875 0.9921875 }
        { 0.9921875 0.703125 }
        { 0.703125 0.703125 }
    } ;

M: tile-rhombus-60 shape-texcoords
    drop {
        { 0.53125 0.21875 }
        { 0.6796875 0.46875 }
        { 0.96875 0.46875 }
        { 0.8203125 0.21875 }
    } ;

M: tile-rhombus-45 shape-texcoords
    drop {
        { 0.9765625 0.6875 }
        { 0.7734375 0.484375 }
        { 0.484375 0.484375 }
        { 0.6875 0.6875 }
    } ;

M: tile-rhombus-30 shape-texcoords
    drop {
        { 0.390625 0.015625 }
        { 0.6875 0.015625 }
        { 0.9453125 0.1640625 }
        { 0.6484375 0.1640625 }
    } ;

M: tile-triangle shape-texcoords
    drop {
        { 0.1640625 0.2578125 }
        { 0.015625 0.0078125 }
        { 0.3125 0.0078125 }
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
M: durian actor-radius drop 0.35 ;
M: shield-mini actor-radius drop 0.45 ;
M: mega actor-radius drop 2.6 ;
M: bumper actor-radius drop 0.5 ;
M: player actor-radius drop 0.5 ;
M: bomb actor-radius drop 0.35 ;
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

