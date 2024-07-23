/* Generated by wbuild from "Toggle.w"
** (generator version $Revision: 2.0 $ of $Date: 93/07/06 16:08:04 $)
*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Converters.h"
#include "ToggleP.h"
static void toggle(Widget,XEvent*,String*,Cardinal*);
static void switch_on(Widget,XEvent*,String*,Cardinal*);
static void switch_off(Widget,XEvent*,String*,Cardinal*);

static XtActionsRec actionsList[] = {
{"toggle", toggle},
{"switch_on", switch_on},
{"switch_off", switch_off},
};

static char defaultTranslations[] = "\
<Btn1Down>: set_shadow(sunken) \n\
<Btn1Up>: toggle() set_shadow() \n\
<Key>Return: toggle() \n\
";
static void _resolve_inheritance(WidgetClass);
static void initialize(Widget ,Widget,ArgList ,Cardinal *);
static Boolean  set_values(Widget ,Widget ,Widget,ArgList ,Cardinal *);
static void expose(Widget,XEvent *,Region );
static void create_on_gc(Widget);
static void create_off_gc(Widget);

/*ARGSUSED*/static void create_on_gc(self)Widget self;
{
    XtGCMask mask = GCFillStyle;
    XGCValues values;

    if (((XfwfToggleWidget)self)->xfwfToggle.on_gc != NULL) XtReleaseGC(self, ((XfwfToggleWidget)self)->xfwfToggle.on_gc);
    if (((XfwfToggleWidget)self)->xfwfToggle.onIcon && ((XfwfToggleWidget)self)->xfwfToggle.onIcon->pixmap != None) {
	values.tile = ((XfwfToggleWidget)self)->xfwfToggle.onIcon->pixmap;
	mask |= GCTile;
    }
    if (((XfwfToggleWidget)self)->xfwfToggle.onIcon && ((XfwfToggleWidget)self)->xfwfToggle.onIcon->mask != None) {
	values.clip_mask = ((XfwfToggleWidget)self)->xfwfToggle.onIcon->mask;
	mask |= GCClipMask;
    }
    values.fill_style = FillTiled;
    ((XfwfToggleWidget)self)->xfwfToggle.on_gc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/static void create_off_gc(self)Widget self;
{
    XtGCMask mask = GCFillStyle;
    XGCValues values;

    if (((XfwfToggleWidget)self)->xfwfToggle.off_gc != NULL) XtReleaseGC(self, ((XfwfToggleWidget)self)->xfwfToggle.off_gc);
    if (((XfwfToggleWidget)self)->xfwfToggle.offIcon && ((XfwfToggleWidget)self)->xfwfToggle.offIcon->pixmap != None) {
	values.tile = ((XfwfToggleWidget)self)->xfwfToggle.offIcon->pixmap;
	mask |= GCTile;
    }
    if (((XfwfToggleWidget)self)->xfwfToggle.offIcon && ((XfwfToggleWidget)self)->xfwfToggle.offIcon->mask != None) {
	values.clip_mask = ((XfwfToggleWidget)self)->xfwfToggle.offIcon->mask;
	mask |= GCClipMask;
    }
    values.fill_style = FillTiled;
    ((XfwfToggleWidget)self)->xfwfToggle.off_gc = XtGetGC(self, mask, &values);
}

static XtResource resources[] = {
{XtNonCallback,XtCOnCallback,XtRCallback,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.onCallback),XtOffsetOf(XfwfToggleRec,xfwfToggle.onCallback),XtRImmediate,(XtPointer)NULL },
{XtNoffCallback,XtCOffCallback,XtRCallback,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.offCallback),XtOffsetOf(XfwfToggleRec,xfwfToggle.offCallback),XtRImmediate,(XtPointer)NULL },
{XtNon,XtCOn,XtRBoolean,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.on),XtOffsetOf(XfwfToggleRec,xfwfToggle.on),XtRImmediate,(XtPointer)False },
{XtNonIcon,XtCOnIcon,XtRIcon,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.onIcon),XtOffsetOf(XfwfToggleRec,xfwfToggle.onIcon),XtRString,(XtPointer)"filledsquare"},
{XtNoffIcon,XtCOffIcon,XtRIcon,sizeof(((XfwfToggleRec*)NULL)->xfwfToggle.offIcon),XtOffsetOf(XfwfToggleRec,xfwfToggle.offIcon),XtRString,(XtPointer)"emptysquare"},
};

