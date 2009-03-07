USING: accessors arrays assocs battlemints.things classes combinators
combinators.short-circuit hashtables io.encodings.utf8 io.files
json.writer kernel literals math math.affine-transforms fry
math.functions math.vectors memoize sequences sequences.squish
svg words xml xml.data xml.syntax xml.traversal sorting quadtrees vectors
math.rectangles io.pathnames ;
IN: battlemints.board-compiler

XML-NS: battlemints-name com.duriansoftware.BattleMints.board

CONSTANT: map-layer-label "Map"

CONSTANT: source-path "/Users/joe/Documents/Code/BattleMints/Board Sources"
CONSTANT: dest-path "/Users/joe/Documents/Code/BattleMints"

: source-file ( filename -- path ) source-path prepend-path ;
: dest-file ( filename -- path ) dest-path prepend-path ;

: board ( basename -- source dest )
    [ ".svg" append source-file ] [ ".board" append dest-file ] bi ;

: id>class ( id -- word )
    "battlemints.things" lookup ;

: href>class ( href -- word )
    dup first CHAR: # = [ rest id>class ] [ "<use> href must be #id form" throw ] if ;

GENERIC: (thing-rep-name) ( thing -- name )
GENERIC: (thing-rep) ( thing -- assoc )

: v>>k ( assoc v k -- assoc ) pick set-at ;

: common-thing-rep ( assoc thing -- assoc )
    [ label>> [ "label" v>>k ] when* ]
    [ spawn?>> [ t "spawn" v>>k ] when ] bi ;

: thing-rep ( thing -- array )
    [ (thing-rep-name) ] [ (thing-rep) ] [ common-thing-rep ] tri 2array ;

: zip-hashtable ( vs ks -- hashtable ) swap zip >hashtable ;

M: object (thing-rep-name) class name>> ;
M: tile-shell (thing-rep-name) drop "tile" ;
M: tile-vertices (thing-rep-name) drop "tile_vertices" ;

M: tile-shell (thing-rep)
    [ center>> ] [ vertex-start>> ] [ vertex-length>> ] tri 3array
    { "center" "vertex_start" "vertex_length" } zip-hashtable ;

M: tile-vertices (thing-rep)
    vertices>> [ [ vertex>> ] [ color>> ] bi 2array ] map
    "vertices" associate ;

M: actor (thing-rep)
    transform>> origin>> "center" associate ;

M: line (thing-rep)
    endpoints>> { "endpoint_a" "endpoint_b" } zip-hashtable ;

M: wallpost (thing-rep)
    center>> "center" associate ;

M: sign (thing-rep)
    [ transform>> origin>> ] [ signface>> ] bi 2array
    { "center" "signface" } zip-hashtable ;

M: switch (thing-rep)
    transform>> [ origin>> ] [ axes { 1.0 0.0 } a.v ] bi 2array
    { "center" "axis" } zip-hashtable ;

M: player (thing-rep)
    call-next-method
    t "camera" v>>k ;

M: goal (thing-rep)
    [ call-next-method ] keep
    next-board>> "next_board" v>>k ;

M: powerup (thing-rep)
    [ call-next-method ] keep
    powerup-kind>> "kind" v>>k ;

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
    {
        [
            {
                { [ dup <use>? ] [ (use>thing) ] }
                { [ dup battlemints-<path>? ] [ (path>thing) ] }
                [ drop f ]
            } cond 
        ]
        [ (tag>>thing) ]
        [ "label" battlemints-name attr >>label ]
        [ "spawn" battlemints-name attr >boolean >>spawn? ]
    } cleave ;

M: object (tag>>thing) drop ;

M: goal (tag>>thing)
    "next-board" battlemints-name attr >>next-board ;

M: powerup (tag>>thing)
    "powerup-kind" battlemints-name attr >>powerup-kind ;

M: sign (tag>>thing)
    "signface" battlemints-name attr >>signface ;

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

: tile-edges ( tiles -- edges )
    [ shape-vertices vertices>edges ] map concat ;

CONSTANT: TILE-EDGE-PRECISION 64

: canonicalize-point ( point -- point' )
    [ TILE-EDGE-PRECISION * round TILE-EDGE-PRECISION / ] map ;

: canonicalize-edge ( edge -- edge' )
    [ canonicalize-point ] map natural-sort ;

: inc-edge-count ( edge hash -- )
    [ canonicalize-edge ] dip inc-at ;

: edge-counts ( tiles -- edge-counts )
    tile-edges dup length <hashtable>
    [ [ inc-edge-count ] curry each ] keep ;

: outer-edges ( tiles -- edges )
    edge-counts [ nip 1 = ] assoc-filter keys ;

: edge>wall ( edge -- wall )
    wall new
        swap >>endpoints ;

: vertex>wallpost ( vertex -- wallpost )
    wallpost new
        swap >>center ;

: walls ( tiles -- walls )
    [ tile? ] filter
    outer-edges
    [ [ edge>wall ] map ] [ edges>vertices [ vertex>wallpost ] map ] bi
    append ;

: push-vertices ( into-vertices vertices color -- into-vertices start length )
    [ dup vertices>> ] [ ] [ [ tile-vertex boa ] curry map ] tri*
    [ [ length ] bi@ ] [ swap push-all ] 2bi ;

: make-shell ( vertices tile -- vertices shell )
    [ shape-vertices [ canonicalize-point ] map ]
    [ shape-color push-vertices ]
    [ shape-center ] tri <tile-shell> ;

: board-extents ( things -- extents )
    { 1.0/0.0 1.0/0.0 } { -1.0/0.0 -1.0/0.0 } rot
    [ thing-extents swapd [ vmin ] [ vmax ] 2bi* ] each 2array ;

: shells ( tiles -- shells )
    [ shape-center ] swizzle
    [ 100 <vector> <tile-vertices> ] dip [ make-shell ] map
    swap suffix ;

: process-things ( things -- things' )
    [ shape? ] partition
    [ [ walls ] [ shells ] bi ] dip 3append ;

: svg>board ( svg -- board )
    svg>things process-things
    [ [ thing-rep ] map ] [ board-extents concat ] bi 2array
    { "things" "bounds" } zip-hashtable ;

: svg-file>board-file ( from-filename to-filename -- )
    [ file>xml svg>board >json ]
    [ utf8 set-file-contents ] bi* ;

