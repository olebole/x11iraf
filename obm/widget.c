/* Copyright(c) 1993 Association of Universities for Research in Astronomy Inc.
 */

#include <stdlib.h>
#include "ObmP.h"
#include "widget.h"
#include "ObmW/ListTree.h"

/*
 * WIDGET class.
 * --------------------------
 * The Widget class is the generic or base class for the window system
 * toolkit widgets supported by the object manager.  The Widget class
 * supports a number of different Xt widget classes using a table driven
 * approach to describe each widget.  Any widget may be created, destroyed,
 * and manipulated in various ways using only the generic Widget class
 * procedures and Widget-specific resources.  The Widget class may be
 * subclassed to support complex Widgets that require custom class-specific
 * commands for use by the GUI code.
 *
 * Generic Widget-class commands:
 *
 *                  set resource-name value
 *                  get resource-name
 *
 *          addCallback procedure-name [callback-name]
 *       deleteCallback procedure-name
 *         setSensitive sensitive
 *          isSensitive
 *
 *		realize
 *	      unrealize
 *	     isRealized
 *                  map
 *                unmap
 *		 manage child [child ...]
 *	       unmanage child [child ...]
 *                popup [grab-kind]
 *              popdown
 *    popupSpringLoaded
 *
 *                 move x y
 *               resize width height border-width
 *            configure x y width height border-width
 *	  parseGeometry user_geom def_geom x y width height
 *   geom = getGeometry x y width height [nogravity]
 *
 *	      setTTName name				# for call($name...)
 *     name = getTTName
 *
 * The most important Widget commands are set/get resource, and the
 * callbacks.  The widget sensitivity can be set and queried using set/get
 * resource, but special procedures are provided to make this common operation
 * more convenient.
 *
 * Class specific functions:
 *
 *	           append text				# text widget
 *       value = getValue				# dialog widget
 *
 *		  setList list [resize]			# list widget
 * 	  value = getItem itemno
 *	        highlight itemno
 *	      unhighlight [itemno]
 *
 *	         getThumb x [y [width height]]		# sliders
 *	        moveThumb x [y]
 *	      resizeThumb width [height]
 *
 *	     setScrollbar position size			# scrollbars
 *
 *	      setLocation x y				# viewport
 *	   setCoordinates x y
 *
 *		   setTop widget			# tabs
 *
 *	      setListTree list				# list tree
 *	   listTreeSelect item [top [child_only] ]
 *      listTreeHighlight item [top [child_only] ]
 *	   listTreeDelete item [top]
 *
 *	         setTable nrows ncols data		# table
 *     attr = getCellAttr row col attr
 *            setCellAttr row col attr value
 *      attr = getColAttr col attr
 *             setColAttr col attr value
 *      attr = getRowAttr row attr
 *             setRowAttr row attr value
 *              deleteCol col 
 *              deleteRow row 
 *                 addCol col width [where]
 *                 addRow row [where]
 *           setTableSize nrows ncols
 *           getTableSize nrows ncols
 *
 *
 * Ideally the widget class should be subclassed for widgets that require
 * class-specific functions, but in simple cases involving standard widgets
 * the support is built into the widget class code as a special case.
 *
 * Special actions (used in translations):
 *
 *                 call (proc [,arg, ...])
 *                popup (menu-name [xoffset [yoffset]])
 *              popdown (menu-name)
 *
 * The "call" action is a very general mechanism which allows any GUI procedure
 * to be registered with any translation using the X translation table
 * mechanism.  The popup and popdown actions are used for popup menus.  The
 * menu will be popped up at the event coordinates plus the optional offsets
 * if given.
 *
 * Event handling:
 *
 *      addEventHandler <procname> <event-mask> [<event-mask>...]
 *
 * Event callback:
 *
 *     userEventHandler widget event-type time wx wy rx ry other
 *
 * In most cases translations are preferable to events, but a GUI can capture
 * raw events if it wishes by adding event handlers.  Nearly all of the X
 * event types are supported.  The callback syntax employs a number of
 * standard arguments, followed by a number of event-specific arguments.
 */

typedef struct rtype *Rtype;
#define	LEN_RHASH 197
Rtype	rhash[LEN_RHASH];

#define	MAX_TREE_LEVELS 10

/* Global widget resources table. */
struct rtype {
	char *name;			/* resource name */
	char *type;			/* resource type */
	unsigned long flag1, flag2;	/* widgets using resource */
	struct rtype *next;		/* next entry on hash thread */
} ObmResources[] = {
#include "obmres.dat"
};

struct callbackType {
	int type;
	char *name;
};

/* Widget callback types. */
struct callbackType callbackTypes[] = {
    {	Ctcallback,			"callback" },
    {	Ctcharmode,			"charmode" },
    {	Ctlinemode,			"linemode" },
    {	CtgetValue,			"getValue" },
    {	CtjumpProc,			"jump" },
    {	CtscrollProc,			"scroll" },
    {	CtpopupCallback,		"popup" },
    {	CtpopdownCallback,		"popdown" },
    {	CtreportCallback,		"report" },
    {	CtstartCallback,		"start" },
    {	CtstopCallback,			"stop" },
};



enum scaleType {						/* MF016 */
    atom, pixel_size, point_size,				/*  /|\  */
    resolution, resolution_x, resolution_y, average_width,	/* / | \ */
    scaledX, scaledY, unscaled, scaledXoverY, uncomputed,	/*   |   */
};								/*   |   */
								/*   |   */
typedef struct _fontProp {					/*   |   */
    char       *name;						/*   |   */
    Atom        atom;						/*   |   */
    enum scaleType type;					/*   |   */
    char found;							/*   |   */
} fontProp;							/*   |   */
								/*   |   */
static fontProp fontNamePropTable[] = {				/*   |   */
        { "FOUNDRY", 0, atom, 0},				/*   |   */
        { "FAMILY_NAME", 0, atom, 0},				/*   |   */
        { "WEIGHT_NAME", 0, atom, 0},				/*   |   */
        { "SLANT", 0, atom, 0},					/*   |   */
        { "SETWIDTH_NAME", 0, atom, 0},				/*   |   */
        { "ADD_STYLE_NAME", 0, atom, 0},			/*   |   */
        { "PIXEL_SIZE", 0, pixel_size, 0},			/*   |   */
        { "POINT_SIZE", 0, point_size, 0},			/*   |   */
        { "RESOLUTION_X", 0, resolution_x, 0},			/*   |   */
        { "RESOLUTION_Y", 0, resolution_y, 0},			/*   |   */
        { "SPACING", 0, atom, 0},				/*   |   */
        { "AVERAGE_WIDTH", 0, average_width, 0},		/*   |   */
        { "CHARSET_REGISTRY", 0, atom, 0},			/*   |   */
        { "CHARSET_ENCODING", 0, atom, 0},			/*   |   */
};								/*   |   */
								/*   |   */
#define NUMITEMS(arr) ((int) (sizeof(arr) / sizeof(arr[0])))	/* \ | / */
								/*  \|/  */
static char *widgetGetFontName(Display *, XFontStruct *);	/* MF016 */


static	void do_text(Widget, XEvent *, String *, Cardinal *);
static	void do_userproc(Widget, XEvent *, String *, Cardinal *);
static	void do_popup(Widget, XEvent *, String *, Cardinal *);
static	void do_popdown(Widget, XEvent *, String *, Cardinal *);
static XtActionsRec widget_actions[] = {
	{"call",      do_userproc},
	{"do_text",   do_text},
	{"popup",     do_popup},
	{"popdown",   do_popdown},
};