XfwfToggleClassRec xfwfToggleClassRec = {
{ /* core_class part */
(WidgetClass) &xfwfButtonClassRec,
"TextToggle",
sizeof(XfwfToggleRec),
NULL,
_resolve_inheritance,
FALSE,
initialize,
NULL,
XtInheritRealize,
actionsList,
3,
resources,
5,
NULLQUARK,
False ,
FALSE ,
False ,
False ,
NULL,
XtInheritResize,
expose,
set_values,
NULL,
XtInheritSetValuesAlmost,
NULL,
XtInheritAcceptFocus,
XtVersion,
NULL,
defaultTranslations,
XtInheritQueryGeometry,
XtInheritDisplayAccelerator,
NULL 
},
{ /* composite_class part */
XtInheritGeometryManager,
XtInheritChangeManaged,
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
XtInherit_set_abs_location,
},
{ /* XfwfLabel_class part */
XtInherit_set_label,
},
{ /* XfwfButton_class part */
0
},
{ /* XfwfToggle_class part */
0
},
};
WidgetClass xfwfToggleWidgetClass = (WidgetClass) &xfwfToggleClassRec;
/*ARGSUSED*/
static void toggle(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    XtVaSetValues(self, "on", !((XfwfToggleWidget)self)->xfwfToggle.on, NULL);
    XtCallCallbackList(self, ((XfwfToggleWidget)self)->xfwfToggle.on ? ((XfwfToggleWidget)self)->xfwfToggle.onCallback : ((XfwfToggleWidget)self)->xfwfToggle.offCallback, event);
}

/*ARGSUSED*/
static void switch_on(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    if (! ((XfwfToggleWidget)self)->xfwfToggle.on) {
        XtVaSetValues(self, "on", True, NULL);
        XtCallCallbackList(self, ((XfwfToggleWidget)self)->xfwfToggle.onCallback, event);
    }
}

/*ARGSUSED*/
static void switch_off(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    if (((XfwfToggleWidget)self)->xfwfToggle.on) {
        XtVaSetValues(self, "on", False, NULL);
        XtCallCallbackList(self, ((XfwfToggleWidget)self)->xfwfToggle.offCallback, event);
    }
}

