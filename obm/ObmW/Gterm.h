#ifndef _Gterm_h
#define _Gterm_h

/* Parameters:
(this is not yet updated for xgterm - the rest of this file is)

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		White
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 foreground	     Foreground		Pixel		Black
 height		     Height		Dimension	240
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 reverseVideo	     ReverseVideo	Boolean		False
 width		     Width		Dimension	320
 x		     Position		Position	0
 y		     Position		Position	0

*/

/*
 * Resource definitions.
 */
#define XtCInt				"Int"

#define XtNalphaFont1			"alphaFont1"
#define XtNalphaFont2			"alphaFont2"
#define XtNalphaFont3			"alphaFont3"
#define XtNalphaFont4			"alphaFont4"
#define XtNalphaFont5			"alphaFont5"
#define XtNalphaFont6			"alphaFont6"
#define XtNalphaFont7			"alphaFont7"
#define XtNalphaFont8			"alphaFont8"

#define XtNdialogFont1			"dialogFont1"
#define XtNdialogFont2			"dialogFont2"
#define XtNdialogFont3			"dialogFont3"
#define XtNdialogFont4			"dialogFont4"
#define XtNdialogFont5			"dialogFont5"
#define XtNdialogFont6			"dialogFont6"
#define XtNdialogFont7			"dialogFont7"
#define XtNdialogFont8			"dialogFont8"

#define XtNdialogBgColor		"dialogBgColor"
#define XtNdialogFgColor		"dialogFgColor"
#define XtNidleCursorBgColor		"idleCursorBgColor"
#define XtNidleCursorFgColor		"idleCursorFgColor"
#define XtNbusyCursorBgColor		"busyCursorBgColor"
#define XtNbusyCursorFgColor		"busyCursorFgColor"
#define XtNginmodeCursorBgColor		"ginmodeCursorBgColor"
#define XtNginmodeCursorFgColor		"ginmodeCursorFgColor"
#define XtNginmodeBlinkInterval		"ginmodeBlinkInterval"
#define XtNcrosshairCursorColor		"crosshairCursorColor"

#define XtNidleCursor			"idleCursor"
#define XtNbusyCursor			"busyCursor"
#define XtNginmodeCursor		"ginmodeCursor"
#define XtNwarpCursor			"warpCursor"
#define XtNraiseWindow			"raiseWindow"
#define XtNdeiconifyWindow		"deiconifyWindow"
#define XtNuseTimers			"useTimers"

#define XtNcolor0			"color0"
#define XtNcolor1			"color1"
#define XtNcolor2			"color2"
#define XtNcolor3			"color3"
#define XtNcolor4			"color4"
#define XtNcolor5			"color5"
#define XtNcolor6			"color6"
#define XtNcolor7			"color7"
#define XtNcolor8			"color8"
#define XtNcolor9			"color9"

#define XtNcmapName			"cmapName"
#define XtNuseGlobalCmap                "useGlobalCmap"
#define XtNcmapInitialize		"cmapInitialize"
#define XtNcopyOnResize			"copyOnResize"
#define XtNbasePixel			"basePixel"
#define XtNcmapUpdate			"cmapUpdate"
#define XtNcmapShadow			"cmapShadow"
#define XtNcmapInterpolate		"cmapInterpolate"
#define XtNmaxRasters			"maxRasters"
#define XtNcacheRasters			"cacheRasters"
#define XtNmaxMappings			"maxMappings"
#define XtNmaxColors			"maxColors"

