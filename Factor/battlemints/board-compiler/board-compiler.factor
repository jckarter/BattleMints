USING: accessors arrays assocs classes combinators combinators.short-circuit
hashtables json.writer kernel literals math math.affine-transforms
math.functions math.vectors memoize sequences sequences.squish svg words
xml.data xml.utilities battlemints.things sorting ;
IN: battlemints.board-compiler

XML-NS: battlemints-name com.duriansoftware.BattleMints.board

CONSTANT: map-layer-label "Map"

: id>class ( id -- word )
    "battlemints.things" lookup ;

: href>class ( href -- word )
    dup first CHAR: # = [ rest id>class ] [ "<use> href must be #id form" throw ] if ;

GENERIC: thing-rep ( thing -- sequence )

: zip-hashtable ( vs ks -- hashtable ) swap zip >hashtable ;

M: shape thing-rep
    [ class name>> ] [ transform>> flatten-transform "transform" associate ] bi 2array ;

M: actor thing-rep
    [ class name>> ] [ transform>> origin>> "center" associate ] bi 2array ;

M: line thing-rep
    [ class name>> ] [ endpoints>> { "endpoint_a" "endpoint_b" } zip-hashtable ] bi
    2array ;

M: wallpost thing-rep
    [ class name>> ] [ center>> "center" associate ] bi 2array ;

: thing-rep-associate ( thing-rep v k -- thing-rep ) pick second set-at ;

M: player thing-rep
    call-next-method
    t "camera" thing-rep-associate ;

M: goal thing-rep
    [ call-next-method ] keep
    next-board>> "next_board" thing-rep-associate ;

M: powerup thing-rep
    [ call-next-method ] keep
    powerup-kind>> "kind" thing-rep-associate ;

: <use>? ( tag -- ? ) "use" svg-name names-match? ;
: <path>? ( tag -- ? ) "path" svg-name names-match? ;
: battlemints-<path>? ( tag -- ? )
    { [ <path>? ] [ "tripwire" battlemints-name attr ] } 1&& ;

GENERIC# (tag>>thing) 1 ( thing tag -- thing )
    
: (use>thing) ( use-tag -- thing )
    [ "href" xlink-name attr href>class new ]
    [ tag-transform >>transform ] bi ;

: (path>thing) ( path-tag -- thing )
    [ "tripwire" battlemints-name attr id>class new ]
    [ tag-d [ p>> ] map >>endpoints ] bi ;

: tag>thing ( tag -- thing )
    dup {
        { [ dup <use>? ] [ (use>thing) ] }
        { [ dup battlemints-<path>? ] [ (path>thing) ] }
        [ drop f ]
    } cond swap (tag>>thing) ;

M: object (tag>>thing) drop ;

M: goal (tag>>thing)
    "next-board" battlemints-name attr >>next-board ;

M: powerup (tag>>thing)
    "powerup-kind" battlemints-name attr >>powerup-kind ;

: children-tags>things ( tag -- things )
    children-tags [ tag>thing ] map ;

: layer? ( tag -- ? )
    {
        [ "g" svg-name names-match? ]
        [ "groupmode" inkscape-name attr "layer" = ]
    } 1&& ;

: layers ( body -- layers )
    children-tags [ layer? ] filter ; 

: map-layer ( svg -- layer )
    body>> layers [ "label" inkscape-name attr "Map" = ] find nip ;

: svg>things ( svg -- things )
    map-layer children-tags>things ;

: tiles ( things -- tiles )
    [ tile? ] filter ;

: tile-edges ( tiles -- edges )
    [ shape-vertices vertices>edges ] map concat ;

CONSTANT: tile-edge-precision 1000

: canonicalize-edge ( edge -- edge' )
    [ tile-edge-precision v*n [ round ] map tile-edge-precision v/n ] map natural-sort ;

: inc-edge-count ( edge hash -- )
    [ canonicalize-edge ] dip inc-at ;

: edge-counts ( tiles -- edge-counts )
    tile-edges dup length <hashtable>
    [ [ inc-edge-count ] curry each ] keep ;

: outer-edges ( tiles -- edges )
    edge-counts [ nip 1 = ] assoc-filter keys ;

: edge>wall ( edge -- wall )
    wall boa ;

: vertex>wallpost ( vertex -- wallpost )
    wallpost boa ;

: walls ( things -- walls )
    tiles outer-edges
    [ [ edge>wall ] map ] [ edges>vertices [ vertex>wallpost ] map ] bi
    append ;

: board-extents ( things -- extents )
    { 1.0/0.0 1.0/0.0 } { -1.0/0.0 -1.0/0.0 } rot
    [ thing-extents swapd [ vmin ] [ vmax ] 2bi* ] each 2array ;

: svg>board ( svg -- board )
    svg>things dup walls append [ [ thing-rep ] map ] [ board-extents concat ] bi 2array
    { "things" "extents" } zip-hashtable ;
