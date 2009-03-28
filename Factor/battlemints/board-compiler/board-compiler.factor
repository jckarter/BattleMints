USING: accessors arrays assocs battlemints.things classes combinators
combinators.short-circuit hashtables io io.binary
generalizations io.encodings.binary io.encodings.string io.encodings.utf8 io.files
kernel literals math math.affine-transforms namespaces fry math.order
math.functions math.vectors memoize method-chains sequences sequences.squish
svg words xml xml.data xml.syntax xml.traversal sorting quadtrees vectors
math.rectangles io.pathnames unicode.categories splitting grouping math.parser ;
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

CONSTANT: board-format-magic   HEX: BA7713BD
CONSTANT: board-format-version 3

CONSTANT: map-layer-label "Map"

CONSTANT: source-path "/Users/joe/Documents/Code/BattleMints/Board Sources"
CONSTANT: dest-path "/Users/joe/Documents/Code/BattleMints"

CONSTANT: powerup-kinds H{
        { "shield" 0 }
        { "invuln" 1 }
    }

ERROR: bad-powerup-kind-error kind ;

CONSTANT: signfaces H{
        { "mini_xing"       0 }
        { "mega_xing"       1 }
        { "narrows"         2 }
        { "spikes"          3 }
        { "slow"            4 }
        { "stop"            5 }
        { "leftarrow"       6 }
        { "rightarrow"      7 }
        { "arrowup"         8 }
        { "arrowupleft"     9 }
        { "arrowleft"      10 }
        { "arrowdownleft"  11 }
        { "arrowdown"      12 }
        { "arrowdownright" 13 }
        { "arrowright"     14 }
        { "arrowupright"   15 }
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
    [ [ [ vertex>> write-vec2 ] [ texcoord>> write-vec2 ] bi ] each ] bi ;

M: transform-thing (write-thing)
    [ transform>> origin>> write-vec2 ]
    [ label>> >label write-int ] bi ;

AFTER: line (write-thing)
    endpoints>> first2 [ write-vec2 ] bi@ ;

AFTER: goal (write-thing)
    [ next-board>> write-pascal-string ]
    [ goal-number>> write-int ]
    [ achieves-goal?>> 1 0 ? write-int ] tri ;

AFTER: loader (write-thing)
    universe-name>> write-pascal-string ;

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

: background-tag ( svg -- background-tag )
    body>> "background-gradient" get-id ;

: parse-style-string ( string -- assoc )
    ";" split [ ":" split1 [ [ blank? ] trim ] bi@ 2array ] map
    H{ } assoc-like ;

: parse-color ( html-color -- r g b )
    "#" ?head [ "invalid color" throw ] unless
    2 group [ hex> 255.0 /f ] map first3 ;

: stop>color ( stop -- color )
    "style" svg-name attr parse-style-string
    [ "stop-color" swap at parse-color ]
    [ "stop-opacity" swap at string>number >float ] bi 4array ;

: background-gradient ( background-tag -- gradient )
    "stop" svg-name tags-named 2 head
    [ [ "offset" svg-name attr ] bi@ <=> ] sort
    [ stop>color ] map ;

: svg-background ( svg -- gradient )
    background-tag background-gradient ;

: svg-theme ( svg -- theme )
    "theme" battlemints-name attr "" or ;

GENERIC# (tag>>thing) 1 ( thing tag -- thing )

CONSTANT: svg>game-transform T{ affine-transform f
        { 1.0  0.0 }
        { 0.0 -1.0 }
        { 0.0  0.0 }
    }

: svg-a>game-a ( affine-transform -- affine-transform' )
    svg>game-transform swap a.
    [ x>> ] [ y>> -1.0 v*n ] [ origin>> ] tri <affine-transform> ;

: svg-v>game-v ( vector -- vector' )
    T{ affine-transform f { 1.0 0.0 } { 0.0 -1.0 } { 0.0 0.0 } } swap a.v ;
    
: (use>thing) ( use-tag -- thing )
    [ "href" xlink-name attr href>class new ]
    [ tag-transform svg-a>game-a >>transform ] bi ;

: endpoints-center ( endpoints -- center )
    [ { 0.0 0.0 } [ v+ ] reduce ] [ length v/n ] bi ;

: (path>thing) ( path-tag -- thing )
    [ "tripwire" battlemints-name attr id>class new ] [
        tag-d [ p>> svg-v>game-v ] map
        [ >>endpoints ]
        [ endpoints-center <translation> >>transform ] bi
    ] bi ;

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

: string>bool ( string -- bool )
    {
        { "true" [ t ] }
        { "false" [ f ] }
    } case ;

M: goal (tag>>thing)
    [ "next-board" battlemints-name attr >>next-board ]
    [ "goal-number" battlemints-name attr string>number >>goal-number ]
    [ "achieves-goal" battlemints-name attr string>bool >>achieves-goal? ] tri ;

M: loader (tag>>thing)
    "universe-name" battlemints-name attr >>universe-name ;

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

: push-vertices ( into-vertices vertices texcoords -- into-vertices start length )
    [ dup vertices>> ] [ ] [ [ tile-vertex boa ] 2map ] tri*
    [ [ length ] bi@ ] [ swap push-all ] 2bi ;

: make-shell ( vertices tile -- vertices shell )
    [ shape-vertices [ canonicalize-point ] map ]
    [ shape-texcoords push-vertices ]
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

TUPLE: board things bounds background theme ;
C: <board> board

: write-board ( board -- )
    H{ } clone labels [
        board-format-magic write-int
        board-format-version write-int
        {
            [ bounds>> first2 [ write-vec2 ] bi@ ]
            [ theme>> write-pascal-string ]
            [ background>> first2 [ write-vec4 ] bi@ ]
            [ things>> [ write-thing ] each ]
        } cleave
    ] with-variable ;

: svg>board-props ( svg -- background theme )
    [ svg-background ] [ svg-theme ] bi ;

: svg>board ( svg -- board )
    [ svg>things process-things dup board-extents ]
    [ svg>board-props ] bi <board> ;

: svg-file>board-file ( from-filename to-filename -- )
    [ file>xml svg>board ]
    [ binary [ write-board ] with-file-writer ] bi* ;

