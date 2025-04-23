/* See LICENSE file for copyright and license details. */

#include <cstring>
#include <argv.hpp>
#include <config/config.hpp>
#include <options.hpp>
#include <majorna.hpp>
#include <schemes.hpp>
#include <x11/xrdb.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sort.hpp>
#ifndef LIMHAMN_ARGUMENT_MANAGER_IMPL
#define LIMHAMN_ARGUMENT_MANAGER_IMPL
#endif
#include <limhamn/argument_manager/argument_manager.hpp>

char* _strstr(const char* f, const char* s) {
	return strstr(const_cast<char*>(f), const_cast<char*>(s));
}

/*
void readargs(int argc, char *argv[]) {
#if !X11
#if !WAYLAND
    die("Why did you think it was a good idea to compile me without support for any display server? You're an idiot. :)");
#endif
#endif

    // check if we should load the xrdb/config, because it needs to be loaded before arguments are checked
    // priority: internal -> config -> xresources -> arguments
    for (int j = 1; j < argc; j++) {
        if (!strcmp(argv[j], "-lcfg") || (!strcmp(argv[j], "--load-config"))) {
            loadconfig = 1;
        } else if (!strcmp(argv[j], "-ncfg") || (!strcmp(argv[j], "--no-load-config"))) {
            loadconfig = 0;
        } else if (!strcmp(argv[j], "-ltm") || (!strcmp(argv[j], "--load-theme"))) {
            loadtheme = 1;
            theme_override = 1;
        } else if (!strcmp(argv[j], "-nltm") || (!strcmp(argv[j], "--no-load-theme"))) {
            loadtheme = 0;
            theme_override = 1;
        } else if (!strcmp(argv[j], "-lbi") || (!strcmp(argv[j], "--load-binds"))) {
            loadbinds = 1;
            binds_override = 1;
        } else if (!strcmp(argv[j], "-nlbi") || (!strcmp(argv[j], "--no-load-binds"))) {
            loadbinds = 0;
            binds_override = 1;
        } else if (!strcmp(argv[j], "-x11") || (!strcmp(argv[j], "--x11"))) {
            protocol = 0;
            protocol_override = 1;
        } else if (!strcmp(argv[j], "-wl") || (!strcmp(argv[j], "--wayland"))) {
            protocol = 1;
            protocol_override = 1;
#if CONFIG
        } else if (!strcmp(argv[j], "-cf") || (!strcmp(argv[j], "--config-file"))) { // specify a config file
            if (argv[j+1]) {
                configfile = argv[++j];
            } else {
                die("This argument requires a second argument.");
            }
        } else if (!strcmp(argv[j], "-bf") || (!strcmp(argv[j], "--bind-file"))) { // specify a bind file
            if (argv[j+1]) {
                bindsfile = argv[++j];
            } else {
                die("This argument requires a second argument.");
            }
        } else if (!strcmp(argv[j], "-tm") || (!strcmp(argv[j], "--theme"))) { // specify a theme
            if (argv[j+1]) {
                themefile = argv[++j];
            } else {
                die("This argument requires a second argument.");
            }
#endif
        }
    }

#if CONFIG
    conf_init();
#endif

    for (int k = 1; k < argc; k++) {
        if (!strcmp(argv[k], "-xrdb") || (!strcmp(argv[k], "--xrdb"))) {
            xresources = 1;
        } else if (!strcmp(argv[k], "-nxrdb") || (!strcmp(argv[k], "--no-xrdb"))) {
            xresources = 0;
        }
    }

#if WAYLAND
    if (protocol) {
        if (connect_display(&state)) {
            protocol = 0;
        }
    }
#if X11
    if (getenv("GNOME_SETUP_DISPLAY")) { // This is a GNOME Wayland session which doesn't implement wlr-layer-shell
        protocol = 0;
    }
#endif
#endif

    // init/read xrdb
#if X11
    if (xresources && !protocol) {
        XrmInitialize();
        load_xresources();
    }
#endif

    // no arguments
    for (int i = 1; i < argc; i++)
        if (!strcmp(argv[i], "-v") || (!strcmp(argv[i], "--version"))) { // prints version information
            puts(VERSION);
            exit(0);
        } else if (!strcmp(argv[i], "-h") || (!strcmp(argv[i], "--help"))) { // help
            usage(0);
        } else if (!strcmp(argv[i], "-it") || (!strcmp(argv[i], "--image-top"))) { // image: top
            imageposition = 0;
        } else if (!strcmp(argv[i], "-ib") || (!strcmp(argv[i], "--image-bottom"))) { // image: bottom
            imageposition = 1;
        } else if (!strcmp(argv[i], "-ic") || (!strcmp(argv[i], "--image-center"))) { // image: center
            imageposition = 2;
        } else if (!strcmp(argv[i], "-itc") || (!strcmp(argv[i], "--image-topcenter"))) { // image: top center
            imageposition = 3;
        } else if (!strcmp(argv[i], "-di") || !(strcmp(argv[i], "--display-icons"))) { // image: display as icons
            imagetype = 0;
        } else if (!strcmp(argv[i], "-df") || !(strcmp(argv[i], "--display-image"))) { // image: display as an image
            imagetype = 1;
        } else if (!strcmp(argv[i], "-b") || (!strcmp(argv[i], "--bottom"))) { // appears at the bottom of the screen
            menuposition = 0;
        } else if (!strcmp(argv[i], "-t") || (!strcmp(argv[i], "--top"))) { // appears at the top of the screen
            menuposition = 1;
        } else if (!strcmp(argv[i], "-c") || (!strcmp(argv[i], "--center"))) {  // appears at the center of the screen
            menuposition = 2;
        } else if (!strcmp(argv[i], "-itt") || (!strcmp(argv[i], "--item-top"))) { // items are placed above prompt
            itemposition = 1;
        } else if (!strcmp(argv[i], "-itb") || (!strcmp(argv[i], "--item-bottom"))) {  // items are placed below prompt
            menuposition = 0;
        } else if (!strcmp(argv[i], "-nm") || (!strcmp(argv[i], "--normal"))) { // normal mode
            mode = 0;
        } else if (!strcmp(argv[i], "-im") || (!strcmp(argv[i], "--insert"))) { // insert mode
            mode = 1;
        } else if (!strcmp(argv[i], "-f") || (!strcmp(argv[i], "--fast"))) { // grabs keyboard before reading stdin
            fast = 1;
        } else if (!strcmp(argv[i], "-r") || (!strcmp(argv[i], "--incremental"))) { // incremental
            incremental = 1;
        } else if (!strcmp(argv[i], "-nr") || (!strcmp(argv[i], "--no-incremental"))) { // no incremental
            incremental = 0;
        } else if (!strcmp(argv[i], "-rm") || (!strcmp(argv[i], "--require-match"))) { // require match
            requirematch = 1;
        } else if (!strcmp(argv[i], "-nrm") || (!strcmp(argv[i], "--no-require-match"))) { // no require match
            requirematch = 0;
        } else if (!strcmp(argv[i], "-ma") || (!strcmp(argv[i], "--mark-items"))) { // allow marking items
            mark = 1;
        } else if (!strcmp(argv[i], "-nma") || (!strcmp(argv[i], "--no-mark-items"))) { // don't allow marking items
            mark = 0;
        } else if (!strcmp(argv[i], "-F") || (!strcmp(argv[i], "--fuzzy"))) {   // fuzzy matching
            fuzzy = 1;
        } else if (!strcmp(argv[i], "-NF") || (!strcmp(argv[i], "--no-fuzzy"))) {   // no fuzzy matching
            fuzzy = 0;
        } else if (!strcmp(argv[i], "-R") || (!strcmp(argv[i], "--regex"))) {   // regex
            regex = 1;
        } else if (!strcmp(argv[i], "-NR") || (!strcmp(argv[i], "--no-regex"))) {   // no regex
            regex = 0;
        } else if (!strcmp(argv[i], "-ix") || (!strcmp(argv[i], "--print-index"))) {   // print index
            printindex = 1;
        } else if (!strcmp(argv[i], "-nix") || (!strcmp(argv[i], "--no-print-index"))) {   // no print index
            printindex = 0;
        } else if (!strcmp(argv[i], "-s") || (!strcmp(argv[i], "--case-sensitive")) || (!strcmp(argv[i], "--sensitive"))) { // case-sensitive item matching
            casesensitive = 1;
        } else if (!strcmp(argv[i], "-ns") || (!strcmp(argv[i], "--case-insensitive")) || (!strcmp(argv[i], "--insensitive"))) { // case-insensitive item matching
            casesensitive = 0;
        } else if (!strcmp(argv[i], "-nso") || (!strcmp(argv[i], "--no-sort"))) { // don't sort
            sortmatches = 0;
        } else if (!strcmp(argv[i], "-so") || (!strcmp(argv[i], "--sort"))) { // sort
            sortmatches = 1;
        } else if (!strcmp(argv[i], "-gc") || (!strcmp(argv[i], "--generate-cache"))) { // generate image cache
            generatecache = 1;
        } else if (!strcmp(argv[i], "-ngc") || (!strcmp(argv[i], "--no-generate-cache"))) { // don't generate image cache
            generatecache = 0;
        } else if (!strcmp(argv[i], "-wm") || (!strcmp(argv[i], "--managed")) || (!strcmp(argv[i], "--x11-client"))) { // display as managed wm window
            managed = 1;
        } else if (!strcmp(argv[i], "-nwm") || (!strcmp(argv[i], "--unmanaged"))) { // don't display as managed wm window
            managed = 0;
        } else if (!strcmp(argv[i], "-gk") || (!strcmp(argv[i], "--grab-keyboard"))) { // grab keyboard
            grabkeyboard = 1;
        } else if (!strcmp(argv[i], "-ngk") || (!strcmp(argv[i], "--no-grab-keyboard"))) { // don't grab keyboard
            grabkeyboard = 0;
        } else if (!strcmp(argv[i], "-na") || (!strcmp(argv[i], "--no-alpha"))) { // disable alpha
            alpha = 0;
        } else if (!strcmp(argv[i], "-a") || (!strcmp(argv[i], "--alpha"))) { // alpha
            alpha = 1;
        } else if (!strcmp(argv[i], "-tp") || (!strcmp(argv[i], "--allow-typing"))) { //  allow the user to type
            type = 1;
        } else if (!strcmp(argv[i], "-nt") || (!strcmp(argv[i], "--no-allow-typing"))) { //  don't allow the user to type
            type = 0;
        } else if (!strcmp(argv[i], "-ol") || (!strcmp(argv[i], "--override-lines"))) {  // allow overriding lines
            overridelines = 1;
        } else if (!strcmp(argv[i], "-nol") || (!strcmp(argv[i], "--no-override-lines"))) {  // don't allow overriding lines
            overridelines = 0;
        } else if (!strcmp(argv[i], "-oc") || (!strcmp(argv[i], "--override-columns"))) {  // allow overriding columns
            overridecolumns = 1;
        } else if (!strcmp(argv[i], "-noc") || (!strcmp(argv[i], "--no-override-columns"))) {  // don't allow overriding columns
            overridecolumns = 0;
        } else if (!strcmp(argv[i], "-P") || (!strcmp(argv[i], "--password"))) {  // is the input a password
            passwd = 1;
        } else if (!strcmp(argv[i], "-nP") || (!strcmp(argv[i], "--no-password"))) {  // is the input a password
            passwd = 0;
        } else if (!strcmp(argv[i], "-hmc") || (!strcmp(argv[i], "--hide-match-count"))) {   // hide match count
            hidematchcount = 1;
        } else if (!strcmp(argv[i], "-smc") || (!strcmp(argv[i], "--show-match-count"))) {  // show match count
            hidematchcount = 0;
        } else if (!strcmp(argv[i], "-hm") || (!strcmp(argv[i], "--hide-mode"))) {   // hide mode indicator
            hidemode = 1;
        } else if (!strcmp(argv[i], "-sm") || (!strcmp(argv[i], "--show-mode"))) {  // show mode indicator
            hidemode = 0;
        } else if (!strcmp(argv[i], "-hit") || (!strcmp(argv[i], "--hide-item"))) {   // hide items
            hideitem = 1;
        } else if (!strcmp(argv[i], "-sit") || (!strcmp(argv[i], "--show-item"))) {  // show items
            hideitem = 0;
        } else if (!strcmp(argv[i], "-hla") || (!strcmp(argv[i], "--hide-left-arrow"))) {   // hide left arrow
            hidelarrow = 1;
        } else if (!strcmp(argv[i], "-sla") || (!strcmp(argv[i], "--show-left-arrow"))) {  // show left arrow
            hidelarrow = 0;
        } else if (!strcmp(argv[i], "-hra") || (!strcmp(argv[i], "--hide-right-arrow"))) {   // hide right arrow
            hiderarrow = 1;
        } else if (!strcmp(argv[i], "-sra") || (!strcmp(argv[i], "--show-right-arrow"))) {  // show right arrow
            hiderarrow = 0;
        } else if (!strcmp(argv[i], "-hpr") || (!strcmp(argv[i], "--hide-prompt"))) {   // hide prompt
            hideprompt = 1;
        } else if (!strcmp(argv[i], "-hpt") || (!strcmp(argv[i], "--hide-pretext"))) {   // hide pretext
            hidepretext = 1;
        } else if (!strcmp(argv[i], "-hpl") || (!strcmp(argv[i], "--hide-powerline"))) {   // hide powerline
            hidepowerline = 1;
        } else if (!strcmp(argv[i], "-hip") || (!strcmp(argv[i], "--hide-input"))) {   // hide input
            hideinput = 1;
        } else if (!strcmp(argv[i], "-spr") || (!strcmp(argv[i], "--show-prompt"))) {  // show prompt
            hideprompt = 0;
        } else if (!strcmp(argv[i], "-spt") || (!strcmp(argv[i], "--show-pretext"))) {  // show pretext
            hidepretext = 0;
        } else if (!strcmp(argv[i], "-spl") || (!strcmp(argv[i], "--show-powerline"))) {  // show prompt
            hidepowerline = 0;
        } else if (!strcmp(argv[i], "-sin") || (!strcmp(argv[i], "--show-input"))) {  // show input
            hideinput = 0;
        } else if (!strcmp(argv[i], "-hc") || (!strcmp(argv[i], "--hide-cursor") || (!strcmp(argv[i], "--hide-caret")))) {   // hide cursor
            hidecaret = 1;
        } else if (!strcmp(argv[i], "-sc") || (!strcmp(argv[i], "--show-cursor") || (!strcmp(argv[i], "--show-caret")))) {  // show cursor
            hidecaret = 0;
        } else if (!strcmp(argv[i], "-hhl") || (!strcmp(argv[i], "--hide-highlighting"))) {   // hide highlighting
            hidehighlight = 1;
        } else if (!strcmp(argv[i], "-shl") || (!strcmp(argv[i], "--show-highlighting"))) {  // show highlighting
            hidehighlight = 0;
        } else if (!strcmp(argv[i], "-hi") || (!strcmp(argv[i], "--hide-image"))) {   // hide image
            hideimage = 1;
        } else if (!strcmp(argv[i], "-si") || (!strcmp(argv[i], "--show-image"))) {  // show image
            hideimage = 0;
        } else if (!strcmp(argv[i], "-hcl") || (!strcmp(argv[i], "--hide-caps"))) {  // hide caps
            hidecaps = 1;
        } else if (!strcmp(argv[i], "-scl") || (!strcmp(argv[i], "--show-caps"))) {  // hide caps
            hidecaps = 0;
        } else if (!strcmp(argv[i], "-ip") || (!strcmp(argv[i], "--indent"))) {  // indent to prompt width
            indentitems = 1;
        } else if (!strcmp(argv[i], "-nip") || (!strcmp(argv[i], "--no-indent"))) {  // don't indent to prompt width
            indentitems = 0;
        } else if (!strcmp(argv[i], "-ci") || (!strcmp(argv[i], "--color-items"))) {  // color items
            coloritems = 1;
        } else if (!strcmp(argv[i], "-nci") || (!strcmp(argv[i], "--no-color-items"))) {  // don't color items
            coloritems = 0;
        } else if (!strcmp(argv[i], "-sgr") || (!strcmp(argv[i], "--sgr"))) { // enable sgr seq support
            sgr = 1;
        } else if (!strcmp(argv[i], "-nsgr") || (!strcmp(argv[i], "--no-sgr"))) { // disable sgr seq support
            sgr = 0;
        } else if (i + 1 == argc) {
            // any of the arguments we checked first
            if ((!strcmp(argv[i], "-xrdb")
                        || !strcmp(argv[i], "-nxrdb")
                        || !strcmp(argv[i], "--xrdb")
                        || !strcmp(argv[i], "--no-xrdb")
                        || !strcmp(argv[i], "-lcfg")
                        || !strcmp(argv[i], "-ncfg")
                        || !strcmp(argv[i], "--load-config")
                        || !strcmp(argv[i], "--no-load-config")
                        || !strcmp(argv[i], "-ltm")
                        || !strcmp(argv[i], "-nltm")
                        || !strcmp(argv[i], "--load-theme")
                        || !strcmp(argv[i], "--no-load-theme")
                        || !strcmp(argv[i], "-lbi")
                        || !strcmp(argv[i], "-nlbi")
                        || !strcmp(argv[i], "-wl")
                        || !strcmp(argv[i], "--wayland")
                        || !strcmp(argv[i], "-x11")
                        || !strcmp(argv[i], "--x11")
                        || !strcmp(argv[i], "--load-binds")
                        || !strcmp(argv[i], "--no-load-binds")
#if CONFIG
                        || !strcmp(argv[i], "-cf")
                        || !strcmp(argv[i], "--config-file")
                        || (configfile && !strcmp(argv[i], configfile))
                        || !strcmp(argv[i], "-tm")
                        || !strcmp(argv[i], "--theme")
                        || (themefile && !strcmp(argv[i], themefile))
                        || !strcmp(argv[i], "-bf")
                        || !strcmp(argv[i], "--bind-file")
                        || (bindsfile && !strcmp(argv[i], bindsfile))
#endif
                        ))
                        continue;
            else
                fprintf(stderr, "majorna: Invalid argument: '%s'\n", argv[i]);

            // dmenu compatibility arguments
        } else if (!strcmp(argv[i], "-i")) { // case-insensitive item matching, for compatibility reasons
            casesensitive = 0;
        } else if (!strcmp(argv[i], "-S")) { // don't sort
            sortmatches = 0;

            // these options take one argument
        } else if (!strcmp(argv[i], "-g") || (!strcmp(argv[i], "--columns"))) { // number of columns in grid
            columns = atoi(argv[++i]);
            if (lines == 0) lines = 1;
        } else if (!strcmp(argv[i], "-mc") || (!strcmp(argv[i], "--max-cache"))) { // max cache
            maxcache = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-cd") || (!strcmp(argv[i], "--cache-dir"))) { // cache directory
            cachedir = argv[++i];
            if (access(cachedir, F_OK) != 0) {
                cachedir = "default";
            }
        } else if (!strcmp(argv[i], "-l") || (!strcmp(argv[i], "--lines"))) { // number of lines in grid
            lines = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-mh") || (!strcmp(argv[i], "--lineheight")) || (!strcmp(argv[i], "--line-height"))) { // line height
            lineheight = atoi(argv[++i]);
            if (columns == 0) columns = 1;
        } else if (!strcmp(argv[i], "-ml") || (!strcmp(argv[i], "--min-lines"))) {
            minlines = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-cw") || (!strcmp(argv[i], "--center-width")) || (!strcmp(argv[i], "-mw") || (!strcmp(argv[i], "--min-width")))) { // center width
            centerwidth = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-txp") || (!strcmp(argv[i], "--text-padding"))) { // text padding
            textpadding = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-lp") || (!strcmp(argv[i], "--vertical-padding"))) { // vertical padding
            menupaddingv = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-hp") || (!strcmp(argv[i], "--horizontal-padding"))) { // horizontal padding
            menupaddingh = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-vem") || (!strcmp(argv[i], "--vertical-margin"))) { // vertical margin
            menumarginv = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-hom") || (!strcmp(argv[i], "--horizontal-margin"))) { // horizontal margin
            menumarginh = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-pri") || (!strcmp(argv[i], "--priority"))) { // high priority (csv format)
            hpitems = tokenize(argv[++i], ",", &hplength);
        } else if (!strcmp(argv[i], "-ig") || (!strcmp(argv[i], "--image-gaps"))) { // gaps between image
            imagegaps = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-la") || (!strcmp(argv[i], "--left-arrow-symbol"))) { // left arrow symbol
            leftarrow = argv[++i];
        } else if (!strcmp(argv[i], "-ra") || (!strcmp(argv[i], "--right-arrow-symbol"))) { // right arrow symbol
            rightarrow = argv[++i];
        } else if (!strcmp(argv[i], "-m") || (!strcmp(argv[i], "--monitor"))) // monitor
            mon = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-bw") || (!strcmp(argv[i], "--border-width"))) { // border width
            borderwidth = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-H") || (!strcmp(argv[i], "--hist-file"))) // hist file location
            histfile = argv[++i];
        else if (!strcmp(argv[i], "-x") || (!strcmp(argv[i], "--x-position")))  // window x offset
            xpos = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-y") || (!strcmp(argv[i], "--y-position")))   // window y offset (from bottom up if -b)
            ypos = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-z") || (!strcmp(argv[i], "--width")))   // make majorna this wide
            menuwidth = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-p") || (!strcmp(argv[i], "--prompt")))   // adds prompt to left of input field
            prompt = argv[++i];
        else if (!strcmp(argv[i], "-pt") || (!strcmp(argv[i], "--pretext")))   // adds pretext
            pretext = argv[++i];
        else if (!strcmp(argv[i], "-It") || (!strcmp(argv[i], "--input")))   // specify initial text
            input = argv[++i];
        else if (!strcmp(argv[i], "-fn") || (!strcmp(argv[i], "--font")))  // font or font set
            fonts[0] = argv[++i];
        else if (!strcmp(argv[i], "-lf") || (!strcmp(argv[i], "--list-file"))) // list file
            listfile = argv[++i];
        else if (!strcmp(argv[i], "-nmt") || (!strcmp(argv[i], "--normal-mode-text")))  // normal mode text
            sp_strncpy(normtext, argv[++i], sizeof(normtext));
        else if (!strcmp(argv[i], "-imt") || (!strcmp(argv[i], "--insert-mode-text")))  // insert mode text
            sp_strncpy(instext, argv[++i], sizeof(instext));
        else if (!strcmp(argv[i], "-clon") || (!strcmp(argv[i], "--caps-lock-on-text")))  // caps lock on text
            sp_strncpy(capslockontext, argv[++i], sizeof(capslockontext));
        else if (!strcmp(argv[i], "-clof") || (!strcmp(argv[i], "--caps-lock-off-text"))) {  // caps lock off text
            sp_strncpy(capslockofftext, argv[++i], sizeof(capslockofftext));

            // dmenu compatibility options
        } else if (!strcmp(argv[i], "-nb")) {  // normal background color
            int ix = ++i;
            sp_strncpy(col_itemnormbg, argv[ix], sizeof(col_itemnormbg));
            sp_strncpy(col_itemnormbg2, argv[ix], sizeof(col_itemnormbg2));
            sp_strncpy(col_menu, argv[ix], sizeof(col_menu));
            sp_strncpy(col_inputbg, argv[ix], sizeof(col_inputbg));
            sp_strncpy(col_promptbg, argv[ix], sizeof(col_promptbg));
        } else if (!strcmp(argv[i], "-nf")) {  // normal foreground color
            int ix = ++i;
            sp_strncpy(col_itemnormfg, argv[ix], sizeof(col_itemnormfg));
            sp_strncpy(col_itemnormfg2, argv[ix], sizeof(col_itemnormfg2));
            sp_strncpy(col_inputfg, argv[ix], sizeof(col_inputfg));
            sp_strncpy(col_promptfg, argv[ix], sizeof(col_promptfg));
        } else if (!strcmp(argv[i], "-sb")) {  // selected background color
            int ix = ++i;
            sp_strncpy(col_itemselbg, argv[ix], sizeof(col_itemselbg));
            sp_strncpy(col_menu, argv[ix], sizeof(col_menu));
            sp_strncpy(col_inputbg, argv[ix], sizeof(col_inputbg));
            sp_strncpy(col_promptbg, argv[ix], sizeof(col_promptbg));
        } else if (!strcmp(argv[i], "-sf")) {  // selected foreground color
            int ix = ++i;
            sp_strncpy(col_itemselfg, argv[ix], sizeof(col_itemselfg));
            sp_strncpy(col_inputfg, argv[ix], sizeof(col_inputfg));
            sp_strncpy(col_promptfg, argv[ix], sizeof(col_promptfg));

            // more
        } else if (!strcmp(argv[i], "-is") || (!strcmp(argv[i], "--image-size"))) { // image size
            char buf[255];
            memset(buf, 0, sizeof(buf));
            memcpy(buf, argv[++i], sizeof(buf)-1);

            if(sscanf(buf, "%dx%d", &imagewidth, &imageheight) == 1)
                imageheight = imagewidth;
        } else if (!strcmp(argv[i], "-ir") || (!strcmp(argv[i], "--image-resize"))) { // image resize
            imageresize = 1;
        } else if (!strcmp(argv[i], "-nir") || (!strcmp(argv[i], "--no-image-resize"))) { // no image resize
            imageresize = 0;
        } else if (!strcmp(argv[i], "-w") || (!strcmp(argv[i], "--embed"))) { // embedding window id
#if X11
            x11.embed = argv[++i];
#endif
        } else if (!strcmp(argv[i], "-n") || (!strcmp(argv[i], "--preselect"))) { // preselected item
            preselected = atoi(argv[++i]);

            // majorna colors
        } else if (!strcmp(argv[i], "-nif") || (!strcmp(argv[i], "--normal-item-foreground"))) { // normal item foreground color
            sp_strncpy(col_itemnormfg, argv[++i], sizeof(col_itemnormfg));
        } else if (!strcmp(argv[i], "-nib") || (!strcmp(argv[i], "--normal-item-background"))) { // normal item background color
            sp_strncpy(col_itemnormbg, argv[++i], sizeof(col_itemnormbg));
        } else if (!strcmp(argv[i], "-nnif") || (!strcmp(argv[i], "--normal-next-item-foreground"))) { // normal next item foreground color
            sp_strncpy(col_itemnormfg2, argv[++i], sizeof(col_itemnormfg2));
        } else if (!strcmp(argv[i], "-nnib") || (!strcmp(argv[i], "--normal-next-item-background"))) { // normal next item background color
            sp_strncpy(col_itemnormbg2, argv[++i], sizeof(col_itemnormbg2));
        } else if (!strcmp(argv[i], "-sif") || (!strcmp(argv[i], "--selected-item-foreground"))) { // selected item foreground color
            sp_strncpy(col_itemselfg, argv[++i], sizeof(col_itemselfg));
        } else if (!strcmp(argv[i], "-sib") || (!strcmp(argv[i], "--selected-item-background"))) { // selected item background color
            sp_strncpy(col_itemselbg, argv[++i], sizeof(col_itemselbg));
        } else if (!strcmp(argv[i], "-npf") || (!strcmp(argv[i], "--normal-item-priority-foreground"))) { // normal item priority foreground color
            sp_strncpy(col_itemnormprifg, argv[++i], sizeof(col_itemnormprifg));
        } else if (!strcmp(argv[i], "-npb") || (!strcmp(argv[i], "--normal-item-priority-background"))) { // normal item priority background color
            sp_strncpy(col_itemnormpribg, argv[++i], sizeof(col_itemnormpribg));
        } else if (!strcmp(argv[i], "-spf") || (!strcmp(argv[i], "--selected-item-priority-foreground"))) { // selected item priority foreground color
            sp_strncpy(col_itemselprifg, argv[++i], sizeof(col_itemselprifg));
        } else if (!strcmp(argv[i], "-spb") || (!strcmp(argv[i], "--selected-item-priority-background"))) { // selected item priority background color
            sp_strncpy(col_itemselpribg, argv[++i], sizeof(col_itemselpribg));
        } else if (!strcmp(argv[i], "-mnbg") || (!strcmp(argv[i], "--menu-background"))) { // menu color
            sp_strncpy(col_menu, argv[++i], sizeof(col_menu));
        } else if (!strcmp(argv[i], "-pfg") || (!strcmp(argv[i], "--prompt-foreground"))) { // prompt fg color
            sp_strncpy(col_promptfg, argv[++i], sizeof(col_promptfg));
        } else if (!strcmp(argv[i], "-pbg") || (!strcmp(argv[i], "--prompt-background"))) { // prompt bg color
            sp_strncpy(col_promptbg, argv[++i], sizeof(col_promptbg));
        } else if (!strcmp(argv[i], "-ifg") || (!strcmp(argv[i], "--input-foreground"))) { // input fg color
            sp_strncpy(col_inputfg, argv[++i], sizeof(col_inputfg));
        } else if (!strcmp(argv[i], "-pfg") || (!strcmp(argv[i], "--input-background"))) { // input bg color
            sp_strncpy(col_inputbg, argv[++i], sizeof(col_inputbg));
        } else if (!strcmp(argv[i], "-nhb") || (!strcmp(argv[i], "--normal-highlight-background"))) { // normal highlight background color
            sp_strncpy(col_hlnormbg, argv[++i], sizeof(col_hlnormbg));
        } else if (!strcmp(argv[i], "-nhf") || (!strcmp(argv[i], "--normal-highlight-foreground"))) { // normal highlight foreground color
            sp_strncpy(col_hlnormfg, argv[++i], sizeof(col_hlnormfg));
        } else if (!strcmp(argv[i], "-shf") || (!strcmp(argv[i], "--selected-highlight-foreground"))) { // selected highlight foreground color
            sp_strncpy(col_hlselfg, argv[++i], sizeof(col_hlselfg));
        } else if (!strcmp(argv[i], "-shb") || (!strcmp(argv[i], "--selected-highlight-background"))) { // selected highlight background color
            sp_strncpy(col_hlselbg, argv[++i], sizeof(col_hlselbg));
        } else if (!strcmp(argv[i], "-nbg") || (!strcmp(argv[i], "--number-background"))) { // numbg
            sp_strncpy(col_numbg, argv[++i], sizeof(col_numbg));
        } else if (!strcmp(argv[i], "-nfg") || (!strcmp(argv[i], "--number-foreground"))) { // numfg
            sp_strncpy(col_numfg, argv[++i], sizeof(col_numfg));
        } else if (!strcmp(argv[i], "-mbg") || (!strcmp(argv[i], "--mode-background"))) { // mode
            sp_strncpy(col_modebg, argv[++i], sizeof(col_modebg));
        } else if (!strcmp(argv[i], "-mfg") || (!strcmp(argv[i], "--mode-foreground"))) { // mode
            sp_strncpy(col_modefg, argv[++i], sizeof(col_modefg));
        } else if (!strcmp(argv[i], "-laf") || (!strcmp(argv[i], "--left-arrow-foreground"))) { // left arrow fg
            sp_strncpy(col_larrowfg, argv[++i], sizeof(col_larrowfg));
        } else if (!strcmp(argv[i], "-raf") || (!strcmp(argv[i], "--right-arrow-foreground"))) { // right arrow fg
            sp_strncpy(col_rarrowfg, argv[++i], sizeof(col_rarrowfg));
        } else if (!strcmp(argv[i], "-lab") || (!strcmp(argv[i], "--left-arrow-background"))) { // left arrow bg
            sp_strncpy(col_larrowbg, argv[++i], sizeof(col_larrowbg));
        } else if (!strcmp(argv[i], "-rab") || (!strcmp(argv[i], "--right-arrow-background"))) { // right arrow bg
            sp_strncpy(col_rarrowbg, argv[++i], sizeof(col_rarrowbg));
        } else if (!strcmp(argv[i], "-bc") || (!strcmp(argv[i], "--border-background"))) { // border
            sp_strncpy(col_border, argv[++i], sizeof(col_border));
        } else if (!strcmp(argv[i], "-cc") || (!strcmp(argv[i], "-cfc")) || (!strcmp(argv[i], "--caret-foreground"))) {   // caret color
            sp_strncpy(col_caretfg, argv[++i], sizeof(col_caretfg));
        } else if (!strcmp(argv[i], "-cbc") || (!strcmp(argv[i], "--caret-background"))) {   // caret color
            sp_strncpy(col_caretbg, argv[++i], sizeof(col_caretbg));
        }

        // sgr colors
        else if (!strcmp(argv[i], "-sgr0") || (!strcmp(argv[i], "--sgr0"))) textcolors[0] = argv[++i]; // sgr color 0
        else if (!strcmp(argv[i], "-sgr1") || (!strcmp(argv[i], "--sgr1"))) textcolors[1] = argv[++i]; // sgr color 1
        else if (!strcmp(argv[i], "-sgr2") || (!strcmp(argv[i], "--sgr2"))) textcolors[2] = argv[++i]; // sgr color 2
        else if (!strcmp(argv[i], "-sgr3") || (!strcmp(argv[i], "--sgr3"))) textcolors[3] = argv[++i]; // sgr color 3
        else if (!strcmp(argv[i], "-sgr4") || (!strcmp(argv[i], "--sgr4"))) textcolors[4] = argv[++i]; // sgr color 4
        else if (!strcmp(argv[i], "-sgr5") || (!strcmp(argv[i], "--sgr5"))) textcolors[5] = argv[++i]; // sgr color 5
        else if (!strcmp(argv[i], "-sgr6") || (!strcmp(argv[i], "--sgr6"))) textcolors[6] = argv[++i]; // sgr color 6
        else if (!strcmp(argv[i], "-sgr7") || (!strcmp(argv[i], "--sgr7"))) textcolors[7] = argv[++i]; // sgr color 7
        else if (!strcmp(argv[i], "-sgr8") || (!strcmp(argv[i], "--sgr8"))) textcolors[8] = argv[++i]; // sgr color 8
        else if (!strcmp(argv[i], "-sgr9") || (!strcmp(argv[i], "--sgr9"))) textcolors[9] = argv[++i]; // sgr color 9
        else if (!strcmp(argv[i], "-sgr10") || (!strcmp(argv[i], "--sgr10"))) textcolors[10] = argv[++i]; // sgr color 10
        else if (!strcmp(argv[i], "-sgr11") || (!strcmp(argv[i], "--sgr11"))) textcolors[11] = argv[++i]; // sgr color 11
        else if (!strcmp(argv[i], "-sgr12") || (!strcmp(argv[i], "--sgr12"))) textcolors[12] = argv[++i]; // sgr color 12
        else if (!strcmp(argv[i], "-sgr13") || (!strcmp(argv[i], "--sgr13"))) textcolors[13] = argv[++i]; // sgr color 13
        else if (!strcmp(argv[i], "-sgr14") || (!strcmp(argv[i], "--sgr14"))) textcolors[14] = argv[++i]; // sgr color 14
        else if (!strcmp(argv[i], "-sgr15") || (!strcmp(argv[i], "--sgr15"))) textcolors[15] = argv[++i]; // sgr color 15
        else
            if ((!strcmp(argv[i], "-xrdb")
                        || !strcmp(argv[i], "-nxrdb")
                        || !strcmp(argv[i], "--xrdb")
                        || !strcmp(argv[i], "--no-xrdb")
                        || !strcmp(argv[i], "-lcfg")
                        || !strcmp(argv[i], "-ncfg")
                        || !strcmp(argv[i], "--load-config")
                        || !strcmp(argv[i], "--no-load-config")
                        || !strcmp(argv[i], "-ltm")
                        || !strcmp(argv[i], "-nltm")
                        || !strcmp(argv[i], "--load-theme")
                        || !strcmp(argv[i], "--no-load-theme")
                        || !strcmp(argv[i], "-lbi")
                        || !strcmp(argv[i], "-nlbi")
                        || !strcmp(argv[i], "-wl")
                        || !strcmp(argv[i], "--wayland")
                        || !strcmp(argv[i], "-x11")
                        || !strcmp(argv[i], "--x11")
                        || !strcmp(argv[i], "--load-binds")
                        || !strcmp(argv[i], "--no-load-binds")
#if CONFIG
                        || !strcmp(argv[i], "-cf")
                        || !strcmp(argv[i], "--config-file")
                        || (configfile && !strcmp(argv[i], configfile))
                        || !strcmp(argv[i], "-tm")
                        || !strcmp(argv[i], "--theme")
                        || (themefile && !strcmp(argv[i], themefile))
                        || !strcmp(argv[i], "-bf")
                        || !strcmp(argv[i], "--bind-file")
                        || (bindsfile && !strcmp(argv[i], bindsfile))
#endif
                        ))
                        continue;
            else
                fprintf(stderr, "majorna: Invalid argument: '%s'\n", argv[i]);

#if !X11
    protocol = 1;
#endif
#if !WAYLAND
    protocol = 0;
#endif
    if (casesensitive) {
        fstrncmp = strncmp;
        fstrstr = _strstr;
    } else {
        fstrncmp = strncasecmp;
        fstrstr = cistrstr;
    }

    if (!columns)
        columns = 1;

    // initial text
    if (input)
        insert(input, strlen(input));
}

void comp_opts(void) {
#if WAYLAND
    fprintf(stdout, "Wayland: Supported\n");
#else
    fprintf(stdout, "Wayland: Unsupported\n");
#endif
#if X11
    fprintf(stdout, "X11: Supported\n");
#else
    fprintf(stdout, "X11: Unsupported\n");
#endif
#if RTL
    fprintf(stdout, "RTL: Supported\n");
#else
    fprintf(stdout, "RTL: Unsupported\n");
#endif
#if IMAGE
    fprintf(stdout, "Images: Supported\n");
#else
    fprintf(stdout, "Images: Unsupported\n");
#endif
#if XINERAMA
    fprintf(stdout, "Xinerama: Supported\n");
#else
    fprintf(stdout, "Xinerama: Unsupported\n");
#endif
#if CONFIG
    fprintf(stdout, "Config: Supported\n");
#else
    fprintf(stdout, "Config: Unsupported\n");
#endif
}
*/
void readargs(int argc, char** argv) {
    limhamn::argument_manager::argument_manager am(argc, argv);
    am.push_back("-h|--help|/h|/help|help", [](const limhamn::argument_manager::collection&) {
        usage(EXIT_SUCCESS);
        exit(EXIT_SUCCESS);
    });
    am.push_back("-v|--version|/v|/version|version", [](const limhamn::argument_manager::collection&) {
        fprintf(stdout, "majorna %s\n", VERSION);
        exit(EXIT_SUCCESS);
    });
    am.push_back("-lcfg|--load-config|/lcfg|/load-config|load-config", [](const limhamn::argument_manager::collection&) {
        loadconfig = 1;
    });
    am.push_back("-ncfg|--no-load-config|/ncfg|/no-load-config|no-load-config", [](const limhamn::argument_manager::collection&) {
        loadconfig = 0;
    });
    am.push_back("-ltm|--load-theme|/ltm|/load-theme|load-theme", [](const limhamn::argument_manager::collection&) {
        loadtheme = 1;
        theme_override = 1;
    });
    am.push_back("-nltm|--no-load-theme|/nltm|/no-load-theme|no-load-theme", [](const limhamn::argument_manager::collection&) {
        loadtheme = 0;
        theme_override = 1;
    });
    am.push_back("-lbi|--load-binds|/lbi|/load-binds|load-binds", [](const limhamn::argument_manager::collection&) {
        loadbinds = 1;
        binds_override = 1;
    });
    am.push_back("-nlbi|--no-load-binds|/nlbi|/no-load-binds|no-load-binds", [](const limhamn::argument_manager::collection&) {
        loadbinds = 0;
        binds_override = 1;
    });
    am.push_back("-x11|--x11|/x11|/x11|x11", [](const limhamn::argument_manager::collection&) {
        protocol = 0;
        protocol_override = 1;
    });
    am.push_back("-wl|--wayland|/wl|/wayland|wayland", [](const limhamn::argument_manager::collection& collection) {
        protocol = 1;
        protocol_override = 1;
    });
#if CONFIG
    am.push_back("-cf|--config-file|/cf|/config-file|config-file", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -c/--config flag requires a file to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        configfile = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-bf|--bind-file|/bf|/bind-file|bind-file", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -b/--bind flag requires a file to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        bindsfile = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-tm|--theme-file|/tm|/theme-file|theme-file", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -t/--theme-file flag requires a file to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        themefile = strdup(collection.arguments.at(++collection.index).c_str());
    });
