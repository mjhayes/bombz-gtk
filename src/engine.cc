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
#include "engine.h"
#include "bombz.h"
#include "scoreboard.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sys/time.h>

// Constructor
engine::engine() : lost(false), winner(false), bombs(0), actualbombs(0), gtime(0), firstclick(false)
{
	bomb_pixbuf = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/bombface6.xpm");
	num_pixbuf[0] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/1.xpm");
	num_pixbuf[1] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/2.xpm");
	num_pixbuf[2] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/3.xpm");
	num_pixbuf[3] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/4.xpm");
	num_pixbuf[4] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/5.xpm");
	num_pixbuf[5] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/6.xpm");
	num_pixbuf[6] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/7.xpm");
	num_pixbuf[7] = Gdk::Pixbuf::create_from_file(DATADIR "/bombz-gtk/8.xpm");
}

// Destructor
engine::~engine()
{	
	// Delete bombz pointer.
	delete bombref;
}

// New game
void engine::new_game()
{
	srand(time(0));		// Set the random seed
	lost = false;		// Set lost to false
	winner = false;		// Set winner to false
	firstclick = false;	// We  have not received a click yet
	bombs = 0;			// Reset total bomb count
	bombcount = 0;		// Reset remembered bomb count
	actualbombs = 0;	// The actual number of remaining bombs 
	gtime = 0;			// Reset the time
	stop_timer();		// Stop any running timers
}

// Set each square's surrounding bomb count
void engine::set_bombs()
{
	// Loop through entire grid
	for (int i = 0; i < width * height; i++)
	{
		// Initialize square.
		bombref->get_square(i)->init((rand() % bombref->get_ratio()) + 1, i, this);
		
		// If this is a bomb, skip this square.
		if (bombref->get_square(i)->get_bomb())
		{
			bombs++;
			continue;
		}
	}
	
	actualbombs = bombs;					// Set actual amount of bombs
	bombcount = actualbombs;				// Set the original amount of bombs
	tiles = width * height - actualbombs;	// Set total amount of non-bomb squares.
}

// Calculate surrounding bombs (this is the hard shet)
int engine::calc_bombs(int n)
{
	int count = 0;
	
	for (int r = n - width; r <= n + width; r += width)
	{	
		// This row is out of range
		if (r < 0 || r >= width * height)
			continue;
		
		for (int c = -1; c <= 1; c++)
		{
			int cur = r + c;	// Current square index
			
			// This is the square we are searching for bombs around,
			// so skip it.
			if (cur == n || cur < 0 || cur >= width * height || (cur / width != r / width))
				continue;
			
			// If this is a bomb, increase our total bomb count
			if (bombref->get_square(cur)->get_bomb())
				count++;
		}
	}
	
	return count;	// This should be the total number of surrounding bombs
}

// Set squares size
void engine::set_size(int w, int h)
{
	width = w;
	height = h;
}

// Toggle empty neighbors
void engine::toggle_neighbors(int n)
{
	for (int r = n - width; r <= n + width; r += width)
	{
		if (r < 0 || r >= width * height)
			continue;
		
		for (int c = -1; c <= 1; c++)
		{
			int cur = r + c;
			
			if (cur == n || cur < 0 || cur >= width * height || (cur / width != r / width))
				continue;
			
			// Check if this is a square we want to flip
			if (	!bombref->get_square(cur)->get_flagged() &&
					!bombref->get_square(cur)->get_bomb() &&
					!bombref->get_square(cur)->get_active())
			{
				bombref->get_square(cur)->set_active(true);
			}	
		}
	}
}

// Toggle empty neighbors (even if one is a bomb!)
void engine::toggle_neighbors_hard(int n)
{
	for (int r = n - width; r <= n + width; r += width)
	{
		if (r < 0 || r >= width * height)
			continue;
		
		for (int c = -1; c <= 1; c++)
		{
			int cur = r + c;
			
			if (cur == n || cur < 0 || cur >= width * height || (cur / width != r / width))
				continue;
			
			// Check if this is a square we want to flip
			if (	!bombref->get_square(cur)->get_flagged() &&
					!bombref->get_square(cur)->get_active())
			{
				bombref->get_square(cur)->set_active(true);
			}	
		}
	}
}

// Get empty neighbors
int engine::get_blank_neighbors(int n)
{
	int count = 0;
	
	for (int r = n - width; r <= n + width; r += width)
	{
		if (r < 0 || r >= width * height)
			continue;
		
		for (int c = -1; c <= 1; c++)
		{
			int cur = r + c;
			
			if (cur == n || cur < 0 || cur >= width * height || (cur / width != r / width))
				continue;
			
			// Check if this is a square we want to flip
			if (	!bombref->get_square(cur)->get_flagged() &&
					!bombref->get_square(cur)->get_bomb())
				count++;
		}
	}
	
	return count;	// This should equal the amount of blank squares surrounding n
}

// Get flagged neighbors
int engine::get_flagged_neighbors(int n)
{
	int count = 0;
	
	for (int r = n - width; r <= n + width; r += width)
	{
		if (r < 0 || r >= width * height)
			continue;
		
		for (int c = -1; c <= 1; c++)
		{
			int cur = r + c;
			
			if (cur == n || cur < 0 || cur >= width * height || (cur / width != r / width))
				continue;
			
			// Check if this is a square we want to flip
			if (bombref->get_square(cur)->get_flagged())
				count++;
		}
	}
	
	return count;	// This should equal the amount of blank squares surrounding n
}


