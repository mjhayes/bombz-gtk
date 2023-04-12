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

#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include "bombz.h"
#include <gtkmm.h>

class bombz;

class scoreboard
{
	public:
		static void show_scoreboard(bombz *b, int page, std::string sstr);
		static void check_high_scores(bombz *b, long t, int c);

	protected:
		static Glib::ustring get_high_scorer(bombz *b, long t, int c);
		static bool compare_score(Glib::ustring str1, Glib::ustring str2);
};

#endif /*SCOREBOARD_H_*/