#define XtNmarkerTranslations		"markerTranslations"
#define XtNdefaultMarker		"defaultMarker"
#define XtNnearEdge			"nearEdge"
#define XtNnearVertex			"nearVertex"
#define XtNmarkerLineWidth		"markerLineWidth"
#define XtNmarkerLineStyle		"markerLineStyle"
#define XtNmarkerFill			"markerFill"
#define XtNmarkerFillColor		"markerFillColor"
#define XtNmarkerFillBgColor		"markerFillBgColor"
#define XtNmarkerFillStyle		"markerFillStyle"
#define XtNxorFill			"xorFill"
#define XtNxorFillColor			"xorFillColor"
#define XtNxorFillBgColor		"xorFillBgColor"
#define XtNmarkerHighlightWidth		"markerHighlightWidth"
#define XtNmarkerHighlightColor		"markerHighlightColor"
#define XtNmarkerCursorFgColor		"markerCursorFgColor"
#define XtNmarkerCursorBgColor		"markerCursorBgColor"
#define XtNmarkerLineLineColor		"markerLineLineColor"
#define XtNmarkerLineKnotColor		"markerLineKnotColor"
#define XtNmarkerLineKnotSize		"markerLineKnotSize"
#define XtNmarkerTextLineColor		"markerTextLineColor"
#define XtNmarkerTextColor		"markerTextColor"
#define XtNmarkerTextBgColor		"markerTextBgColor"
#define XtNmarkerTextBorder		"markerTextBorder"
#define XtNmarkerTextFont		"markerTextFont"
#define XtNmarkerTextString		"markerTextString"
#define XtNmarkerRectLineColor		"markerRectLineColor"
#define XtNmarkerRectKnotColor		"markerRectKnotColor"
#define XtNmarkerRectKnotSize		"markerRectKnotSize"
#define XtNmarkerBoxLineColor		"markerBoxLineColor"
#define XtNmarkerBoxKnotColor		"markerBoxKnotColor"
#define XtNmarkerBoxKnotSize		"markerBoxKnotSize"
#define XtNmarkerCircleLineColor	"markerCircleLineColor"
#define XtNmarkerCircleKnotColor	"markerCircleKnotColor"
#define XtNmarkerCircleKnotSize		"markerCircleKnotSize"
#define XtNmarkerEllipseLineColor	"markerEllipseLineColor"
#define XtNmarkerEllipseKnotColor	"markerEllipseKnotColor"
#define XtNmarkerEllipseKnotSize	"markerEllipseKnotSize"
#define XtNmarkerPgonLineColor		"markerPgonLineColor"
#define XtNmarkerPgonKnotColor		"markerPgonKnotColor"
#define XtNmarkerPgonKnotSize		"markerPgonKnotSize"


/* Deep Frame Definitions */
#define XtNdialogBgColorStr             "dialogBgColorStr"
#define XtNdialogFgColorStr             "dialogFgColorStr"
#define XtNidleCursorBgColorStr         "idleCursorBgColorStr"
#define XtNidleCursorFgColorStr         "idleCursorFgColorStr"
#define XtNbusyCursorBgColorStr         "busyCursorBgColorStr"
#define XtNbusyCursorFgColorStr         "busyCursorFgColorStr"
#define XtNginmodeCursorBgColorStr      "ginmodeCursorBgColorStr"
#define XtNginmodeCursorFgColorStr      "ginmodeCursorFgColorStr"
#define XtNcrosshairCursorColorStr      "crosshairCursorColorStr"

#define XtNcolor0Str                    "color0Str"
#define XtNcolor1Str                    "color1Str"
#define XtNcolor2Str                    "color2Str"
#define XtNcolor3Str                    "color3Str"
#define XtNcolor4Str                    "color4Str"
#define XtNcolor5Str                    "color5Str"
#define XtNcolor6Str                    "color6Str"
#define XtNcolor7Str                    "color7Str"
#define XtNcolor8Str                    "color8Str"
#define XtNcolor9Str                    "color9Str"

#define XtNmarkerFillColorStr           "markerFillColorStr"
#define XtNmarkerFillBgColorStr         "markerFillBgColorStr"

#define XtNmarkerHighlightColorStr      "markerHighlightColorStr"
#define XtNmarkerCursorFgColorStr       "markerCursorFgColorStr"
#define XtNmarkerCursorBgColorStr       "markerCursorBgColorStr"

#define XtNmarkerLineLineColorStr       "markerLineLineColorStr"
#define XtNmarkerLineKnotColorStr       "markerLineKnotColorStr"

#define XtNmarkerTextLineColorStr       "markerTextLineColorStr"
#define XtNmarkerTextColorStr           "markerTextColorStr"
#define XtNmarkerTextBgColorStr         "markerTextBgColorStr"

#define XtNmarkerRectLineColorStr       "markerRectLineColorStr"
#define XtNmarkerRectKnotColorStr       "markerRectKnotColorStr"

#define XtNmarkerBoxLineColorStr        "markerBoxLineColorStr"
#define XtNmarkerBoxKnotColorStr        "markerBoxKnotColorStr"

#define XtNmarkerCircleLineColorStr     "markerCircleLineColorStr"
#define XtNmarkerCircleKnotColorStr     "markerCircleKnotColorStr"

#define XtNmarkerEllipseLineColorStr    "markerEllipseLineColorStr"
#define XtNmarkerEllipseKnotColorStr    "markerEllipseKnotColorStr"

#define XtNmarkerPgonLineColorStr       "markerPgonLineColorStr"
#define XtNmarkerPgonKnotColorStr       "markerPgonKnotColorStr"

#define XtNmarkerPointLineColorStr      "markerPointLineColorStr"
#define XtNmarkerPointKnotColorStr      "markerPointKnotColorStr"
/* Deep Frame Definitions */



