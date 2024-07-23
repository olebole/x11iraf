/* Generated by wbuild from "Board.w"
** (generator version $Revision: 2.0 $ of $Date: 93/07/06 16:08:04 $)
*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <X11/Shell.h>
#include "BoardP.h"
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void set_abs_location(
#if NeedFunctionPrototypes
Widget,unsigned  int ,int ,int ,int ,int 
#endif
);
static void resize(
#if NeedFunctionPrototypes
Widget
#endif
);
static XtGeometryResult  query_geometry(
#if NeedFunctionPrototypes
Widget,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
static XtGeometryResult  geometry_manager(
#if NeedFunctionPrototypes
Widget ,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
static void change_managed(
#if NeedFunctionPrototypes
Widget
#endif
);
#define ceil(r) (-(int )(-(r )))


static void generate_location(
#if NeedFunctionPrototypes
Widget
#endif
);
static void get_core_geometry(
#if NeedFunctionPrototypes
Widget,Position *,Position *,Dimension *,Dimension *
#endif
);
static void set_location(
#if NeedFunctionPrototypes
Widget,unsigned  int 
#endif
);
#define skip_blanks(s) while (isspace (*s ))s ++


static char * scan(
#if NeedFunctionPrototypes
char *,Position *,float *
#endif
);
static void interpret_location(
#if NeedFunctionPrototypes
Widget
#endif
);
/*ARGSUSED*/static void generate_location(self)Widget self;
{
    char tmp[100];

    (void) sprintf(tmp, "%d+%f %d+%f %d+%f %d+%f",
		   ((XfwfBoardWidget)self)->xfwfBoard.abs_x, ((XfwfBoardWidget)self)->xfwfBoard.rel_x, ((XfwfBoardWidget)self)->xfwfBoard.abs_y, ((XfwfBoardWidget)self)->xfwfBoard.rel_y, ((XfwfBoardWidget)self)->xfwfBoard.abs_width, ((XfwfBoardWidget)self)->xfwfBoard.rel_width,
		   ((XfwfBoardWidget)self)->xfwfBoard.abs_height, ((XfwfBoardWidget)self)->xfwfBoard.rel_height);
    XtFree(((XfwfBoardWidget)self)->xfwfBoard.location);
    ((XfwfBoardWidget)self)->xfwfBoard.location = XtNewString(tmp);
}
/*ARGSUSED*/static void get_core_geometry(self,x,y,width,height)Widget self;Position * x;Position * y;Dimension * width;Dimension * height;
{
    Widget parent;
    Position px, py;
    Dimension pw, ph;
    float h;

    parent = ((XfwfBoardWidget)self)->core.parent;
    if (XtIsSubclass(((XfwfBoardWidget)self)->core.parent, xfwfBoardWidgetClass))
	((XfwfBoardWidgetClass)parent->core.widget_class)->xfwfCommon_class.compute_inside(parent, &px, &py, &pw, &ph);
    else {
	px = 0;
	py = 0;
	pw = ((XfwfBoardWidget)parent)->core.width;
	ph = ((XfwfBoardWidget)parent)->core.height;
    }

    *x = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_x * pw + ((XfwfBoardWidget)self)->xfwfBoard.abs_x * ((XfwfBoardWidget)self)->xfwfBoard.hunit) + px;
    *y = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_y * ph + ((XfwfBoardWidget)self)->xfwfBoard.abs_y * ((XfwfBoardWidget)self)->xfwfBoard.vunit) + py;
    h = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_width * pw + ((XfwfBoardWidget)self)->xfwfBoard.abs_width * ((XfwfBoardWidget)self)->xfwfBoard.hunit);
    *width = h < 1.0 ? 1 : h;
    h = ceil(((XfwfBoardWidget)self)->xfwfBoard.rel_height * ph + ((XfwfBoardWidget)self)->xfwfBoard.abs_height * ((XfwfBoardWidget)self)->xfwfBoard.vunit);
    *height = h < 1.0 ? 1 : h;
}
/*ARGSUSED*/static void set_location(self,flags)Widget self;unsigned  int  flags;
{
    Widget parent;
    Position px, py;
    Dimension pw, ph;

    parent = ((XfwfBoardWidget)self)->core.parent;
    if (XtIsSubclass(((XfwfBoardWidget)self)->core.parent, xfwfBoardWidgetClass))
	((XfwfBoardWidgetClass)parent->core.widget_class)->xfwfCommon_class.compute_inside(parent, &px, &py, &pw, &ph);
    else {
	px = 0;
	py = 0;
	pw = ((XfwfBoardWidget)parent)->core.width;
	ph = ((XfwfBoardWidget)parent)->core.height;
    }
    if (flags & CWX) {
	((XfwfBoardWidget)self)->xfwfBoard.rel_x = 0.0;
	((XfwfBoardWidget)self)->xfwfBoard.abs_x = ceil((((XfwfBoardWidget)self)->core.x - px)/((XfwfBoardWidget)self)->xfwfBoard.hunit);
    }
    if (flags & CWY) {
	((XfwfBoardWidget)self)->xfwfBoard.rel_y = 0.0;
	((XfwfBoardWidget)self)->xfwfBoard.abs_y = ceil((((XfwfBoardWidget)self)->core.y - py)/((XfwfBoardWidget)self)->xfwfBoard.vunit);
    }
    if (flags & CWWidth) {
	((XfwfBoardWidget)self)->xfwfBoard.rel_width = 0.0;
	((XfwfBoardWidget)self)->xfwfBoard.abs_width = ceil(((XfwfBoardWidget)self)->core.width/((XfwfBoardWidget)self)->xfwfBoard.hunit);
    }
    if (flags & CWHeight) {
	((XfwfBoardWidget)self)->xfwfBoard.rel_height = 0.0;
        ((XfwfBoardWidget)self)->xfwfBoard.abs_height = ceil(((XfwfBoardWidget)self)->core.height/((XfwfBoardWidget)self)->xfwfBoard.vunit);
    }
}
/*ARGSUSED*/static char * scan(s,absval,relval)char * s;Position * absval;float * relval;
{
    Position n;
    char *t;
    Boolean minus;

    *absval = 0;
    *relval = 0.0;
    n = strtol(s, &t, 0);
    if (*t != '.') {				/* Found an integer */
	*absval = n;
	s = t;
	skip_blanks(s);
	if (*s != '+' && *s != '-') return s;	/* Nothing follows */
	n = strtol(s + 1, &t, 0);
	if (*t != '.') return s;		/* It's not a float */
	minus = (*s == '-');
	*relval = (float) strtod(s + 1, &s);	/* Found a float */
	if (minus) *relval = - *relval;
	return s;
    } else {					/* Found a float */
	*relval = (float) strtod(s, &s);
	skip_blanks(s);
	if (*s != '+' && *s != '-') return s;	/* Nothing follows */
	n = strtol(s + 1, &t, 0);
	if (*t == '.') return s;		/* It's not an integer */
	if (*s == '-') *absval = -n; else *absval = n;
	return t;
    }
}
/*ARGSUSED*/static void interpret_location(self)Widget self;
{
    char *s, *t;

    s = ((XfwfBoardWidget)self)->xfwfBoard.location;
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_x, &((XfwfBoardWidget)self)->xfwfBoard.rel_x);
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_y, &((XfwfBoardWidget)self)->xfwfBoard.rel_y);
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_width, &((XfwfBoardWidget)self)->xfwfBoard.rel_width);
    s = scan(s, &((XfwfBoardWidget)self)->xfwfBoard.abs_height, &((XfwfBoardWidget)self)->xfwfBoard.rel_height);
}

