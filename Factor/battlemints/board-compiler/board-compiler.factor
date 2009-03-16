USING: accessors arrays assocs battlemints.things classes combinators
combinators.short-circuit hashtables io io.binary
generalizations io.encodings.binary io.encodings.string io.encodings.utf8 io.files
kernel literals math math.affine-transforms namespaces fry
math.functions math.vectors memoize method-chains sequences sequences.squish
svg words xml xml.data xml.syntax xml.traversal sorting quadtrees vectors
math.rectangles io.pathnames ;
IN: battlemints.board-compiler

! utility
: write-int ( n -- )
    4 >le write ;
: write-pascal-string ( string -- )
    utf8 encode [ length write1 ] [ write ] bi ;
: write-vec2 ( sequence -- )
    first2 [ float>bits write-int ] bi@ ;
: write-vec4 ( sequence -- )
    first4 [ float>bits write-int ] 4 napply ;
! end utility

XML-NS: battlemints-name com.duriansoftware.BattleMints.board

CONSTANT: map-layer-label "Map"

CONSTANT: source-path "/Users/joe/Documents/Code/BattleMints/Board Sources"
CONSTANT: dest-path "/Users/joe/Documents/Code/BattleMints"

CONSTANT: powerup-kinds H{
        { "shield" 0 }
        { "invuln" 1 }
    }

ERROR: bad-powerup-kind-error kind ;

CONSTANT: signfaces H{
        { "mini_xing"  0 }
        { "mega_xing"  1 }
        { "narrows"    2 }
        { "spikes"     3 }
        { "slow"       4 }
        { "stop"       5 }
        { "leftarrow"  6 }
        { "rightarrow" 7 }
    }

ERROR: bad-signface-error kind ;

SYMBOL: labels

: >label ( string -- int )
    [ labels get ?at [ ] [ labels get assoc-size 1+ [ swap labels get set-at ] keep ] if ]
    [ 0 ] if* ;

: source-file ( filename -- path ) source-path prepend-path ;
: dest-file ( filename -- path ) dest-path prepend-path ;

: bb ( basename -- source dest )
    [ ".svg" append source-file ] [ ".bb" append dest-file ] bi ;

: id>class ( id -- word )
    "battlemints.things" lookup ;

: href>class ( href -- word )
    dup first CHAR: # = [ rest id>class ] [ "<use> href must be #id form" throw ] if ;

GENERIC: thing-written-name ( thing -- name )
GENERIC: (write-thing) ( thing -- )

: thing-metaflags ( thing -- int )
    spawn?>> [ 1 ] [ 0 ] if ;

: write-thing ( thing -- )
    [ thing-written-name write-pascal-string ]
    [ thing-metaflags write-int ]
    [ (write-thing) ] tri ;

M: object thing-written-name class name>> ;
M: tile-shell thing-written-name drop "tile" ;
M: tile-vertices thing-written-name drop "tile_vertices" ;

M: tile-vertices (write-thing)
    vertices>>
    [ length>> write-int ]
    [ [ [ vertex>> write-vec2 ] [ color>> write-vec4 ] bi ] each ] bi ;

M: transform-thing (write-thing)
    [ transform>> origin>> write-vec2 ]
    [ label>> >label write-int ] bi ;

AFTER: line (write-thing)
    endpoints>> first2 [ write-vec2 ] bi@ ;

AFTER: goal (write-thing)
    next-board>> write-pascal-string ;

AFTER: tile-shell (write-thing)
    [ vertex-start>> write-int ] [ vertex-length>> write-int ] bi ;

AFTER: powerup (write-thing)
    powerup-kind>> powerup-kinds ?at [ bad-powerup-kind-error ] unless write-int ;

AFTER: sign (write-thing)
    signface>> signfaces ?at [ bad-signface-error ] unless write-int ;

AFTER: switch (write-thing)
    transform>> axes { 1.0 0.0 } a.v write-vec2 ;

: <use>? ( tag -- ? ) "use" svg-name names-match? ;
: <path>? ( tag -- ? ) "path" svg-name names-match? ;
: battlemints-<path>? ( tag -- ? )
    { [ <path>? ] [ "tripwire" battlemints-name attr ] } 1&& ;

GENERIC# (tag>>thing) 1 ( thing tag -- thing )
    
: (use>thing) ( use-tag -- thing )
    [ "href" xlink-name attr href>class new ]
    [ tag-transform >>transform ] bi ;

: endpoints-center ( endpoints -- center )
    [ { 0.0 0.0 } [ v+ ] reduce ] [ length v/n ] bi ;

: (path>thing) ( path-tag -- thing )
    [ "tripwire" battlemints-name attr id>class new ]
    [ tag-d [ p>> ] map [ >>endpoints ] [ endpoints-center <translation> >>transform ] bi ] bi ;

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
        swap [ >>endpoints ] [ endpoints-center <translation> >>transform ] bi ;

: vertex>wallpost ( vertex -- wallpost )
    wallpost new
        swap <translation> >>transform ;

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
    [ shape-center <translation> ] tri <tile-shell> ;

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

TUPLE: board things bounds ;
C: <board> board

: write-board ( board -- )
    H{ } clone labels [
        [ bounds>> first2 [ write-vec2 ] bi@ ]
        [ things>> [ write-thing ] each ] bi
    ] with-variable ;

: svg>board ( svg -- board )
    svg>things process-things dup board-extents <board> ;

: svg-file>board-file ( from-filename to-filename -- )
    [ file>xml svg>board ]
    [ binary [ write-board ] with-file-writer ] bi* ;
