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

#ifndef BOMBZ_H_
#define BOMBZ_H_

#include "button.h"
#include "engine.h"
#include <gtkmm.h>

class engine;
class button;

class bombz : public Gtk::Window
{
public:
	// Constructors/destructors
	bombz(int width, int height);
	virtual ~bombz();
	
	// Methods
	void draw_grid(int width, int height);
	void update_toolbar();		// Update all toolbar widgets
	void set_face(int num); 	// Set the current face
	button* get_square(int i);
	int get_ratio();
	int get_skill();

protected:
	// Widgets
	Gtk::VBox vb, vb1;		// Main vertical box
	Gtk::HBox hb1;			// Contains counter and new game
	Gtk::Label lb1, lb2;	// Bombs remaining, time elapsed
	Gtk::Button newbut;		// New button
	Gtk::Alignment align;	// Alignment widget
	Gtk::Alignment a_bomb, a_time;
	
	// Methods
	void build_main_menu();		// Build the main menubar
	void build_toolbar();		// Build the toolbar
	void gengfx(Glib::ustring bombstr, Glib::ustring timestr);
	
	// Data
	button *squares;				// Squares array
	Gtk::HBox *hb;					// HBox array
	bool firstload;					// Is this the first load
	int gridwidth, gridheight;		// Current grid width and height
	int gridratio;					// Bomb to square ratio
	engine *eng;					// Engine object
	int skill;						// Easy (0), medium (1), or hard (2).
	Gtk::Image toolface[4], diginum[8];
	Gtk::HBox hb_bomb, hb_time;
	
	// References
	Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	
	// Signal handlers
	virtual void on_menu_file_new_easy();
	virtual void on_menu_file_new_medium();
	virtual void on_menu_file_new_hard();
	virtual void on_menu_file_new_custom();
	virtual void on_menu_file_scoreboard();
	virtual void on_menu_file_quit();
	virtual void on_menu_help_about();
	virtual void on_button_clicked();
	virtual void on_menu_help_about_url(Gtk::Window &w, Glib::ustring s);
};

#endif /*BOMBZ_H_*/