static XtResource resources[] = {
{XtNabs_x,XtCAbs_x,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_x),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_x),XtRImmediate,(XtPointer)0 },
{XtNrel_x,XtCRel_x,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_x),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_x),XtRString,(XtPointer)"0.0"},
{XtNabs_y,XtCAbs_y,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_y),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_y),XtRImmediate,(XtPointer)0 },
{XtNrel_y,XtCRel_y,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_y),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_y),XtRString,(XtPointer)"0.0"},
{XtNx,XtCX,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->core.x),XtOffsetOf(XfwfBoardRec,core.x),XtRImmediate,(XtPointer)MAGICNUM },
{XtNy,XtCY,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->core.y),XtOffsetOf(XfwfBoardRec,core.y),XtRImmediate,(XtPointer)MAGICNUM },
{XtNabs_width,XtCAbs_width,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_width),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_width),XtRImmediate,(XtPointer)0 },
{XtNrel_width,XtCRel_width,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_width),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_width),XtRString,(XtPointer)"1.0"},
{XtNabs_height,XtCAbs_height,XtRPosition,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.abs_height),XtOffsetOf(XfwfBoardRec,xfwfBoard.abs_height),XtRImmediate,(XtPointer)0 },
{XtNrel_height,XtCRel_height,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.rel_height),XtOffsetOf(XfwfBoardRec,xfwfBoard.rel_height),XtRString,(XtPointer)"1.0"},
{XtNwidth,XtCWidth,XtRDimension,sizeof(((XfwfBoardRec*)NULL)->core.width),XtOffsetOf(XfwfBoardRec,core.width),XtRImmediate,(XtPointer)MAGICNUM },
{XtNheight,XtCHeight,XtRDimension,sizeof(((XfwfBoardRec*)NULL)->core.height),XtOffsetOf(XfwfBoardRec,core.height),XtRImmediate,(XtPointer)MAGICNUM },
{XtNhunit,XtCHunit,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.hunit),XtOffsetOf(XfwfBoardRec,xfwfBoard.hunit),XtRString,(XtPointer)"1.0"},
{XtNvunit,XtCVunit,XtRFloat,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.vunit),XtOffsetOf(XfwfBoardRec,xfwfBoard.vunit),XtRString,(XtPointer)"1.0"},
{XtNlocation,XtCLocation,XtRString,sizeof(((XfwfBoardRec*)NULL)->xfwfBoard.location),XtOffsetOf(XfwfBoardRec,xfwfBoard.location),XtRImmediate,(XtPointer)NULL },
};