static	void call_callbacks(WidgetObject, int, char *);
static	void widgetEvent(Widget, ObmCallback, XEvent *, Boolean *);
static	void widgetSetDestroy(ObmObject);
static	void widgetDestroy(ObmObject);
static	void widgetCallback(Widget, WidgetObject, void *);
static	void widgetSCCallback(Widget w, WidgetObject obj, void *);
static	void widgetJPCallback(Widget w, WidgetObject obj, void *);
static	void widgetSPCallback(Widget w, WidgetObject obj, void *);
static	void widgetPUCallback(Widget w, WidgetObject obj, void *);
static	void widgetPDCallback(Widget w, WidgetObject obj, void *);
static	void widgetSBCallback(Widget w, WidgetObject obj, void *);
static	void widgetSECallback(Widget w, WidgetObject obj, void *);
static	void widgetRPCallback(Widget w, WidgetObject obj, void *);
static	void widgetRGCallback(Widget w, WidgetObject obj, void *);
static	void widgetLTHCallback(Widget w, WidgetObject obj, void *);
static	void widgetLTACallback(Widget w, WidgetObject obj, void *);
static	void widgetTCCCallback(Widget w, WidgetObject obj, void *);
static	int widgetSet(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGet(MsgContext, Tcl_Interp *, int, char **);
static	int widgetMap(MsgContext, Tcl_Interp *, int, char **);
static	int widgetUnmap(MsgContext, Tcl_Interp *, int, char **);
static	int widgetRealize(MsgContext, Tcl_Interp *, int, char **);
static	int widgetUnrealize(MsgContext, Tcl_Interp *, int, char **);
static	int widgetIsRealized(MsgContext, Tcl_Interp *, int, char **);
static	int widgetPopup(MsgContext, Tcl_Interp *, int, char **);
static	int widgetPopupSpringLoaded(MsgContext, Tcl_Interp *, int, char **);
static	int widgetPopdown(MsgContext, Tcl_Interp *, int, char **);
static	int widgetAddCallback(MsgContext, Tcl_Interp *, int, char **);
static	int widgetDeleteCallback(MsgContext, Tcl_Interp *, int, char **);
static	int widgetMove(MsgContext, Tcl_Interp *, int, char **);
static	int widgetResize(MsgContext, Tcl_Interp *, int, char **);
static	int widgetConfigure(MsgContext, Tcl_Interp *, int, char **);
static	int widgetParseGeometry(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetGeometry(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetSensitive(MsgContext, Tcl_Interp *, int, char **);
static	int widgetIsSensitive(MsgContext, Tcl_Interp *, int, char **);
static	int widgetManage(MsgContext, Tcl_Interp *, int, char **);
static	int widgetUnmanage(MsgContext, Tcl_Interp *, int, char **);
static	int widgetAppend(MsgContext, Tcl_Interp *, int, char **);
static	int widgetAddEventHandler(MsgContext, Tcl_Interp *, int, char **);
static	int widgetRemoveEventHandler(MsgContext, Tcl_Interp *, int, char **);
static	int widgetHighlight(MsgContext, Tcl_Interp *, int, char **);
static	int widgetUnhighlight(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetTop(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetList(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetItem(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetValue(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetThumb(MsgContext, Tcl_Interp *, int, char **);
static	int widgetMoveThumb(MsgContext, Tcl_Interp *, int, char **);
static	int widgetResizeThumb(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetScrollbar(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetTTName(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetTTName(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetListTree(MsgContext, Tcl_Interp *, int, char **);
static	int widgetListTreeSelect(MsgContext, Tcl_Interp *, int, char **);
static	int widgetListTreeHighlight(MsgContext, Tcl_Interp *, int, char **);
static	int widgetListTreeDelete(MsgContext, Tcl_Interp *, int, char **);
static 	int widgetSetLocation(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetCoordinates(MsgContext, Tcl_Interp *, int, char **);
#ifndef OSI_COMPLIANT
static	int widgetSetTable(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetCellAttr(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetCellAttr(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetColAttr(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetColAttr(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetRowAttr(MsgContext, Tcl_Interp *, int, char **);
static	int widgetDeleteRow(MsgContext, Tcl_Interp *, int, char **);
static	int widgetAddRow(MsgContext, Tcl_Interp *, int, char **);
static	int widgetGetTableSize(MsgContext, Tcl_Interp *, int, char **);
static	int widgetDeleteCol(MsgContext, Tcl_Interp *, int, char **);
static	int widgetAddCol(MsgContext, Tcl_Interp *, int, char **);
static	int widgetSetTableSize(MsgContext, Tcl_Interp *, int, char **);
#endif
static	int get_itemno(WidgetObject, char *);
static	int buildTreeList(Widget, Tcl_Interp *, ListTreeItem *, const char *);
static	int widgetGetRowAttr(MsgContext, Tcl_Interp *, int, char **);


/* WidgetClassInit -- Initialize the class record for the widget class.
 */
void
WidgetClassInit (ObmContext obm, ObjClassRec classrec)
{
	int hashval, n;
	char *ip;
	ObjClassRec widgetclass;
	static int hashed = 0;
	Tcl_Interp *tcl;
	MsgContext msg;
	Rtype rp, hp;
	int i;

	/* The base class for all Widget classes is "Widget". */
	widgetclass = obmGetClassrec ("Widget");

	/* Install the class methods. */
	classrec->ClassDestroy = WidgetClassDestroy;
	classrec->Create = WidgetCreate;
	classrec->Destroy = WidgetDestroy;
	classrec->Evaluate = WidgetEvaluate;

	/* Since there can be many instances of the widget object and they
	 * all respond to the same class messages, a single interpreter is
	 * used for all widget instances.  By default all Widget subclasses
	 * use the interperter for the base Widget class.
	 */
	if (!widgetclass->class_data) {
	    msg = (MsgContext) XtMalloc (sizeof (struct msgContext));
	    msg->tcl = tcl = Tcl_CreateInterp();
	    widgetclass->class_data = (XtPointer) msg;
	    msg->level = 0;

	    /* Register widget-object actions. */
	    Tcl_CreateCommand (tcl,
		"addCallback", (Tcl_CmdProc *) widgetAddCallback, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"deleteCallback", (Tcl_CmdProc *) widgetDeleteCallback, (ClientData)msg,
		NULL);
	    Tcl_CreateCommand (tcl,
		"addEventHandler", (Tcl_CmdProc *) widgetAddEventHandler, (ClientData)msg,
		NULL);
	    Tcl_CreateCommand (tcl,
		"removeEventHandler", (Tcl_CmdProc *) widgetRemoveEventHandler,
		(ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"set", (Tcl_CmdProc *) widgetSet, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"get", (Tcl_CmdProc *) widgetGet, (ClientData)msg, NULL);

	    /* Text Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"append", (Tcl_CmdProc *) widgetAppend, (ClientData)msg, NULL);

	    /* List Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"setList", (Tcl_CmdProc *) widgetSetList, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getItem", (Tcl_CmdProc *) widgetGetItem, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"highlight", (Tcl_CmdProc *) widgetHighlight, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"unhighlight", (Tcl_CmdProc *) widgetUnhighlight, (ClientData)msg, NULL);

	    /* Dialog Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"getValue", (Tcl_CmdProc *) widgetGetValue, (ClientData)msg, NULL);

	    /* Slider Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"getThumb", (Tcl_CmdProc *) widgetGetThumb, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"moveThumb", (Tcl_CmdProc *) widgetMoveThumb, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"resizeThumb", (Tcl_CmdProc *) widgetResizeThumb, (ClientData)msg, NULL);

	    /* Scrollbar Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"setScrollbar", (Tcl_CmdProc *) widgetSetScrollbar, (ClientData)msg, NULL);

	    /* Viewport Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"setLocation", (Tcl_CmdProc *) widgetSetLocation, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setCoordinates", (Tcl_CmdProc *) widgetSetCoordinates, (ClientData)msg, NULL);

	    /* Tabs Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"setTop", (Tcl_CmdProc *) widgetSetTop, (ClientData)msg, NULL);

	    /* Tree Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"setListTree", (Tcl_CmdProc *) widgetSetListTree, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"listTreeSelect", (Tcl_CmdProc *) widgetListTreeSelect, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"listTreeDelete", (Tcl_CmdProc *) widgetListTreeDelete, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"listTreeHighlight", (Tcl_CmdProc *) widgetListTreeHighlight,
		(ClientData)msg, NULL);

#ifndef OSI_COMPLIANT
	    /* Table Widget Callbacks */
	    Tcl_CreateCommand (tcl,
		"setTable", (Tcl_CmdProc *) widgetSetTable, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getCellAttr", (Tcl_CmdProc *) widgetGetCellAttr, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setCellAttr", (Tcl_CmdProc *) widgetSetCellAttr, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setColAttr", (Tcl_CmdProc *) widgetSetColAttr, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getColAttr", (Tcl_CmdProc *) widgetGetColAttr, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setRowAttr", (Tcl_CmdProc *) widgetSetRowAttr, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getRowAttr", (Tcl_CmdProc *) widgetGetRowAttr, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"deleteCol", (Tcl_CmdProc *) widgetDeleteCol, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"deleteRow", (Tcl_CmdProc *) widgetDeleteRow, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"addCol", (Tcl_CmdProc *) widgetAddCol, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"addRow", (Tcl_CmdProc *) widgetAddRow, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setTableSize", (Tcl_CmdProc *) widgetSetTableSize, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getTableSize", (Tcl_CmdProc *) widgetGetTableSize, (ClientData)msg, NULL);
#endif

	    Tcl_CreateCommand (tcl,
		"realize", (Tcl_CmdProc *) widgetRealize, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"unrealize", (Tcl_CmdProc *) widgetUnrealize, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"isRealized", (Tcl_CmdProc *) widgetIsRealized, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"map", (Tcl_CmdProc *) widgetMap, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"unmap", (Tcl_CmdProc *) widgetUnmap, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"manage", (Tcl_CmdProc *) widgetManage, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"unmanage", (Tcl_CmdProc *) widgetUnmanage, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"popup", (Tcl_CmdProc *) widgetPopup, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"popupSpringLoaded", (Tcl_CmdProc *) widgetPopupSpringLoaded,
		(ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"popdown", (Tcl_CmdProc *) widgetPopdown, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setSensitive", (Tcl_CmdProc *) widgetSetSensitive, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"isSensitive", (Tcl_CmdProc *) widgetIsSensitive, (ClientData)msg, NULL);

	    Tcl_CreateCommand (tcl,
		"move", (Tcl_CmdProc *) widgetMove, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"resize", (Tcl_CmdProc *) widgetResize, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"configure", (Tcl_CmdProc *) widgetConfigure, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"parseGeometry", (Tcl_CmdProc *) widgetParseGeometry, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getGeometry", (Tcl_CmdProc *) widgetGetGeometry, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"setTTName", (Tcl_CmdProc *) widgetSetTTName, (ClientData)msg, NULL);
	    Tcl_CreateCommand (tcl,
		"getTTName", (Tcl_CmdProc *) widgetGetTTName, (ClientData)msg, NULL);

	    /* Register any actions. */
	    XtAppAddActions (obm->app_context, widget_actions,
		XtNumber(widget_actions));
	    XtRegisterGrabAction (do_popup, True,
		(unsigned)(ButtonPressMask | ButtonReleaseMask),
		GrabModeAsync, GrabModeAsync);
	}

	/* Build a hash index for the global widget resources table. */
	if (!hashed) {
	    for (i=0;  i < XtNumber(ObmResources);  i++) {
		rp = &ObmResources[i];
		n = MAX_HASHCHARS;
		for (hashval=0, ip=rp->name;  --n >= 0 && *ip;  ip++)
		    hashval += (hashval + *ip);
		if ((hp = rhash[hashval%LEN_RHASH])) {
		    for (  ;  hp->next;  hp = hp->next)
			;
		    hp->next = rp;
		} else
		    rhash[hashval%LEN_RHASH] = rp;
	    }
	    hashed++;
	}
}


/* WidgetClassDestroy -- Custom destroy procedure for the widget class.
 */
void
WidgetClassDestroy (ObmContext obm, ObjClassRec classrec)
{
	MsgContext msg = (MsgContext) classrec->class_data;

	if (msg) {
	    if (msg->tcl)
		Tcl_DeleteInterp (msg->tcl);
	    XtFree ((char *)msg);
	    classrec->class_data = NULL;
	}
}


/* WidgetCreate -- Create an instance of a widget object.
 */
ObmObject
WidgetCreate (
  ObmContext obm,
  const char *name,
  ObjClassRec classrec,
  const char *parent,
  ArgList args,
  int nargs)
{
	WidgetObject obj, pobj;
	Widget w, pw;

	/* Create the widget object descriptor. */
	obj = (WidgetObject) XtCalloc (1, sizeof (struct widgetObject));
	obj->widget.obm = obm;

	/* The widget "toplevel" is a special case.  This is the toplevel
	 * widget of the entire application and it is created separately
	 * when the application starts up.  When we are called to "create"
	 * this widget all we do is create a descriptor for it, so that it
	 * will appear in the object list like any other widget.
	 */
	if (strcmp (name, "toplevel") == 0) {
	    w = obm->toplevel;
	    pw = NULL;
	} else {
	    /* Convert parent name to Widget. */
	    if ((pobj = (WidgetObject) obmFindObject (obm, parent)) == NULL)
		return (NULL);
	    pw = pobj->widget.w;

	    /* Create the widget. */
	    if (classrec->object_type == OtShell) {
		w = XtCreatePopupShell (name,
		    *classrec->widget_class, pw, args, nargs);
	    } else if (obmClass (classrec, WtObject)) {
		w = XtCreateWidget (name,
		    *classrec->widget_class, pw, args, nargs);
	    } else {
		w = XtCreateManagedWidget (name,
		    *classrec->widget_class, pw, args, nargs);
	    }
	}

	/* Set the pointer to the superclass if subclass of Widget. */
	if (strcmp (classrec->name, "Widget") != 0)
	    obj->core.superclass = obmGetClassrec ("Widget");

	/* Register any class callback procedures. */
	if (obmClass (classrec, WtGrip) ||
	    obmClass (classrec, WtList) ||
	    obmClass (classrec, WtMultiList) ||
	    obmClass (classrec, WtSmeBSB) ||
	    obmClass (classrec, WtCommand) ||
	    obmClass (classrec, WtMenuButton) ||
	    obmClass (classrec, WtTabs) ||
	    obmClass (classrec, WtToggle) ||
	    obmClass (classrec, WtArrow)) {

	  XtAddCallback (w, XtNcallback, (XtCallbackProc) widgetCallback, obj);

	} else if (obmClass (classrec, WtListTree)) {
	    XtAddCallback (w, XtNhighlightCallback, (XtCallbackProc) widgetLTHCallback, obj);
	    XtAddCallback (w, XtNactivateCallback, (XtCallbackProc) widgetLTACallback, obj);

	} else if (obmClass (classrec, WtRepeater)) {
	    XtAddCallback (w, XtNcallback, (XtCallbackProc) widgetCallback, obj);
	    XtAddCallback (w, XtNstartCallback, (XtCallbackProc) widgetSBCallback, obj);
	    XtAddCallback (w, XtNstopCallback, (XtCallbackProc) widgetSECallback, obj);

	} else if (obmClass (classrec, WtStripChart)) {
	    XtAddCallback (w, XtNgetValue, (XtCallbackProc) widgetSCCallback, obj);

	} else if (obmClass (classrec, WtScrollbar)) {
	    XtAddCallback (w, XtNjumpProc, (XtCallbackProc) widgetJPCallback, obj);
	    XtAddCallback (w, XtNscrollProc, (XtCallbackProc) widgetSPCallback, obj);

	} else if (obmClass (classrec, WtShell) ||
		   obmClass (classrec, WtSimpleMenu)) {

	    XtAddCallback (w, XtNpopupCallback, (XtCallbackProc) widgetPUCallback, obj);
	    XtAddCallback (w, XtNpopdownCallback, (XtCallbackProc) widgetPDCallback, obj);

	} else if (obmClass (classrec, WtPanner) ||
		   obmClass (classrec, WtPorthole) ||
		   obmClass (classrec, WtViewport)) {

	    XtAddCallback (w, XtNreportCallback, (XtCallbackProc) widgetRPCallback, obj);

	} else if (obmClass (classrec, WtTextButton) ||
		   obmClass (classrec, WtIcon)) {
	    XtAddCallback (w, XtNactivate, (XtCallbackProc) widgetCallback, obj);

	} else if (obmClass (classrec, WtGroup) ||
		   obmClass (classrec, WtRadioGroup)) {
	    XtAddCallback (w, XtNactivate, (XtCallbackProc) widgetRGCallback, obj);

	} else if (obmClass (classrec, WtTextToggle)) {
	    XtAddCallback (w, XtNonCallback, (XtCallbackProc) widgetCallback, obj);
	    XtAddCallback (w, XtNoffCallback, (XtCallbackProc) widgetCallback, obj);

	} else if (obmClass (classrec, WtSlider2d) ||
		   obmClass (classrec, WtScrollbar2)) {

	    XtVaGetValues (w, "scrollResponse", &obj->widget.response_cb, NULL);
	    XtAddCallback (w, XtNscrollCallback, (XtCallbackProc) widgetJPCallback, obj);
	    XtAddCallback (w, XtNscrollCallback, (XtCallbackProc) widgetSPCallback, obj);
	}

	obj->widget.w = w;
	strcpy (obj->widget.translation_table_name, name);
	return ((ObmObject) obj);
}


/* WidgetDestroy -- Destroy an instance of a widget object.
 */
void
WidgetDestroy (ObmObject object)
{
	WidgetObject obj = (WidgetObject) object;
	WidgetPrivate wp = &obj->widget;
	ObmCallback cb, next;

	/* Ignore the second call to Destroy. */
	if (obj->core.being_destroyed++)
	    return;

	/* Free any callback descriptors. */
	for (cb = wp->callback;  cb;  cb = next) {
	    next = cb->next;
	    XtFree ((char *)cb);
	}

	/* Free any event handler descriptors. */
	for (cb = wp->event_handler;  cb;  cb = next) {
	    next = cb->next;
	    XtFree ((char *)cb);
	}

	/* Free any object data.  Note that free is used, not XtFree, i.e.
	 * we can't assume that Xt allocated the buffer.
	 */
	if (wp->data)
	    Tcl_Free (wp->data);

	/* Mark any widget children as being destroyed so that we don't try
	 * to destroy them twice.
	 */
	if (!wp->widget_destroyed) {
	    widgetSetDestroy (object);
	    widgetDestroy (object);
	}
}


/* widgetSetDestroy -- Set the being_destroyed flag on all the children of a
 * widget object to indicate that the widget itself has already been destroyed.
 * This happens when a widget tree is destroyed in one toolkit call by
 * destroying the top level widget, leaving the object descriptors intact
 * while the widgets have already been destroyed.
 */
static void
widgetSetDestroy (ObmObject obj)
{
	int i;
	ObmObject child;
	int object_type;

	for (i=0;  i < obj->core.nchildren;  i++) {
	    child = obj->core.children[i];
	    object_type = child->core.classrec->object_type;
	    if (object_type == OtShell || object_type == OtNonShell)
		widgetSetDestroy (child);
	}

	((WidgetObject)obj)->widget.widget_destroyed = True;
}


/* widgetDestroy -- Destroy a widget and all of its descendents.  We can't
 * just call XtDestroyWidget to do this because while this will destroy all
 * the normal and popup children of a widget, it won't destroy any top level
 * shells and their children.
 */
static void
widgetDestroy (ObmObject obj)
{
	int i;
	WidgetObject wobj = (WidgetObject) obj;
	WidgetClass *widget_class;
	ObmObject child;
	int object_type;

	for (i=0;  i < obj->core.nchildren;  i++) {
	    child = obj->core.children[i];
	    widget_class = child->core.classrec->widget_class;
	    if (widget_class == &topLevelShellWidgetClass)
		widgetDestroy (child);
	}

	XtUnrealizeWidget (wobj->widget.w);
	XtDestroyWidget (wobj->widget.w);
}


/* WidgetEvaluate -- Evaluate a widget command or message.
 */
int
WidgetEvaluate (ObmObject object, const char *command)
{
	WidgetObject obj = (WidgetObject) object;
	Tcl_Interp *tcl, *server = obj->widget.obm->tcl;
	MsgContext omsg = (MsgContext) obj->core.classrec->class_data;
	MsgContext pmsg = (MsgContext) obj->core.superclass->class_data;

	/* Since the class wide interpreter is used to evaluate the message
	 * we can't pass the object descriptor directly to the class procedure
	 * referenced in the message.  Instead we pass the object reference
	 * in the message descriptor.
	 */
	Tcl_SetResult (server, "", TCL_STATIC);

	/* First try to get the widget subclass to accept the message. */
	if (omsg && (tcl = omsg->tcl) && obmClientCommand(tcl,command)) {
	    omsg->object[++omsg->level] = object;
	    if (Tcl_Eval (tcl, command) == TCL_OK) {
		if (*Tcl_GetStringResult (tcl))
		  Tcl_SetResult (server, (char *) Tcl_GetStringResult (tcl),
				 TCL_VOLATILE);
		omsg->level--;
		return (TCL_OK);

	    } else {
		static char invalid[] = "invalid command name";
		omsg->level--;

		/* Exit with an error return if the class code recognized
		 * the command but failed to execute it.
		 */
		if (strncmp (Tcl_GetStringResult (tcl), invalid, strlen(invalid)) != 0)
		    goto error;
	    }
	}

	/* If the subclass code did not recognize the command pass the
	 * message on to the base Widget class.
	 */
	if (pmsg && pmsg != omsg && (tcl = pmsg->tcl) &&
		obmClientCommand(tcl,command)) {
	    pmsg->object[++pmsg->level] = object;
	    if (Tcl_Eval (tcl, command) == TCL_OK) {
	    	if (*Tcl_GetStringResult (tcl))
		    Tcl_SetResult (server, (char *) Tcl_GetStringResult (tcl),
				   TCL_VOLATILE);
		pmsg->level--;
		return (TCL_OK);
	    } else
		pmsg->level--;
	}

error:
	if (*Tcl_GetStringResult (tcl))
	    Tcl_SetResult (server, (char *) Tcl_GetStringResult (tcl), TCL_VOLATILE);
	else {
	    /* Supply a default error message if none was returned. */
	    Tcl_SetResult (server, obmClientCommand (tcl, command) ?
		"evaluation error" : "invalid command", TCL_VOLATILE);
	}
	Tcl_SetErrorLine (server, Tcl_GetErrorLine (tcl));
	return (TCL_ERROR);
}


/* widgetGetPointer -- Return the widget descriptor for an object of class
 * widget.  Used by non-widget Obm code to get the widget handle from an
 * object descriptor.
 */
Widget
widgetGetPointer (ObmObject object)
{
	WidgetObject obj = (WidgetObject) object;
	return (obj->widget.w);
}


/* widgetToObject -- Convert a widget pointer to an OBM object name.
 */
WidgetObject
widgetToObject (ObmContext obm, Widget w)
{
	int i;
	WidgetPrivate wp;
        ObmObject objs[256];
	int nobjs;

	obm_nameToObjectList (obm, XtName(w), NULL, &nobjs, objs);
	for (i=0;  i < nobjs;  i++)
	    wp = &((WidgetObject)objs[i])->widget;
	    if (wp->w == w)
		return ((WidgetObject)objs[i]);

	return (NULL);
}


/* widgetAddCallback -- Add a callback procedure to the callback list for
 * a widget.  If no callback name is given, "callback" is assumed.
 *
 *  Usage:	addCallback <procedure-name> [<callback-name>]
 *
 * Specific widgets only support certain types of callbacks.  There is no
 * checking that the callback type specified is supported by a widget; the
 * wrong type of callback can be registered, but it will never be called.
 */
static int 
widgetAddCallback (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmCallback cb, new_cb;
	char *s_proc, *s_type;
	int callback_type, i;

	s_proc = argv[1];
	s_type = (argc > 2) ? argv[2] : NULL;

	/* Determine callback type. */
	if (s_type) {
	    callback_type = Ctcallback;
	    for (i=0;  i < XtNumber(callbackTypes);  i++)
		if (strcmp (s_type, callbackTypes[i].name) == 0) {
		    callback_type = callbackTypes[i].type;
		    break;
		}
	} else if (obmClass (obj->core.classrec, WtAsciiText)) {
	    callback_type = Ctlinemode;
	} else if (obmClass (obj->core.classrec, WtPanner) ||
		   obmClass (obj->core.classrec, WtPorthole) ||
		   obmClass (obj->core.classrec, WtViewport)) {
	    callback_type = CtreportCallback;
	} else if (obmClass (obj->core.classrec, WtSlider2d) ||
		   obmClass (obj->core.classrec, WtScrollbar2) ||
		   obmClass (obj->core.classrec, WtScrollbar)) {
	    callback_type = CtjumpProc;
	} else
	    callback_type = Ctcallback;

	/* Special handling for asciiText callbacks. */
	if (obmClass (obj->core.classrec, WtAsciiText))
	    if (callback_type == Ctlinemode) {
		char text_translations[SZ_LINE];
		XtTranslations translations;
		sprintf (text_translations, "<Key>Return: do_text(%p, %s) ",
		    wp->obm, XtName(wp->w));
		translations = XtParseTranslationTable (text_translations);
		XtOverrideTranslations (wp->w, translations);
	    } else {
		XtAddCallback (XawTextGetSource(wp->w), XtNcallback,
		    (XtCallbackProc) widgetCallback, obj);
	    }

	/* Create callback record. */
	new_cb = (ObmCallback) XtCalloc (1, sizeof (obmCallback));
	new_cb->callback_type = callback_type;
	strncpy (new_cb->name, s_proc, SZ_NAME);

	/* Add callback to tail of callback list. */
	if (wp->callback) {
	    for (cb = wp->callback;  cb->next;  cb = cb->next)
		;
	    cb->next = new_cb;
	} else
	    wp->callback = new_cb;

	return (TCL_OK);
}


/* widgetDeleteCallback -- Delete a callback procedure previously registered
 * for a widget.
 *
 *  Usage:	deleteCallback <procedure-name>
 */
static int 
widgetDeleteCallback (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmCallback cb, prev;

	/* Locate and delete procedure entry in callback list. */
	for (prev=NULL, cb=wp->callback;  cb;  prev=cb, cb=cb->next)
	    if (strcmp (cb->name, argv[1]) == 0) {
		if (prev)
		    prev->next = cb->next;
		else
		    wp->callback = cb->next;
		XtFree ((char *)cb);
		break;
	    }

	return (TCL_OK);
}


/* widgetCallback -- Generic callback procedure, used for most widgets.
 * The callback procedure is called with the widget name as the first
 * argument, followed by zero or more additional arguments which depend upon
 * the callback type.
 */
static void
widgetCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	ObjClassRec classrec = obj->core.classrec;
	char buffer[SZ_COMMAND];
	char *message = buffer;
	int callback_type, i;

	/* Default callback type. */
	callback_type = Ctcallback;

	if (obmClass (classrec, WtAsciiSrc) ||
	    obmClass (classrec, WtAsciiText)) {

	    char *string;
	    Arg args[1];

	    XtSetArg (args[0], "string", &string);
	    XtGetValues (wp->w, args, 1);
	    sprintf (message, "{%s}", string);
	    callback_type = Ctcharmode;

	} else if (obmClass (classrec, WtGrip)) {
	    GripCallDataRec *grip = (GripCallDataRec *) call_data;

	    /* The message, if any, is given in the grip translation table
	     * as the arguments to the GripAction routine.  These differ
	     * depending upon the event; we just pass on the arguments and
	     * ignore the event attributes.
	     */
	    message[0] = '\0';
	    for (i=0;  i < grip->num_params;  i++) {
		strcat (message, " ");
		strcat (message, grip->params[i]);
	    }

	} else if (obmClass (classrec, WtList)) {
	    XawListReturnStruct *list = (XawListReturnStruct *) call_data;

	    /* The message is the string value of the list element
	     * selected, followed by its index.
	     */
	    sprintf (message, "{%s} %d", list->string, list->list_index);

	} else if (obmClass (classrec, WtMultiList)) {
	    XfwfMultiListReturnStruct *list =
		(XfwfMultiListReturnStruct *) call_data;
	    Boolean state, sensitive;
	    char *ip, *op;
	    int buflen, need, i;
	    char *string;

	    /* The message consists of an array of string values of the
	     * currently selected list elements in the form { {s} {s} ...},
	     * followed by a array of the indices { n n ...}.
	     */
	    buflen = SZ_COMMAND;
	    if (!(message = XtMalloc (buflen)))
		return;

	    /* Generate list of item strings. */
	    op = message;
	    *op++ = '{';
	    for (i=0;  i < list->num_selected;  i++) {
		XfwfMultiListGetItemInfo ((XfwfMultiListWidget)wp->w,
		    list->selected_items[i], &string, &state, &sensitive);
		need = strlen(string)+3 + list->num_selected * 6;
		if (buflen < (op-message)+need) {
		    buflen += max (need, SZ_COMMAND);
		    if (!(message = XtRealloc (buffer, buflen)))
			return;
		}
		*op++ = ' ';
		*op++ = '{';
		for (ip=string;  (*op = *ip++);  op++)
		    ;
		*op++ = '}';
	    }
	    *op++ = ' ';
	    *op++ = '}';

	    /* Append list of indices.  We allocated space for these above. */
	    *op++ = ' ';
	    *op++ = '{';
	    for (i=0;  i < list->num_selected;  i++) {
		sprintf (op, " %d", list->selected_items[i]);
		while (*op)
		    op++;
	    }
	    *op++ = ' ';
	    *op++ = '}';
	    *op++ = '\0';

	} else if (obmClass (classrec, WtToggle)) {
	    Arg args[1];
	    Boolean state;

	    /* The callback for a toggle does not pass any call data,
	     * but we return the value of the "state" resource anyway
	     * to indicate the state of the toggle.
	     */
	    XtSetArg (args[0], XtNstate, &state);
	    XtGetValues (wp->w, args, 1);
	    sprintf (message, "%s", state ? TRUESTR : FALSESTR);

	} else if (obmClass (classrec, WtTextToggle)) {
	    Arg args[1];
	    Boolean state;

	    /* For this widget the value of the "on" resource indicates
	     * the state of the toggle.
	     */
	    XtSetArg (args[0], XtNon, &state);
	    XtGetValues (wp->w, args, 1);
	    sprintf (message, "%s", state ? TRUESTR : FALSESTR);

	} else {
	    /* The default case, which works for most simple callbacks.
	     * Only the widget name is returned.
	     */
	    message = NULL;
	}

	call_callbacks (obj, callback_type, message);

	if (message && message != buffer)
	    XtFree (message);
}


/* widgetRGCallback -- Radiogroup or Group callback.  The argument list for
 * the callback is one of the following:
 *
 * selectionStyle = multiple:	widget-name { label label ... }
 * selectionStyle = one,single:	widget-name [label | "none"]
 *
 * Here label refers to the label of the selected TextToggle widget or widgets.
 * In the case of selectionStyle=multiple, the list will be empty if no widgets
 * are currently selected.
 */
static void
widgetRGCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	long selection = (long) call_data;
	char *op;
	int i;

	char message[SZ_COMMAND];
	SelectionType selectionType;
	WidgetList children;
	Cardinal nchildren;
	Arg args[10];
	char *label;

	/* Get list of child widgets in the group. */
	XtSetArg (args[0], XtNselectionStyle, &selectionType);
	XtSetArg (args[1], XtNnumChildren, &nchildren);
	XtSetArg (args[2], XtNchildren, &children);
	XtGetValues (wp->w, args, 3);

	op = message;
	if (selectionType == XfwfMultipleSelection) {
	    *op++ = '{';
	    for (i=0;  selection > 0 && i < min(32,nchildren);  i++)
		if (selection & (1 << i)) {
		    XtSetArg (args[0], XtNlabel, &label);
		    XtGetValues (children[i], args, 1);
		    *op++ = ' ';
		    sprintf (op, "\"%s\"", label);
		    while (*op)
			op++;
		}
	    *op++ = '}';

	} else {
	    if (selection < 0 || selection >= nchildren)
		label = "none";
	    else {
		XtSetArg (args[0], XtNlabel, &label);
		XtGetValues (children[selection], args, 1);
	    }
	    sprintf (op, "\"%s\"", label);
	    while (*op)
		op++;
	}
	*op++ = '\0';

	call_callbacks (obj, Ctcallback, message);
}


/* widgetLTHCallback -- ListTree highlight callback.
 */
static void
widgetLTHCallback (Widget w, WidgetObject obj, void * call_data)
{
	ListTreeMultiReturnStruct *list;
	ListTreeItem *item;
	char message[SZ_COMMAND], buf[SZ_LINE];
	int i;

	list = (ListTreeMultiReturnStruct *) call_data;
	if (!list->items)
	    return;


	/* The message is the string value of the list element selected
	 * and a bottom-up path to the root.
	 */
	sprintf (message, "{%s %d} ",
	    list->items[0]->text, list->items[0]->open);

	strcat (message, "{ ");
	for (i=0; i < list->count; i++) {
	    item = list->items[i];
	    sprintf (buf, "{ %s } ", item->text);
	    strcat (message, buf);
	    while (item->parent) {
		item = item->parent;
		sprintf (buf, "{ %s } ", item->text);
		strcat (message, buf);
	    }
	}
	strcat (message, "}");

	call_callbacks (obj, Ctcallback, message);
}


/* widgetLTACallback -- ListTree activate callback.
 */
static void
widgetLTACallback (Widget w, WidgetObject obj, void * call_data)
{
	ListTreeActivateStruct *ret;
	ListTreeMultiReturnStruct ret2;
	ListTreeItem *item;
	char message[SZ_COMMAND], buf[SZ_LINE];
	int i, count;

        ret = (ListTreeActivateStruct *) call_data;

	/* The message is the string value of the list element selected,
	 * and a bottom-up path to the root.
	 */
	sprintf (message, "{%s %d} ", ret->item->text, ret->item->open);

	strcat (message, "{ ");
	item = ret->item;
	sprintf (buf, "{ %s } ", item->text);
	strcat (message, buf);
	while (item->parent) {
	    item = item->parent;
	    sprintf (buf, "{ %s } ", item->text);
	    strcat (message, buf);
        }
	strcat (message, "}");

	call_callbacks (obj, Ctcallback, message);
}


/* widgetSBCallback -- Repeater start callback.
 */
static void
widgetSBCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	call_callbacks (obj, CtstartCallback, NULL);
}


/* widgetSECallback -- Repeater stop callback.
 */
static void
widgetSECallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	call_callbacks (obj, CtstopCallback, NULL);
}


/* widgetRPCallback -- Report callback used by the panner, porthole, and
 * viewport widgets to report any changes in the position or size of the
 * thumb (panner) or child widget (porthole, viewport).
 */
static void
widgetRPCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	XawPannerReport *rp = (XawPannerReport *) call_data;
	char message[100];

	/* Return args: changed x y w h cw ch */
	sprintf (message, "0%o %d %d %d %d %d %d", rp->changed,
	    rp->slider_x, rp->slider_y, rp->slider_width, rp->slider_height,
	    rp->canvas_width, rp->canvas_height);

	call_callbacks (obj, CtreportCallback, message);
}


/* widgetJPCallback -- Jump callback for the scroll bar widget.  This is
 * called when the thumb port of the scroll bar is dragged or moved (button 2).
 */
static void
widgetJPCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	XfwfScrollInfo *info = (XfwfScrollInfo *) call_data;
	XfwfScrollInfo update_info;
	int flags = info->flags;
	char message[100];

	if (obmClass (obj->core.classrec, WtScrollbar)) {
	    /* Athena scrollbar.  The call_data gives the position of the
	     * thumb in percent.
	     */
	    sprintf (message, "%0.6f", *((float *)call_data));
	    call_callbacks (obj, CtjumpProc, message);

	} else if (info->reason != XfwfSDrag) {
	    /* Slider2d callback:	widget-name x y 
	     * Scrollbar2 callback:	widget-name fraction
	     *
	     * Scrollbars return the same fraction value regardless of
	     * whether the scrollbar is vertical or horizontal.
	     * widgetSPCallback below is always called when the slider moves,
	     * so we don't need to call response_cb here.
	     */
	    if (obmClass (obj->core.classrec, WtSlider2d)) {
		XfwfGetThumb (wp->w, &update_info);
		sprintf (message, "%0.5f %0.5f",
		    (flags & XFWF_HPOS) ? info->hpos : update_info.hpos,
		    (flags & XFWF_VPOS) ? info->vpos : update_info.vpos);
	    } else {
		if (info->flags & XFWF_HPOS)
		    sprintf (message, "%0.5f", info->hpos);
		else if (info->flags & XFWF_VPOS)
		    sprintf (message, "%0.5f", info->vpos);
	    }

	    /* Call the callbacks. */
	    call_callbacks (obj, CtjumpProc, message);
	}
}


/* widgetSPCallback -- Scroll callback for the scroll bar widget.  This is
 * used for incremental scrolling (button 1 or 3).
 */
static void
widgetSPCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	char message[100];

	if (obmClass (obj->core.classrec, WtScrollbar)) {
	    /* Athena scrollbar.  The call_data gives the distance in pixels
	     * of the pointer from the top of the scrollbar, and is positive
	     * for button 1 and negative for button 3.
	     */
	    sprintf (message, "%d", (int)call_data);
	    call_callbacks (obj, CtscrollProc, message);

	} else {
	    /* FWF scrollbar2 or slider2d.
	     */
	    XfwfScrollInfo *info = (XfwfScrollInfo *) call_data;
	    XfwfScrollInfo update_info;
	    int flags = info->flags;

	    /* Slider2d callback:   widget-name x y 
	     * Scrollbar2 callback: widget-name fraction
	     *
	     * Scrollbars return the same fraction value regardless of
	     * whether the scrollbar is vertical or horizontal.
	     */
	    if (obmClass (obj->core.classrec, WtSlider2d)) {
		XfwfGetThumb (wp->w, &update_info);
		sprintf (message, "%0.5f %0.5f",
		    (flags & XFWF_HPOS) ? info->hpos : update_info.hpos,
		    (flags & XFWF_VPOS) ? info->vpos : update_info.vpos);
	    } else {
		if (flags & XFWF_HPOS)
		    sprintf (message, "%0.5f", info->hpos);
		else if (flags & XFWF_VPOS)
		    sprintf (message, "%0.5f", info->vpos);
	    }

	    /* Call the callbacks. */
	    call_callbacks (obj, CtscrollProc, message);

	    /* Update the slider. */
	    update_info = *info;
	    update_info.reason = XfwfSNotify;
	    wp->response_cb (NULL, w, (void *) &update_info);
	}
}


/* widgetSCCallback -- Strip chart callback procedure.  This is called by the
 * strip chart widget every "update" seconds to get the next value to be
 * plotted.
 */
static void
widgetSCCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	char *callback_name;
	ObmCallback cb;
	int status, i;

	callback_name = "getValue";
	for (i=0;  i < XtNumber(callbackTypes);  i++)
	    if (callbackTypes[i].type == CtgetValue) {
		callback_name = callbackTypes[i].name;
		break;
	    }

	/* Call the callback procedure to get the next value to be plotted
	 * in the strip chart.  This is a numeric (e.g. floating point) value
	 * returned as the function value of the callback procedure.  Multiple
	 * callbacks can be registered, but only the first such procedure
	 * will be called.
	 */
	for (cb = wp->callback;  cb;  cb = cb->next) {
	    if (cb->callback_type != CtgetValue)
		continue;
	    status = Tcl_VarEval (obm->tcl,
		cb->name, " ",
		obj->core.name, " ",
		callback_name, " ",
		NULL);
	    if (status == TCL_OK)
	        *((double *)call_data) = atof (Tcl_GetStringResult (obm->tcl));
	    else {
		const char *errstr = Tcl_GetVar (obm->tcl, "errorInfo", 0);
		fprintf (stderr, "Error on line %d in %s: %s\n",
		    Tcl_GetErrorLine (obm->tcl), cb->name,
		    errstr ? errstr : Tcl_GetStringResult (obm->tcl));
	    }
	    break;
	}
}


/* widgetPUCallback -- Popup callback, used by the shell and simpleMenu
 * widgets.  Called when the window pops up.
 */
static void
widgetPUCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	call_callbacks (obj, CtpopupCallback, NULL);
}


/* widgetPDCallback -- Popdown callback, used by the shell and simpleMenu
 * widgets.  Called when the window pops down.
 */
static void
widgetPDCallback (Widget w, WidgetObject obj, void * call_data)
{
	WidgetPrivate wp = &obj->widget;
	call_callbacks (obj, CtpopdownCallback, NULL);
}


/* call_callbacks -- Call all the callbacks of the given type for the given
 * widget object, passing the given message on the argument list.
 */
static void
call_callbacks (WidgetObject obj, int callback_type, char *message)
{
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	ObmCallback cb;
	char *callback_name;
	int status, i;

	callback_name = callbackTypes[0].name;
	for (i=0;  i < XtNumber(callbackTypes);  i++)
	    if (callback_type == callbackTypes[i].type) {
		callback_name = callbackTypes[i].name;
		break;
	    }

	/* Deliver the message to all the callback procedures registered for
	 * this widget for which the callback type matches.  The callback
	 * arguments are:
	 *
	 * 	widget-name callback-name callback-args
	 *
	 * where the callback-args depend on the callback.
	 */
	for (cb = wp->callback;  cb;  cb = cb->next) {
	    if (cb->callback_type != callback_type)
		continue;

	    if (message) {
		status = Tcl_VarEval (obm->tcl,
		    cb->name, " ",
		    obj->core.name, " ",
		    callback_name, " ",
		    message, " ",
		    NULL); 
	    } else {
		status = Tcl_VarEval (obm->tcl,
		    cb->name, " ",
		    obj->core.name, " ",
		    callback_name, " ",
		    NULL); 
	    }

	    if (status != TCL_OK) {
		const char *errstr = Tcl_GetVar (obm->tcl, "errorInfo", 0);
		fprintf (stderr, "Error on line %d in %s: %s\n",
		    Tcl_GetErrorLine (obm->tcl), cb->name,
		     errstr ? errstr : Tcl_GetStringResult (obm->tcl));
	    }
	}
}


/* do_text -- Translation action procedure for the text widget, called when
 * return is typed to process an input string ("linemode" callback type).
 */
static void
do_text (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	char *message, *s;
	WidgetObject obj;
	ObmCallback cb;
	Arg args[1];

	/* do_text (0xXXXX, object-name) */
	if (*num_params >= 2) {
	    ObmContext obm = (ObmContext) strtol (params[0], NULL, 0);
	    char *object = params[1];

	    if (obm && (obj = (WidgetObject) obmFindObject (obm, object))) {
		XtSetArg (args[0], XtNstring, &s);
		XtGetValues (obj->widget.w, args, 1);

		if (!(message = XtMalloc (strlen(s) + 10)))
		    return;
		sprintf (message, "{%s}", s);

		call_callbacks (obj, Ctlinemode, message);
		XtFree (message);
	    }
	}
}


/* do_userproc -- Translation action procedure used to call general user
 * action procedures in the interpreter.  The name of the user procedure to
 * be called is given as the first argument in the translation.  For example,
 * the translation "call(foo,a,b,c)" would cause procedure foo to be called
 * with the arguments (a,b,c).  The following arguments are special:
 *
 *	Argument	Replaced by
 *
 *	$name		translation table name (defaults to widget name)
 *	$time		event->time
 *	$x		event->x
 *	$y		event->y
 *	$x_root		event->x_root
 *	$y_root		event->y_root
 *
 * The "user procedure" can be any server procedure.
 */
static void
do_userproc (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	char *ip, *op;
	ObmContext obm = global_obm_handle;
	char cmd[SZ_COMMAND], *param;
	int x, y, x_root, y_root;
	int status, arg;
	Time time;

	if (*num_params < 1)
	    return;

	time = 0;
	x = y = 0;
	x_root = y_root = 0;

	/* Get common event parameters. */
	switch (event->type) {
	case KeyPress:
	case KeyRelease:
	    {	XKeyEvent *ev = (XKeyEvent *) event;
		time = ev->time;
		x = ev->x;  y = ev->y;
		x_root = ev->x_root;  y_root = ev->y_root;
	    }
	    break;
	case ButtonPress:
	case ButtonRelease:
	    {	XButtonEvent *ev = (XButtonEvent *) event;
		time = ev->time;
		x = ev->x;  y = ev->y;
		x_root = ev->x_root;  y_root = ev->y_root;
	    }
	    break;
	case MotionNotify:
	    {	XMotionEvent *ev = (XMotionEvent *) event;
		time = ev->time;
		x = ev->x;  y = ev->y;
		x_root = ev->x_root;  y_root = ev->y_root;
	    }
	    break;
	case EnterNotify:
	case LeaveNotify:
	    {	XCrossingEvent *ev = (XCrossingEvent *) event;
		time = ev->time;
		x = ev->x;  y = ev->y;
		x_root = ev->x_root;  y_root = ev->y_root;
	    }
	    break;
	}

	/* Copy name of server procedure to be called. */
	for (ip=params[0], op=cmd;  *ip;  )
	    *op++ = *ip++;
	*op++ = ' ';

	/* Copy the remaining arguments. */
	for (arg=1;  arg < *num_params;  arg++) {
	    param = params[arg];
	    if (*param == '$') {
		if (strcmp (param, "$name") == 0) {
		    /* Return the current translation table name for the
		     * widget (defaults to the widget name).
		     */
		    WidgetObject obj;
		    char *name;

		    if ((obj = widgetToObject (obm, w)))
			name = obj->widget.translation_table_name;
		    else
			name = XtName (w);

		    for (ip = name;  *ip;  )
			*op++ = *ip++;
		    *op++ = ' ';

		} else if (strcmp (param, "$time") == 0) {
		    sprintf (op, "%lu ", time);
		    while (*op)
			op++;
		} else if (strcmp (param, "$x") == 0) {
		    sprintf (op, "%d ", x);
		    while (*op)
			op++;
		} else if (strcmp (param, "$y") == 0) {
		    sprintf (op, "%d ", y);
		    while (*op)
			op++;
		} else if (strcmp (param, "$x_root") == 0) {
		    sprintf (op, "%d ", x_root);
		    while (*op)
			op++;
		} else if (strcmp (param, "$y_root") == 0) {
		    sprintf (op, "%d ", y_root);
		    while (*op)
			op++;
		} else {
		    for (ip=param;  *ip;  )
			*op++ = *ip++;
		    *op++ = ' ';
		}
	    } else {
		for (ip=param;  *ip;  )
		    *op++ = *ip++;
		*op++ = ' ';
	    }
	}

	*op = '\0';
	status = Tcl_Eval (obm->tcl, cmd);
	if (status != TCL_OK) {
	    fprintf (stderr, "Error on line %d of %s: %s\n",
		Tcl_GetErrorLine (obm->tcl), params[0], Tcl_GetStringResult (obm->tcl));
	}
}


/* widgetSetTTName -- Set the translation table name for a widget.  This
 * is the name passed in the $name field of a translation table action
 * procedure called with the "call" action from a translation table.
 *
 * Usage:	setTTName name
 *
 * The default translation table name is the name of the widget.  Note that
 * some widget subclasses (e.g. marker) may set the translation table name
 * automatically when the widget changes the translation table.
 */
static int 
widgetSetTTName (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	if (argc < 2)
	    return (TCL_ERROR);
	
	widget_setTTName (obj, argv[1]);
	return (TCL_OK);
}

void
widget_setTTName (WidgetObject obj, char *name)
{
	WidgetPrivate wp = &obj->widget;
	strncpy (wp->translation_table_name, name, SZ_NAME);
	wp->translation_table_name[SZ_NAME-1] = '\0';
}


/* widgetGetTTName -- Get the translation table name for a widget.
 *
 * Usage:	name = getTTName
 */
static int 
widgetGetTTName (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	Tcl_SetResult (wp->obm->tcl, widget_getTTName(obj), TCL_VOLATILE);
	return (TCL_OK);
}

char *
widget_getTTName (WidgetObject obj)
{
	WidgetPrivate wp = &obj->widget;
	return (wp->translation_table_name);
}


/* do_popup -- Popup a menu (or other spring loaded popup) at the location
 * of the event which triggered this action.
 *
 *    Usage:  popup(menu-name [xoffset [yoffset]])
 */
static void
do_popup (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	char *ip, *op;
	ObmContext obm = global_obm_handle;
	XKeyEvent *ev = (XKeyEvent *) event;
	Boolean spring_loaded;
	Dimension menu_width, menu_height, menu_borderWidth;
	Position menu_x, menu_y;
	int xoffset, yoffset;
	char *menu_name;
	WidgetObject obj;
	Widget menu;

	if (*num_params < 1)
	    return;

	menu_name = params[0];
	xoffset = (*num_params >= 2) ? atoi(params[1]) : -10;
	yoffset = (*num_params >= 3) ? atoi(params[2]) : -10;

	if (!(obj = (WidgetObject) obmFindObject (obm, menu_name)))
	    return;
	else
	    menu = obj->widget.w;

	/* Evidently SimpleMenu requires that the following be called to
	 * properly initialize things.
	 */
	if (obmClass (obj->core.classrec, WtSimpleMenu))
	    XtCallActionProc (XtParent(menu), "XawPositionSimpleMenu",
		event, params, *num_params);

	XtVaGetValues (menu,
	    XtNwidth,		&menu_width,
	    XtNheight,		&menu_height,
	    XtNborderWidth,	&menu_borderWidth,
	    NULL);

	menu_width = menu_width + 2 * menu_borderWidth;
	menu_height = menu_height + 2 * menu_borderWidth;
	menu_x = ev->x_root + xoffset;
	menu_y = ev->y_root + yoffset;

	if (menu_x >= 0) {
	    int scr_width = WidthOfScreen(XtScreen(menu));
	    if ((int)(menu_x + menu_width) > scr_width)
		menu_x = scr_width - menu_width;
	}
	if (menu_x < 0)
	    menu_x = 0;

	if (menu_y >= 0) {
	    int scr_height = HeightOfScreen(XtScreen(menu));
	    if ((int)(menu_y + menu_height) > scr_height)
		menu_y = scr_height - menu_height;
	}
	if (menu_y < 0)
	    menu_y = 0;

	XtVaSetValues (menu,
	    XtNx,  menu_x,
	    XtNy,  menu_y,
	    NULL);

	if (event->type == ButtonPress)
	    spring_loaded = True;
	else if (event->type == KeyPress || event->type == EnterNotify)
	    spring_loaded = False;
	else {
	    /* should not happen. */
	    spring_loaded = False;
	}

	if (spring_loaded)
	    XtPopupSpringLoaded (menu);
	else
	    XtPopup (menu, XtGrabNonexclusive);
}


/* do_popdown -- Pop down a menu.
 *
 *    Usage:  popdown(menu-name)
 */
static void
do_popdown (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	char *ip, *op;
	ObmContext obm = global_obm_handle;
	XKeyEvent *ev = (XKeyEvent *) event;
	char *menu_name;
	WidgetObject obj;
	Widget menu;

	if (*num_params < 1)
	    return;

	menu_name = params[0];
	if ((obj = (WidgetObject) obmFindObject (obm, menu_name))) {
	    menu = obj->widget.w;
	    XtPopdown (menu);
	}
}


/* widgetSet -- Set a widget resource.
 *
 *  Usage:	set <resource-name> <value>
 */
static int 
widgetSet (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	char *ip;
	int hashval, n;
	Rtype rp;
	XrmValue from, to;
	Arg args[1];

	/* Lookup resource. There can be multiple entries for a given resource
	 * if the resource has a different type in different widgets.  A
	 * resource entry can be used only if both the name matches and the
	 * bitflags indicate that the target widget uses the resource.
	 */
	for (hashval=0, ip=argv[1], n=MAX_HASHCHARS;  --n >= 0 && *ip;  ip++)
	    hashval += (hashval + *ip);

	if ((rp = rhash[hashval%LEN_RHASH])) {
	    for (  ;  rp;  rp = rp->next)
		if (!strcmp(rp->name,argv[1]) &&
		    obmClass (obj->core.classrec, rp->flag1, rp->flag2))
		    break;
	}

	/* For a string type resource, no value string arg indicates the
	 * null string.
	 */
	if (!rp || argc < 3 && strcmp (rp->type, XtRString))
	    return (TCL_ERROR);

	/* If the resource entry was found, convert the resource value
	 * from a string to whatever the resource type is, and set the
	 * resource value.
	 */
	from.size = strlen (argv[2]) + 1;
	from.addr = argv[2];

	if (strcmp (rp->type, XtRString) == 0) {
	    XtSetArg (args[0], rp->name, argc < 3 ? "" : argv[2]);
	    XtSetValues (wp->w, args, 1);

	    /* The following is for text widgets. */
	    if (obmClass (obj->core.classrec, WtAsciiText)) {
		ObmCallback cb;

		wp->text_newline = 0;
		wp->text_pos = strlen (argv[2]);

		/* If linemode is in effect set insertion point to EOL. */
		for (cb = wp->callback;  cb;  cb = cb->next)
		    if (cb->callback_type == Ctlinemode) {
			XawTextSetInsertionPoint (wp->w, wp->text_pos);
			break;
		    }
	    }

	} else if (strcmp (rp->type, XtRDimension) == 0 ||
		   strcmp (rp->type, XtRPosition) == 0) {

	    Dimension value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRBool) == 0 ||
		   strcmp (rp->type, XtRBoolean) == 0) {

	    Boolean value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value.  The numeric values "0" and "1" are
	     * permitted as well as the resource values "true" and "false".
	     */
	    if (strcmp (from.addr, "0") == 0) {
		value = False;
		goto set_bval;
	    } else if (strcmp (from.addr, "1") == 0) {
		value = True;
		goto set_bval;
	    } else if (XtConvertAndStore (wp->w,XtRString,&from,rp->type,&to)) {
set_bval:	XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRAtom) == 0 ||
		   strcmp (rp->type, XtRCardinal) == 0 ||
		   strcmp (rp->type, XtRInt) == 0) {

	    int value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRFloat) == 0) {
	    union {
		int int_value;
		float float_value;
	    } value;

	    to.addr = (void *) &value.float_value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value.int_value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRPixel) == 0) {
	    Pixel value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRPixmap) == 0 ||
		   strcmp (rp->type, XtRBitmap) == 0) {

	    Pixmap value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if ((value = findPixmap (obm, (char *)from.addr)) ||
		XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {

		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRIcon) == 0) {
	    Icon *value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if ((value = findIcon (obm, (char *)from.addr)) ||
		XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {

		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRCursor) == 0) {
	    Cursor value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if ((value = findCursor (obm, (char *)from.addr)) ||
		XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {

		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRFontStruct) == 0) {
	    XFontStruct *value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRVisual) == 0) {
	    Visual *value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRWidget) == 0) {
	    Widget value;

	    /* Convert resource value. */
	    if ((value = XtNameToWidget (obm->toplevel, argv[2]))) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }

	} else if (strcmp (rp->type, XtRTranslationTable) == 0) {
	    XtTranslations value;

	    /* Convert resource value. */
	    value = XtParseTranslationTable (argv[2]);
	    XtSetArg (args[0], rp->name, value);
	    XtSetValues (wp->w, args, 1);

	} else {
	    void * value;
	    to.addr = (void *) &value;
	    to.size = sizeof(value);

	    /* Convert resource value. */
	    if (XtConvertAndStore (wp->w, XtRString,&from, rp->type,&to)) {
		XtSetArg (args[0], rp->name, value);
		XtSetValues (wp->w, args, 1);
	    }
	}

	return (TCL_OK);
}


/* widgetGet -- Get a widget resource value as a string.
 *
 *  Usage:	get <resource-name>
 */
static int 
widgetGet (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	char rbuf[SZ_MESSAGE];
	char *result = rbuf;
	char *ip;
	int hashval, n;
	Rtype rp;

	if (argc < 2)
	    return (TCL_ERROR);

	/* Lookup widget. There can be multiple entries for a given resource
	 * if the resource has a different type in different widgets.  A
	 * resource entry can be used only if both the name matches and the
	 * bitflags indicate that the target widget uses the resource.
	 */
	for (hashval=0, ip=argv[1], n=MAX_HASHCHARS;  --n >= 0 && *ip;  ip++)
	    hashval += (hashval + *ip);

	if ((rp = rhash[hashval%LEN_RHASH])) {
	    for (  ;  rp;  rp = rp->next)
		if (!strcmp(rp->name,argv[1]) &&
		    obmClass (obj->core.classrec, rp->flag1, rp->flag2))
		    break;
	}
	if (!rp)
	    return (TCL_ERROR);

	/* Return the resource value as a string.  In general type converters
	 * are only registered for string-to-whatever conversions, so we
	 * cannot reproduce the original string value for all resource types.
	 * We can return a valid string value for the simple types (boolean,
	 * integer, floating, string).  An attempt is made to convert widget
	 * pointers to widget names.  For everything else, we just return
	 * the hex representation of the resource value.
	 */
	if (strcmp (rp->type, XtRBool) == 0 ||
	    strcmp (rp->type, XtRBoolean) == 0) {

	    Boolean value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    strcpy (result, value ? TRUESTR : FALSESTR);

	} else if (
	    strcmp (rp->type, XtRDimension) == 0 ||
	    strcmp (rp->type, XtRPosition) == 0) {

	    short value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    sprintf (result, "%d", value);

	} else if (
	    strcmp (rp->type, XtRAtom) == 0 ||
	    strcmp (rp->type, XtRCardinal) == 0 ||
	    strcmp (rp->type, XtRInt) == 0) {
	    
	    int value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    sprintf (result, "%d", value);

	} else if (strcmp (rp->type, XtRFloat) == 0) {
	    float value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    sprintf (result, "%g", value);

	} else if (strcmp (rp->type, XtRString) == 0) {
	    char *value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    result = (char *)value;

	} else if (strcmp (rp->type, XtRWidget) == 0) {
	    Widget value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    result = XtName (value);

	} else if (strcmp (rp->type, XtRFontStruct) == 0) {	/* MF016 */
	    void * value;  Arg args[1];
  	    XFontStruct *font_struct;
  	    ObmContext obm = wp->obm;
  	    char *name = NULL;

	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    sprintf (result, "%p", value);

  	    font_struct = (XFontStruct *) value;
  	    name = widgetGetFontName (obm->display, font_struct);

  	    if (font_struct == NULL || name == NULL)
	        name = XtNewString("-*-*-*-R-*-*-*-120-*-*-*-*-ISO8859-1");
	    strcpy (result, name);
	    XtFree ((char *)name);

	} else {
	    void * value;  Arg args[1];
	    XtSetArg (args[0], rp->name, &value);
	    XtGetValues (wp->w, args, 1);
	    sprintf (result, "%p", value);
	}

	Tcl_SetResult (wp->obm->tcl, result, TCL_VOLATILE);
	return (TCL_OK);
}


/* widgetAppend -- Append data to a text widget.
 *
 *  Usage:	append <text>
 */
static int 
widgetAppend (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;

	char *ip, *op;
	char buf[SZ_COMMAND];
	XawTextBlock tx;
	char *text;

	if (!(obmClass (obj->core.classrec, WtAsciiText))) {
	    Tcl_SetResult (obm->tcl, "not a text widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else
	    text = argv[1];

	if (wp->text_pos == 0) {
	    Arg args[1];
	    XtSetArg (args[0], XtNstring, "");
	    XtSetValues (wp->w, args, 1);
	}

	op = buf;
	if (wp->text_newline)
	    *op++ = '\n';

	for (ip=text;  *ip;  )
	    *op++ = *ip++;

	if ((wp->text_newline = (*(ip-1) == '\n')))
	    op--;

	*op = '\0';

	tx.ptr = buf;
	tx.length = op - buf;
	tx.format = FMT8BIT;
	tx.firstPos = 0;

	XawTextReplace (wp->w, wp->text_pos, wp->text_pos, &tx);
	XawTextSetInsertionPoint (wp->w, (wp->text_pos += (op - buf)));

	return (TCL_OK);
}


/* widgetSetList -- Set the item list of a list widget.
 *
 * Usage:	setList list [resize]
 *
 * The list is a simple list of strings, passed as a single string argument to
 * setList (quotes, braces, etc. may be used to quote strings containing
 * special characters).
 */
static int 
widgetSetList (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	Boolean resize;
	const char **items;
	int nitems;
	char *list;

	if (!(obmClass (obj->core.classrec, WtList) ||
	      obmClass (obj->core.classrec, WtMultiList))) {
	    Tcl_SetResult (obm->tcl, "not a list widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else
	    list = argv[1];

	resize = (argc > 2) ? (strcmp (argv[2], "resize") == 0) : False;

	if (Tcl_SplitList (obm->tcl, list, &nitems, &items) != TCL_OK)
	    return (TCL_ERROR);

	if ((obmClass (obj->core.classrec, WtList)))
	  XawListChange (wp->w, (char **)items, nitems, 0, resize);
	else if ((obmClass (obj->core.classrec, WtMultiList)))
	    XfwfMultiListSetNewData ((XfwfMultiListWidget)wp->w,
				     (String *) items, nitems, 0, resize, NULL);

	if (wp->data)
	    Tcl_Free (wp->data);

	wp->data = (char *) items;
	wp->datalen = nitems;

	return (TCL_OK);
}


/* widgetGetItem -- Get an item in a list widget.
 *
 * Usage:	value = getItem itemno
 *
 * If ITEMNO is a number the indicated list item is returned, or the string
 * "EOF" if the requested item is beyond the end of the list.  Otherwise the
 * currently selected item (or list of items in the case of a MultiList
 * widget) is returned, and the index (list of indices) of the item is
 * returned in the output variable ITEMNO.  If no item is currently selected
 * ITEMNO will be set to "none" ({}) on output.
 */
static int 
widgetGetItem (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;

	int nelem;
	String *list;
	char *ip, *op;
	XawListReturnStruct *itemp;
	char *s_itemno, *s_item;
	char buf[SZ_NUMBER];
	int requested;
	char *itemno;

	if (!(obmClass (obj->core.classrec, WtList) ||
	      obmClass (obj->core.classrec, WtMultiList))) {
	    Tcl_SetResult (obm->tcl, "not a list widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else
	    itemno = argv[1];

	if (isdigit (itemno[0])) {
	    /* Get indexed item. */
	    requested = atoi (itemno);
	    list = (String *) wp->data;
	    nelem = wp->datalen;
	    if (requested < nelem)
		s_item = list[requested];
	    else
		s_item = "EOF";

	} else if (obmClass (obj->core.classrec, WtList)) {
	    /* Athena list: get the currently selected list item.
	     */
	    itemp = XawListShowCurrent (wp->w);
	    if (!itemp || itemp->list_index == XAW_LIST_NONE) {
		s_itemno = "none";
		s_item = "";
	    } else {
		sprintf (buf, "%d", itemp->list_index);
		s_itemno = buf;
		s_item = itemp->string;
	    }
	    if ((Tcl_SetVar (obm->tcl, itemno, s_itemno, 0)) == NULL)
		return (TCL_ERROR);

	    Tcl_SetResult (obm->tcl, s_item, TCL_VOLATILE);

	} else {
	    /* MultiList: get currently selected items.
	     */
	    XfwfMultiListReturnStruct *list;
	    char *buffer, *strlist, *indexlist, *string;
	    Boolean state, sensitive;
	    int buflen, need, i;

	    list = XfwfMultiListGetHighlighted ((XfwfMultiListWidget)wp->w);
	    buflen = SZ_COMMAND;
	    if (!(buffer = XtMalloc (buflen)))
		return (TCL_ERROR);

	    /* Generate list of item strings. */
	    strlist = op = buffer;
	    *op++ = '{';
	    for (i=0;  i < list->num_selected;  i++) {
		XfwfMultiListGetItemInfo ((XfwfMultiListWidget)wp->w,
		    list->selected_items[i], &string, &state, &sensitive);
		need = strlen(string)+3 + list->num_selected * 6;
		if (buflen < (op-buffer)+need) {
		    buflen += max (need, SZ_COMMAND);
		    if (!(buffer = XtRealloc (buffer, buflen)))
			return (TCL_ERROR);
		}
		*op++ = ' ';
		*op++ = '{';
		for (ip=string;  (*op = *ip++);  op++)
		    ;
		*op++ = '}';
	    }
	    *op++ = '}';
	    *op++ = '\0';

	    /* Append list of indices.  We allocated space for these above. */
	    indexlist = op;
	    *op++ = '{';
	    for (i=0;  i < list->num_selected;  i++) {
		sprintf (op, " %d", list->selected_items[i]);
		while (*op)
		    op++;
	    }
	    *op++ = '}';
	    *op++ = '\0';

	    if ((Tcl_SetVar (obm->tcl, itemno, indexlist, 0)) == NULL)
		return (TCL_ERROR);

	    Tcl_SetResult (obm->tcl, strlist, TCL_VOLATILE);
	    XtFree (buffer);
	}

	return (TCL_OK);
}


/* widgetHighlight -- Highlight an item in a list widget.
 *
 * Usage:	highlight itemno
 *
 * The indicated item of the list is highlighted as if the item had been
 * selected by the user.  Any previously highlighted item is unhighlighted.
 * List items may be specified by either the element number or by name.
 */
static int 
widgetHighlight (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	int itemno;

	if (!(obmClass (obj->core.classrec, WtList) ||
	      obmClass (obj->core.classrec, WtMultiList))) {
	    Tcl_SetResult (obm->tcl, "not a list widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else
	    itemno = get_itemno (obj, argv[1]);

	if (itemno >= 0 && itemno < wp->datalen)
	    if (obmClass (obj->core.classrec, WtList))
		XawListHighlight (wp->w, itemno);
	    else
		XfwfMultiListHighlightItem ((XfwfMultiListWidget)wp->w, itemno);

	return (TCL_OK);
}


/* widgetUnhighlight -- Unhighlight the currently highlighted item in a
 * list widget.
 *
 * Usage:	unhighlight [itemno]
 *
 * If itemno is not given all list elements are unhighlighted, otherwise
 * the given entry is unhighlighted.  The itemno argument may be either
 * the actual item number, or the name of the list element.
 */
static int 
widgetUnhighlight (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	int itemno;

	if (!(obmClass (obj->core.classrec, WtList) ||
	      obmClass (obj->core.classrec, WtMultiList))) {
	    Tcl_SetResult (obm->tcl, "not a list widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	itemno = (argc > 1) ? get_itemno(obj,argv[1]) : -1;

	if (obmClass (obj->core.classrec, WtList)) {
	    if (itemno >= 0) {
		XawListReturnStruct *itemp = XawListShowCurrent (wp->w);
		if (itemp && itemp->list_index == itemno)
		    XawListUnhighlight (wp->w);
	    } else
		XawListUnhighlight (wp->w);
	} else {
	    if (itemno >= 0) {
		XfwfMultiListUnhighlightItem ((XfwfMultiListWidget)wp->w,
		    itemno);
	    } else
		XfwfMultiListUnhighlightAll ((XfwfMultiListWidget)wp->w);
	}

	return (TCL_OK);
}


/* get_itemno -- Get the item number of an item in a list widget, given
 * either the ascii representation of the item number, or the item string.
 */
static int
get_itemno (WidgetObject obj, char *itemstr)
{
	WidgetPrivate wp = &obj->widget;
	int i;

	if (isdigit (*itemstr))
	    return (atoi(itemstr));
	else {
	    /* Check first to see if the named item is the currently
	     * selected, or most recently referenced item.
	     */
	    if (obmClass (obj->core.classrec, WtList)) {
		XawListReturnStruct *itemp;
		itemp = XawListShowCurrent (wp->w);
		if (itemp && strcmp (itemp->string, itemstr) == 0)
		    return (itemp->list_index);

	    } else if (obmClass (obj->core.classrec, WtMultiList)) {
		XfwfMultiListReturnStruct *list;
		list = XfwfMultiListGetHighlighted ((XfwfMultiListWidget)wp->w);
		if (list->string && strcmp (list->string, itemstr) == 0)
		    return (list->item);
	    }

	    /* Search the full list. */
	    for (i=0;  i < wp->datalen;  i++)
		if (strcmp (((String *)wp->data)[i], itemstr) == 0)
		    return (i);
	}

	return (-1);
}


/* widgetGetValue -- Get the text value of a dialog widget.
 *
 * Usage:	value = getValue
 */
static int 
widgetGetValue (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	char *value;

	if (!(obmClass (obj->core.classrec, WtDialog))) {
	    Tcl_SetResult (obm->tcl, "not a dialog widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	value = XawDialogGetValueString (wp->w);
	Tcl_SetResult (obm->tcl, value, TCL_VOLATILE);

	return (TCL_OK);
}


/* widgetGetThumb -- Get the position and size of the thumb of a slider2d
 * widget.
 *
 * Usage:	getThumb x [y [width [height]]]
 */
static int 
widgetGetThumb (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	XfwfScrollInfo info;
	char buf[SZ_NUMBER];

	if (!(obmClass (obj->core.classrec, WtSlider2d))) {
	    Tcl_SetResult (obm->tcl, "not a slider2d widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	XfwfGetThumb (wp->w, &info);

	if (argc > 1) {
	    sprintf (buf, "%g", info.hpos);
	    if ((Tcl_SetVar (obm->tcl, argv[1], buf, 0)) == NULL)
		return (TCL_ERROR);
	}
	if (argc > 2) {
	    sprintf (buf, "%g", info.vpos);
	    if ((Tcl_SetVar (obm->tcl, argv[2], buf, 0)) == NULL)
		return (TCL_ERROR);
	}
	if (argc > 3) {
	    sprintf (buf, "%g", info.hsize);
	    if ((Tcl_SetVar (obm->tcl, argv[3], buf, 0)) == NULL)
		return (TCL_ERROR);
	}
	if (argc > 4) {
	    sprintf (buf, "%g", info.vsize);
	    if ((Tcl_SetVar (obm->tcl, argv[4], buf, 0)) == NULL)
		return (TCL_ERROR);
	}

	return (TCL_OK);
}


/* widgetMoveThumb -- Move the thumb of a slider2D widget.
 *
 * Usage:	moveThumb x [y]
 *
 * The thumb of a slider2D wiget is set to the given position specified as
 * a fraction of the widget's width or height.  The widget and height
 * arguments should be floating point values in the range 0.0 to 1.0.
 */
static int 
widgetMoveThumb (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	double x, y;

	if (!(obmClass (obj->core.classrec, WtSlider2d))) {
	    Tcl_SetResult (obm->tcl, "not a slider2d widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else {
	    x = atof (argv[1]);
	    y = (argc > 2) ? atof(argv[2]) : 0.0;
	}

	x = max(0, min(1, x));
	y = max(0, min(1, y));

	XfwfMoveThumb (wp->w, x, y);
	return (TCL_OK);
}


/* widgetResizeThumb -- Resize the thumb of a slider2D widget.
 *
 * Usage:	resizeThumb width [height]
 *
 * The thumb of a slider2D wiget is set to the given fraction of the widget's
 * width or height.  The widget and height arguments should be floating point
 * values in the range 0.0 to 1.0.
 */
static int 
widgetResizeThumb (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	double width, height;

	if (!(obmClass (obj->core.classrec, WtSlider2d))) {
	    Tcl_SetResult (obm->tcl, "not a slider2d widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else {
	    width = atof (argv[1]);
	    height = (argc > 2) ? atof(argv[2]) : 1.0;
	}

	width = max(0, min(1, width));
	height = max(0, min(1, height));

	XfwfResizeThumb (wp->w, width, height);
	return (TCL_OK);
}


/* widgetSetScrollbar -- Set the position and size of a scrollbar.
 *
 * Usage:	setScrollbar position size
 *
 * The thumb of a scrollbar wiget is set to the given position and size
 * specified as a fraction of the widget's width or height.  The position and
 * height arguments should be floating point values in the range 0.0 to 1.0.
 */
static int 
widgetSetScrollbar (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	double position, size;

	if (!(obmClass (obj->core.classrec, WtScrollbar) ||
	      obmClass (obj->core.classrec, WtScrollbar2))) {
	    Tcl_SetResult (obm->tcl, "not a scrollbar widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 3) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else {
	    position = atof (argv[1]);
	    size = atof (argv[2]);
	}

	position = max(0, min(1, position));
	size = max(0, min(1, size));

	if (obmClass (obj->core.classrec, WtScrollbar))
	    XawScrollbarSetThumb (wp->w, position, size);
	else
	    XfwfSetScrollbar (wp->w, position, size);

	return (TCL_OK);
}


/* widgetSetLocation -- Set the position of a Viewport.
 *
 * Usage:	setLocation x y
 *
 */
static int 
widgetSetLocation (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	float x, y;

	if (!(obmClass (obj->core.classrec, WtViewport))) {
	    Tcl_SetResult (obm->tcl, "not a viewport widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 3) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else {
	    x = atof (argv[1]);
	    y = atof (argv[2]);
	}

	XawViewportSetLocation (wp->w, x, y);

	return (TCL_OK);
}


/* widgetSetCoordinates -- Set the coordinates of a Viewport.
 *
 * Usage:	setCoordinates x y
 *
 */
static int 
widgetSetCoordinates (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	int	 x, y;

	if (!(obmClass (obj->core.classrec, WtViewport))) {
	    Tcl_SetResult (obm->tcl, "not a viewport widget", TCL_STATIC);
	    return (TCL_ERROR);
	}

	if (argc < 3) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	} else {
	    x = atoi (argv[1]);
	    y = atoi (argv[2]);
	}

	XawViewportSetCoordinates (wp->w, (int)x, (int)y);

	return (TCL_OK);
}


/* widgetSetTop -- Raise the child of a Tabs widget.
 *
 * Usage:	setTop widget
 *
 */
static int 
widgetSetTop (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	ObmObject child = (ObmObject) NULL;

	if (!(obmClass (obj->core.classrec, WtTabs))) {
	    Tcl_SetResult (obm->tcl, "not a tabs widget", TCL_STATIC);
	    return (TCL_ERROR);
	} else if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing argument", TCL_STATIC);
	    return (TCL_ERROR);
	}

	/* Get the child object pointer and raise it. */
	child = obmFindObject (obm, argv[1]);
	if (child)
	    XawTabsSetTop (widgetGetPointer (child), False);

	return (TCL_OK);
}


/* widgetSetListTree -- Set a ListTree hierarchy.
 *
 * Usage:	setListTree list [append]
 *
 * The list is specified as a hierarchical Tcl list of the form
 *
 *	     {a1 {b1 {a2 {a3 b3}} b2} c1}
 *
 * This would produce an indented list something like:
 *
 *		a1
 *		b1
 *		 |- a2
 *		    |- a3
 *		    |- b3
 *		 |- b2
 *		c1
 *
 */
static int 
widgetSetListTree (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	Boolean append;
	char *list, buf[SZ_LINE];
	const char **items;
	int nitems, item;
	static char **sv_items = NULL;
	static int sv_nitems;
	ListTreeItem *val;


	/* Do some error checking first. */
	if (!(obmClass (obj->core.classrec, WtListTree))) {
	    Tcl_SetResult (obm->tcl, "not a ListTree widget", TCL_STATIC);
	    return (TCL_ERROR);
	} else if (argc < 2) {
	    Tcl_SetResult (obm->tcl, "missing list argument", TCL_STATIC);
	    return (TCL_ERROR);
	}

	list = argv[1];
	append = (argc > 2) ? (strcmp (argv[2], "append") == 0) : False;

	/* Delete the old tree. */
	if (!append && sv_items) {
	    while ((val = ListTreeFirstItem(wp->w)))
	        ListTreeDelete (wp->w, val);
	    free ((char *) sv_items);
	    sv_items = NULL;
	}

	/* Split the list so we can parse as needed. */
        if (Tcl_SplitList (tcl, list, &nitems, &items) != TCL_OK)
            return (TCL_ERROR);

        /* Get local copy of argc and argv.  */
        if (!sv_nitems) {
            sv_items = (char **) XtMalloc (nitems * sizeof(char *));
            memmove (sv_items, items, nitems * sizeof(char *)); 
        }

	if (append && sv_items) {
            sv_items = (char **) XtRealloc ((char *)sv_items,
		(sv_nitems + nitems * 1) * sizeof(char *));
            memmove (sv_items+sv_nitems, items, nitems * sizeof(char *)); 
	}

	for (item=0; item < nitems; item++) {
	    /* Build the top-level tree, children are built recursively
	     * in the routine.
	     */
	    if (buildTreeList (wp->w, tcl, NULL, items[item]) != TCL_OK) {
		free ((char *) items);
		return (TCL_ERROR);
	    }
	}

ret:	Tcl_Free ((char *) items);
	return (TCL_OK);
}


/* buildTreeList -- Recursively build a tree from a list of nested Tcl
 * lists.  This is used to fill out the ListTree widget values.
 */
static int
buildTreeList (Widget w, Tcl_Interp *tcl, ListTreeItem *parent, const char *item)
{
	const char **fields, **entry;
	int i, nentries, nfields, field;
	char buf[SZ_LINE];
	ListTreeItem *level;

	/* Split the list so we can parse as needed. */
        if (Tcl_SplitList (tcl, item, &nfields, &fields) != TCL_OK) {
            sprintf (buf, "bad item '%s' in tree list", item);
            Tcl_AppendResult (tcl, buf, NULL);
            return (TCL_ERROR);
	}

	/* First item is always added to the list, it may be either the
 	 * parent of another list or a single item.
	 */
	level = ListTreeAdd (w, parent, fields[0]);

	/* For each of the items, split it and recursively call ourselves
	 * until it gets added as a single item.
	 */
	for (field=1; field < nfields; field++) {
            if (Tcl_SplitList (tcl, fields[field], &nentries, &entry) != TCL_OK)
                return (TCL_ERROR);

	    for (i=0; i < nentries; i++)
	        buildTreeList (w, tcl, level, entry[i]);
	}

	Tcl_Free ((char *) fields);
/*	free ((char *) entry);*/
	return (TCL_OK);
}


/* widgetListTreeSelect -- Select the specified item from a ListTree.
 *
 * Usage:       listTreeSelect item [ top [children_only] ]
 *
 * The 'item' may be one of:
 *
 *		all			open all children in list
 *		none			close all children in list
 *
 * If 'toplevel' is specified then 'item' is assumed to be a child of
 * that node.  If 'children_only' is set then only the children of the
 * specified item will be opened (applies to all/none only). The return
 * message is a pair of lists of the form
 *
 *	{ value state } { parent1 parent2 ... }
 *
 * where the 'value' is the label of the item selected, 'state' is an int
 * indicating whether the node is open or closed, and 'parentN' is a list
 * of node names chaining back to the top level of the tree.
 *
 */
static int
widgetListTreeSelect (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char *top, *name;
	char message[SZ_COMMAND], buf[SZ_LINE];
	int i, count;
	ListTreeItem *item, *titem, *first;


	if (argc < 2)
	    return (TCL_ERROR);

	name = argv[1];
	first = ListTreeFirstItem (wp->w);

	if (strcmp(argv[1], "all") == 0) {
	    if (argc == 4) {
		top = argv[2];
	        titem = ListTreeFindSiblingName (wp->w, first, top);
	        item = ListTreeFindChildName (wp->w, titem, name);
	        ListTreeOpenAll (wp->w, (item ? item : titem), 1);
		ListTreeHighlightItem (wp->w, (item ? item : titem));
	        sprintf (message, "{%s 1} { }",
		    (item ? item->text : titem->text));
	    } else {
	        ListTreeOpenAll (wp->w, (ListTreeItem *)NULL, 0);
	        strcpy (message, "{all 1} { }");
	    }


	} else if (strcmp(argv[1], "none") == 0) {
	    if (argc == 4) {
		top = argv[2];
	        titem = ListTreeFindSiblingName (wp->w, first, top);
	        item = ListTreeFindChildName (wp->w, titem, name);
	        ListTreeCloseAll (wp->w, (item ? item : titem), 0);
	        sprintf (message, "{%s 0} { }",
		    (item ? item->text : titem->text));
	    } else {
	        ListTreeCloseAll (wp->w, (ListTreeItem *)NULL, 0);
	        strcpy (message, "{all 0} { }");
	    }

	} else {
	    if (argc == 3) {
		top = argv[2];
	        titem = ListTreeFindSiblingName (wp->w, first, top);
	        if (titem)
		    ListTreeOpenAll (wp->w, titem, 0);
	        item = ListTreeFindChildNameInTree ((ListTreeWidget) wp->w, titem, name);
	        item = (item ? item : titem);
	    } else {
		titem = ListTreeFindSiblingName (wp->w, first, name);
		if (strcmp (name, titem->text) == 0)
		    item = titem;
		else
	            item = ListTreeFindChildNameInTree ((ListTreeWidget) wp->w, titem, name);
	    }
	    ListTreeHighlightItem (wp->w, item);
	    ListTreeOpenAll (wp->w, item, 0);

	    /* The message is the string value of the list element selected,
	     * and a bottom-up path to the root.
	     */
	    sprintf (message, "{%s %d} ", item->text, item->open);

	    strcat (message, "{ ");
	    sprintf (buf, "{ %s } ", item->text);
	    strcat (message, buf);
	    while (item->parent) {
	        item = item->parent;
	        sprintf (buf, "{ %s } ", item->text);
	        strcat (message, buf);
            }

	    strcat (message, "}");

	}

	/* Call all the callbacks with the message. */
	call_callbacks (obj, Ctcallback, message);

        return (TCL_OK);
}


/* widgetListTreeHighlight -- Highlight but do not select the specified item
 * from a ListTree.
 *
 * Usage:       listTreeHighlight item [ top ]
 *
 * The 'item' is given as a node name of the tree.  If 'top' is specified
 * then 'item' is assumed to be a child of that node.  If 'children_only' is
 * set then only the children of the specified item will be opened (applies
 * to all/none only). The return message is a pair of lists of the form
 *
 *	{ value state } { parent1 parent2 ... }
 *
 * where the 'value' is the label of the item selected, 'state' is an int
 * indicating whether the node is open or closed, and 'parentN' is a list
 * of node names chaining back to the top level of the tree.
 *
 */
static int
widgetListTreeHighlight (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char *top, *name;
	char message[SZ_COMMAND], buf[SZ_LINE];
	int i, count;
	ListTreeItem *item, *titem, *first, *op;


	if (argc < 2)
	    return (TCL_ERROR);

	name = argv[1];
	first = ListTreeFirstItem (wp->w);

	if (argc == 3) {
	    top = argv[2];
	    titem = ListTreeFindSiblingName (wp->w, first, top);
	    item = ListTreeFindChildNameInTree ((ListTreeWidget) wp->w, titem, name);
	    item = (item ? item : titem);

	    /* Now chain back up thru the parents and open the nodes.
	    */
	    for (op=item ; op->parent && op->parent != first; op = op->parent) {
	        if (op->open == 0) 
		    ListTreeOpenAll (wp->w, op, 1);
	    }

	} else {
	    if (first->open == 0)
	        ListTreeOpenAll (wp->w, first, 0);
	    titem = ListTreeFindChildNameInTree ((ListTreeWidget) wp->w, first, name);
	    if (titem && strcmp (name, titem->text) == 0)
		item = titem;
	    else
	        item = ListTreeFindChildNameInTree ((ListTreeWidget) wp->w, titem, name);
	}
	ListTreeHighlightItem (wp->w, item);

	/* The message is the string value of the list element selected,
	 * and a bottom-up path to the root.
	 */
	sprintf (message, "{%s %d} ", item->text, item->open);

	strcat (message, "{ ");
	sprintf (buf, "{ %s } ", item->text);
	strcat (message, buf);
	while (item->parent) {
	    item = item->parent;
	    sprintf (buf, "{ %s } ", item->text);
	    strcat (message, buf);
        }
	strcat (message, "}");

	/* Call all the callbacks with the message. */
	call_callbacks (obj, Ctcallback, message);

        return (TCL_OK);
}


/* widgetListTreeDelete -- Delete the specified item from a ListTree.
 *
 * Usage:       listTreeDelete item [top]
 *
 * The 'item' may 'all' to delete the entire list or a named element.
 * If 'toplevel' is specified then 'item' is assumed to be a child of
 * that node.  If 'children_only' is set then only the children of the
 * specified item will be opened (applies to all/none only). The return
 * message is a pair of lists of the form
 *
 *	{ value state } { parent1 parent2 ... }
 *
 * where the 'value' is the label of the item selected, 'state' is an int
 * indicating whether the node is open or closed, and 'parentN' is a list
 * of node names chaining back to the top level of the tree.
 *
 */
static int
widgetListTreeDelete (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char *top, *name;
	ListTreeItem *item, *titem, *first;


	if (argc < 2)
	    return (TCL_ERROR);

	name = argv[1];
	first = ListTreeFirstItem (wp->w);

	if (strcmp(argv[1], "all") == 0) {
	    if (argc == 3) {
		top = argv[2];
	        titem = ListTreeFindSiblingName (wp->w, first, top);
		if (strcmp (top, titem->text) == 0)
		    item = titem;
		else
	            item = ListTreeFindChildName (wp->w, titem, name);

	        ListTreeDelete(wp->w, item);

	    } else {
	        while ((item = ListTreeFirstItem(wp->w)))
	            ListTreeDelete (wp->w, item);
	    }

	} else {
	    if (argc == 3) {
		top = argv[2];
	        titem = ListTreeFindSiblingName (wp->w, first, top);
	        item = ListTreeFindChildName (wp->w, titem, name);
	    } else {
	        titem = ListTreeFindSiblingName (wp->w, first, name);
		if (strcmp (name, titem->text) == 0)
		    item = titem;
		else
	            item = ListTreeFindChildName (wp->w, titem, name);
	    }

	    /* Now delete the item from the list. */
	    ListTreeDelete(wp->w, item);
	}


	/* Call all the callbacks with the message.
	call_callbacks (obj, Ctcallback, message);
	 */

        return (TCL_OK);
}


#ifndef OSI_COMPLIANT
/* widgetSetTable -- Set the contents of a Table widget.
 *
 * Usage:       setTable nrows ncols data
 *
 * The table data is specified as a Tcl list of the form:
 *
 *	{ {r1c1 r1c2 ... r1cN}
 *	  {r2c1 r2c2 ... r2cN}
 *	  	:
 *	  {rNc1 rNc2 ... rNcN} }
 *
 * String values must be quoted, rows/cols will be truncated or cleared if
 * the specified table size does not agree with the size of the data table
 * being loaded.
 *
 */
static int
widgetSetTable (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;

	int i, j;
	int	nrows, ncols, ndrows=0, ndcols=0, onrows, oncols;
	const char	*list = NULL, **rows = NULL, **cols = NULL;


	if (argc < 4)
	    return (TCL_ERROR);

	/* Get the arguments */
	nrows = atoi(argv[1]);
	ncols = atoi(argv[2]);
	list  = argv[3];

	/* Resize the table if needed. */	        
	XawTableGetSize (wp->w, &onrows, &oncols);
	if (onrows != nrows || oncols != ncols)
	    XawTableSetNewSize (wp->w, nrows, ncols);

       /* Split the list so we can parse the rows.  */
        if (Tcl_SplitList (tcl, list, &ndrows, &rows) != TCL_OK)
            return (TCL_ERROR);

	/* Set the labels for the table.  Clear any extra row or column
	 * labels in case we didn't get enough data, ignore extra data in
	 * the table if it's more than the size we're trying to create.
	 */
	for (i=0; i < ndrows; i++) {
            if (Tcl_SplitList (tcl, rows[i], &ndcols, &cols) != TCL_OK)
                return (TCL_ERROR);

	    for (j=0; j < ndcols; j++)
	        XawTableSetLabel (wp->w, i, j, cols[j]);
	}

	Tcl_Free ((char *) rows);
	Tcl_Free ((char *) cols);
        return (TCL_OK);
}


/* widgetGetCellAttr -- Get the given attribute of a Table cell.
 *
 * Usage:       setGellAttr row col attribute value
 *
 *
 * The cell position is given as a 1-indexed array element where the UL
 * of the table is cell (1,1).  Allowed attributes for a cell include:
 *
 *		label			label text (string)
 *		background		background color (string)
 *		foreground		foreground color (string)
 *
 */
static int
widgetGetCellAttr (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
        XrmValue from, to;
	unsigned long bg, fg;
	int	row, col;
	char	*attr, *value;


	if (argc < 4)
	    return (TCL_ERROR);

	row 	= atoi(argv[1]) - 1;
	col 	= atoi(argv[2]) - 1;
	attr  	= argv[3];

	if (strcmp(attr, "label") == 0) 
	    value = XawTableGetLabelByPosition (wp->w, row, col);
	else 
	    return (TCL_ERROR);

        Tcl_SetResult (wp->obm->tcl, value, TCL_VOLATILE);
        return (TCL_OK);
}


/* widgetSetCellAttr -- Set the given attribute of a Table cell.
 *
 * Usage:       setCellAttr row col attribute value
 *
 *
 * The cell position is given as a 1-indexed array element where the UL
 * of the table is cell (1,1).  Allowed attributes for a cell include:
 *
 *		label			label text (string)
 *		background		background color (string)
 *		foreground		foreground color (string)
 *
 */
static int
widgetSetCellAttr (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
        XrmValue from, to;
	unsigned long bg, fg;
	int	row, col;
	char	*attr, *value;


	if (argc < 5)
	    return (TCL_ERROR);

	/* Get the arguments. */
	row 	= atoi(argv[1]) - 1;
	col 	= atoi(argv[2]) - 1;
	attr  	= argv[3];
	value  	= argv[4];

	if (strcmp(attr, "label") == 0) {
	    XawTableSetLabel (wp->w, row, col, value);

	} else if (strcmp(attr, "background") == 0) {
            from.size = strlen (value) + 1;
            from.addr = value;
            to.addr = (void *) &bg;
            to.size = sizeof(bg);

            if (!XtConvertAndStore (wp->w, XtRString, &from, XtRPixel, &to))
                bg = BlackPixelOfScreen (obm->screen);

	    XawTableSetCellBackground (wp->w, row, col, bg);

	} else if (strcmp(attr, "foreground") == 0) {
            from.size = strlen (value) + 1;
            from.addr = value;
            to.addr = (void *) &fg;
            to.size = sizeof(fg);

            if (!XtConvertAndStore (wp->w, XtRString, &from, XtRPixel, &to))
                fg = BlackPixelOfScreen (obm->screen);

	    XawTableSetCellForeground (wp->w, row, col, fg);
	}

        return (TCL_OK);
}


/* widgetSetColAttr -- Set the given attribute of a Table column.
 *
 * Usage:       setColAttr col attribute value
 *
 * The column position is given as a 1-indexed array element where the UL
 * of the table is cell (1,1).  Allowed attributes for a column include:
 *
 *		width			column width (pixels)
 *		background		background color (string)
 *		foreground		foreground color (string)
 *		justify			text justification (string)
 *
 */
static int
widgetSetColAttr (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
        XrmValue from, to;
	unsigned long bg, fg;
	int	cols[128], widths[128];
	int	nitems, row, col, nrows, ncols, i;
	const char	**items;
	char	*attr;


	if (argc < 4)
	    return (TCL_ERROR);

	/* Get the arguments.  NOTE: need to bounds check the length of the
	 * list with the array.
 	 */
        if (Tcl_SplitList (tcl, argv[1], &nitems, &items) != TCL_OK)
            return (TCL_ERROR);
	else {
	    if (nitems == 1)
	        col = atoi(argv[1]) - 1;
	    else {
	        if (nitems > 128)
                    return (TCL_ERROR);
	        for (i=0; i < nitems; i++)
	            cols[i] = atoi(items[i]) - 1;
	    }
	}
	Tcl_Free((char *)items);
	attr  	= argv[2];

	/* Get current table size. */
	XawTableGetSize (wp->w, &nrows, &ncols);

	if (strcmp(attr, "width") == 0) {
	    /* Reset the column width. */
	    if (nitems == 1)
	        XawTableSetColumnWidth (wp->w, col, atoi(argv[3]));
	    else {
        	if (Tcl_SplitList (tcl, argv[3], &nitems, &items) != TCL_OK)
	            return (TCL_ERROR);
	        if (nitems > 128)
		    Tcl_Free((char *)items);
                    return (TCL_ERROR);
	        for (i=0; i < nitems; i++)
	            widths[i] = atoi(items[i]) - 1;
	        XawTableSetMultiColumnWidths (wp->w, cols, widths, nitems);
		Tcl_Free((char *)items);
	    }

	} else if (strcmp(attr, "background") == 0) {
	    /* Reset the column background color. */
            from.size = strlen (argv[3]) + 1;
            from.addr = argv[3];
            to.addr = (void *) &bg;
            to.size = sizeof(bg);

            if (!XtConvertAndStore (wp->w, XtRString, &from, XtRPixel, &to))
                bg = BlackPixelOfScreen (obm->screen);

	    for (i=0; i < nrows; i++)
	        XawTableSetCellBackground (wp->w, i, col, bg);

	} else if (strcmp(attr, "foreground") == 0) {
	    /* Reset the column foreground color. */
            from.size = strlen (argv[3]) + 1;
            from.addr = argv[3];
            to.addr = (void *) &fg;
            to.size = sizeof(fg);

            if (!XtConvertAndStore (wp->w, XtRString, &from, XtRPixel, &to))
                fg = BlackPixelOfScreen (obm->screen);

	    for (i=0; i < nrows; i++)
	        XawTableSetCellForeground (wp->w, i, col, fg);

	} else if (strcmp(attr, "justify") == 0) {
	    /* Reset the column text justification. */
	    if (strcmp(argv[3], "left") == 0)
	        XawTableSetColumnJustify (wp->w, col, XtJustifyLeft);
	    else if (strcmp(argv[3], "center") == 0)
	        XawTableSetColumnJustify (wp->w, col, XtJustifyCenter);
	    else if (strcmp(argv[3], "right") == 0)
	        XawTableSetColumnJustify (wp->w, col, XtJustifyRight);
	    else 
	        XawTableSetColumnJustify (wp->w, col, XtJustifyLeft);
	}

        return (TCL_OK);
}


/* widgetGetColAttr -- Get the requested attribute of a Table column.
 *
 * Usage:       attr = getColAttr col attribute
 *
 *
 * The column position is given as a 1-indexed array element where the UL
 * of the table is cell (1,1).  Allowed attributes for a column include:
 *
 *	width			column width
 *	pixelWidth		foreground color
 *	justify			text justification
 *
 */
static int
widgetGetColAttr (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char rbuf[SZ_MESSAGE];
	char *result = rbuf, *attr;
	int col, nrows, ncols, width, pixelWidth;
	XtJustify justify;


	if (argc < 3)
	    return (TCL_ERROR);

	/* Get the arguments. */
	col 	= atoi(argv[1]) - 1;
	attr  	= argv[2];

	/* Get current table size. */
	XawTableGetSize (wp->w, &nrows, &ncols);

	if (strcmp(attr, "width") == 0) {
	    width = XawTableGetColumnWidth (wp->w, col);

	} else if (strcmp(attr, "pixelWidth") == 0) {
	    pixelWidth = XawTableGetColumnPixelWidth (wp->w, col);

	} else if (strcmp(attr, "justify") == 0) {
	    justify = XawTableGetColumnJustify (wp->w, col);

	    /* Reset the column text justification. */
	    if (justify == XtJustifyLeft)
		strcpy (result, "left");
	    else if (justify == XtJustifyCenter)
		strcpy (result, "center");
	    else if (justify == XtJustifyRight)
		strcpy (result, "right");
	    else 
		strcpy (result, "left");
	}

        Tcl_SetResult (wp->obm->tcl, result, TCL_VOLATILE);
        return (TCL_OK);
}


/* widgetSetRowAttr -- Set the given attribute of a Table row.
 *
 * Usage:       setRowAttr row attribute value
 *
 * The row position is given as a 1-indexed array element where the UL
 * of the table is cell (1,1).  Allowed attributes for a row include:
 *
 *	background		background color
 *	foreground		foreground color
 */
static int
widgetSetRowAttr (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
        XrmValue from, to;
	unsigned long bg, fg;
	int	row, col, nrows, ncols, i;
	char	*attr;


	if (argc < 4)
	    return (TCL_ERROR);

	/* Get the arguments. */
	row 	= atoi(argv[1]) - 1;
	attr  	= argv[2];

	/* Get current table size. */
	XawTableGetSize (wp->w, &nrows, &ncols);

	if (strcmp(attr, "background") == 0) {
	    /* Reset the column background color. */
            from.size = strlen (argv[3]) + 1;
            from.addr = argv[3];
            to.addr = (void *) &bg;
            to.size = sizeof(bg);

            if (!XtConvertAndStore (wp->w, XtRString, &from, XtRPixel, &to))
                bg = BlackPixelOfScreen (obm->screen);

	    for (i=0; i < ncols; i++)
	        XawTableSetCellBackground (wp->w, row, i, bg);

	} else if (strcmp(attr, "foreground") == 0) {
	    /* Reset the column foreground color. */
            from.size = strlen (argv[3]) + 1;
            from.addr = argv[3];
            to.addr = (void *) &fg;
            to.size = sizeof(fg);

            if (!XtConvertAndStore (wp->w, XtRString, &from, XtRPixel, &to))
                fg = BlackPixelOfScreen (obm->screen);

	    for (i=0; i < ncols; i++)
	        XawTableSetCellForeground (wp->w, row, i, fg);
	}

        return (TCL_OK);
}


/* widgetGetRowAttr -- Get the requested attribute of a Table column.
 *
 * Usage:       attr = getRowAttr row attribute
 *
 *
 * The column position is given as a 1-indexed array element where the UL
 * of the table is cell (1,1).  Allowed attributes for a column include:
 *
 *	<none yet>
 *
 */
static int
widgetGetRowAttr (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char rbuf[SZ_MESSAGE];
	char *result = rbuf, *attr;
	int row, nrows, ncols, width, pixelWidth;
	XtJustify justify;


	if (argc < 3)
	    return (TCL_ERROR);

	/* Get the arguments. */
	row 	= atoi(argv[1]) - 1;
	attr  	= argv[2];

	/* Get current table size. */
	XawTableGetSize (wp->w, &nrows, &ncols);

        return (TCL_OK);
}


/* widgetDeleteCol -- Delete the specified columns from the table.
 *
 * Usage:       deleteCol column
 *
 */
static int
widgetDeleteCol (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
	int	col;


	if (argc < 2)
	    return (TCL_ERROR);

	/* Get the arguments. */
	col 	= atoi(argv[1]) - 1;

	/* Delete the specified row. */
	XawTableDeleteColumn (wp->w, col);

        return (TCL_OK);
}


/* widgetAddCol -- Add a new column to the Table.
 *
 * Usage:       addCol col width
 *
 * The column may be specified in one of the following ways:
 *
 *	first		make column the first column in the table
 *	last		make column the last column in the table
 *	<num>		make column the N-th column in the table
 *
 * The column width is specified as a character width.  Data for the
 * column must be added separately using the setColAttr function to
 * set individual labels.
 *
 */
static int
widgetAddCol (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char	*col = NULL;
	int 	nrows, ncols, width, colnum = 0;


	if (argc < 3)
	    return (TCL_ERROR);

	/* Get the arguments. */
	col = argv[1];
	width = atoi(argv[2]);

	/* Add the specified column. */
	XawTableGetSize (wp->w, &nrows, &ncols);
	colnum = max (0, min (ncols, atoi (col) - 1));

	if (colnum == 0 || streq(col, "first"))
	    XawTablePrependColumn (wp->w, width);
	else if (colnum == ncols || streq (col, "last"))
	    XawTableAppendColumn (wp->w, width);
	else
	    XawTableInsertColumn (wp->w, colnum, width);

        return (TCL_OK);
}


/* widgetDeleteRow -- Delete the specified rows from the table.
 *
 * Usage:       deleteRow row
 *
 */
static int
widgetDeleteRow (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
	int	row;


	if (argc < 2)
	    return (TCL_ERROR);

	/* Get the arguments. */
	row 	= atoi(argv[1]) - 1;

	/* Delete the specified row. */
	XawTableDeleteRow (wp->w, row);

        return (TCL_OK);
}


/* widgetAddRow -- Add a new row to the Table.
 *
 * Usage:       addRow row
 *
 * The row may be specified in one of the following ways:
 *
 *	first		make row the first row in the table
 *	last		make row the last row in the table
 *	<num>		make row the N-th row in the table
 *
 * Data for the column must be added separately using the setColAttr
 * function to set individual labels.
 *
 */
static int
widgetAddRow (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char *row = NULL;
	int nrows, ncols, rownum = 0;


	if (argc < 2)
	    return (TCL_ERROR);

	/* Get the arguments. */
	row = argv[1];

	/* Add the specified column. */
	XawTableGetSize (wp->w, &nrows, &ncols);
	rownum = max (0, min (nrows, atoi (row) - 1));

	if (rownum == 0 || streq(row, "first"))
	    XawTablePrependRow (wp->w);
	else if (rownum == nrows || streq (row, "last"))
	    XawTableAppendRow (wp->w);
	else
	    XawTableInsertRow (wp->w, rownum);

        return (TCL_OK);
}


/* widgetSetTableSize -- Set the size of the specified table.
 *
 * Usage:       setTableSize nrows ncols
 *
 */
static int
widgetSetTableSize (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	int	nrows, ncols;


	if (argc < 3)
	    return (TCL_ERROR);

	nrows = atoi (argv[1]);
	ncols = atoi (argv[2]);
	if (XawTableSetNewSize (wp->w, nrows, ncols) >= 0)
            return (TCL_OK);
	else
            return (TCL_ERROR);
}



/* widgetGetTableSize -- Get the size of the specified table.
 *
 * Usage:       getTableSize nrows ncols
 *
 */
static int
widgetGetTableSize (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
        WidgetObject obj = (WidgetObject) msg->object[msg->level];
        WidgetPrivate wp = &obj->widget;
        ObmContext obm = wp->obm;
	char	buf[16], *nrows, *ncols;
	int	nr, nc;


	if (argc < 3)
	    return (TCL_ERROR);

	nrows = argv[1];
	ncols = argv[2];

	XawTableGetSize (wp->w, &nr, &nc);

	sprintf (buf, "%d", nr);
        Tcl_SetVar (wp->obm->tcl, nrows, buf, 0);
	sprintf (buf, "%d", nc);
        Tcl_SetVar (wp->obm->tcl, ncols, buf, 0);

        return (TCL_OK);
}
#endif


/* widgetRealize -- Realize a widget.  This activates and assigns windows for
 * a widget and all of its descendants.  Realizing a widget does not in itself
 * cause it to appear on the screen.
 *
 *  Usage:	realize
 */
static int 
widgetRealize (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	XtRealizeWidget (wp->w);
	return (TCL_OK);
}


/* widgetUnrealize -- Unrealize a widget.  This destroys the windows assigned
 * to a widget and all of its descendants.
 *
 *  Usage:	unrealize
 */
static int 
widgetUnrealize (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	XtUnrealizeWidget (wp->w);
	return (TCL_OK);
}


/* widgetIsRealized -- Test whether a widget is realized.
 *
 *  Usage:	isRealized
 */
static int 
widgetIsRealized (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Boolean sensitive;

	if (XtIsRealized (wp->w))
	    Tcl_SetResult (wp->obm->tcl, TRUESTR, TCL_STATIC);
	else
	    Tcl_SetResult (wp->obm->tcl, FALSESTR, TCL_STATIC);
	return (TCL_OK);
}


/* widgetMap -- Map a widget.
 *
 *  Usage:	map
 */
static int 
widgetMap (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	XtRealizeWidget (wp->w);
	XtMapWidget (wp->w);
	return (TCL_OK);
}


/* widgetUnmap -- Unmap a widget.
 *
 *  Usage:	unmap
 */
static int 
widgetUnmap (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	Widget w = wp->w;

	if (!XtIsRealized(w) || !XtWindow(w))
	    return (TCL_ERROR);

	XmuUpdateMapHints (obm->display, XtWindow(w), NULL);
	XWithdrawWindow (obm->display, XtWindow(w),
	    XScreenNumberOfScreen(obm->screen));

	return (TCL_OK);
}


/* widgetManage -- Manage a list of child widgets.  These should share the
 * same common parent, a geometry widget of some sort.  Managing the
 * children makes them appear in the window, possibly causing the other
 * children to be rearranged in the window.
 *
 *  Usage:	manage child [child ...]
 *
 * This message should be sent to the geometry widget which is the parent
 * of the children.
 */
static int 
widgetManage (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Widget w, children[512];
	int nchildren, i;

	for (i=1, nchildren=0;  i < argc;  i++)
	    if ((w = XtNameToWidget (wp->w, argv[i])))
		children[nchildren++] = w;
		
	XtManageChildren (children, nchildren);
	return (TCL_OK);
}


/* widgetUnmanage -- Unmanage a list of child widgets.  These should share the
 * same common parent, a geometry widget of some sort.  Unmanaging the
 * children makes them disappear from the window and be removed from geometry
 * management, possibly causing the other children to be rearranged in the
 * window.
 *
 *  Usage:	unmanage child [child ...]
 *
 * This message should be sent to the geometry widget which is the parent
 * of the children.
 */
static int 
widgetUnmanage (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Widget w, children[512];
	int nchildren, i;

	for (i=1, nchildren=0;  i < argc;  i++)
	    if ((w = XtNameToWidget (wp->w, argv[i])))
		children[nchildren++] = w;

	XtUnmanageChildren (children, nchildren);
	return (TCL_OK);
}


/* widgetPopup -- Popup a shell widget.  If no grab is indicated the popup
 * can remain up while other windows accept input.
 *
 *  Usage:	popup [grab-kind]
 */
static int 
widgetPopup (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	XtGrabKind grab;

	grab = XtGrabNone;
	if (argc >= 2) {
	    if (strcmp (argv[1], "GrabNone") == 0)
		grab = XtGrabNone;
	    else if (strcmp (argv[1], "GrabNonexclusive") == 0)
		grab = XtGrabNonexclusive;
	    else if (strcmp (argv[1], "GrabExclusive") == 0)
		grab = XtGrabExclusive;
	}

	XtPopup (wp->w, grab);
	return (TCL_OK);
}


/* widgetPopdown -- Popdown a shell widget.
 *
 *  Usage:	popdown
 */
static int 
widgetPopdown (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	XtPopdown (wp->w);
	return (TCL_OK);
}


/* widgetPopupSpringLoaded -- Popup a shell widget, e.g., a popup menu.
 * This implies an exclusive grab.
 *
 *  Usage:	popupSpringLoaded
 */
static int
widgetPopupSpringLoaded (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;

	XtPopupSpringLoaded (wp->w);
	return (TCL_OK);
}


/* widgetMove -- Move a widget to the given window relative coordinates.
 *
 *  Usage:	move x y
 */
static int 
widgetMove (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Arg args[10];  int nargs=0;

	if (argc < 3)
	    return (TCL_ERROR);

	XtSetArg (args[nargs], XtNx, atoi(argv[1]));  nargs++;
	XtSetArg (args[nargs], XtNy, atoi(argv[2]));  nargs++;

	XtSetValues (wp->w, args, nargs);

	return (TCL_OK);
}


/* widgetResize -- Resize a widget.
 *
 *  Usage:	resize width height [border-width]
 */
static int 
widgetResize (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Arg args[10];  int nargs=0;

	if (argc < 3)
	    return (TCL_ERROR);

	XtSetArg (args[nargs], XtNwidth, atoi(argv[1]));  nargs++;
	XtSetArg (args[nargs], XtNheight, atoi(argv[2]));  nargs++;
	if (argc > 3) {
	    XtSetArg (args[nargs], XtNborderWidth, atoi(argv[3]));
	    nargs++;
	}

	XtSetValues (wp->w, args, nargs);

	return (TCL_OK);
}


/* widgetConfigure -- Configure a widget, i.e., execute a simultaneous
 * move and resize.
 *
 *  Usage:	configure x y width height [border-width]
 */
static int 
widgetConfigure (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Arg args[10];  int nargs=0;

	if (argc < 3)
	    return (TCL_ERROR);

	XtSetArg (args[nargs], XtNx, atoi(argv[1]));  nargs++;
	XtSetArg (args[nargs], XtNy, atoi(argv[2]));  nargs++;
	XtSetArg (args[nargs], XtNwidth, atoi(argv[3]));  nargs++;
	XtSetArg (args[nargs], XtNheight, atoi(argv[4]));  nargs++;
	if (argc > 5) {
	    XtSetArg (args[nargs], XtNborderWidth, atoi(argv[5]));
	    nargs++;
	}

	XtSetValues (wp->w, args, nargs);

	return (TCL_OK);
}


/* widgetParseGeometry -- Compute the position and size of a region within
 * a window , given a user defined geometry and a default geometry.
 *
 *  Usage:	parseGeometry user_geom def_geom x y width height
 *
 * Geometries are specified as in X, e.g. 123x456+5-5.  The default geometry
 * must be fully specified.
 */
static int 
widgetParseGeometry (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;

	int uflags, dflags;
	Dimension winWidth, winHeight;
	int need, x, y, width, height;
	char *user_geom, *def_geom;
	char *s_x, *s_y, *s_width, *s_height;
	unsigned int u_width, u_height;
	unsigned int d_width, d_height;
	int u_x, u_y, d_x, d_y;
	char buf[SZ_NUMBER];

	if (argc != 7)
	    return (TCL_ERROR);

	user_geom = argv[1];
	def_geom  = argv[2];
	s_x	  = argv[3];
	s_y	  = argv[4];
	s_width	  = argv[5];
	s_height  = argv[6];

	XtVaGetValues (wp->w,
	    XtNwidth,	&winWidth,
	    XtNheight,	&winHeight,
	    NULL);

	/* Parse the default geometry. */
	dflags = XParseGeometry (def_geom,  &d_x, &d_y, &d_width, &d_height);
	need = (XValue | YValue | WidthValue | HeightValue);
	if ((dflags & need) != need) {
	    Tcl_SetResult (obm->tcl,
		"default geometry not fully qualified", TCL_VOLATILE);
	    return (TCL_ERROR);
	}

	/* Parse the user supplied geometry. */
	uflags = XParseGeometry (user_geom, &u_x, &u_y, &u_width, &u_height);

	/* Compute the final geometry.  This is constrained to fit within
	 * the given window.
	 */
	width = (uflags & WidthValue) ? u_width : d_width;
	width = max(0, min((int)winWidth, width));

	height = (uflags & HeightValue) ? u_height : d_height;
	height = max(0, min((int)winHeight, height));

	if (uflags & XValue)
	    x = (uflags & XNegative) ? winWidth + u_x - width : u_x;
	else
	    x = (dflags & XNegative) ? winWidth + d_x - width : d_x;
	x = max(0, min((int)winWidth-width, x));

	if (uflags & YValue)
	    y = (uflags & YNegative) ? winHeight + u_y - height : u_y;
	else
	    y = (dflags & YNegative) ? winHeight + d_y - height : d_y;
	y = max(0, min((int)winHeight-height, y));

	/* Output the results.
	 */
	sprintf (buf, "%d", x);
	if ((Tcl_SetVar (obm->tcl, s_x, buf, 0)) == NULL)
	    return (TCL_ERROR);
	sprintf (buf, "%d", y);
	if ((Tcl_SetVar (obm->tcl, s_y, buf, 0)) == NULL)
	    return (TCL_ERROR);
	sprintf (buf, "%d", width);
	if ((Tcl_SetVar (obm->tcl, s_width, buf, 0)) == NULL)
	    return (TCL_ERROR);
	sprintf (buf, "%d", height);
	if ((Tcl_SetVar (obm->tcl, s_height, buf, 0)) == NULL)
	    return (TCL_ERROR);

	return (TCL_OK);
}


/* widgetGetGeometry -- Given a subregion within a rectangular window compute
 * the geometry specification which best describes the region.
 *
 *  Usage:	geom = getGeometry x y width height [nogravity]
 *
 * If gravity is enabled (the default) and the rect is near an edge or corner
 * the specified geometry will be in the form -X-Y to cause the region to
 * track the edge or corner of the window.  Otherwise the absolute coordinates
 * of the region are returned.
 */
static int 
widgetGetGeometry (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;

	char *op;
	Dimension winWidth, winHeight;
	int dist, gravity, x, y, width, height;
	char buf[128];

	if (argc < 5)
	    return (TCL_ERROR);

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	width = atoi(argv[3]);
	height = atoi(argv[4]);
	gravity = (argc < 6 || strncmp(argv[5],"no",2) != 0);

	XtVaGetValues (wp->w,
	    XtNwidth,	&winWidth,
	    XtNheight,	&winHeight,
	    NULL);

	sprintf (buf, "%dx%d", width, height);
	for (op=buf;  *op;  )
	    op++;

	if (gravity && (dist = winWidth - (x + width)) < 10)
	    sprintf (op, "-%d", dist);
	else
	    sprintf (op, "+%d", x);
	while (*op)
	    op++;

	if (gravity && (dist = winHeight - (y + height)) < 10)
	    sprintf (op, "-%d", dist);
	else
	    sprintf (op, "+%d", y);

	Tcl_SetResult (wp->obm->tcl, buf, TCL_VOLATILE);
	return (TCL_OK);
}


/* widgetSetSensitive -- Set the sensitivity of a widget.
 *
 *  Usage:	setSensitive <sensitive>
 */
static int 
widgetSetSensitive (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Boolean sensitive;

	sensitive = FALSE;
	if (argc >= 2)
	    if (strcmp (argv[1], "true") == 0 ||
		strcmp (argv[1], "True") == 0 ||
		strcmp (argv[1], "TRUE") == 0 ||
		strcmp (argv[1], "1") == 0) {

		sensitive = TRUE;
	    }

	XtSetSensitive (wp->w, sensitive);
	return (TCL_OK);
}


/* widgetIsSensitive -- Test the sensitivity of a widget.
 *
 *  Usage:	isSensitive
 */
static int 
widgetIsSensitive (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	Boolean sensitive;

	if (XtIsSensitive (wp->w))
	    Tcl_SetResult (wp->obm->tcl, TRUESTR, TCL_STATIC);
	else
	    Tcl_SetResult (wp->obm->tcl, FALSESTR, TCL_STATIC);

	return (TCL_OK);
}


/*
 * Event handling facility.
 */

/* Event masks. */
#define	NonMaskable	0
struct evMask {
	char *name;
	int mask;
} eventMasks[] = {
	{ "nonMaskable",		NonMaskable },
	{ "button1MotionMask",		Button1MotionMask },
	{ "button2MotionMask",		Button2MotionMask },
	{ "button3MotionMask",		Button3MotionMask },
	{ "button4MotionMask",		Button4MotionMask },
	{ "button5MotionMask",		Button5MotionMask },
	{ "buttonMotionMask",		ButtonMotionMask },
	{ "buttonPressMask",		ButtonPressMask },
	{ "buttonReleaseMask",		ButtonReleaseMask },
	{ "colormapChangeMask",		ColormapChangeMask },
	{ "enterWindowMask",		EnterWindowMask },
	{ "exposureMask",		ExposureMask },
	{ "focusChangeMask",		FocusChangeMask },
	{ "keyPressMask",		KeyPressMask },
	{ "keyReleaseMask",		KeyReleaseMask },
	{ "keymapStateMask",		KeymapStateMask },
	{ "leaveWindowMask",		LeaveWindowMask },
	{ "noEventMask",		NoEventMask },
	{ "ownerGrabButtonMask",	OwnerGrabButtonMask },
	{ "pointerMotionHintMask",	PointerMotionHintMask },
	{ "pointerMotionMask",		PointerMotionMask },
	{ "propertyChangeMask",		PropertyChangeMask },
	{ "resizeRedirectMask",		ResizeRedirectMask },
	{ "structureNotifyMask",	StructureNotifyMask },
	{ "substructureNotifyMask",	SubstructureNotifyMask },
	{ "substructureRedirectMask",	SubstructureRedirectMask },
	{ "visibilityChangeMask",	VisibilityChangeMask },
};

/* Event types. */
struct evType {
	char *name;
	int type;
} eventTypes[] = {
	{ "buttonPress",		ButtonPress },
	{ "buttonRelease",		ButtonRelease },
	{ "circulateNotify",		CirculateNotify },
	{ "circulateRequest",		CirculateRequest },
	{ "clientMessage",		ClientMessage },
	{ "colormapNotify",		ColormapNotify },
	{ "configureNotify",		ConfigureNotify },
	{ "configureRequest",		ConfigureRequest },
	{ "createNotify",		CreateNotify },
	{ "destroyNotify",		DestroyNotify },
	{ "enterNotify",		EnterNotify },
	{ "expose",			Expose },
	{ "focusIn",			FocusIn },
	{ "focusOut",			FocusOut },
	{ "graphicsExpose",		GraphicsExpose },
	{ "gravityNotify",		GravityNotify },
	{ "keyPress",			KeyPress },
	{ "keyRelease",			KeyRelease },
	{ "keymapNotify",		KeymapNotify },
	{ "leaveNotify",		LeaveNotify },
	{ "mapNotify",			MapNotify },
	{ "mapRequest",			MapRequest },
	{ "mappingNotify",		MappingNotify },
	{ "motionNotify",		MotionNotify },
	{ "noExpose",			NoExpose },
	{ "propertyNotify",		PropertyNotify },
	{ "reparentNotify",		ReparentNotify },
	{ "resizeRequest",		ResizeRequest },
	{ "selectionClear",		SelectionClear },
	{ "selectionNotify",		SelectionNotify },
	{ "selectionRequest",		SelectionRequest },
	{ "unmapNotify",		UnmapNotify },
	{ "visibilityNotify",		VisibilityNotify },
};


/* widgetAddEventHandler -- Add a custom event handler to a widget.  A list
 * of event masks is given to define the classes of events the user supplied
 * event handling procedure is to receive.
 *
 *  Usage:	addEventHandler <procname> <event-mask> [<event-mask>...]
 */
static int 
widgetAddEventHandler (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	ObmCallback cb, new_cb;
	int event_mask, i, j;
	Boolean nonmaskable;

	if (argc < 3)
	    return (TCL_ERROR);

	event_mask = 0;
	nonmaskable = FALSE;

	/* Get the event mask. */
	for (j=2;  j < argc;  j++) {
	    for (i=0;  i < XtNumber(eventMasks);  i++) {
		if (strcmp (eventMasks[i].name, argv[j]) == 0) {
		    if (eventMasks[i].mask == NonMaskable)
			nonmaskable = TRUE;
		    else
			event_mask |= eventMasks[i].mask;
		    break;
		}
	    }
	}

	/* Create event handler record. */
	new_cb = (ObmCallback) XtCalloc (1, sizeof (obmCallback));
	strcpy (new_cb->name, argv[1]);
	new_cb->u.obj = (ObmObject) obj;
	new_cb->client_data = (XtPointer) event_mask;

	/* Add record to tail of event handler list. */
	if (wp->event_handler) {
	    for (cb = wp->event_handler;  cb->next;  cb = cb->next)
		;
	    cb->next = new_cb;
	} else
	    wp->event_handler = new_cb;

	/* Post event handler. */
	XtAddEventHandler (wp->w, event_mask, nonmaskable, (XtEventHandler) widgetEvent,
			   new_cb);

	return (TCL_OK);
}

/* widgetRemoveEventHandler -- Remove an event handler previously posted
 * with addEventHandler, above.
 *
 * Usage:	removeEventHandler procname
 */
static int
widgetRemoveEventHandler (MsgContext msg, Tcl_Interp *tcl, int argc, char **argv)
{
	WidgetObject obj = (WidgetObject) msg->object[msg->level];
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	ObmCallback cb, pcb;
	Boolean nonmaskable;
	char *procname;

	if (argc < 2)
	    return (TCL_ERROR);

	procname = argv[1];
	nonmaskable = False;

	for (cb = wp->event_handler, pcb=NULL;  cb;  pcb=cb, cb = cb->next)
	    if (strcmp (cb->name, procname) == 0)
		break;

	if (cb) {
	    XtRemoveEventHandler (wp->w, (int) cb->client_data, nonmaskable,
		(XtEventHandler) widgetEvent, cb);
	    if (pcb)
		pcb->next = cb->next;
	    else
		wp->event_handler = NULL;
	    XtFree ((char *)cb);
	}

	return (TCL_OK);
}


/* widgetEvent -- Generic event handler called when a widget event handler
 * posted by addEventHandler is called.
 *
 * The user event handler is called as
 *
 *	userEventHandler widget event-type time wx wy rx ry other
 *
 * where "other" is an event-type specific list of fields describing the
 * the event.
 */
static void
widgetEvent (Widget w, ObmCallback cb, XEvent *event, Boolean *continue_to_dispatch)
{
	WidgetObject obj = (WidgetObject) cb->u.obj;
	WidgetPrivate wp = &obj->widget;
	ObmContext obm = wp->obm;
	char cmd[SZ_COMMAND];
	char *ip, *op;
	int i, j;
	int status;

	/* Our job is to translate the X event into a call to a widget server
	 * procedure.  Start with the callback procedure name.
	 */
	for (ip = cb->name, op=cmd;  *ip;  )
	    *op++ = *ip++;
	*op++ = ' ';

	/* Add the name of the widget that received the event. */
	for (ip = obj->core.name;  *ip;  )
	    *op++ = *ip++;
	*op++ = ' ';

	/* Add the event type. */
	for (i=0;  i < XtNumber(eventTypes);  i++) {
	    if (eventTypes[i].type == event->type) {
		for (ip = eventTypes[i].name;  *ip;  )
		    *op++ = *ip++;
		*op++ = ' ';
		break;
	    }

	    /* Ignore events we don't know anything about. */
	    if (i >= XtNumber(eventTypes))
		return;
	}

	/* Add the event specific fields. */
	switch (event->type) {
	case KeyPress:
	case KeyRelease:
	    {	XKeyPressedEvent *ev = (XKeyPressedEvent *) event;
		char buf[20];
		int n;

		sprintf (op, "%lu %d %d %d %d ",
		    ev->time, ev->x, ev->y, ev->x_root, ev->y_root);
		while (*op) op++;

		*op++ = '{';
		if ((n = XLookupString(ev,buf,sizeof(buf),NULL,NULL)) > 0) {
		    for (ip=buf;  --n >= 0;  )
			if (*ip <= ' ') {
			    *op++ = '^';
			    *op++ = *ip++ + 'A' - 1;
			} else if (isprint (*ip)) {
			    *op++ = *ip++;
			} else
			    ip++;
		} else {
		    /* This case occurs when only a modifier is typed. */
		    for (ip = "??";  (*op++ = *ip++); )
			;
		}
		*op++ = ' ';
		op = widgetEventState (op, ev->state);
		while (op > cmd && isspace (*(op-1)))
		    --op;
		*op++ = '}';
	    }
	    break;

	case ButtonPress:
	case ButtonRelease:
	    {	XButtonPressedEvent *ev = (XButtonPressedEvent *) event;

		sprintf (op, "%lu %d %d %d %d ",
		    ev->time, ev->x, ev->y, ev->x_root, ev->y_root);
		while (*op) op++;

		*op++ = '{';
		sprintf (op, "%d ", ev->button);  while (*op) op++;
		op = widgetEventState (op, ev->state);
		while (op > cmd && isspace (*(op-1)))
		    --op;
		*op++ = '}';
	    }
	    break;

	case KeymapNotify:
	    {	XKeymapEvent *ev = (XKeymapEvent *) event;
		KeySym keysym;

		sprintf (op, "0 0 0 0 0 ");
		while (*op) op++;

		*op++ = '{';
		for (j=0;  j < 32;  j++) {
		    for (i=0;  i < 8;  i++)
			if ((ev->key_vector[j]) & (1 << i)) {
			    keysym = XKeycodeToKeysym (obm->display,
				j * 8 + i, 0);
			    if ((ip = XKeysymToString (keysym))) {
				while (*ip)
				    *op++ = *ip++;
				*op++ = ' ';
			    }
			}
		}
		*op++ = '}';
	    }
	    break;

	case MotionNotify:
	case EnterNotify:
	case LeaveNotify:
	    {	XPointerMovedEvent *ev = (XPointerMovedEvent *) event;

		sprintf (op, "%lu %d %d %d %d ",
		    ev->time, ev->x, ev->y, ev->x_root, ev->y_root);
		while (*op) op++;

		*op++ = '{';
		op = widgetEventState (op, ev->state);
		while (op > cmd && isspace (*(op-1)))
		    --op;
		*op++ = '}';
	    }
	    break;

	case FocusIn:
	case FocusOut:
	    {	XFocusChangeEvent *ev = (XFocusChangeEvent *) event;

		sprintf (op, "0 0 0 0 0 ");
		while (*op) op++;
	    }
	    break;

	case Expose:
	    {	XExposeEvent *ev = (XExposeEvent *) event;

		sprintf (op, "0 %d %d 0 0 ", ev->x, ev->y);
		while (*op) op++;

		*op++ = '{';
		sprintf (op, "%d ", ev->width);  while (*op) op++;
		sprintf (op, "%d ", ev->height);  while (*op) op++;
		sprintf (op, "%d ", ev->count);  while (*op) op++;
		*op++ = '}';
	    }
	    break;

	case GraphicsExpose:
	    {	XGraphicsExposeEvent *ev = (XGraphicsExposeEvent *) event;

		sprintf (op, "0 %d %d 0 0 ", ev->x, ev->y);
		while (*op) op++;

		*op++ = '{';
		sprintf (op, "%d ", ev->width);  while (*op) op++;
		sprintf (op, "%d ", ev->height);  while (*op) op++;
		sprintf (op, "%d ", ev->count);  while (*op) op++;
		*op++ = '}';
	    }
	    break;

	case NoExpose:
	case ColormapNotify:
	case PropertyNotify:
	case VisibilityNotify:
	case ResizeRequest:
	case CirculateNotify:
	case ConfigureNotify:
	case CreateNotify:
	case DestroyNotify:
	case GravityNotify:
	case MapNotify:
	case MappingNotify:
	case ReparentNotify:
	case SelectionNotify:
	case UnmapNotify:
	    {
		sprintf (op, "0 0 0 0 0 ");
		while (*op) op++;
	    }
	    break;

	case CirculateRequest:
	case ConfigureRequest:
	case MapRequest:
	case SelectionRequest:
	    {
		sprintf (op, "0 0 0 0 0 ");
		while (*op) op++;
	    }
	    break;


	case ClientMessage:
	case SelectionClear:
	    {
		sprintf (op, "0 0 0 0 0 ");
		while (*op) op++;
	    }
	    break;
	}
	*op = '\0';

	/* Call the user supplied event handler. */
	status = Tcl_Eval (obm->tcl, cmd);
	if (status != TCL_OK) {
	    fprintf (stderr, "Error on line %d of %s: %s\n",
		Tcl_GetErrorLine (obm->tcl), cb->name, Tcl_GetStringResult (obm->tcl));
	}
}


/* widgetEventState -- Encode the "state" field of an event struct.
 */
char *
widgetEventState (char *op, unsigned int state)
{
	if (state & ShiftMask)
	    { sprintf (op, "shift ");  while (*op) op++; }
	if (state & LockMask)
	    { sprintf (op, "lock ");  while (*op) op++; }
	if (state & ControlMask)
	    { sprintf (op, "control ");  while (*op) op++; }
	if (state & Mod1Mask)
	    { sprintf (op, "mod1 ");  while (*op) op++; }
	if (state & Mod2Mask)
	    { sprintf (op, "mod2 ");  while (*op) op++; }
	if (state & Mod3Mask)
	    { sprintf (op, "mod3 ");  while (*op) op++; }
	if (state & Mod4Mask)
	    { sprintf (op, "mod4 ");  while (*op) op++; }
	if (state & Mod5Mask)
	    { sprintf (op, "mod5 ");  while (*op) op++; }

	*op = '\0';
	return (op);
}



/* widgetGetFontName -- Encode the font name in a string in XLFD format.
 */

#define	SZ_FONT_NAME		128

static char *
widgetGetFontName (Display *display, XFontStruct *fs) /* MF016 */
{
        int i;
        unsigned long val;
	char *name = (char *) malloc (SZ_FONT_NAME), *str, *lp;

	name[0] = '\0';
        if (fs) {
            for (i=0; i < NUMITEMS(fontNamePropTable); i++) {
                fontNamePropTable[i].atom = 
                    XInternAtom(display, fontNamePropTable[i].name, 0);
                if (XGetFontProperty (fs, fontNamePropTable[i].atom, &val)) {
                    switch (fontNamePropTable[i].type) {
                    case atom:
                        str = XGetAtomName (display, (Atom)val);
			for (lp=str; *lp; lp++)
			    if (isupper(*lp))
				*lp = tolower(*lp);
                        strcat (name, str);
			XFree (str);
                        break;

                    case pixel_size:
                    case point_size:
                    case resolution:
                    case resolution_x:
                    case resolution_y:
                    case average_width:
                    case scaledX:
                    case scaledY:
                    case unscaled:
                    case scaledXoverY:
                    case uncomputed:
                        sprintf(name, "%s%lu", name, val);
                        break;
                    }
                } else
                    strcat(name, "*");

                if (i != (NUMITEMS(fontNamePropTable)-1))
                    strcat(name, "-");
            }
	}

	return (name);
}
