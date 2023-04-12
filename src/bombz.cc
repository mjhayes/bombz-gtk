/*
 * This file is part of bombz-gtk.
 *
 * bombz-gtk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bombz-gtk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bombz-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../config.h"
#include "bombz.h"
#include "scoreboard.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define RATIO 7

// Constructor
bombz::bombz(int width, int height) :	firstload(true), skill(0), align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
										a_bomb(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0), a_time(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0)
{
	// Load toolbar face graphics.
	toolface[0].set(DATADIR "/bombz-gtk/normal.png");
	toolface[1].set(DATADIR "/bombz-gtk/win.png");
	toolface[2].set(DATADIR "/bombz-gtk/dead.png");
	toolface[3].set(DATADIR "/bombz-gtk/clicking.png");
	
	// Load digital number graphics.
	diginum[0].set(DATADIR "/bombz-gtk/n0.xpm");
	diginum[1].set(DATADIR "/bombz-gtk/n0.xpm");
	diginum[2].set(DATADIR "/bombz-gtk/n0.xpm");
	diginum[3].set(DATADIR "/bombz-gtk/n0.xpm");
	diginum[4].set(DATADIR "/bombz-gtk/n0.xpm");
	diginum[5].set(DATADIR "/bombz-gtk/nco.xpm");
	diginum[6].set(DATADIR "/bombz-gtk/n0.xpm");
	diginum[7].set(DATADIR "/bombz-gtk/n0.xpm");
	
	set_title("bombz-gtk");				// Set window title
	set_border_width(0);				// Set window padding
	set_resizable(false);				// Set resizable
	vb1.set_border_width(5);			// Give a little padding for the grid
	eng = new engine();					// Initialize the game engine
	eng->set_bombref(this);				// Set a reference to this in eng
	
	gridratio = RATIO;					// Set default grid ratio
	gridwidth = width;					// Current grid width
	gridheight = height;				// Current grid height
	
	add(vb);							// Add main vertical box
	build_main_menu();					// Build the main menu
	align.add(vb1);
	vb.pack_end(align);					// Add align to vb
	build_toolbar();					// Build the toolbar
	draw_grid(gridwidth, gridheight);	// Draw button grid
	
	firstload = false;					// Set firstload to false
}

// Destructor
bombz::~bombz()
{
	delete[] hb;
	delete[] squares;
}

// Draw the button grid
void bombz::draw_grid(int width, int height)
{	
	// Change the toolbar face to normal
	set_face(0);
	
	// Do some clean up for new game
	if (!firstload && (gridwidth != width || gridheight != height))
	{
		// Delete the squares and hbox arrays.
		delete[] hb;
		delete[] squares;
	}
	
	// Create required squares and hboxes.
	// We only want to do this if the user has selected a new grid
	// size.
	if (firstload || (gridwidth != width || gridheight != height))
	{
		squares = new button[width * height];
		hb = new Gtk::HBox[height];
	}
	
	// Set grid size and reinitialize game status.
	eng->set_size(width, height);
	eng->new_game();
	
	// Initialize and set number of bombs for each square.
	eng->set_bombs();
	
	// Fill vbox with squares
	if (firstload || (gridwidth != width || gridheight != height))
	{
		for (int h = 0; h < height; h++)
		{
			for (int i = width * h; i < width * h + width; i++)
				hb[h].pack_start(squares[i], Gtk::PACK_SHRINK);

			vb1.pack_start(hb[h], Gtk::PACK_SHRINK);
		}
	}
	
	gridwidth = width;		// Set the new current grid width
	gridheight = height;	// Set the new current grid height
	
	// Show all children (everything graphical is done being set up)
	show_all_children();
	
	// Update toolbar
	update_toolbar();
}

// Get an instance of a square.
button* bombz::get_square(int i)
{
	return &squares[i];
}

// Get current grid ratio.
int bombz::get_ratio()
{
	return gridratio;
}

// Get current skill level.
int bombz::get_skill()
{
	return skill;
}

// Build the menubar
void bombz::build_main_menu()
{
	// Create actions for menus and toolbars:
	m_refActionGroup = Gtk::ActionGroup::create();

	// File|New sub menu:
	m_refActionGroup->add(	Gtk::Action::create("FileNewEasy",
							Gtk::Stock::YES, "Easy", "Start an easy game."),
							sigc::mem_fun(*this, &bombz::on_menu_file_new_easy));

	m_refActionGroup->add(	Gtk::Action::create("FileNewMedium",
							Gtk::Stock::YES, "Medium", "Start a medium game."),
							sigc::mem_fun(*this, &bombz::on_menu_file_new_medium));

	m_refActionGroup->add(	Gtk::Action::create("FileNewHard",
							Gtk::Stock::NO, "Hard", "Start a hard game."),
							sigc::mem_fun(*this, &bombz::on_menu_file_new_hard));
	
	m_refActionGroup->add(	Gtk::Action::create("FileNewCustom",
							Gtk::Stock::SELECT_COLOR, "Custom...", "Start a custom game."),
							sigc::mem_fun(*this, &bombz::on_menu_file_new_custom));
	
	// Scoreboard
	m_refActionGroup->add(	Gtk::Action::create("FileScoreboard",
							Gtk::Stock::PASTE, "Scoreboard", "View the top scores."),
							sigc::mem_fun(*this, &bombz::on_menu_file_scoreboard));
	
	// Help menu:
	m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
	m_refActionGroup->add(	Gtk::Action::create("HelpAbout",
							Gtk::Stock::ABOUT, "About", "About this program."),
							sigc::mem_fun(*this, &bombz::on_menu_help_about));


	// File menu:
	m_refActionGroup->add(Gtk::Action::create("FileMenu", "Game"));
	
	// Sub-menu.
	m_refActionGroup->add(Gtk::Action::create(	"FileNew",
												Gtk::Stock::EXECUTE, "New", "New game."));
	m_refActionGroup->add(	Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
							sigc::mem_fun(*this, &bombz::on_menu_file_quit));
	
	// Create UIManager
	m_refUIManager = Gtk::UIManager::create();
	m_refUIManager->insert_action_group(m_refActionGroup);
	
	// Add the keyboard shortcuts
	add_accel_group(m_refUIManager->get_accel_group());
	
	// Layout the actions in a menubar and toolbar:
	Glib::ustring ui_info = 
		"<ui>"
		"	<menubar name='MenuBar'>"
		"		<menu action='FileMenu'>"
		"			<menu action='FileNew'>"
		"				<menuitem action='FileNewEasy'/>"
		"				<menuitem action='FileNewMedium'/>"
		"				<menuitem action='FileNewHard'/>"
		"				<separator/>"
		"				<menuitem action='FileNewCustom'/>"
		"			</menu>"
		"			<separator/>"
		"			<menuitem action='FileScoreboard'/>"
		"			<separator/>"
		"			<menuitem action='FileQuit'/>"
		"		</menu>"
		"		<menu action='HelpMenu'>"
        "			<menuitem action='HelpAbout'/>"
        "		</menu>"
		"	</menubar>"
        "</ui>";
	
	#ifdef GLIBMM_EXCEPTIONS_ENABLED
	try
	{
		m_refUIManager->add_ui_from_string(ui_info);
	}
	catch(const Glib::Error& ex)
	{
		std::cerr << "building menus failed: " <<  ex.what();
	}
	#else
	std::auto_ptr<Glib::Error> ex;
	m_refUIManager->add_ui_from_string(ui_info, ex);
	if(ex.get())
	{
		std::cerr << "building menus failed: " <<  ex->what();
	}
	#endif //GLIBMM_EXCEPTIONS_ENABLED

	// Place menu into menu_widget
	Gtk::Widget *menu_widget = m_refUIManager->get_widget("/MenuBar");
	if (menu_widget)
		vb.pack_start(*menu_widget, Gtk::PACK_SHRINK);
}

// Build the toolbar
void bombz::build_toolbar()
{
	hb1.set_border_width(2);	// Set hbox padding
	hb1.set_homogeneous(true);	// Each widget occupies the same amount of space

	// Set button size and connect click signal
	newbut.set_size_request(44, 44);
	newbut.signal_clicked().connect(sigc::mem_fun(*this, &bombz::on_button_clicked));
	
	// Add images to hb_bomb.
	hb_bomb.pack_start(diginum[0], Gtk::PACK_SHRINK);
	hb_bomb.pack_start(diginum[1], Gtk::PACK_SHRINK);
	hb_bomb.pack_start(diginum[2], Gtk::PACK_SHRINK);
	
	// Add images to hb_time.
	hb_time.pack_start(diginum[3], Gtk::PACK_SHRINK);
	hb_time.pack_start(diginum[4], Gtk::PACK_SHRINK);
	hb_time.pack_start(diginum[5], Gtk::PACK_SHRINK);
	hb_time.pack_start(diginum[6], Gtk::PACK_SHRINK);
	hb_time.pack_start(diginum[7], Gtk::PACK_SHRINK);
	
	// Add to alignment containers.
	a_bomb.add(hb_bomb);
	a_time.add(hb_time);
	
	update_toolbar();	// Update the toolbar's content
	
	// Add widgets to hbox
	hb1.pack_start(a_bomb);
	hb1.pack_start(newbut, Gtk::PACK_SHRINK);
	hb1.pack_start(a_time);
	
	// Add hbox to vb
	vb.pack_start(hb1);
}

// Generate bomb count and timer graphics.
void bombz::gengfx(Glib::ustring bombstr, Glib::ustring timestr)
{
	int code, i;
	Glib::ustring path;
	
	// Update bomb graphics.
	for (int i = 0; i < bombstr.length(); i++)
	{
		code = bombstr[i] - 48;
		
		path = Glib::ustring::compose(DATADIR "/bombz-gtk/n%1.xpm", code);
		diginum[i].set(path);
	}
	
	// Update timer graphics.
	for (int i = 0; i < timestr.length(); i++)
	{
		code = timestr[i] - 48;
		
		if (code != 10)
		{
			path = Glib::ustring::compose(DATADIR "/bombz-gtk/n%1.xpm", code);
			diginum[3 + i].set(path);
		}
	}
}

// Set dynamic content on toolbar
void bombz::update_toolbar()
{
	// Set initial label text
	Glib::ustring bombstr;
	int bcount = eng->get_bombs();
	
	// Format bomb count.
	if (bcount < 10)
	 	bombstr = Glib::ustring::compose("00%1", bcount);
	else if (bcount < 100)
		bombstr = Glib::ustring::compose("0%1", bcount);
	else
		bombstr = Glib::ustring::compose("%1", bcount);
	
	// Format time in X min X secs
	int time = eng->get_time();
	int min = int(time / 60);
	int sec = time - min * 60;
	Glib::ustring minstr, secstr, timestr;
	
	// Format minutes
	if (min < 10)
		minstr = Glib::ustring::compose("0%1", min);
	else
		minstr = Glib::ustring::compose("%1", min);
	
	// Format seconds
	if (sec < 10)
		secstr = Glib::ustring::compose("0%1", sec);
	else
		secstr = Glib::ustring::compose("%1", sec);
	
	// Mins and seconds
	timestr = minstr + ":" + secstr;
	
	// Generate graphics for bombs and time.
	gengfx(bombstr, timestr);
}

// File->New->Easy
void bombz::on_menu_file_new_easy()
{
	skill = 0;
	gridratio = 7;
	draw_grid(9, 9);
}

// File->New->Medium
void bombz::on_menu_file_new_medium()
{
	skill = 1;
	gridratio = 6;
	draw_grid(16, 16);
}

// File->New->Hard
void bombz::on_menu_file_new_hard()
{
	skill = 2;
	gridratio = 5;
	draw_grid(30, 16);
}

// File->New->Custom
void bombz::on_menu_file_new_custom()
{
	Gtk::Dialog custom("Custom...", *this, false, true);
	Gtk::Frame sizeframe("Grid size"), ratioframe("Bombs-to-tile ratio");
	Gtk::Label lbwidth("Width (50 max): "), lbheight("Height (50 max): "), lbratio("~1 : "), lbcustom;
	Gtk::VBox *vbmain = custom.get_vbox(), vbsize;
	Gtk::HBox hbwidth, hbheight, hbratio;
	Gtk::Entry ewidth, eheight, eratio;
	Glib::ustring strwidth, strheight, strratio;
	
	// Configure labels
	lbwidth.set_alignment(0.0, 0.5);
	lbheight.set_alignment(0.0, 0.5);
	lbratio.set_alignment(0.0, 0.5);
	
	// Big label
	lbcustom.set_markup("<span size='xx-large' weight='bold'>Custom Game</span>");
	
	// Configure width entry box
	strwidth = Glib::ustring::compose("%1", gridwidth);
	ewidth.set_max_length(2);
	ewidth.set_width_chars(2);
	ewidth.set_text(strwidth);
	
	// Configure height entry box
	strheight = Glib::ustring::compose("%1", gridheight);
	eheight.set_max_length(2);
	eheight.set_width_chars(2);
	eheight.set_text(strheight);
	
	// Configure ratio entry box
	strratio = Glib::ustring::compose("%1", gridratio);
	eratio.set_max_length(2);
	eratio.set_width_chars(2);
	eratio.set_text(strratio);
	
	// Pack the width hbox
	hbwidth.set_border_width(2);
	hbwidth.pack_start(lbwidth);
	hbwidth.pack_start(ewidth, Gtk::PACK_SHRINK);
	
	// Pack the height hbox
	hbheight.set_border_width(2);
	hbheight.pack_start(lbheight);
	hbheight.pack_start(eheight, Gtk::PACK_SHRINK);
	
	// Pack the size vbox
	vbsize.set_border_width(4);
	vbsize.pack_start(hbwidth);
	vbsize.pack_start(hbheight);
	
	// Pack the ratio hbox
	hbratio.set_border_width(6);
	hbratio.pack_start(lbratio);
	hbratio.pack_start(eratio, Gtk::PACK_SHRINK);
	
	// Add hboxes to size frame
	sizeframe.set_border_width(6);
	sizeframe.add(vbsize);
	
	// Add hboxes to ratio frame
	ratioframe.set_border_width(6);
	ratioframe.add(hbratio);
	
	// Set the dialog's padding
	custom.set_border_width(10);
	
	// Pack on the frames
	vbmain->pack_start(lbcustom);
	vbmain->pack_start(sizeframe);
	vbmain->pack_start(ratioframe);
	
	// Add buttons
	custom.add_button(Gtk::Stock::OK, 1);
	custom.add_button(Gtk::Stock::CANCEL, 2);
	custom.set_default_response(1);
	
	custom.show_all_children();
	int result = custom.run();
	
	// Get result
	if (result == 1) // OK
	{
		// <Retard-proofing>
		if (ewidth.get_text() == "")
			ewidth.set_text("0");
		if (eheight.get_text() == "")
			eheight.set_text("0");
		if (eratio.get_text() == "")
			eratio.set_text("0");
		// </Retard-proofing>
		
		std::istringstream	bwidth(ewidth.get_text().raw()),
							bheight(eheight.get_text().raw()),
							bratio(eratio.get_text().raw());
		int tmpwidth, tmpheight, tmpratio;
		
		// Convert the entry box strings to integers
		bwidth >> tmpwidth;
		bheight >> tmpheight;
		bratio >> tmpratio;
		
		// Check that we have a valid width
		if (tmpwidth > 50 || tmpwidth < 2)
			tmpwidth = gridwidth;
		
		// Check that we have a valid height
		if (tmpheight > 50 || tmpheight < 2)
			tmpheight = gridheight;
			
		// Check that we have a valid ratio
		if (tmpratio < 2)
			tmpratio = gridratio;
		
		// draw the new grid
		gridratio = tmpratio;
		skill = 3;
		draw_grid(tmpwidth, tmpheight);
	}
}

// File->Quit
void bombz::on_menu_file_quit()
{
	hide();
}

// File->Scoreboard
void bombz::on_menu_file_scoreboard()
{
	scoreboard::show_scoreboard(this, 0, "");
}

// Help->About
void bombz::on_menu_help_about()
{
	// Authors
	std::list<Glib::ustring> authors;
	authors.push_back("Matt Hayes <nobomb@gmail.com>");
	
	// Artists
	std::list<Glib::ustring> artists;
	artists.push_back("Jake LeMaster <ssoundasleep@gmail.com>");
	artists.push_back("Matt Hayes <nobomb@gmail.com>");
	
	// The almighty GPL!
	Glib::ustring license = 
		"This program is free software: you can redistribute it and/or modify\n"
    	"it under the terms of the GNU General Public License as published by\n"
    	"the Free Software Foundation, either version 3 of the License, or\n"
    	"(at your option) any later version.\n\n"
    	"This program is distributed in the hope that it will be useful,\n"
    	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    	"GNU General Public License for more details.\n\n"
    	"You should have received a copy of the GNU General Public License\n"
    	"along with this program.  If not, see <http://www.gnu.org/licenses/>.";
	
	Gtk::AboutDialog::set_url_hook(sigc::mem_fun(*this, &bombz::on_menu_help_about_url));
	Gtk::AboutDialog about;
	
	Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/logo_test.png"); 
	
	// Set dialog properties.
	about.set_transient_for(*this);
	about.set_authors(authors);
	about.set_artists(artists);
	about.set_comments("The minesweeper clone of the gods.");
	about.set_license(license);
	about.set_logo(pb);
	about.set_name("bombz-gtk");
	about.set_program_name("bombz-gtk");
	about.set_version("1.0");
	about.set_website("http://code.google.com/p/bombz-gtk/");
	about.set_website_label("http://code.google.com/p/bombz-gtk/");
	
	// Run it.
	about.run();
}

// About (show url)
void bombz::on_menu_help_about_url(Gtk::Window &w, Glib::ustring s)
{
	
}

// Button clicked for new game
void bombz::on_button_clicked()
{
	draw_grid(gridwidth, gridheight);
}

// Set current face for toolbar button
void bombz::set_face(int num)
{
	newbut.remove();	// Clear the old pixlabel
	newbut.set_image(toolface[num]);
}