static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfToggleWidgetClass c = (XfwfToggleWidgetClass) class;
  XfwfToggleWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfToggleWidgetClass) return;
  super = (XfwfToggleWidgetClass)class->core_class.superclass;
}
/*ARGSUSED*/static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
{
    int status;
    Dimension w1, w2, w;

    ((XfwfToggleWidget)self)->xfwfToggle.on_gc = NULL;
    ((XfwfToggleWidget)self)->xfwfToggle.off_gc = NULL;
    create_on_gc(self);
    create_off_gc(self);

    w1 = ((XfwfToggleWidget)self)->xfwfToggle.onIcon ? ((XfwfToggleWidget)self)->xfwfToggle.onIcon->attributes.width : 0;
    w2 = ((XfwfToggleWidget)self)->xfwfToggle.offIcon ? ((XfwfToggleWidget)self)->xfwfToggle.offIcon->attributes.width : 0;
    w = max(w1, w2);
    if (w != 0) XtVaSetValues(self, XtNleftMargin, ((XfwfToggleWidget)self)->xfwfLabel.leftMargin + w, NULL);
}
/*ARGSUSED*/static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
{
    Boolean redraw = False, compute_margin = False;
    Dimension w1, w2, w;

    if (((XfwfToggleWidget)self)->xfwfToggle.onIcon != ((XfwfToggleWidget)old)->xfwfToggle.onIcon) {
	create_on_gc(self);
	compute_margin = True;
    }
    if (((XfwfToggleWidget)self)->xfwfToggle.offIcon != ((XfwfToggleWidget)old)->xfwfToggle.offIcon) {
	create_off_gc(self);
	compute_margin = True;
    }
    if (((XfwfToggleWidget)self)->xfwfToggle.on != ((XfwfToggleWidget)old)->xfwfToggle.on) {
        redraw = True;
    }
    if (compute_margin) {
	/* Compute w = old margin between icons and text */
	w1 = ((XfwfToggleWidget)old)->xfwfToggle.onIcon ? ((XfwfToggleWidget)old)->xfwfToggle.onIcon->attributes.width : 0;
	w2 = ((XfwfToggleWidget)old)->xfwfToggle.offIcon ? ((XfwfToggleWidget)old)->xfwfToggle.offIcon->attributes.width : 0;
	w = ((XfwfToggleWidget)old)->xfwfLabel.leftMargin - max(w1, w2);
	/* Compute new left margin w = w + width of icons */
	w1 = ((XfwfToggleWidget)self)->xfwfToggle.onIcon ? ((XfwfToggleWidget)self)->xfwfToggle.onIcon->attributes.width : 0;
	w2 = ((XfwfToggleWidget)self)->xfwfToggle.offIcon ? ((XfwfToggleWidget)self)->xfwfToggle.offIcon->attributes.width : 0;
	w = w + max(w1, w2);
	if (((XfwfToggleWidget)old)->xfwfLabel.leftMargin != w) {
	    XtVaSetValues(self, XtNleftMargin, w, NULL);
	    redraw = False;
	} else
	    redraw = True;
    }
    return redraw;
}
/*ARGSUSED*/static void expose(self,event,region)Widget self;XEvent * event;Region  region;
{
    Position x, y;
    Dimension w, h;

    if (! XtIsRealized(self)) return;
    xfwfButtonClassRec.core_class.expose(self, event, region);
    ((XfwfToggleWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    if (((XfwfToggleWidget)self)->xfwfToggle.on && ((XfwfToggleWidget)self)->xfwfToggle.onIcon) {
	y = y + (h - ((XfwfToggleWidget)self)->xfwfToggle.onIcon->attributes.height)/2;
	XSetTSOrigin(XtDisplay(self), ((XfwfToggleWidget)self)->xfwfToggle.on_gc, x, y);
	XSetClipOrigin(XtDisplay(self), ((XfwfToggleWidget)self)->xfwfToggle.on_gc, x, y);
	XFillRectangle(XtDisplay(self), XtWindow(self), ((XfwfToggleWidget)self)->xfwfToggle.on_gc, x, y,
		       ((XfwfToggleWidget)self)->xfwfToggle.onIcon->attributes.width,
		       ((XfwfToggleWidget)self)->xfwfToggle.onIcon->attributes.height);
    } else if (((XfwfToggleWidget)self)->xfwfToggle.offIcon) {
	y = y + (h - ((XfwfToggleWidget)self)->xfwfToggle.offIcon->attributes.height)/2;
	XSetTSOrigin(XtDisplay(self), ((XfwfToggleWidget)self)->xfwfToggle.off_gc, x, y);
	XSetClipOrigin(XtDisplay(self), ((XfwfToggleWidget)self)->xfwfToggle.off_gc, x, y);
	XFillRectangle(XtDisplay(self), XtWindow(self), ((XfwfToggleWidget)self)->xfwfToggle.off_gc, x, y,
		       ((XfwfToggleWidget)self)->xfwfToggle.offIcon->attributes.width,
		       ((XfwfToggleWidget)self)->xfwfToggle.offIcon->attributes.height);
    }
}
