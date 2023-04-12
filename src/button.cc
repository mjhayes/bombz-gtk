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
#include "button.h"
#include <iostream>

#define SIZE 20

// Constructor
button::button() : flagged(false), selected(false), bomb(false), bombset(false), bombs(0)
{
	// Connect signal_clicked to on_button_clicked
	signal_button_press_event().connect_notify( sigc::mem_fun(*this, &button::on_button_press) );
	signal_pressed().connect( sigc::mem_fun(*this, &button::on_button_pressed) );
	signal_released().connect( sigc::mem_fun(*this, &button::on_button_released) );
	
	// Keyboard presses
	signal_key_press_event().connect_notify( sigc::mem_fun(*this, &button::on_key_press) );
}

// Destructor
button::~button()
{
}

// Init
void button::init(int r, int n, engine *ref)
{
	engref = ref;					// Engine reference
	set_size_request(SIZE, SIZE);	// Set button size
	num = n;						// Set button number
	
	// Reset states.
	done = false;
	
	// Remove the red background from clicked bomb.
	if (was_bomb)
	{
		unset_bg(Gtk::STATE_NORMAL);
		unset_bg(Gtk::STATE_ACTIVE);
		unset_bg(Gtk::STATE_PRELIGHT);
		unset_bg(Gtk::STATE_SELECTED);
		unset_bg(Gtk::STATE_INSENSITIVE);
	}
	
	was_bomb = false;
	flagged = false;
	selected = false;
	bomb = false;
	bombset = false;
	bombs = 0;
	set_active(false);	
	set_sensitive(true);
	
	// Remove any graphics from the button.
	remove();
	
	// We are done configuring.
	done = true;
	
	// Set as a bomb if r == 1
	if (r == 1)
		bomb = true;
}

// on button pressed
void button::on_button_pressed()
{
	// If we have lost or won, do not change the face anymore
	if (engref->get_lost() || engref->get_winner())
		return;
		
	engref->set_face(3);
}

// on button released
void button::on_button_released()
{
	// Fart.
	if (engref->get_lost() || engref->get_winner())
		return;
		
	engref->set_face(0);
}

// on_button_clicked event
void button::on_button_press(GdkEventButton* event)
{
	// Set surrounding bombs count if we haven't done so already.
	if (!bomb && !bombset)
	{
		set_bombs(engref->calc_bombs(num));
		bombset = true;
	}
	
	// Left + Right click, clear surrounding tiles.
	if ((get_active() && event->button == 1 && event->state >= 1024 && event->state <= 1042) ||
		(get_active() && event->button == 3 && event->state >= 256 && event->state <= 272)) {
		// If this square has atleast 1 bomb surrounding it, and we have
		// an equal number of squares flagged, toggle the rest of the
		// surrounding squares.
		if (bombs > 0 && engref->get_flagged_neighbors(num) == bombs)
			engref->toggle_neighbors_hard(num);
	}
	// Right click, make sure button has not been activated.
	else if (	event->button == 3  &&
				!get_active() &&
				!engref->get_winner())
	{
		// No flag, add it.
		if (!flagged && engref->get_bombs() > 0)
		{
			add_pixlabel(DATADIR "/bombz-gtk/flag.xpm", "");	// Add pixlabel
			engref->dec_bombs(num);	// Decrease the bomb count
			flagged = true;			// Remember this has been flagged
		}
		// Flag, so remove it.
		else if (flagged)
		{
			remove();				// Removes the pixlabel
			engref->inc_bombs(num);	// Increase bomb count
			flagged = false;		// Remember this is not flagged
		}
	}
}

// on_key_press event
void button::on_key_press(GdkEventKey* event)
{
	// Ctrl + Enter, clear surrounding tiles.
	if (get_active() && event->keyval == 65293 &&
		(event->state == 4 || event->state == 20 || event->state == 6 || event->state == 22))
	{
		// If this square has atleast 1 bomb surrounding it, and we have
		// an equal number of squares flagged, toggle the rest of the
		// surrounding squares.
		if (bombs > 0 && engref->get_flagged_neighbors(num) == bombs)
			engref->toggle_neighbors_hard(num);
	}
	// Shift + Enter (Same action as right click) Flags
	else if (!get_active() && !engref->get_winner() &&
		event->keyval == 65293 &&
		(event->state == 1 || event->state == 3 || event->state == 17 || event->state == 19))
	{
		// No flag, add it.
		if (!flagged)
		{
			add_pixlabel(DATADIR "/bombz-gtk/flag.xpm", "");	// Add pixlabel
			engref->dec_bombs(num);	// Decrease the bomb count
			flagged = true;			// Remember this has been flagged
		}
		// Flag, so remove it.
		else
		{
			remove();				// Removes the pixlabel
			engref->inc_bombs(num);	// Increase bomb count
			flagged = false;		// Remember this is not flagged
		}
	}
}

// on toggled
void button::on_toggled()
{
	// Make sure the button is not re-initializing.
	if (!done)
		return;
	
	// Check if this is the first click
	if (!engref->get_firstclick())
	{
		engref->set_firstclick();	// Game has begun
		
		// First click should not be a bomb!
		if (bomb)
		{
			engref->set_extra_bomb();
			bomb = false;
		}
	}
	
	// Set surrounding bombs count if we haven't done so already.
	if (!bomb && !bombset)
	{
		set_bombs(engref->calc_bombs(num));
		bombset = true;
	}
	
	// Button was just selected
	if (!selected && !flagged && get_active())
	{
		if (bomb)
		{
			set_image(*manage(new Gtk::Image(engref->bomb_pixbuf)));
			
			// Player lost, tell the freaking engine!
			if (!engref->get_lost())
			{
				was_bomb = true;
				
				// Set the losing bomb bg to red
				Gdk::Color c("red");
				modify_bg(get_state(), c);
				
				// Run the losing method
				engref->game_lost();
			}
		}
		else
		{
			// Decrease number of unclicked tiles.
			if (!engref->get_lost())
				engref->dec_tiles(num);
			
			// Only display a count label if there is atleast 1 bomb
			if (bombs > 0)
				set_image(*manage(new Gtk::Image(engref->num_pixbuf[bombs - 1])));
			// All neighboring empty cells will be toggled
			else
			{	
				// Only toggle if we have at least one blank neighbor to this blank square
				if (!engref->get_lost() && engref->get_blank_neighbors(num) > 0)
					engref->toggle_neighbors(num);
			}
		}
		
		selected = true;
	}
	// Button was trying to be deselected, but that is not allowed
	else if (selected && !get_active())
	{
		set_active(true);
	}
	// A flagged button was trying to be selected
	else if (flagged && get_active())
	{
		set_active(false);
	}
}

// Return flagged
bool button::get_flagged()
{
	return flagged;
}

// Set flagged
void button::set_flagged(bool b)
{
	// We need to remove the flag graphic if we are unflagging
	if (b == false)
		remove();
	
	flagged = b;
}

// Return selected
bool button::get_selected()
{
	return selected;
}

// Return bomb status
bool button::get_bomb()
{
	return bomb;
}

// Set bomb status
void button::set_bomb(bool b)
{
	bomb = b;
}

// Set number of surrounding bombs
void button::set_bombs(int n)
{
	bombs = n;
}

// Get number of surrounding bombs
int button::get_bombs()
{
	return bombs;
}
