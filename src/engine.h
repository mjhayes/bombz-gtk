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

#ifndef ENGINE_H_
#define ENGINE_H_

#include "button.h"
#include "bombz.h"
#include <gtkmm.h>

class button;
class bombz;

class engine
{
public:
	// Constructors/destructors
	engine();
	virtual ~engine();
	
	// Methods
	void new_game();
	void set_bombs();
	void set_size(int w, int h);
	void toggle_neighbors(int n);
	void toggle_neighbors_hard(int n);
	int get_blank_neighbors(int n);
	int get_flagged_neighbors(int n);
	void game_lost();
	void set_bombref(bombz *ref);
	bool get_lost();
	bool get_winner();
	void dec_tiles(int n);
	int get_bombs();
	void inc_bombs(int n);
	void dec_bombs(int n);
	void set_extra_bomb();
	int get_time();
	bool get_firstclick();
	void set_firstclick();
	void set_face(int num);
	int calc_bombs(int n);
	
	// Data
	Glib::RefPtr<Gdk::Pixbuf> bomb_pixbuf;
	Glib::RefPtr<Gdk::Pixbuf> num_pixbuf[8];

protected:
	// Methods
	void start_timer();
	void stop_timer();
	bool on_timeout(int n);

	// Data
	int width, height;
	bool lost, winner;
	int bombs, actualbombs, tiles;
	int bombcount;
	int gtime;
	double acc_time;
	struct timeval start_time, end_time;
	bool firstclick;
	sigc::connection conn;
	
	// Refs
	bombz *bombref;
};

#endif /*ENGINE_H_*/