/*
 * Gterm widget flags.
 */
#define GtSet			1		/* drawing mode */
#define GtClear			2
#define GtInvert		3

#define GtOutline		1		/* line styles */
#define GtPoint			2
#define GtSolid			3
#define GtDashed		4
#define GtDotted		5
#define GtDashDot		6
#define GtDash3Dot		7

#define GtNoCursor		0		/* cursor types */
#define GtGinmodeCursor		1
#define GtBusyCursor		2
#define GtIdleCursor		3

#define GtDefault		0		/* raster types */
#define GtClient		1
#define GtServer		2

#define GtPixel			0		/* coordinate types */
#define GtNDC			1

#define	GtMap			0		/* mapping direction */
#define GtUnmap			1

#define GtWindow		0		/* drawable types */
#define GtWidget		1

#define R_OpcodeMask		0000017		/* rasterop definitions */
#define R_Transient		0000020
#define R_RefreshAll		0000040
#define R_RefreshNone		0000100
#define	R_MFMask		0777000

#define	MF_NEAREST		0001000		/* antialiasing functions */
#define	MF_BILINEAR		0002000
#define	MF_AREA			0004000
#define	MF_BLKAVG		0010000
#define	MF_BOXCAR		0020000
#define	MF_LOWPASS		0040000
#define	MF_GAUSSIAN		0100000

#define	GmText			"text"		/* graphics marker types */
#define	GmLine			"line"
#define	GmPolyline		"polyline"
#define	GmRectangle		"rectangle"
#define	GmBox			"box"
#define	GmCircle		"circle"
#define	GmEllipse		"ellipse"
#define	GmPolygon		"polygon"

#define	Gm_Text			1		/* integer codes for above */
#define	Gm_Line			2
#define	Gm_Polyline		3
#define	Gm_Rectangle		4
#define	Gm_Box			5
#define	Gm_Circle		6
#define	Gm_Ellipse		7
#define	Gm_Polygon		8
#define	Gm_NTypes		8

#define	GmType			"type"		/* marker attributes */
#define	GmActivated		"activated"
#define	GmVisible		"visible"
#define	GmSensitive		"sensitive"
#define	GmAutoRedraw		"autoRedraw"
#define	GmTranslations		"translations"
#define	GmX			"x"
#define	GmY			"y"
#define	GmWidth			"width"
#define	GmHeight		"height"
#define	GmRotangle		"rotangle"
#define	GmHighlightColor	"highlightColor"
#define	GmLineColor		"lineColor"
#define	GmLineWidth		"lineWidth"
#define	GmLineStyle		"lineStyle"
#define	GmKnotColor		"knotColor"
#define	GmKnotSize		"knotSize"
#define	GmFill			"fill"
#define	GmFillColor		"fillColor"
#define	GmFillBgColor		"fillBgColor"
#define	GmFillPattern		"fillPattern"
#define	GmFillStyle		"fillStyle"
#define	GmTextColor		"textColor"
#define	GmTextBgColor		"textBgColor"
#define	GmTextBorder		"textBorder"
#define	GmImageText		"imageText"
#define	GmFont			"font"
#define	GmRotIndicator		"rotIndicator"			/* MF020 */

#define	GmEvNotify		00001		/* marker callback events */
#define	GmEvMoveResize		00002
#define	GmEvModify		00004
#define	GmEvRedraw		00010
#define	GmEvDestroy		00020
#define	GmEvInput		00040
#define	GmEvFocusIn		00100
#define	GmEvFocusOut		00200
#define	GmEvConstraint		00400

/* Double version of XPoint. */
struct dPoint {
	double x;
	double y;
};
typedef struct dPoint DPoint;

typedef struct _GtermRec *GtermWidget;
typedef struct _GtermClassRec *GtermWidgetClass;

extern WidgetClass gtermWidgetClass;

/*
 * Gterm function declarations
 */
typedef unsigned char	uchar;
typedef void (*GtCallbackProc)();

extern void GtActivate(GtermWidget w);
extern void GtDeactivate(GtermWidget w);
extern void GtReset(GtermWidget w);
extern void GtFlush(GtermWidget w);
extern int GtReady(GtermWidget w);
extern void GtTimerInhibit(GtermWidget w, Boolean state);
extern void GtSetDataLevel(GtermWidget w, int ival);
extern void GtDeleteInputProc(GtermWidget w, GtCallbackProc userfcn, XtPointer client_data);
extern void GtDeleteResizeProc(GtermWidget w, GtCallbackProc userfcn, XtPointer client_data);
extern void GtDeleteResetProc(GtermWidget w, GtCallbackProc userfcn, XtPointer client_data);
extern void GtPostInputProc(GtermWidget w, GtCallbackProc userfcn, XtPointer client_data);
extern void GtPostResetProc(GtermWidget w, GtCallbackProc userfcn, XtPointer client_data);
extern void GtPostResizeProc(GtermWidget w, GtCallbackProc userfcn, XtPointer client_data);
extern int GtQueryRaster(GtermWidget w, int raster, int *type,
			 int *width, int *height, int *depth);
