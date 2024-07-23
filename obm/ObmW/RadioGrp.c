/* Generated by wbuild from "RadioGrp.w"
** (generator version $Revision: 2.0 $ of $Date: 93/07/06 16:08:04 $)
*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Toggle.h"
#include "RadioGrpP.h"
static void _resolve_inheritance(WidgetClass);
static void class_initialize(void);
static void initialize(Widget ,Widget,ArgList ,Cardinal *);
static Boolean  set_values(Widget ,Widget ,Widget,ArgList ,Cardinal *);
static void create_toggles(Widget);
#define done(type, value) do {\
	if (to->addr != NULL) {\
	    if (to->size < sizeof(type)) {\
	        to->size = sizeof(type);\
	        return False;\
	    }\
	    *(type*)(to->addr) = (value);\
        } else {\
	    static type static_val;\
	    static_val = (value);\
	    to->addr = (XtPointer)&static_val;\
        }\
        to->size = sizeof(type);\
        return True;\
    }while (0 )


static Boolean  cvtStringToStringArray(Display *,XrmValuePtr ,Cardinal *,XrmValuePtr ,XrmValuePtr ,XtPointer *);
static StringArray  newStringArray(StringArray );
static void freeStringArray(StringArray );

/*ARGSUSED*/static void create_toggles(self)Widget self;
{
    Cardinal i;
    StringArray s;

    for (i = 0; i < ((XfwfRadioGroupWidget)self)->composite.num_children; i++) 
	XtDestroyWidget(((XfwfRadioGroupWidget)self)->composite.children[i]);

    if (((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels == NULL) return;

    for (s = ((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels, i = 0; s[i] != NULL; i++) {
	(void) XtVaCreateManagedWidget
	    (s[i], xfwfToggleWidgetClass, self, XtNlabel, s[i], XtNshrinkToFit,
	     True, XtNborderWidth, 0, XtNframeWidth, 0, NULL);
    }
}
/*ARGSUSED*/static Boolean  cvtStringToStringArray(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
{
    String t, s;
    StringArray a = NULL;
    Cardinal i;
    char delim;

    if (*num_args != 0)
	XtAppErrorMsg(XtDisplayToApplicationContext(display),
		      "cvtStringToStringArray", "wrongParameters",
		      "XtToolkitError",
		      "String to StringArray conversion needs no arguments",
		      (String*) NULL, (Cardinal*) NULL);

    delim = ((String) from->addr)[0];
    s = XtNewString((String) from->addr + 1);
    i = 0;
    while (s && *s) {
	t = strchr(s, delim);
        if (t) *t = '\0';
	a = (StringArray) XtRealloc((String) a, (i + 1) * sizeof(*a));
	a[i] = s;
	i++;
        s = t ? t + 1 : NULL;
    }
    a = (StringArray) XtRealloc((String) a, (i + 1) * sizeof(*a));
    a[i] = NULL;
    done(StringArray, a);
}
/*ARGSUSED*/static StringArray  newStringArray(a)StringArray  a;
{
    Cardinal n, i;
    StringArray s;

    if (!a) return NULL;
    for (s = a, n = 0; s[n]; n++) ;
    s = (StringArray) XtMalloc((n + 1) * sizeof(*s));
    for (i = 0; i < n; i++) s[i] = XtNewString(a[i]);
    s[n] = NULL;
    return s;
}
/*ARGSUSED*/static void freeStringArray(a)StringArray  a;
{
    Cardinal i;

    if (!a) return;
    for (i = 0; a[i]; i++) XtFree(a[i]);
    XtFree((String) a);
}

static XtResource resources[] = {
{XtNlabels,XtCLabels,XtRStringArray,sizeof(((XfwfRadioGroupRec*)NULL)->xfwfRadioGroup.labels),XtOffsetOf(XfwfRadioGroupRec,xfwfRadioGroup.labels),XtRImmediate,(XtPointer)NULL },
};

XfwfRadioGroupClassRec xfwfRadioGroupClassRec = {
{ /* core_class part */
(WidgetClass) &xfwfGroupClassRec,
"RadioGroup",
sizeof(XfwfRadioGroupRec),
class_initialize,
_resolve_inheritance,
FALSE,
initialize,
NULL,
XtInheritRealize,
NULL,
0,
resources,
1,
NULLQUARK,
False ,
FALSE ,
False ,
False ,
NULL,
XtInheritResize,
XtInheritExpose,
set_values,
NULL,
XtInheritSetValuesAlmost,
NULL,
XtInheritAcceptFocus,
XtVersion,
NULL,
NULL,
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
{ /* XfwfRowCol_class part */
XtInherit_layout,
},
{ /* XfwfGroup_class part */
0
},
{ /* XfwfRadioGroup_class part */
0
},
};
WidgetClass xfwfRadioGroupWidgetClass = (WidgetClass) &xfwfRadioGroupClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfRadioGroupWidgetClass c = (XfwfRadioGroupWidgetClass) class;
  XfwfRadioGroupWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfRadioGroupWidgetClass) return;
  super = (XfwfRadioGroupWidgetClass)class->core_class.superclass;
}
/*ARGSUSED*/static void class_initialize()
{
    XtSetTypeConverter(XtRString, "StringArray", cvtStringToStringArray,
		       NULL, 0, XtCacheNone, NULL);
}
/*ARGSUSED*/static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
{
    ((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels = newStringArray(((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels);
    create_toggles(self);
}
/*ARGSUSED*/static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
{
    if (((XfwfRadioGroupWidget)old)->xfwfRadioGroup.labels != ((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels) {
	freeStringArray(((XfwfRadioGroupWidget)old)->xfwfRadioGroup.labels);
	((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels = newStringArray(((XfwfRadioGroupWidget)self)->xfwfRadioGroup.labels);
	create_toggles(self);
    }
    return False;
}
