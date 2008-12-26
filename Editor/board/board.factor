USING: math.geometry.rect json.reader json.writer assocs kernel fry sequences math.vectors
shuffle cairo cairo.ffi cairo.gadgets macros combinators accessors 
arrays math math.constants alien.c-types ui.gadgets math.order
colors opengl locals combinators.short-circuit ui.gestures generalizations
vectors namespaces ui.tools.inspector ui.gadgets.packs symbols ui
ui.gadgets.labels ui.gadgets.buttons hashtables classes io.encodings.utf8
io.files specialized-arrays.double ;
IN: mint-editor.board

: BORDER-THICKNESS 0.004 ;
: GOAL-THICKNESS 0.1 ;
: HANDLE-RADIUS 0.008 ;
: HANDLE-COLOR 0.0 0.0 0.0 0.5 ;
: HANDLE-SELECT-COLOR 1.0 0.2 0.2 0.5 ;

SYMBOL: board

TUPLE: board-gadget < cairo-gadget
    things
    zoom origin 
    selected-handles
    tool
    inspector ;

: adjusted-for-board-zoom ( r -- r' )
    board get zoom>> * ; inline
: handle-radius ( -- r )
    HANDLE-RADIUS adjusted-for-board-zoom ; inline
: border-thickness ( -- r )
    BORDER-THICKNESS adjusted-for-board-zoom ; inline

TUPLE: wall-strip vertices closed ;
TUPLE: goal endpoint-a endpoint-b next-board ;
TUPLE: sphere center color radius mass spring ;
TUPLE: enemy < sphere accel responsiveness ;
TUPLE: player center ;

! ! !

GENERIC: init-thing ( assoc thing -- thing )

MACRO: keys-to-slots ( pairs -- )
    [ '[ _ swap at @ ] ] { } assoc>map
    '[ swap _ cleave ] ;

MACRO: slots-to-keys ( pairs -- )
    [ length ]
    [ [ swap '[ [ @ _ ] dip set-at ] ] { } assoc>map ] bi
    '[ _ <hashtable> [ _ 2cleave ] keep ] ;

M: wall-strip init-thing
    {
        { "vertices" [ >vector >>vertices ] }
        { "closed"   [ >>closed ] }
    } keys-to-slots ;

M: goal init-thing
    {
        { "endpoint_a" [ >>endpoint-a ] }
        { "endpoint_b" [ >>endpoint-b ] }
        { "next_board" [ >>next-board ] }
    } keys-to-slots ;

M: sphere init-thing
    {
        { "center" [ >>center ] }
        { "color"  [ first4 <rgba> >>color ] }
        { "radius" [ >>radius ] }
        { "mass"   [ >>mass   ] }
        { "spring" [ >>spring ] }
    } keys-to-slots ;

M: enemy init-thing
    {
        { "center" [ >>center ] }
        { "color"  [ first4 <rgba> >>color ] }
        { "radius" [ >>radius ] }
        { "mass"   [ >>mass   ] }
        { "spring" [ >>spring ] }
        { "accel"  [ >>accel  ] }
        { "responsiveness" [ >>responsiveness ] }
    } keys-to-slots ;

M: player init-thing
    {
        { "center" [ >>center ] }
    } keys-to-slots ;

: new-thing ( hash class -- thing )
    new init-thing ;

GENERIC: thing>assoc ( thing -- assoc )

M: wall-strip thing>assoc
    {
        { "vertices" [ vertices>> ] }
        { "closed" [ closed>> ] }
    } slots-to-keys ;
M: goal thing>assoc
    {
        { "endpoint_a" [ endpoint-a>> ] }
        { "endpoint_b" [ endpoint-b>> ] }
        { "next_board" [ next-board>> ] }
    } slots-to-keys ;
M: sphere thing>assoc
    {
        { "center" [ center>> ] }
        { "color"  [ color>> color>raw 4array ] }
        { "radius" [ radius>> ] }
        { "mass"   [ mass>>   ] }
        { "spring" [ spring>> ] }
    } slots-to-keys ;
M: enemy thing>assoc
    {
        { "center" [ center>> ] }
        { "color"  [ color>> color>raw 4array ] }
        { "radius" [ radius>> ] }
        { "mass"   [ mass>>   ] }
        { "spring" [ spring>> ] }
        { "accel"  [ accel>>  ] }
        { "responsiveness" [ responsiveness>> ] }
    } slots-to-keys ;
M: player thing>assoc
    {
        { "center" [ center>> ] }
        { "camera" [ drop t ] }
    } slots-to-keys ;

GENERIC: deletable? ( thing -- ? )

M: object deletable? drop t ;
M: player deletable? drop f ;

GENERIC: extents ( things -- min max )

M: wall-strip extents
    vertices>>
    [ {  1.0/0.0  1.0/0.0 } [ vmin ] reduce ]
    [ { -1.0/0.0 -1.0/0.0 } [ vmax ] reduce ] bi ;
M: goal extents
    [ endpoint-a>> ] [ endpoint-b>> ] bi [ vmin ] [ vmax ] 2bi ;
M: sphere extents
    [ center>> ] [ radius>> dup 2array ] bi [ v- ] [ v+ ] 2bi ;
M: player extents
    center>> dup ;

M: sequence extents
    { 1.0/0.0 1.0/0.0 } { -1.0/0.0 -1.0/0.0 } rot
    [ extents swapd [ vmin ] [ vmax ] 2bi* ] each ;

! ! !

: thing-class-names
    H{
        { "player" player }
        { "wall_strip" wall-strip }
        { "goal"   goal }
        { "sphere" sphere }
        { "enemy"  enemy }
    } ;

: string>thing-class ( string -- class ) thing-class-names at ;
: thing-class>string ( class -- string ) thing-class-names value-at ;

! ! !

TUPLE: handle-base thing # point ;

TUPLE: center-handle < handle-base ;
TUPLE: radius-handle < handle-base ;
TUPLE: endpoint-a-handle < handle-base ;
TUPLE: endpoint-b-handle < handle-base ;
TUPLE: vertex-handle < handle-base ;

GENERIC: handles ( thing -- sequence )

M: handle-base equal?
    over handle-base?
    [ { [ [ thing>> ] bi@ eq? ] [ [ point>> ] bi@ = ] } 2&& ]
    [ 2drop f ] if ; 

M: wall-strip handles
    dup vertices>> [ rot vertex-handle boa ] with map-index ;

M: goal handles
    {
        [ 0 over endpoint-a>> endpoint-a-handle boa ]
        [ 1 over endpoint-b>> endpoint-b-handle boa ]
    } cleave 2array ;

M: sphere handles
    {
        [ 0 over center>> center-handle boa ]
        [ 1 over [ center>> ] [ radius>> 0 2array ] bi v+ radius-handle boa ]
    } cleave 2array ;

M: player handles
    0 over center>> center-handle boa 1array ;

: update-handle ( handle -- handle' )
    [ #>> ] [ thing>> handles ] bi nth ;

! ! !

GENERIC# drag-handle 1 ( delta handle thing -- )

M:: center-handle drag-handle ( delta handle thing -- )
    thing  [ delta v+ ] change-center drop
    handle [ delta v+ ] change-point drop ;

M:: radius-handle drag-handle ( delta handle thing -- )
    thing  [ delta first + ] change-radius drop
    handle [ delta { 1 0 } v* v+ ] change-point drop ;

M:: endpoint-a-handle drag-handle ( delta handle thing -- )
    thing  [ delta v+ ] change-endpoint-a drop
    handle [ delta v+ ] change-point drop ;

M:: endpoint-b-handle drag-handle ( delta handle thing -- )
    thing  [ delta v+ ] change-endpoint-b drop
    handle [ delta v+ ] change-point drop ;

M:: vertex-handle drag-handle ( delta handle thing -- )
    handle #>> thing vertices>> [ delta v+ ] change-nth
    handle [ delta v+ ] change-point drop ;

! ! !

GENERIC: draw ( thing -- )

: text-extents ( string -- extents )
    cr swap "cairo_text_extents_t" <c-object> [ cairo_text_extents ] keep ;

: extents-offset-to-center ( extents -- offset )
    [ [ cairo_text_extents_t-width     ] [ cairo_text_extents_t-height    ] bi 2array 0.5 v*n ]
    [ [ cairo_text_extents_t-x_bearing ] [ cairo_text_extents_t-y_bearing ] bi 2array ] bi
    v+ vneg ;

:: draw-text-centered ( center string -- )
    cr center first2 cairo_move_to
    cr cairo_save
    cr 0.08 -0.08 cairo_scale
    cr string text-extents extents-offset-to-center first2 cairo_rel_move_to
    cr string cairo_show_text
    cr cairo_restore
    cr cairo_new_path ;

: path-strip ( vertices -- )
    [ first cr swap first2 cairo_move_to ]
    [ rest [ cr swap first2 cairo_line_to ] each ] bi ;

M: wall-strip draw
    [ vertices>> path-strip ]
    [ closed>> [ cr cairo_close_path ] when ] bi
    cr border-thickness cairo_set_line_width
    cr 0.0 0.0 0.0 1.0 cairo_set_source_rgba
    cr cairo_stroke ;

: path-line ( thing -- )
    cr swap
    [ endpoint-a>> first2 cairo_move_to ]
    [ endpoint-b>> first2 cairo_line_to ] 2bi ;

: perpendicular ( vec -- vec* ) reverse { -1.0 1.0 } v* ;

: line-normal ( line -- normal )
    [ endpoint-b>> ] [ endpoint-a>> ] bi v-
    dup { 0.0 0.0 } = 
    [ drop { 0.0 1.0 } ]
    [ perpendicular normalize ] if ;

:: draw-goal-row ( goal line-offset dash-offset -- )
    cr cairo_save
    cr goal line-normal GOAL-THICKNESS line-offset * v*n first2 cairo_translate
    cr GOAL-THICKNESS cairo_set_line_width
    cr GOAL-THICKNESS dup double-array{ } 2sequence underlying>> 2 dash-offset cairo_set_dash
    goal path-line
    cr cairo_stroke
    cr cairo_restore ;

M: goal draw
    [ -0.5 0.0            draw-goal-row ]
    [  0.5 GOAL-THICKNESS draw-goal-row ] bi ;

M: sphere draw
    cr swap
    [ [ center>> first2 ] [ radius>> ] bi 0 2 pi * cairo_arc ] 
    [ color>> color>raw cairo_set_source_rgba ] 2bi
    cr cairo_fill_preserve
    cr border-thickness cairo_set_line_width
    cr 0.0 0.0 0.0 1.0 cairo_set_source_rgba
    cr cairo_stroke ;

M: enemy draw
    dup call-next-method
    center>> "E" draw-text-centered ;

M: player draw
    cr 0.0 0.0 0.0 1.0 cairo_set_source_rgba
    center>>
    [ "P" draw-text-centered ]
    [
        cr swap first2 0.5 0 2 pi * cairo_arc
        cr cairo_stroke
    ] bi ;

M: handle-base draw
    cr swap point>> first2 handle-radius 0 2 pi * cairo_arc
    cr HANDLE-COLOR cairo_set_source_rgba
    cr cairo_fill ;

: draw-selected ( handle -- )
    cr swap point>> first2 handle-radius border-thickness + 0 2 pi * cairo_arc
    cr border-thickness cairo_set_line_width
    cr HANDLE-SELECT-COLOR cairo_set_source_rgba
    cr cairo_stroke ;

! ! !

SINGLETONS:
    drag-tool
    wall-tool goal-tool sphere-tool enemy-tool
    wall-join-tool wall-split-tool
    delete-tool clone-tool ;

M: board-gadget handles
    things>> [ handles ] map concat ;

: <board-gadget> ( -- gadget )
    board-gadget new-gadget
        12.0 >>zoom 
        { 0.0 0.0 } >>origin
        V{ } clone >>selected-handles
        drag-tool >>tool ;

! ! !

: assoc>board ( assoc -- board )
    <board-gadget>
    "things" rot at [ first2 swap string>thing-class new-thing ] V{ } map-as
    >>things ;

: (thing>board-assoc-entry) ( thing -- sequence )
    [ class thing-class>string ] [ thing>assoc ] bi 2array ;

: board>assoc ( board -- assoc )
    things>> 2 <hashtable>
    [ [ extents append "bounds" ] dip set-at ]
    [ [ [ (thing>board-assoc-entry) ] { } map-as "things" ] dip set-at ]
    [ nip ] 2tri ;

: <fresh-board> ( -- gadget )
    <board-gadget>
        V{
            {  -6.0  -6.0 }
            {  -6.0   6.0 }
            {   6.0   6.0 }
            {   6.0  -6.0 }
        } clone t wall-strip boa 
        {  -2.5   0.0 } 0.7 1.0 1.0 1.0 <rgba> 1.0 1.0 1.0 sphere boa
        {   0.0   0.0 } player boa
        3array >vector >>things ;

: save-board ( board filename -- )
    [ board>assoc >json ] dip utf8 set-file-contents ;

: <board-top-row> ( board filename -- pack )
    drop
    <inspector-gadget> [ >>inspector ] keep 2array
    <shelf>
        1 >>fill
        swap add-gadgets ;

: (board-tool) ( board tool label -- button )
    [ '[ drop _ _ >>tool drop ] ] [ <label> ] bi* swap <bevel-button> ;

: <board-bottom-row> ( board filename -- pack )
    [
        drop {
            { drag-tool "drag" }
            { wall-tool "wall" }
            { goal-tool "goal" }
            { sphere-tool "sphere" }
            { enemy-tool "enemy" }
            { wall-join-tool "join walls" }
            { wall-split-tool "split walls" }
            { clone-tool "clone" }
            { delete-tool "delete" }
        } [ first2 (board-tool) ] with map
    ] [ '[ drop _ _ save-board ] "save" swap <bevel-button> suffix ] 2bi
    <shelf> swap add-gadgets ;

: <board-controls> ( board filename -- blob )
    <pile> -rot
    [ <board-top-row> add-gadget ] [ <board-bottom-row> add-gadget ] 2bi ;

: new-board ( filename -- )
    [ <fresh-board> swap <board-controls> ] [ open-window ] bi ;

: open-board ( filename -- )
    [ utf8 file-contents json> assoc>board ] [ <board-controls> ]
    [ open-window ] tri ;

! ! !

: board-scale ( gadget -- scale )
    [ dim>> first2 min ] [ zoom>> ] bi / { 1.0 -1.0 } n*v ;
: board-translate ( gadget -- translate )
    dim>> 0.5 v*n ;

: set-board-transform ( gadget -- )
    cr cairo_identity_matrix
    [ cr swap board-translate first2 cairo_translate ]
    [ cr swap board-scale first2 cairo_scale ] 
    [ cr swap origin>> -1 v*n first2 cairo_translate ] tri ;

: draw-board ( board -- )
    {
        [ set-board-transform ]
        [ things>> [ draw ] each ]
        [ handles [ draw ] each ]
        [ selected-handles>> [ update-handle draw-selected ] each ]
    } cleave ;

M: board-gadget render-cairo*
    dup board [ draw-board ] with-variable ;

M: board-gadget pref-dim*
    drop { 600 600 } ;

: user-space-click-loc ( gadget -- point )
    {
        [ hand-click-rel ] 
        [ board-translate v- ] 
        [ board-scale v/ ]
        [ origin>> v+ ]
    } cleave ;

: find-handle ( pt handles -- handle/f )
    [ update-handle point>> v- norm-sq handle-radius sq <= ] with find nip ;

: ?inspect-object ( object x x inspector/f -- )
    [ inspect-object ] [ 3drop ] if* ;

: inspect-nothing ( board -- )
    f f pick inspector>> inspect-object ;

: inspect-thing ( thing board -- )
    [ f f ] dip inspector>> ?inspect-object ;

: add-selected-handle ( handle gadget -- )
    selected-handles>> push ;
: remove-selected-handle ( handle gadget -- )
    selected-handles>> delete ;

: toggle-handle ( handle gadget -- )
    [ 2dup selected-handles>> member? [ remove-selected-handle ] [ add-selected-handle ] if ]
    [ inspect-nothing ] bi ;

: select-handle ( handle gadget -- )
    [ [ 1vector ] dip (>>selected-handles) ]
    [ [ thing>> ] dip inspect-thing ] 2bi ;

: clear-selected-handles ( gadget -- )
    V{ } clone >>selected-handles inspect-nothing ;

: handle-under-cursor ( board -- handle )
    dup board [ [ user-space-click-loc ] [ handles ] bi <reversed> find-handle ] with-variable ;

! ! !

SYMBOLS: board-last-drag-loc board-dragged? ;

GENERIC: board-button-down ( gadget tool -- )
GENERIC: board-mod-button-down ( gadget tool -- )
GENERIC: board-button-up ( gadget tool -- )
GENERIC: board-mod-button-up ( gadget tool -- )
GENERIC: board-drag ( gadget tool -- )

: drag-handles ( delta handles -- )
    [ dup thing>> drag-handle ] with each ;

: drag-origin ( delta gadget -- )
    swap '[ _ v- ] change-origin drop ;

M: object board-button-down     2drop ;
M: object board-mod-button-down board-button-down ;
M: object board-button-up       2drop ;
M: object board-mod-button-up   board-button-up ;
M: object board-drag            2drop ;

:: board-mouse-scroll ( gadget tool -- )
    gadget [ scroll-direction get second + ] change-zoom relayout-1 ;

: reset-last-drag-loc ( -- )
    { 0.0 0.0 } board-last-drag-loc set-global
    f board-dragged? set-global ;
: update-last-drag-loc ( -- )
    drag-loc board-last-drag-loc set-global
    t board-dragged? set-global ;

: drag-delta ( -- loc )
    drag-loc board-last-drag-loc get v- ;
: dragged? ( -- ? )
    board-dragged? get ;

: user-space-drag-delta ( gadget -- point )
    drag-delta swap board-scale v/ ;

board-gadget H{
    { T{ button-down f f      1 } [ dup tool>> board-button-down     reset-last-drag-loc ] }
    { T{ button-down f { S+ } 1 } [ dup tool>> board-mod-button-down reset-last-drag-loc ] }
    { T{ button-up   f f      1 } [ dup tool>> board-button-up ] }
    { T{ button-up   f { S+ } 1 } [ dup tool>> board-mod-button-up ] }
    { T{ drag        f        1 } [ dup tool>> board-drag     update-last-drag-loc ] }
    { T{ mouse-scroll           } [ dup tool>> board-mouse-scroll ] }
} set-gestures 

:: spawn-thing ( board thing handle# -- )
    thing board things>> push
    handle# thing handles nth board select-handle
    board drag-tool >>tool drop ;

M: drag-tool board-button-down
    drop
    [ dup handle-under-cursor [ swap select-handle ] [ clear-selected-handles ] if* ]
    [ relayout-1 ] bi ;

M: drag-tool board-mod-button-down
    drop [ handle-under-cursor ] [ '[ _ toggle-handle ] when* ] [ relayout-1 ] tri ;

M: drag-tool board-drag
    drop
    [ user-space-drag-delta ]
    [ dup selected-handles>> dup empty? [ drop drag-origin ] [ nip drag-handles ] if ]
    [ relayout-1 ] tri ;

M: wall-tool board-button-down
    drop dup user-space-click-loc dup 2array >vector f wall-strip boa 1 spawn-thing ;

M: goal-tool board-button-down
    drop dup user-space-click-loc dup "" goal boa 1 spawn-thing ;

M: sphere-tool board-button-down
    drop dup user-space-click-loc 1.0 1.0 1.0 1.0 <rgba> 0.0 1.0 1.0 sphere boa 1 spawn-thing ;

M: enemy-tool board-button-down
    drop
    dup user-space-click-loc
    1.0 1.0 1.0 1.0 <rgba>
    0.0
    1.0
    1.0
    0.015
    1.0 enemy boa 1 spawn-thing ;

M: wall-join-tool board-button-down
    drop dup handle-under-cursor [
        [ #>> ] [ thing>> vertices>> ] bi
        dup length 2 > [ delete-nth ] [ 2drop ] if 
        [ clear-selected-handles ] [ relayout-1 ] bi
    ] [ drop ] if* ;

: insert-nth ( elt i seq -- )
    [ 1array ] [ dup ] [ ] tri* replace-slice ;

M:: wall-split-tool board-button-down ( gadget tool -- )
    gadget handle-under-cursor [
        [ [ point>> ] [ #>> 1+ ] [ thing>> vertices>> ] tri insert-nth ]
        [ [ #>> 1+ ] [ thing>> handles ] bi nth gadget select-handle ] bi
        gadget drag-tool >>tool relayout-1
    ] when* ;

: ?delete ( thing seq -- )
    over deletable? [ delete ] [ 2drop ] if ;

M: delete-tool board-button-up
    drop dragged? not [
        {
            [ clear-selected-handles ]
            [ handle-under-cursor ]
            [ things>> '[ thing>> _ ?delete ] when* ]
            [ relayout-1 ]
        } cleave
    ] [ drop ] if ;

M: clone-tool board-button-down
    drop dup handle-under-cursor
    [ dup thing>> dup deletable? [ clone swap #>> spawn-thing ] [ 3drop ] if ]
    [ drop ] if* ;

