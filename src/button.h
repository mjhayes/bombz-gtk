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

#ifndef BUTTON_H_
#define BUTTON_H_

#include "engine.h"
#include <gtkmm.h>

class engine;

class button : public Gtk::ToggleButton
{
public:
	// Constructors/destructors
	button();
	virtual ~button();
	
	// Methods
	void init(int r, int n, engine *ref);
	bool get_flagged();
	void set_flagged(bool b);
	bool get_selected();
	bool get_bomb();
	void set_bomb(bool b);
	void set_bombs(int n);
	int get_bombs();
	
protected:
	// Signals
	virtual void on_button_press(GdkEventButton* event);
	virtual void on_button_pressed();
	virtual void on_button_released();
	virtual void on_toggled();
	virtual void on_key_press(GdkEventKey* event);
	
	// Data
	bool flagged;
	bool selected;
	bool was_bomb;
	bool bomb;
	bool bombset;
	bool done;
	int bombs;
	engine *engref;
	int num;
};

#endif /*BUTTON_H_*/