#endif


    // do nothing, since we'll have further parsing to do
    am.execute([](const std::string&) {});

#if CONFIG
    conf_init();
#endif

    am = limhamn::argument_manager::argument_manager(argc, argv);
    am.push_back("-h|--help|/h|/help|help", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-v|--version|/v|/version|version", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-lcfg|--load-config|/lcfg|/load-config|load-config", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-ncfg|--no-load-config|/ncfg|/no-load-config|no-load-config", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-ltm|--load-theme|/ltm|/load-theme|load-theme", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-nltm|--no-load-theme|/nltm|/no-load-theme|no-load-theme", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-lbi|--load-binds|/lbi|/load-binds|load-binds", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-nlbi|--no-load-binds|/nlbi|/no-load-binds|no-load-binds", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-x11|--x11|/x11|/x11|x11", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-wl|--wayland|/wl|/wayland|wayland", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-cf|--config-file|/cf|/config-file|config-file", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-bf|--bind-file|/bf|/bind-file|bind-file", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-tm|--theme-file|/tm|/theme-file|theme-file", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-xrdb|--xrdb|/xrdb|/xrdb|xrdb", [](const limhamn::argument_manager::collection&) {
        xresources = true;
    });
    am.push_back("-nxrdb|--no-xrdb|/nxrdb|/no-xrdb|no-xrdb", [](const limhamn::argument_manager::collection&) {
        xresources = false;
    });

    // try wayland and fall back to x11 if it fails