XfwfBoardClassRec xfwfBoardClassRec = {
{ /* core_class part */
(WidgetClass) &xfwfFrameClassRec,
"Board",
sizeof(XfwfBoardRec),
NULL,
_resolve_inheritance,
FALSE,
initialize,
NULL,
XtInheritRealize,
NULL,
0,
resources,
15,
NULLQUARK,
False ,
FALSE ,
False ,
False ,
NULL,
resize,
XtInheritExpose,
set_values,
NULL,
XtInheritSetValuesAlmost,
NULL,
XtInheritAcceptFocus,
XtVersion,
NULL,
NULL,
query_geometry,
XtInheritDisplayAccelerator,
NULL 
},
{ /* composite_class part */
geometry_manager,
change_managed,
XtInheritInsertChild,
XtInheritDeleteChild,
NULL
},
{ /* XfwfCommon_class part */
XtInherit_compute_inside,
XtInherit_highlight_border,
XtInherit_unhighlight_border,
XtInherit_would_accept_focus,
XtInherit_traverse,
XtInherit_choose_color,
XtInherit_lighter_color,
XtInherit_darker_color,
NULL ,
},
{ /* XfwfFrame_class part */
0
},
{ /* XfwfBoard_class part */
set_abs_location,
},
};
WidgetClass xfwfBoardWidgetClass = (WidgetClass) &xfwfBoardClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfBoardWidgetClass c = (XfwfBoardWidgetClass) class;
  XfwfBoardWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (CompositeClassExtensionRec *)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = (XtPointer)ext;
  if (class == xfwfBoardWidgetClass) return;
  super = (XfwfBoardWidgetClass)class->core_class.superclass;
  if (c->xfwfBoard_class.set_abs_location == XtInherit_set_abs_location)
    c->xfwfBoard_class.set_abs_location = super->xfwfBoard_class.set_abs_location;
}
/*ARGSUSED*/static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
{
    XtWidgetGeometry reply;
    int i;

    if (((XfwfBoardWidget)self)->xfwfBoard.location != ((XfwfBoardWidget)old)->xfwfBoard.location) {
	XtFree(((XfwfBoardWidget)old)->xfwfBoard.location);
	((XfwfBoardWidget)self)->xfwfBoard.location = XtNewString(((XfwfBoardWidget)self)->xfwfBoard.location);
	interpret_location(self);
	get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
    } else if (ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_x*((XfwfBoardWidget)self)->xfwfBoard.hunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_x*((XfwfBoardWidget)old)->xfwfBoard.hunit)
	       || ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_width*((XfwfBoardWidget)self)->xfwfBoard.hunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_width*((XfwfBoardWidget)old)->xfwfBoard.hunit)
	       || ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_y*((XfwfBoardWidget)self)->xfwfBoard.vunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_y*((XfwfBoardWidget)old)->xfwfBoard.vunit)
	       || ceil(((XfwfBoardWidget)self)->xfwfBoard.abs_height*((XfwfBoardWidget)self)->xfwfBoard.vunit) != ceil(((XfwfBoardWidget)old)->xfwfBoard.abs_height*((XfwfBoardWidget)old)->xfwfBoard.vunit)
	       || ((XfwfBoardWidget)self)->xfwfBoard.rel_x != ((XfwfBoardWidget)old)->xfwfBoard.rel_x
	       || ((XfwfBoardWidget)self)->xfwfBoard.rel_y != ((XfwfBoardWidget)old)->xfwfBoard.rel_y
	       || ((XfwfBoardWidget)self)->xfwfBoard.rel_width != ((XfwfBoardWidget)old)->xfwfBoard.rel_width
	       || ((XfwfBoardWidget)self)->xfwfBoard.rel_height != ((XfwfBoardWidget)old)->xfwfBoard.rel_height) {
	get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
	generate_location(self);
    } else if (((XfwfBoardWidget)self)->core.x != ((XfwfBoardWidget)old)->core.x
	       || ((XfwfBoardWidget)self)->core.y != ((XfwfBoardWidget)old)->core.y
	       || ((XfwfBoardWidget)self)->core.width != ((XfwfBoardWidget)old)->core.width
	       || ((XfwfBoardWidget)self)->core.height != ((XfwfBoardWidget)old)->core.height) {
	set_location(self, CWX | CWY | CWWidth | CWHeight);
	generate_location(self);
    }
    if (((XfwfBoardWidget)self)->xfwfCommon.highlightThickness + ((XfwfBoardWidget)self)->xfwfFrame.frameWidth + ((XfwfBoardWidget)self)->xfwfFrame.outerOffset + ((XfwfBoardWidget)self)->xfwfFrame.innerOffset
	!= ((XfwfBoardWidget)old)->xfwfCommon.highlightThickness + ((XfwfBoardWidget)old)->xfwfFrame.frameWidth + ((XfwfBoardWidget)old)->xfwfFrame.outerOffset
	+ ((XfwfBoardWidget)self)->xfwfFrame.innerOffset) {
	for (i = 0; i < ((XfwfBoardWidget)self)->composite.num_children; i++) {
	    (void) XtQueryGeometry(((XfwfBoardWidget)self)->composite.children[i], NULL, &reply);
	    XtConfigureWidget(((XfwfBoardWidget)self)->composite.children[i], reply.x, reply.y, reply.width,
			      reply.height, reply.border_width);
	}
    }
    return False;
}
/*ARGSUSED*/static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
{
    if (((XfwfBoardWidget)self)->xfwfBoard.location != NULL) {
	((XfwfBoardWidget)self)->xfwfBoard.location = XtNewString(((XfwfBoardWidget)self)->xfwfBoard.location);
	interpret_location(self);
	get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
    } else if (((XfwfBoardWidget)self)->core.x != MAGICNUM || ((XfwfBoardWidget)self)->core.y != MAGICNUM
	       || ((XfwfBoardWidget)self)->core.width != MAGICNUM || ((XfwfBoardWidget)self)->core.height != MAGICNUM) {
	set_location(self, CWX | CWY | CWWidth | CWHeight);
	generate_location(self);
    } else {
	generate_location(self);
	get_core_geometry(self, &((XfwfBoardWidget)self)->core.x, &((XfwfBoardWidget)self)->core.y, &((XfwfBoardWidget)self)->core.width, &((XfwfBoardWidget)self)->core.height);
    }
}
/*ARGSUSED*/static void set_abs_location(self,flags,x,y,w,h)Widget self;unsigned  int  flags;int  x;int  y;int  w;int  h;
{
    if (flags & ((CWX | CWY | CWWidth | CWHeight) == 0)) return;
    if (flags & CWX) ((XfwfBoardWidget)self)->core.x = x;
    if (flags & CWY) ((XfwfBoardWidget)self)->core.y = y;
    if (flags & CWWidth) ((XfwfBoardWidget)self)->core.width = w;
    if (flags & CWHeight) ((XfwfBoardWidget)self)->core.height = h;
    set_location(self, flags);
    generate_location(self);
}
/*ARGSUSED*/static void resize(self)Widget self;
{
    int i;
    XtWidgetGeometry reply;
    Widget child;

    for (i = 0; i < ((XfwfBoardWidget)self)->composite.num_children; i++) {
	child = ((XfwfBoardWidget)self)->composite.children[i];
	(void) XtQueryGeometry(child, NULL, &reply);
	XtConfigureWidget(child, reply.x, reply.y, reply.width,
			  reply.height, reply.border_width);
    }
}
/*ARGSUSED*/static XtGeometryResult  query_geometry(self,request,reply)Widget self;XtWidgetGeometry * request;XtWidgetGeometry * reply;
{
    reply->request_mode = CWX | CWY | CWWidth | CWHeight;
    get_core_geometry(self, &reply->x, &reply->y,
		      &reply->width, &reply->height);
    return XtGeometryAlmost;
}
/*ARGSUSED*/static XtGeometryResult  geometry_manager(child,request,reply)Widget  child;XtWidgetGeometry * request;XtWidgetGeometry * reply;
{ Widget self = XtParent(child); {
    Widget self = XtParent(child);
    Dimension wd, ht, bw;
    Position x, y;

    /* Get complete geometry, from request or current value */
    x = request->request_mode & CWX ? request->x : ((XfwfBoardWidget)child)->core.x;
    y = request->request_mode & CWY ? request->y : ((XfwfBoardWidget)child)->core.y;
    wd = request->request_mode & CWWidth ? request->width : ((XfwfBoardWidget)child)->core.width;
    ht = request->request_mode & CWHeight ? request->height : ((XfwfBoardWidget)child)->core.height;
    bw = request->request_mode & CWBorderWidth ? request->border_width
	: ((XfwfBoardWidget)child)->core.border_width;

    XtConfigureWidget(child, x, y, wd, ht, bw);
    return XtGeometryDone;
}
}
/*ARGSUSED*/static void change_managed(self)Widget self;
{
    Widget top = self, w;

    while (! XtIsSubclass(top, shellWidgetClass)) top = XtParent(top) ;
    for (w = self; w != top; w = XtParent(w)) XtInstallAllAccelerators(w, top);
}