extern void GtSetRaster();
extern int GtGetRaster();
extern void GtSetCursorPos();
extern void GtGetCursorPos();
extern void GtSetCursorType();
extern void GtBell();
extern void GtSetLogRes();
extern void GtGetLogRes();
extern void GtGetPhysRes();
extern void GtSetPhysRes();
extern void GtSetLineWidth();
extern void GtSetLineStyle();
extern void GtSetColorIndex();
extern void GtSetFillType();
extern void GtClearScreen();
extern void GtDrawPolyline();
extern void GtDrawPolymarker();
extern void GtDrawPolygon();
extern void GtDrawAlphaText();
extern void GtGetAlphaTextSize();
extern void GtStartDialog();
extern void GtEndDialog();
extern void GtEraseDialog();
extern void GtDrawDialogText();
extern void GtGetDialogTextSize();
extern void GtRasterInit(GtermWidget w);
extern void initialize_shadow_pixmap ();
extern int GtAssignRaster(GtermWidget w, int raster, XtPointer drawable, int type);
extern int GtCreateRaster(GtermWidget w, int raster, int type,
			  int width, int height, int depth);
extern void GtDestroyRaster(GtermWidget w, int raster);
extern int GtNextRaster(GtermWidget w);
extern int GtNRasters(GtermWidget w);
extern int GtWritePixels(GtermWidget w, int raster, uchar *pixels, int nbits,
			 int x1, int y1, int nx, int ny);
extern int GtReadPixels(GtermWidget w, int raster, uchar *pixels, int nbits,
			int x1, int y1, int nx, int ny);
extern void GtRefreshPixels(GtermWidget w, int raster, int ct,
			    int x1, int y1, int nx, int ny);
extern int GtGetClientPixel();
extern int GtSetPixels(GtermWidget w, int raster, int ct, int x1, int y1,
		       int nx, int ny, int color, int rop);
extern Pixmap GtExtractPixmap(GtermWidget w, int src, int ctype,
			      int x, int y, int width, int height);
extern int GtInsertPixmap(GtermWidget w, Pixmap pixmap, int dst, int ctype,
			  int x, int y, int width, int height);
extern int GtNextColormap();
extern void GtFreeColormap();
extern int GtWriteColormap(GtermWidget w, int map, int first, int nelem,
			   ushort *r, ushort *g, ushort *b);
extern int GtReadColormap(GtermWidget w, int map, int first, int nelem,
			  ushort *r, ushort *g, ushort *b);
extern int GtLoadColormap(GtermWidget w, int map, float offset, float slope);
extern int GtQueryColormap();
extern void GtSetColormapFocus();
extern void GtInitMappings();
extern int GtNextMapping();
extern void GtFreeMapping();
extern int GtDisableMapping();
extern void GtLowerMapping();
extern void GtRaiseMapping();
extern int GtActiveMapping();
extern int GtEnableMapping();
extern void GtRefreshMapping();
extern int GtSetMapping();
extern int GtGetMapping();
extern int GtCompareMappings();
extern int GtCopyRaster();
extern int GtSelectRaster();
extern void GtSetDisplayRaster();
extern void GtMarkerInit();
extern void GtMapVector();
extern void GtPixelToNDC();
extern void GtNDCToPixel();
extern void GtOverrideTranslations();
extern void GtWriteAlphaCursor();
extern void GtEraseAlphaCursor();
extern void GtSetTextRes();
extern void GtReadIomap();
extern void GtWriteIomap();
extern void GtReadLUT();

extern void GmAddCallback();
extern void GmDeleteCallback();
extern XtPointer GmCreate();
extern int GmDestroy();
extern int GmGetAttribute();
extern void GmGetAttributes();
extern void GmGetBoundingBox();
extern int GmGetVertices();
extern int GmStrToType();
extern int GmStrToEvent();
extern int GmStrToFunction();
extern void GmLower();
extern int GmSetAttributes();
extern int GmSetAttribute();
extern void GmMarkpos();
extern void GmRedraw();
extern void GmNotify();
extern void GmRaise();
extern void GmSetVertices();

#endif /* _Gterm_h */