#if WAYLAND
    if (protocol) {
        if (connect_display(&state)) {
            protocol = false;
        }
    }
    // success, but if it's gnome, we need to revert to X because gnome is retarded
#if X11
    if (getenv("GNOME_SETUP_DISPLAY")) {
        protocol = false;
    }
#endif
#endif

    // init/read xrdb
#if X11
    if (xresources && !protocol) {
        XrmInitialize();
        load_xresources();
    }
#endif

    am = limhamn::argument_manager::argument_manager(argc, argv);

    // add empty for the arguments we just parsed
    am.push_back("-h|--help|/h|/help|help", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-v|--version|/v|/version|version", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-lcfg|--load-config|/lcfg|/load-config|load-config", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-ncfg|--no-load-config|/ncfg|/no-load-config|no-load-config", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-ltm|--load-theme|/ltm|/load-theme|load-theme", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-nltm|--no-load-theme|/nltm|/no-load-theme|no-load-theme", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-lbi|--load-binds|/lbi|/load-binds|load-binds", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-nlbi|--no-load-binds|/nlbi|/no-load-binds|no-load-binds", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-x11|--x11|/x11|/x11|x11", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-wl|--wayland|/wl|/wayland|wayland", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-cf|--config-file|/cf|/config-file|config-file", [](limhamn::argument_manager::collection&c) {c.index++;});
    am.push_back("-bf|--bind-file|/bf|/bind-file|bind-file", [](limhamn::argument_manager::collection&c) {c.index++;});
    am.push_back("-tm|--theme-file|/tm|/theme-file|theme-file", [](limhamn::argument_manager::collection&c) {c.index++;});
    am.push_back("-xrdb|--xrdb|/xrdb|/xrdb|xrdb", [](const limhamn::argument_manager::collection&) {});
    am.push_back("-nxrdb|--no-xrdb|/nxrdb|/no-xrdb|no-xrdb", [](const limhamn::argument_manager::collection&) {});

    // add the rest of the arguments
    am.push_back("-mh|--line-height|/mh|/line-height|line-height", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -mh/--line-height flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        lineheight = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-cw|--center-width|/cw|/center-width|center-width", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -cw/--center-width flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        centerwidth = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-l|--lines|/l|/lines|lines", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -l/--lines flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        lines = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-g|--columns|/g|/columns|columns", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -g/--columns flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        columns = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-ml|--min-lines|/ml|/min-lines|min-lines", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ml/--min-lines flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        minlines = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-gc|--generate-cache|/gc|/generate-cache|generate-cache", [](limhamn::argument_manager::collection&) {
        generatecache = true;
    });
    am.push_back("-ngc|--no-generate-cache|/ngc|/no-generate-cache|no-generate-cache", [](limhamn::argument_manager::collection&) {
        generatecache = false;
    });
    am.push_back("-mc|--max-cache|/mc|/max-cache|max-cache", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -mc/--max-cache flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        maxcache = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-cd|--cache-dir|/cd|/cache-dir|cache-dir", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -cd/--cache-dir flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        cachedir = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ix|--print-index|/ix|/print-index|print-index", [](limhamn::argument_manager::collection&) {
        printindex = true;
    });
    am.push_back("-nix|--no-print-index|/nix|/no-print-index|no-print-index", [](limhamn::argument_manager::collection&) {
        printindex = false;
    });
    am.push_back("-f|--fast|/f|/fast|fast", [](limhamn::argument_manager::collection&) {
        fast = true;
    });
    am.push_back("-r|--incremental|/r|/incremental|incremental", [](limhamn::argument_manager::collection&) {
        incremental = true;
    });
    am.push_back("-nr|--no-incremental|/nr|/no-incremental|no-incremental", [](limhamn::argument_manager::collection&) {
        incremental = false;
    });
    am.push_back("-rm|--require-match|/rm|/require-match|require-match", [](limhamn::argument_manager::collection&) {
        requirematch = true;
    });
    am.push_back("-nrm|--no-require-match|/nrm|/no-require-match|no-require-match", [](limhamn::argument_manager::collection&) {
        requirematch = false;
    });
    am.push_back("-ma|--mark-items|/ma|/mark-items|mark-items", [](limhamn::argument_manager::collection&) {
        mark = true;
    });
    am.push_back("-nma|--no-mark-items|/nma|/no-mark-items|no-mark-items", [](limhamn::argument_manager::collection&) {
        mark = false;
    });
    am.push_back("-F|--fuzzy|/F|/fuzzy|fuzzy", [](limhamn::argument_manager::collection&) {
        fuzzy = true;
    });
    am.push_back("-NF|--no-fuzzy|/NF|/no-fuzzy|no-fuzzy", [](limhamn::argument_manager::collection&) {
        fuzzy = false;
    });
    am.push_back("-R|--regex|/R|/regex|regex", [](limhamn::argument_manager::collection&) {
        regex = true;
    });
    am.push_back("-NR|--no-regex|/NR|/no-regex|no-regex", [](limhamn::argument_manager::collection&) {
        regex = false;
    });
    am.push_back("-P|--password|/P|/password|password", [](limhamn::argument_manager::collection&) {
        passwd = true;
    });
    am.push_back("-nP|--no-password|/nP|/no-password|no-password", [](limhamn::argument_manager::collection&) {
        passwd = false;
    });
    am.push_back("-p|--prompt|/p|/prompt|prompt", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -p/--prompt flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        prompt = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-pt|--pretext|/pt|/pretext|pretext", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -pt/--pretext flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        pretext = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-It|--input|/It|/input|input", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -It/--input flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        input = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ip|--indent|/ip|/indent|indent", [](limhamn::argument_manager::collection&) {
        indentitems = true;
    });
    am.push_back("-nip|--no-indent|/nip|/no-indent|no-indent", [](limhamn::argument_manager::collection&) {
        indentitems = false;
    });
    am.push_back("-ci|--color-items|/ci|/color-items|color-items", [](limhamn::argument_manager::collection&) {
        coloritems = true;
    });
    am.push_back("-nci|--no-color-items|/nci|/no-color-items|no-color-items", [](limhamn::argument_manager::collection&) {
        coloritems = false;
    });
    am.push_back("-sgr|--sgr|/sgr|/sgr|sgr", [](limhamn::argument_manager::collection&) {
        sgr = true;
    });
    am.push_back("-nsgr|--no-sgr|/nsgr|/no-sgr|no-sgr", [](limhamn::argument_manager::collection&) {
        sgr = false;
    });
    am.push_back("-a|--alpha|/a|/alpha|alpha", [](limhamn::argument_manager::collection&) {
        alpha = true;
    });
    am.push_back("-na|--no-alpha|/na|/no-alpha|no-alpha", [](limhamn::argument_manager::collection&) {
        alpha = false;
    });
    am.push_back("-tp|--allow-typing|/tp|/allow-typing|allow-typing", [](limhamn::argument_manager::collection&) {
        type = true;
    });
    am.push_back("-nt|--no-allow-typing|/nt|/no-allow-typing|no-allow-typing", [](limhamn::argument_manager::collection&) {
        type = false;
    });
    am.push_back("-ol|--override-lines|/ol|/override-lines|override-lines", [](limhamn::argument_manager::collection&) {
        overridelines = true;
    });
    am.push_back("-nol|--no-override-lines|/nol|/no-override-lines|no-override-lines", [](limhamn::argument_manager::collection&) {
        overridelines = false;
    });
    am.push_back("-oc|--override-columns|/oc|/override-columns|override-columns", [](limhamn::argument_manager::collection&) {
        overridecolumns = true;
    });
    am.push_back("-noc|--no-override-columns|/noc|/no-override-columns|no-override-columns", [](limhamn::argument_manager::collection&) {
        overridecolumns = false;
    });
    am.push_back("-x|--x-position|/x|/x-position|x-position", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -x/--x-position flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        xpos = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-y|--y-position|/y|/y-position|y-position", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -y/--y-position flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        ypos = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-n|--preselect|/n|/preselect|preselect", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -n/--preselect flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        preselected = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-z|--width|/z|/width|width", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -z/--width flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        menuwidth = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-nmt|--normal-mode-text|/nmt|/normal-mode-text|normal-mode-text", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nmt/--normal-mode-text flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        normtext = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-imt|--insert-mode-text|/imt|/insert-mode-text|insert-mode-text", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -imt/--insert-mode-text flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        instext = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-clon|--caps-lock-on-text|/clon|/caps-lock-on-text|caps-lock-on-text", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -clon/--caps-lock-on-text flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        capslockontext = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-clof|--caps-lock-off-text|/clof|/caps-lock-off-text|caps-lock-off-text", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -clof/--caps-lock-off-text flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        capslockofftext = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-bw|--border-width|/bw|/border-width|border-width", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -bw/--border-width flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        borderwidth = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-sort|--sort|/sort|/sort|sort", [](limhamn::argument_manager::collection&) {
        sortmatches = true;
    });
    am.push_back("-nsort|--no-sort|/nsort|/no-sort|no-sort", [](limhamn::argument_manager::collection&) {
        sortmatches = false;
    });
    am.push_back("-pri|--priority|/pri|/priority|priority", [](limhamn::argument_manager::collection& collection) {
        /*
        } else if (!strcmp(argv[i], "-pri") || (!strcmp(argv[i], "--priority"))) { // high priority (csv format)
            hpitems = tokenize(argv[++i], ",", &hplength);
            */
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -p/--priority flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        const auto& items = tokenize(collection.arguments.at(++collection.index), ",");

        // convert std::vector<std::string> to char**, because we haven't converted the rest of the code yet
        const auto length = items.size();
        hpitems = new char*[length];
        for (size_t i = 0; i < length; ++i) {
            hpitems[i] = strdup(items.at(i).c_str());
        }

        hplength = length;
    });
    am.push_back("-s|--case-sensitive|/s|/case-sensitive|case-sensitive", [](limhamn::argument_manager::collection&) {
        casesensitive = true;
    });
    am.push_back("-ncs|--no-case-sensitive|/ncs|/no-case-sensitive|no-case-sensitive", [](limhamn::argument_manager::collection&) {
        casesensitive = false;
    });
    am.push_back("-ci|--case-insensitive|/ci|/case-insensitive|case-insensitive", [](limhamn::argument_manager::collection&) {
        casesensitive = false;
    });
    am.push_back("-nm|--normal|/nm|/normal|normal", [](limhamn::argument_manager::collection&) {
        mode = 0;
    });
    am.push_back("-im|--insert|/im|/insert|insert", [](limhamn::argument_manager::collection&) {
        mode = 1;
    });
    am.push_back("-t|--top|/t|/top|top", [](limhamn::argument_manager::collection&) {
        menuposition = 1;
    });
    am.push_back("-b|--bottom|/b|/bottom|bottom", [](limhamn::argument_manager::collection&) {
        menuposition = 0;
    });
    am.push_back("-c|--center|/c|/center|center", [](limhamn::argument_manager::collection&) {
        menuposition = 2;
    });
    am.push_back("-itt|--item-top|/itt|/item-top|item-top", [](limhamn::argument_manager::collection&) {
        itemposition = 1;
    });
    am.push_back("-itb|--item-bottom|/itb|/item-bottom|item-bottom", [](limhamn::argument_manager::collection&) {
        itemposition = 0;
    });
    am.push_back("-hm|--hide-mode|/hm|/hide-mode|hide-mode", [](limhamn::argument_manager::collection&) {
        hidemode = true;
    });
    am.push_back("-nhm|--no-hide-mode|/nhm|/no-hide-mode|no-hide-mode", [](limhamn::argument_manager::collection&) {
        hidemode = false;
    });
    am.push_back("-hmc|--hide-match-count|/hmc|/hide-match-count|hide-match-count", [](limhamn::argument_manager::collection&) {
        hidematchcount = true;
    });
    am.push_back("-nhmc|--no-hide-match-count|/nhmc|/no-hide-match-count|no-hide-match-count", [](limhamn::argument_manager::collection&) {
        hidematchcount = false;
    });
    am.push_back("-hla|--hide-left-arrow|/hla|/hide-left-arrow|hide-left-arrow", [](limhamn::argument_manager::collection&) {
        hidelarrow = true;
    });
    am.push_back("-nhla|--no-hide-left-arrow|/nhla|/no-hide-left-arrow|no-hide-left-arrow", [](limhamn::argument_manager::collection&) {
        hidelarrow = false;
    });
    am.push_back("-hra|--hide-right-arrow|/hra|/hide-right-arrow|hide-right-arrow", [](limhamn::argument_manager::collection&) {
        hiderarrow = true;
    });
    am.push_back("-nhra|--no-hide-right-arrow|/nhra|/no-hide-right-arrow|no-hide-right-arrow", [](limhamn::argument_manager::collection&) {
        hiderarrow = false;
    });
    am.push_back("-hit|--hide-item|/hit|/hide-item|hide-item", [](limhamn::argument_manager::collection&) {
        hideitem = true;
    });
    am.push_back("-nhit|--no-hide-item|/nhit|/no-hide-item|no-hide-item", [](limhamn::argument_manager::collection&) {
        hideitem = false;
    });
    am.push_back("-hpr|--hide-prompt|/hpr|/hide-prompt|hide-prompt", [](limhamn::argument_manager::collection&) {
        hideprompt = true;
    });
    am.push_back("-nhpr|--no-hide-prompt|/nhpr|/no-hide-prompt|no-hide-prompt", [](limhamn::argument_manager::collection&) {
        hideprompt = false;
    });
    am.push_back("-hpt|--hide-pretext|/hpt|/hide-pretext|hide-pretext", [](limhamn::argument_manager::collection&) {
        hidepretext = true;
    });
    am.push_back("-nhpt|--no-hide-pretext|/nhpt|/no-hide-pretext|no-hide-pretext", [](limhamn::argument_manager::collection&) {
        hidepretext = false;
    });
    am.push_back("-hip|--hide-input|/hip|/hide-input|hide-input", [](limhamn::argument_manager::collection&) {
        hideinput = true;
    });
    am.push_back("-nhip|--no-hide-input|/nhip|/no-hide-input|no-hide-input", [](limhamn::argument_manager::collection&) {
        hideinput = false;
    });
    am.push_back("-hpl|--hide-powerline|/hpl|/hide-powerline|hide-powerline", [](limhamn::argument_manager::collection&) {
        hidepowerline = true;
    });
    am.push_back("-nhpl|--no-hide-powerline|/nhpl|/no-hide-powerline|no-hide-powerline", [](limhamn::argument_manager::collection&) {
        hidepowerline = false;
    });
    am.push_back("-hc|--hide-caret|/hc|/hide-caret|hide-caret|--hide-cursor|/hide-cursor|hide-cursor", [](limhamn::argument_manager::collection&) {
        hidecaret = true;
    });
    am.push_back("-nhc|--no-hide-caret|/nhc|/no-hide-caret|no-hide-caret|--no-hide-cursor|/no-hide-cursor|no-hide-cursor", [](limhamn::argument_manager::collection&) {
        hidecaret = false;
    });
    am.push_back("-hhl|--hide-highlighting|/hhl|/hide-highlighting|hide-highlighting", [](limhamn::argument_manager::collection&) {
        hidehighlight = true;
    });
    am.push_back("-nhhl|--no-hide-highlighting|/nhhl|/no-hide-highlighting|no-hide-highlighting", [](limhamn::argument_manager::collection&) {
        hidehighlight = false;
    });
    am.push_back("-hi|--hide-image|/hi|/hide-image|hide-image", [](limhamn::argument_manager::collection&) {
        hideimage = true;
    });
    am.push_back("-nhi|--no-hide-image|/nhi|/no-hide-image|no-hide-image", [](limhamn::argument_manager::collection&) {
        hideimage = false;
    });
    am.push_back("-hcl|--hide-caps|/hcl|/hide-caps|hide-caps", [](limhamn::argument_manager::collection&) {
        hidecaps = true;
    });
    am.push_back("-nhcl|--no-hide-caps|/nhcl|/no-hide-caps|no-hide-caps", [](limhamn::argument_manager::collection&) {
        hidecaps = false;
    });
    am.push_back("-sm|--show-mode|/sm|/show-mode|show-mode", [](limhamn::argument_manager::collection&) {
        hidemode = false;
    });
    am.push_back("-nsm|--no-show-mode|/nsm|/no-show-mode|no-show-mode", [](limhamn::argument_manager::collection&) {
        hidemode = true;
    });
    am.push_back("-smc|--show-match-count|/smc|/show-match-count|show-match-count", [](limhamn::argument_manager::collection&) {
        hidematchcount = false;
    });
    am.push_back("-sla|--show-left-arrow|/sla|/show-left-arrow|show-left-arrow", [](limhamn::argument_manager::collection&) {
        hidelarrow = false;
    });
    am.push_back("-sra|--show-right-arrow|/sra|/show-right-arrow|show-right-arrow", [](limhamn::argument_manager::collection&) {
        hiderarrow = false;
    });
    am.push_back("-sit|--show-item|/sit|/show-item|show-item", [](limhamn::argument_manager::collection&) {
        hideitem = false;
    });
    am.push_back("-spr|--show-prompt|/spr|/show-prompt|show-prompt", [](limhamn::argument_manager::collection&) {
        hideprompt = false;
    });
    am.push_back("-spt|--show-pretext|/spt|/show-pretext|show-pretext", [](limhamn::argument_manager::collection&) {
        hidepretext = false;
    });
    am.push_back("-sin|--show-input|/sin|/show-input|show-input", [](limhamn::argument_manager::collection&) {
        hideinput = false;
    });
    am.push_back("-spl|--show-powerline|/spl|/show-powerline|show-powerline", [](limhamn::argument_manager::collection&) {
        hidepowerline = false;
    });
    am.push_back("-sc|--show-caret|/sc|/show-caret|show-caret|--show-cursor|/show-cursor|show-cursor", [](limhamn::argument_manager::collection&) {
        hidecaret = false;
    });
    am.push_back("-shl|--show-highlighting|/shl|/show-highlighting|show-highlighting", [](limhamn::argument_manager::collection&) {
        hidehighlight = false;
    });
    am.push_back("-si|--show-image|/si|/show-image|show-image", [](limhamn::argument_manager::collection&) {
        hideimage = false;
    });
    am.push_back("-scl|--show-caps|/scl|/show-caps|show-caps", [](limhamn::argument_manager::collection&) {
        hidecaps = false;
    });
    am.push_back("-m|--monitor|/m|/monitor|monitor", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -m/--monitor flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        mon = stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-w|--embed|/w|/embed|embed", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -w/--embed flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        x11.embed = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-H|--hist-file|/H|/hist-file|hist-file", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -H/--hist-file flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        histfile = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-lf|--list-file|/lf|/list-file|list-file", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -lf/--list-file flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        listfile = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ig|--image-gaps|/ig|/image-gaps|image-gaps", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ig/--image-gaps flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        imagegaps = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-txp|--text-padding|/txp|/text-padding|text-padding", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -txp/--text-padding flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        textpadding = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-lp|--vertical-padding|/lp|/vertical-padding|vertical-padding", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -lp/--vertical-padding flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        menupaddingv = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-hp|--horizontal-padding|/hp|/horizontal-padding|horizontal-padding", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -hp/--horizontal-padding flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        menupaddingh = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-vem|--vertical-margin|/vem|/vertical-margin|vertical-margin", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -vem/--vertical-margin flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        menumarginv = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-hom|--horizontal-margin|/hom|/horizontal-margin|horizontal-margin", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -hom/--horizontal-margin flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        menumarginh = std::stoi(collection.arguments.at(++collection.index));
    });
    am.push_back("-la|--left-arrow-symbol|/la|/left-arrow-symbol|left-arrow-symbol", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -la/--left-arrow-symbol flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        leftarrow = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ra|--right-arrow-symbol|/ra|/right-arrow-symbol|right-arrow-symbol", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ra/--right-arrow-symbol flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        rightarrow = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-is|--image-size|/is|/image-size|image-size", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -is/--image-size flag requires a value to be specified. Either WIDTHxHEIGHT or SIZE.\n";
            std::exit(EXIT_FAILURE);
        }

        // split the value by 'x'
        const auto& items = tokenize(collection.arguments.at(++collection.index), "x");
        if (items.size() != 2) {
            try {
                imagewidth = imageheight = std::stoi(collection.arguments.at(collection.index));
            } catch (const std::invalid_argument&) {
                std::cerr << "The -is/--image-size flag requires a value to be specified. The argument specified is invalid. Either WIDTHxHEIGHT or SIZE.\n";
                std::exit(EXIT_FAILURE);
            }
        } else {
            imagewidth = std::stoi(items.at(0));
            imageheight = std::stoi(items.at(1));
        }
    });
    am.push_back("-it|--image-top|/it|/image-top|image-top", [](limhamn::argument_manager::collection&) {
        imageposition = 0;
    });
    am.push_back("-ib|--image-bottom|/ib|/image-bottom|image-bottom", [](limhamn::argument_manager::collection&) {
        imageposition = 1;
    });
    am.push_back("-ic|--image-center|/ic|/image-center|image-center", [](limhamn::argument_manager::collection&) {
        imageposition = 2;
    });
    am.push_back("-itc|--image-topcenter|/itc|/image-topcenter|image-topcenter", [](limhamn::argument_manager::collection&) {
        imageposition = 3;
    });
    am.push_back("-di|--display-icons|/di|/display-icons|display-icons", [](limhamn::argument_manager::collection&) {
        imagetype = false;
    });
    am.push_back("-df|--display-image|/df|/display-image|display-image", [](limhamn::argument_manager::collection&) {
        imagetype = true;
    });
    am.push_back("-ir|--image-resize|/ir|/image-resize|image-resize", [](limhamn::argument_manager::collection&) {
        imageresize = true;
    });
    am.push_back("-nir|--no-image-resize|/nir|/no-image-resize|no-image-resize", [](limhamn::argument_manager::collection&) {
        imageresize = false;
    });
    am.push_back("-wm|--managed|/wm|/managed|managed", [](limhamn::argument_manager::collection&) {
        managed = true;
    });
    am.push_back("-nwm|--unmanaged|/nwm|/unmanaged|unmanaged", [](limhamn::argument_manager::collection&) {
        managed = false;
    });
    am.push_back("-gk|--grab-keyboard|/gk|/grab-keyboard|grab-keyboard", [](limhamn::argument_manager::collection&) {
        grabkeyboard = true;
    });
    am.push_back("-ngk|--no-grab-keyboard|/ngk|/no-grab-keyboard|no-grab-keyboard", [](limhamn::argument_manager::collection&) {
        grabkeyboard = false;
    });
    am.push_back("-fn|--font|/fn|/font|font", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -fn/--font flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        font = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nif|--normal-item-foreground|/nif|/normal-item-foreground|normal-item-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nif/--normal-item-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nib|--normal-item-background|/nib|/normal-item-background|normal-item-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nib/--normal-item-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nnif|--normal-next-item-foreground|/nnif|/normal-next-item-foreground|normal-next-item-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nnif/--normal-next-item-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormfg2 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nnib|--normal-next-item-background|/nnib|/normal-next-item-background|normal-next-item-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nnib/--normal-next-item-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormbg2 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sif|--selected-item-foreground|/sif|/selected-item-foreground|selected-item-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sif/--selected-item-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemselfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sib|--selected-item-background|/sib|/selected-item-background|selected-item-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sib/--selected-item-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemselbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-npf|--normal-item-priority-foreground|/npf|/normal-item-priority-foreground|normal-item-priority-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -npf/--normal-item-priority-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormprifg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-npb|--normal-item-priority-background|/npb|/normal-item-priority-background|normal-item-priority-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -npb/--normal-item-priority-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormpribg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-spf|--selected-item-priority-foreground|/spf|/selected-item-priority-foreground|selected-item-priority-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -spf/--selected-item-priority-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemselprifg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-spb|--selected-item-priority-background|/spb|/selected-item-priority-background|selected-item-priority-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -spb/--selected-item-priority-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemselpribg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-pfg|--prompt-foreground|/pfg|/prompt-foreground|prompt-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -pfg/--prompt-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_promptfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-pbg|--prompt-background|/pbg|/prompt-background|prompt-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -pbg/--prompt-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_promptbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ifg|--input-foreground|/ifg|/input-foreground|input-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ifg/--input-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_inputfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ibg|--input-background|/ibg|/input-background|input-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ibg/--input-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_inputbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ptfg|--pretext-foreground|/ptfg|/pretext-foreground|pretext-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ptfg/--pretext-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_pretextfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-ptbg|--pretext-background|/ptbg|/pretext-background|pretext-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -ptbg/--pretext-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_pretextbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-mnbg|--menu-background|/mnbg|/menu-background|menu-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -mnbg/--menu-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_menu = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nhf|--normal-highlight-foreground|/nhf|/normal-highlight-foreground|normal-highlight-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nhf/--normal-highlight-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_hlnormfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nhb|--normal-highlight-background|/nhb|/normal-highlight-background|normal-highlight-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nhb/--normal-highlight-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_hlnormbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-shf|--selected-highlight-foreground|/shf|/selected-highlight-foreground|selected-highlight-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -shf/--selected-highlight-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_hlselfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-shb|--selected-highlight-background|/shb|/selected-highlight-background|selected-highlight-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -shb/--selected-highlight-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_hlselbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nfg|--number-foreground|/nfg|/number-foreground|number-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nfg/--number-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_numfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nbg|--number-background|/nbg|/number-background|number-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nbg/--number-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_numbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-mfg|--mode-foreground|/mfg|/mode-foreground|mode-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -mfg/--mode-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_modefg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-mbg|--mode-background|/mbg|/mode-background|mode-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -mbg/--mode-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_modebg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-laf|--left-arrow-foreground|/laf|/left-arrow-foreground|left-arrow-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -laf/--left-arrow-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_larrowfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-raf|--right-arrow-foreground|/raf|/right-arrow-foreground|right-arrow-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -raf/--right-arrow-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_rarrowfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-lab|--left-arrow-background|/lab|/left-arrow-background|left-arrow-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -lab/--left-arrow-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_larrowbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-rab|--right-arrow-background|/rab|/right-arrow-background|right-arrow-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -rab/--right-arrow-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_rarrowbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-cfc|--caret-foreground|/cfc|/caret-foreground|caret-foreground", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -cfc/--caret-foreground flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_caretfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-cbc|--caret-background|/cbc|/caret-background|caret-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -cbc/--caret-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_caretbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-bc|--border-background|/bc|/border-background|border-background", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -bc/--border-background flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_border = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr0|--sgr0|/sgr0|/sgr0|sgr0", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr0/--sgr0 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr1 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr1|--sgr1|/sgr1|/sgr1|sgr1", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr1/--sgr1 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr1 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr2|--sgr2|/sgr2|/sgr2|sgr2", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr2/--sgr2 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr2 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr3|--sgr3|/sgr3|/sgr3|sgr3", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr3/--sgr3 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr3 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr4|--sgr4|/sgr4|/sgr4|sgr4", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr4/--sgr4 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr4 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr5|--sgr5|/sgr5|/sgr5|sgr5", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr5/--sgr5 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr5 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr6|--sgr6|/sgr6|/sgr6|sgr6", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr6/--sgr6 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr6 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr7|--sgr7|/sgr7|/sgr7|sgr7", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr7/--sgr7 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr7 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr8|--sgr8|/sgr8|/sgr8|sgr8", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr8/--sgr8 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr8 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr9|--sgr9|/sgr9|/sgr9|sgr9", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr9/--sgr9 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr9 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr10|--sgr10|/sgr10|/sgr10|sgr10", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr10/--sgr10 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr10 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr11|--sgr11|/sgr11|/sgr11|sgr11", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr11/--sgr11 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr11 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr12|--sgr12|/sgr12|/sgr12|sgr12", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr12/--sgr12 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr12 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr13|--sgr13|/sgr13|/sgr13|sgr13", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr13/--sgr13 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr13 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr14|--sgr14|/sgr14|/sgr14|sgr14", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr14/--sgr14 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr14 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sgr15|--sgr15|/sgr15|/sgr15|sgr15", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sgr15/--sgr15 flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_sgr15 = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-S", [](limhamn::argument_manager::collection&) {
        sortmatches = false;
    });
    am.push_back("-i", [](limhamn::argument_manager::collection&) {
        casesensitive = false;
    });

    // TODO: Fix up the following flags
    am.push_back("-nb", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nb flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_menu = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-nf", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -nf flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemnormfg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sb", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sb flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemselbg = strdup(collection.arguments.at(++collection.index).c_str());
    });
    am.push_back("-sf", [](limhamn::argument_manager::collection& collection) {
        if (collection.arguments.size() <= collection.index + 1) {
            std::cerr << "The -sf flag requires a value to be specified.\n";
            std::exit(EXIT_FAILURE);
        }

        col_itemselfg = strdup(collection.arguments.at(++collection.index).c_str());
    });

    am.execute([](const std::string& arg) {
        std::cerr << "Unknown argument(s) provided: " << arg << "\n";
        std::exit(EXIT_FAILURE);
    });
}

