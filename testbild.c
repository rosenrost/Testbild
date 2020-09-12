/** Test pattern
 *  by Oliver Graf <og@graf-bussmeier.de>
 *
 *  Created:       Jun  2, 2019
 *  Last modified: Jun  2, 2019
 */


#include <gem.h>
#include <osbind.h>
#include <string.h>


static short vdi_handle;
static short work_out[57];

static short screen_width;
static short screen_height;

static short hline_width;
static short vline_width;

static void draw_pattern();
static void draw_colours(short x, short y, short w, short h);
static void draw_colour_box(short x, short y, short w, short h, short xr, short colour_count, const short colour[]);

static void clip(short x1, short y1, short x2, short y2);
static void clip_screen();
static void draw_bar(short x1, short y1, short x2, short y2, short colour);
static void draw_hline(short x, short y1, short y2, short colour);
static void draw_vline(short x1, short x2, short y, short colour);


#define SCREEN_MAX_X    work_out[0]
#define SCREEN_MAX_Y    work_out[1]
#define SCREEN_COLORS   work_out[13]

#define ARRAYSIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

#define TITLE           "ATARI TEST PATTERN"


int
main(int argc, char* argv[])
{
    int app_id = appl_init();

    if (app_id < 0) {
        // AES init failed
        return 1;
    }

    static short work_in[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 };
    short font_width, font_height;
    short dummy;

    work_in[0] = Getrez() + 2;

    vdi_handle = graf_handle(&font_width, &font_height, &dummy, &dummy);
    v_opnvwk(work_in, &vdi_handle, work_out);

    screen_width  = SCREEN_MAX_X + 1;
    screen_height = SCREEN_MAX_Y + 1;
    hline_width   = (screen_width  + 160) / 320;
    vline_width   = (screen_height + 100) / 200;

    if (SCREEN_COLORS < 16) {
        form_alert(1, "[1][ This program does not work | with less than 16 colours. ][ Exit ]");
    } else {
        clip_screen();

        form_dial(FMD_START, 0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height);

        wind_update(BEG_UPDATE);
        wind_update(BEG_MCTRL);

        graf_mouse(M_OFF, NULL);
        draw_pattern();
        evnt_keybd();
        graf_mouse(M_ON, NULL);

        wind_update(END_MCTRL);
        wind_update(END_UPDATE);

        // Workaround for multitasking environments:
        // Show a dummy menu bar to force desktop
        // to redraw its own one after termination.
        OBJECT dummy_menu[] = {
            { -1,  1,  4, G_IBOX,   OF_NONE,   OS_NORMAL,   { 0L               }, 0, 0,               screen_width,   screen_height   }, //  0: root (i.e. whole screen area)
            {  4,  2,  2, G_BOX,    OF_NONE,   OS_NORMAL,   { 0L               }, 0, 0,               screen_width,   font_height + 2 }, //  1: menu bar
            {  1,  3,  3, G_IBOX,   OF_NONE,   OS_NORMAL,   { 0L               }, 0, 0,               screen_width,   font_height     }, //  2: top entries area
            {  2, -1, -1, G_TITLE,  OF_NONE,   OS_NORMAL,   { (long)" Desk "   }, 0, 0,               font_width * 6, font_height * 1 }, //  3: first (and only) top entry
            {  0,  5,  5, G_IBOX,   OF_NONE,   OS_NORMAL,   { 0L               }, 0, 2 + font_height, font_width * 8, font_height * 8 }, //  4: child area of top entry
            {  4,  6,  6, G_BOX,    OF_NONE,   OS_NORMAL,   { 0L               }, 0, 2 + font_height, font_width * 8, font_height * 8 }, //  5: box of child area (8 children)
            {  7, -1, -1, G_STRING, OF_NONE,   OS_NORMAL,   { (long)"  About " }, 0, 0 * font_height, font_width * 8, font_height * 1 }, //  6: first child entry
            {  8, -1, -1, G_STRING, OF_NONE,   OS_DISABLED, { (long)"--"       }, 0, 2 * font_height, font_width * 8, font_height * 1 }, //  7: second child entry
            {  9, -1, -1, G_STRING, OF_NONE,   OS_NORMAL,   { (long)"  "       }, 0, 3 * font_height, font_width * 8, font_height * 1 }, //  8: third child entry
            { 10, -1, -1, G_STRING, OF_NONE,   OS_NORMAL,   { (long)"  "       }, 0, 4 * font_height, font_width * 8, font_height * 1 }, //  9: fourth child entry
            { 11, -1, -1, G_STRING, OF_NONE,   OS_NORMAL,   { (long)"  "       }, 0, 5 * font_height, font_width * 8, font_height * 1 }, // 10: fifth child entry
            { 12, -1, -1, G_STRING, OF_NONE,   OS_NORMAL,   { (long)"  "       }, 0, 6 * font_height, font_width * 8, font_height * 1 }, // 11: sixth child entry
            { 13, -1, -1, G_STRING, OF_NONE,   OS_NORMAL,   { (long)"  "       }, 0, 7 * font_height, font_width * 8, font_height * 1 }, // 12: seventh child entry
            {  5, -1, -1, G_STRING, OF_LASTOB, OS_NORMAL,   { (long)"  "       }, 0, 8 * font_height, font_width * 8, font_height * 1 }, // 13: eighth child entry
        };
        menu_bar(dummy_menu, MENU_INSTALL);

        form_dial(FMD_FINISH, 0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height);
    }

    v_clsvwk(vdi_handle);
    appl_exit();

    return 0;
}