// Game is lost, take action.
void engine::game_lost()
{
	lost = true;	// Set lost to true
	stop_timer();	// Stop the game timer
	
	bombref->set_face(2);	// Set the toolbar face to dead
	
	// Press all squares and deactivate everything.
	for (int i = 0; i < width * height; i++)
	{
		if (bombref->get_square(i)->get_flagged())
			bombref->get_square(i)->set_flagged(false);
		
		// If this square isn't toggled, activate it.
		if (!bombref->get_square(i)->get_active())
			bombref->get_square(i)->set_active(true);
		
		// Desensitize all non-bombs!
		if (!bombref->get_square(i)->get_bomb())
			bombref->get_square(i)->set_sensitive(false);
	}
}

// Set a bombz pointer
void engine::set_bombref(bombz *ref)
{
	bombref = ref;
}

// Get lost
bool engine::get_lost()
{
	return lost;
}

// Get winner
bool engine::get_winner()
{
	return winner;
}

// Decrease total number of unclicked tiles.
void engine::dec_tiles(int n)
{
	tiles--;
	
	if (tiles == 0 && actualbombs == 0)
	{
		gettimeofday(&end_time, NULL);
		long elapsed_seconds  = end_time.tv_sec  - start_time.tv_sec;
		long elapsed_useconds = end_time.tv_usec - start_time.tv_usec;
		long elapsed_mtime = ((elapsed_seconds) * 1000 + elapsed_useconds/1000.0) + 0.5;
		
		winner = true;	// Set winner to true
		stop_timer();	// Stop the game timer
		
		bombref->set_face(1);	// Set the toolbar face to win

		// I need to deactivate and activate the square again.
		// This is because for some reason the square will not fully
		// finish the click before I pop up the high score name input.
		// Bleh, this is all I could come up with for now.
		bombref->get_square(n)->set_active(false);
		bombref->get_square(n)->set_active(true);
		
		// Check if we have a high score.
		scoreboard::check_high_scores(bombref, elapsed_mtime, bombcount);
	}
}

// Get total number of bombs
int engine::get_bombs()
{
	return bombs;
}

// Increase bomb count
void engine::inc_bombs(int n)
{
	bombs++;	// Increase visual bomb count
	
	// Increase actual bomb count, if this is a bomb
	if (bombref->get_square(n)->get_bomb())
		actualbombs++;
	
	// Update the bomb count label
	Glib::ustring str = Glib::ustring::compose("Bombs: %1", bombs);
	bombref->update_toolbar();
}

// Decrease bomb count
void engine::dec_bombs(int n)
{
	bombs--;	// decrease visual bomb count
	
	// Decrease actual bomb count, if this is a bomb
	if (bombref->get_square(n)->get_bomb())
		actualbombs--;
	
	// Update the bomb count label
	bombref->update_toolbar();
	
	// See if we have no bombs remaining
	if (actualbombs == 0 && tiles == 0)
	{
		gettimeofday(&end_time, NULL);
		long elapsed_seconds  = end_time.tv_sec  - start_time.tv_sec;
		long elapsed_useconds = end_time.tv_usec - start_time.tv_usec;
		long elapsed_mtime = ((elapsed_seconds) * 1000 + elapsed_useconds/1000.0) + 0.5;
		
		winner = true;	// Set winner to true
		stop_timer();	// Stop the game timer
		
		bombref->set_face(1);	// Set the toolbar face to win
		
		// Check if we have a high score.
		scoreboard::check_high_scores(bombref, elapsed_mtime, bombcount);
	}
}

// Randomly set an extra bomb
void engine::set_extra_bomb()
{
	int r;
	while (1)
	{
		// Random array element
		r = rand() % (width * height);
		
		// Turn this sucka into a bomb
		if (!bombref->get_square(r)->get_bomb())
		{
			bombref->get_square(r)->set_bomb(true);
			break;
		}
	}
}

// Get time
int engine::get_time()
{
	return gtime;
}

// Get first click
bool engine::get_firstclick()
{
	return firstclick;
}

// Set first click
void engine::set_firstclick()
{
	gettimeofday(&start_time, NULL);
	
	firstclick = true;	// Set firstclick to true
	start_timer();		// Start the game timer
}

// Start game timer
void engine::start_timer()
{
	sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &engine::on_timeout), 0);

	// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, 1000);
}

// Stop game timer
void engine::stop_timer()
{
	conn.disconnect();	// Disconnect the timer
}

// On timeout
bool engine::on_timeout(int n)
{	
	gtime++;	// Increase time
	
	// Loser message.
	if (gtime == 6000)
	{
		game_lost();
				
		Gtk::MessageDialog hell(*bombref, "Noob.");
		hell.set_secondary_text("You are a giant noob.\n"
								"Who takes an hour and 40 minutes to finish a game of bombz?\n"
								"You probably actually went AFK and forgot you had a game running.\n"
								"Look for Vacation Mode in bombz-gtk v10.0.");
		hell.run();
		
		return true;
	}
	
	bombref->update_toolbar();	// Update main toolbar
	
	return true;
}

void engine::set_face(int num)
{
	bombref->set_face(num);
}