void usage(int status) {
    // print help
    fputs("majorna ", status ? stderr : stdout);
    fputs(VERSION, status ? stderr : stdout);
    fputs(": fancy dynamic menu\n\n"
            "- Arguments -\n"
            "majorna -mh,      --line-height <height>                      Set majorna line height to <height>\n"
            "majorna -cw,      --center-width <width>                      Set width to <width> when centered\n"
            "majorna -l,       --lines <line>                              Set line count to stdin\n"
            "majorna -g,       --columns <grid>                            Set the number of grids to <grid>\n"
            "majorna -ml,      --min-lines <lines>                         Minimum number of lines allowed\n"
            "majorna -gc,      --generate-cache                            Generate image cache\n"
            "majorna -ngc,     --no-generate-cache                         Don't generate image cache\n"
            "majorna -mc,      --max-cache <size>                          Set max image cache size to <size>\n"
            "majorna -cd,      --cache-dir <dir>                           Set cache directory to <dir>\n"
            "majorna -ix,      --print-index                               Print index instead of actual text\n"
            "majorna -nix,     --no-print-index                            Don't print index instead of actual text\n"
            "majorna -f,       --fast                                      Grabs keyboard before reading stdin\n"
            "majorna -r,       --incremental                               Print text every time a key is pressed\n"
            "majorna -nr,      --no-incremental                            Don't print text every time a key is pressed\n"
            "majorna -rm,      --require-match                             Require that input text matches an item\n"
            "majorna -nrm,     --no-require-match                          Don't require that input text matches an item\n"
            "majorna -ma,      --mark-items                                Allow marking/selecting multiple items\n"
            "majorna -nma,     --no-mark-items                             Don't allow marking/selecting multiple items\n"
            "majorna -F,       --fuzzy                                     Enable fuzzy matching\n"
            "majorna -NF,      --no-fuzzy                                  Disable fuzzy matching\n"
            "majorna -R,       --regex                                     Enable regex matching\n"
            "majorna -NR,      --no-regex                                  Disable regex matching\n"
            "majorna -P,       --password                                  Hide characters\n"
            "majorna -nP,      --no-password                               Don't hide characters\n"
            "majorna -p,       --prompt <text>                             Set majorna prompt text to <text>\n"
            "majorna -pt,      --pretext <text>                            Set majorna pretext to <text>\n"
            "majorna -It,      --input <text>                              Set initial input text to <text>\n"
            "majorna -ip,      --indent                                    Indent items to prompt width\n"
            "majorna -nip,     --no-indent                                 Don't indent items to prompt width\n"
            "majorna -ci,      --color-items                               Color items\n"
            "majorna -nci,     --no-color-items                            Don't color items\n"
            "majorna -sgr,     --sgr                                       Interpret SGR sequences\n"
            "majorna -nsgr,    --no-sgr                                    Display SGR sequences as text\n"
            "majorna -a,       --alpha                                     Enable alpha\n"
            "majorna -na,      --no-alpha                                  Disable alpha\n"
            "majorna -tp,      --allow-typing                              Allow the user to type\n"
            "majorna -nt,      --no-allow-typing                           Don't allow typing, the user must select an option\n"
            "majorna -ol,      --override-lines                            Allow keybinds to override lines\n"
            "majorna -oc,      --override-columns                          Allow keybinds to override columns\n"
            "majorna -nol,     --no-override-lines                         Don't allow keybinds to override lines\n"
            "majorna -noc,     --no-override-columns                       Don't allow keybinds to override columns\n"
            , status ? stderr : stdout);

    fputs(
            "majorna -x,       --x-position <x offset>                     Offset majorna x position by <x offset> (X11 only)\n"
            "majorna -y,       --y-position <y offset>                     Offset majorna y position by <y offset> (X11 only)\n"
            "majorna -n,       --preselect <line>                          Preselect <line> in the list of items\n"
            "majorna -z,       --width <width>                             Width of the majorna window\n"
            "majorna -nmt,     --normal-mode-text <text>                   Set normal mode text to <text>\n"
            "majorna -imt,     --insert-mode-text <text>                   Set insert mode text to <text>\n"
            "majorna -clon,    --caps-lock-on-text <text>                  Set caps lock on text to <text>\n"
            "majorna -clof,    --caps-lock-off-text <text>                 Set caps lock off text to <text>\n"
            "majorna -bw,      --border-width <width>                      Set width of the border to <width>. 0 will disable the border (X11 only)\n"
            "majorna -so,      --sort                                      Sort matches\n"
            "majorna -nso,     --no-sort                                   Don't sort matches\n"
            "majorna -pri,     --priority <pri1,pri2,pri3>                 Specify a list of items that take priority\n"
            "majorna -s,       --case-sensitive                            Use case-sensitive matching\n"
            "majorna -ns,      --case-insensitive                          Use case-insensitive matching\n"
            "majorna -nm,      --normal                                    Start majorna in normal mode\n"
            "majorna -im,      --insert                                    Start majorna in insert mode\n"
            "majorna -t,       --top                                       Position majorna at the top of the screen\n"
            "majorna -b,       --bottom                                    Position majorna at the bottom of the screen\n"
            "majorna -c,       --center                                    Position majorna at the center of the screen\n"
            "majorna -itt,     --item-top                                  Position items above all other elements\n"
            "majorna -itb,     --item-bottom                               Position items below all other elements\n"
            , status ? stderr : stdout);

    // more args
    fputs(
            "majorna -hm,      --hide-mode                                 Hide mode indicator\n"
            "majorna -hmc,     --hide-match-count                          Hide match count\n"
            "majorna -hla,     --hide-left-arrow                           Hide left arrow\n"
            "majorna -hra,     --hide-right-arrow                          Hide right arrow\n"
            "majorna -hit,     --hide-item                                 Hide items\n"
            "majorna -hpr,     --hide-prompt                               Hide prompt\n"
            "majorna -hpt,     --hide-pretext                              Hide pretext\n"
            "majorna -hip,     --hide-input                                Hide input\n"
            "majorna -hpl,     --hide-powerline                            Hide powerline\n"
            "majorna -hc,      --hide-caret                                Hide caret\n"
            "majorna -hhl,     --hide-highlighting                         Hide highlight\n"
            "majorna -hi,      --hide-image                                Hide image\n"
            "majorna -hcl,     --hide-caps                                 Hide caps lock indicator\n"
            "majorna -sm,      --show-mode                                 Show mode indicator\n"
            "majorna -smc,     --show-match-count                          Show match count\n"
            "majorna -sla,     --show-left-arrow                           Show left arrow\n"
            "majorna -sra,     --show-right-arrow                          Show right arrow\n"
            "majorna -sit,     --show-item                                 Show items\n"
            "majorna -spr,     --show-prompt                               Show prompt\n"
            "majorna -spt,     --show-pretext                              Show pretext\n"
            "majorna -sin,     --show-input                                Show input\n"
            "majorna -spl,     --show-powerline                            Show powerline\n"
            "majorna -sc,      --show-caret                                Show caret\n"
            "majorna -shl,     --show-highlighting                         Show highlight\n"
            "majorna -si,      --show-image                                Show image\n"
            "majorna -scl,     --show-caps                                 Show caps lock indicator\n"
            "majorna -xrdb,    --xrdb                                      Load .Xresources on runtime (X11 only)\n"
            "majorna -nxrdb,   --no-xrdb                                   Don't load .Xresources on runtime (X11 only)\n"
            "majorna -m,       --monitor <monitor>                         Specify a monitor to run majorna on (X11 only)\n"
            "majorna -w,       --embed <window id>                         Embed majorna inside <window id> (X11 only)\n"
            "majorna -H,       --hist-file <hist file>                     Specify a file to save the history to\n"
            "majorna -lf,      --list-file <list file>                     Specify a file to load entries from\n"
            "majorna -ig,      --image-gaps <gaps>                         Set image gaps to <gaps>\n"
            "majorna -txp,     --text-padding <padding>                    Set text padding to <padding>\n"
            "majorna -lp,      --vertical-padding <padding>                Set the vertical padding to <padding>\n"
            "majorna -hp,      --horizontal-padding <padding>              Set the horizontal padding to <padding>\n"
            "majorna -vem,     --vertical-margin <margin>                  Set the vertical margin to <margin>\n"
            "majorna -hom,     --horizontal-margin <margin>                Set the horizontal margin to <margin>\n"
            "majorna -la,      --left-arrow-symbol <symbol>                Set the left arrow to <symbol>\n"
            "majorna -ra,      --right-arrow-symbol <symbol>               Set the right arrow to <symbol>\n"
            , status ? stderr : stdout);

    // image related
    fputs(
            "majorna -is,      --image-size <size>                         Set image size to <size>\n"
            "majorna -it,      --image-top                                 Position the image at the top\n"
            "majorna -ib,      --image-bottom                              Position the image at the bottom\n"
            "majorna -ic,      --image-center                              Position the image in the center\n"
            "majorna -itc,     --image-topcenter                           Position the image in the top center\n"
            "majorna -di,      --display-icons                             Display the images as icons\n"
            "majorna -df,      --display-image                             Display the images as images in the image pane\n"
            "majorna -ir,      --image-resize                              Allow majorna to resize itself to fit the image\n"
            "majorna -nir,     --no-image-resize                           Don't allow majorna to resize itself to fit the image\n"
            , status ? stderr : stdout);

    // general/config related
    fputs(
            "majorna -wm,      --managed, --x11-client                     Spawn majorna as a window manager controlled client/window (X11 only)\n"
            "majorna -nwm,     --unmanaged                                 Don't spawn majorna as a window manager controlled client/window (X11 only)\n"
            "majorna -gk,      --grab-keyboard                             Grab keyboard on runtime\n"
            "majorna -ngk,     --no-grab-keyboard                          Don't grab keyboard on runtime\n"
            "majorna -cf,      --config-file <file>                        Set config file to load to <file>\n"
            "majorna -lcfg,    --load-config                               Load majorna configuration (~/.config/majorna/majorna.conf)\n"
            "majorna -ncfg,    --no-load-config                            Don't load majorna configuration (~/.config/majorna/majorna.conf)\n"
            "majorna -bf,      --bind-file <file>                          Exclusively load binds from <file>\n"
            "majorna -lbi,     --load-binds                                Load majorna binds (~/.config/majorna/binds.conf)\n"
            "majorna -nlbi,    --no-load-binds                             Don't load majorna binds (~/.config/majorna/binds.conf)\n"
            "majorna -tm,      --theme-file <theme>                             Load theme <theme>\n"
            "majorna -ltm,     --load-theme                                Load theme\n"
            "majorna -nltm,    --no-load-theme                             Don't load theme\n"
            "majorna -x11,     --x11                                       Run majorna in X11 mode\n"
            "majorna -wl,      --wayland                                   Run majorna in Wayland mode\n"
            "\n", status ? stderr : stdout);

    // colors
    fputs(
            "- Appearance arguments -\n"
            "majorna -fn,      --font  <font>                              Set the majorna font to <font>\n"
            "majorna -nif,     --normal-item-foreground <color>            Set the normal item foreground color\n"
            "majorna -nib,     --normal-item-background <color>            Set the normal item background color\n"
            "majorna -nnif,    --normal-next-item-foreground <color>       Set the normal next item foreground color\n"
            "majorna -nnib,    --normal-next-item-background <color>       Set the normal next item background color\n"
            "majorna -sif,     --selected-item-foreground <color>          Set the selected item foreground color\n"
            "majorna -sib,     --selected-item-background <color>          Set the selected item background color\n"
            "majorna -npf,     --normal-item-priority-foreground <color>   Set the normal item (high priority) foreground color\n"
            "majorna -npb,     --normal-item-priority-background <color>   Set the normal item (high priority) background color\n"
            "majorna -spf,     --selected-item-priority-foreground <color> Set the selected item (high priority) foreground color\n"
            "majorna -spb,     --selected-item-priority-background <color> Set the selected item (high priority) background color\n"
            "majorna -pfg,     --prompt-foreground <color>                 Set the prompt foreground color\n"
            "majorna -pbg,     --prompt-background <color>                 Set the prompt background color\n"
            "majorna -ifg,     --input-foreground <color>                  Set input foreground color\n"
            "majorna -ibg,     --input-background <color>                  Set input background color\n"
            "majorna -ptfg,    --pretext-foreground <color>                Set pretext foreground color\n"
            "majorna -ptbg,    --pretext-background <color>                Set pretext background color\n"
            "majorna -mnbg,    --menu-background <color>                   Set the menu background color\n"
            "majorna -nhf,     --normal-highlight-foreground <color>       Set the normal highlight foreground color\n"
            "majorna -nhb,     --normal-highlight-background <color>       Set the normal highlight background color\n"
            "majorna -shf,     --selected-highlight-foreground <color>     Set the selected highlight foreground color\n"
            "majorna -shb,     --selected-highlight-background <color>     Set the selected highlight background color\n"
            "majorna -nfg,     --number-foreground <color>                 Set the foreground color for the match count\n"
            "majorna -nbg,     --number-background <color>                 Set the background color for the match count\n"
            "majorna -mfg,     --mode-foreground <color>                   Set the foreground color for the mode indicator\n"
            "majorna -mbg,     --mode-background <color>                   Set the background color for the mode indicator\n"
            "majorna -laf,     --left-arrow-foreground <color>             Set the left arrow foreground color\n"
            "majorna -raf,     --right-arrow-foreground <color>            Set the right arrow foreground color\n"
            "majorna -lab,     --left-arrow-background <color>             Set the left arrow background color\n"
            "majorna -rab,     --right-arrow-background <color>            Set the right arrow background color\n"
            "majorna -cfc,     --caret-foreground <color>                  Set the caret foreground color\n"
            "majorna -cbc,     --caret-background <color>                  Set the caret background color\n"
            "majorna -bc,      --border-background <color>                 Set the border color\n"
            , status ? stderr : stdout);

    // sgr sequences
    fputs(
            "majorna -sgr0,    --sgr0 <color>                              Set the SGR 0 color\n"
            "majorna -sgr1,    --sgr1 <color>                              Set the SGR 1 color\n"
            "majorna -sgr2,    --sgr2 <color>                              Set the SGR 2 color\n"
            "majorna -sgr3,    --sgr3 <color>                              Set the SGR 3 color\n"
            "majorna -sgr4,    --sgr4 <color>                              Set the SGR 4 color\n"
            "majorna -sgr5,    --sgr5 <color>                              Set the SGR 5 color\n"
            "majorna -sgr6,    --sgr6 <color>                              Set the SGR 6 color\n"
            "majorna -sgr7,    --sgr7 <color>                              Set the SGR 7 color\n"
            "majorna -sgr8,    --sgr8 <color>                              Set the SGR 8 color\n"
            "majorna -sgr9,    --sgr9 <color>                              Set the SGR 9 color\n"
            "majorna -sgr10,   --sgr10 <color>                             Set the SGR 10 color\n"
            "majorna -sgr11,   --sgr11 <color>                             Set the SGR 11 color\n"
            "majorna -sgr12,   --sgr12 <color>                             Set the SGR 12 color\n"
            "majorna -sgr13,   --sgr13 <color>                             Set the SGR 13 color\n"
            "majorna -sgr14,   --sgr14 <color>                             Set the SGR 14 color\n"
            "majorna -sgr15,   --sgr15 <color>                             Set the SGR 15 color\n"
            "\n", status ? stderr : stdout);

    // dmenu compat
    fputs(
            "- dmenu compatibility -\n"
            "majorna -S                                                    Don't sort matches\n"
            "majorna -i                                                    Use case-insensitive matching\n"
            "majorna -nb <color>                                           Set the normal background color\n"
            "majorna -nf <color>                                           Set the normal foreground color\n"
            "majorna -sb <color>                                           Set the selected background color\n"
            "majorna -sf <color>                                           Set the selected foreground color\n"
            "\n"
            "See the majorna(1) man page for more information.\n"
            , status ? stderr : stdout);

    exit(status);
}