void
draw_pattern()
{
    draw_bar(0, 0, SCREEN_MAX_X, SCREEN_MAX_Y, G_LBLACK);

    const short half_width  = screen_width  / 2;
    const short half_height = screen_height / 2;

    short center_x  = half_width  - 1;
    short center_y  = half_height - 1;
    short grid_step = half_width / 9;

    for (register short offset = 0; offset < half_width; offset += grid_step) {
        draw_vline(center_x - offset, 0, SCREEN_MAX_Y, G_WHITE);
        draw_vline(center_x + offset, 0, SCREEN_MAX_Y, G_WHITE);
    }

    for (register short offset = 0; offset < half_height; offset += grid_step) {
        draw_hline(0, SCREEN_MAX_X, center_y - offset, G_WHITE);
        draw_hline(0, SCREEN_MAX_X, center_y + offset, G_WHITE);
    }

    short colour_area_width  = 10 * grid_step;
    short colour_area_height =  8 * grid_step;

    draw_colours(center_x - colour_area_width  / 2
                ,center_y - colour_area_height / 2
                ,colour_area_width
                ,colour_area_height
                );

    vsl_color(vdi_handle, G_WHITE);
    vsl_width(vdi_handle, MAX(vline_width, hline_width));
    v_arc(vdi_handle, center_x, center_y, grid_step * 5 + grid_step / 2, 0, 3600);
    vsl_width(vdi_handle, 1);
}


void
draw_colours(short x, short y, short w, short h)
{
    static const short colours_1[] =
    {
        G_LWHITE,
        G_LYELLOW,
        G_LCYAN,
        G_LGREEN,
        G_LMAGENTA,
        G_LRED,
        G_LBLUE,
        G_BLACK
    };

    static const short colours_2[] =
    {
        G_BLACK,
        G_LBLACK,
        G_LWHITE,
        G_WHITE
    };

    static const short colours_3[] =
    {
        G_WHITE,
        G_YELLOW,
        G_CYAN,
        G_GREEN,
        G_MAGENTA,
        G_RED,
        G_BLUE,
        G_LBLACK
    };

    const short xr = x + w - 1;

    draw_bar(x, y, xr, y + h - 1, G_WHITE);

    short xc = x + hline_width;
    short yc = y + vline_width;
    short hc = h / 4;

    draw_colour_box(xc, yc, w / ARRAYSIZE(colours_1), hc, xr, ARRAYSIZE(colours_1), colours_1);

    yc += hc;
    draw_colour_box(xc, yc, w / ARRAYSIZE(colours_2), hc, xr, ARRAYSIZE(colours_2), colours_2);

    yc += hc;
    draw_colour_box(xc, yc, w / ARRAYSIZE(colours_3), hc, xr, ARRAYSIZE(colours_3), colours_3);

    yc += hc - vline_width;
    hc  = h / 8;

    draw_bar(x + hline_width, yc + vline_width, xr, yc + hc - 1, G_BLACK);

    short chw, chh, cew, ceh;

    vswr_mode(vdi_handle, MD_XOR);
    vst_color(vdi_handle, G_WHITE);
    vst_height(vdi_handle, 13 * ((hc - 2 - 2 * vline_width) / 13), &chw, &chh, &cew, &ceh);

    short wt = cew * strlen(TITLE);
    short ht = ceh;
    short xt = x + (w - wt) / 2;
    short yt = yc + hc - (ceh - chh) - (hc - ht) / 2;

    v_gtext(vdi_handle, xt, yt, TITLE);
    vswr_mode(vdi_handle, MD_REPLACE);

    yc += hc;
    hc  = h / 8;

    clip(x + hline_width, yc + vline_width, xr, yc + hc - 1);

    vsf_color(vdi_handle, G_BLACK);
    vsf_interior(vdi_handle, FIS_SOLID);
    vsf_perimeter(vdi_handle, 1);

    const short lw = 1;

    for (register int x1 = x + hline_width; x1 < x + w; x1 += 2 * lw) {
        short pxyarray[] = { x1, yc + vline_width, x1 + lw - 1, yc + hc - 1 };
        v_bar(vdi_handle, pxyarray);
    }

    clip_screen();
}


void
draw_colour_box(short x, short y, short w, short h, short xr, short colour_count, const short colour[])
{
    short x2 = x + w - 1;

    for (register short i = 0, x1 = x; i < colour_count; ++i, x1 += w, x2 += w) {
        if (i == colour_count - 1) {
            x2 = xr;
        }

        draw_bar(x1, y, x2, y + h - vline_width - 1, colour[i]);
    }
}


void
clip(short x1, short y1, short x2, short y2)
{
    short pxyarray[4] = { x1, y1, x2, y2 };
    vs_clip(vdi_handle, 1, pxyarray);
}


void
clip_screen()
{
    clip(0, 0, SCREEN_MAX_X, SCREEN_MAX_Y);
}


void
draw_bar(short x1, short y1, short x2, short y2, short colour)
{
    vsf_color(vdi_handle, colour);
    vsf_interior(vdi_handle, FIS_SOLID);
    vsf_perimeter(vdi_handle, 1);

    short pxyarray[] = { x1, y1, x2, y2 };
    v_bar(vdi_handle, pxyarray);
}


void
draw_hline(short x1, short x2, short y, short colour)
{
    draw_bar(x1, y, x2, y + (vline_width - 1), colour);
}


void
draw_vline(short x, short y1, short y2, short colour)
{
    draw_bar(x, y1, x + (hline_width - 1), y2, colour);
}
